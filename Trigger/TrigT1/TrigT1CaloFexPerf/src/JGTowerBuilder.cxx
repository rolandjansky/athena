/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JGTowerBuilder.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "AthLinks/ElementLink.h"

namespace
{
  const static SG::AuxElement::Decorator<float> decArea("area");
  const static SG::AuxElement::Decorator<std::size_t> decNextEtaIndex("nextEtaIndex");
  const static SG::AuxElement::Decorator<std::size_t> decPreviousEtaIndex("previousEtaIndex");
  const static SG::AuxElement::Decorator<std::size_t> decNextPhiIndex("nextPhiIndex");
  const static SG::AuxElement::Decorator<std::size_t> decPreviousPhiIndex("previousPhiIndex");
  const static SG::AuxElement::Decorator<std::size_t> decIndexInFront("indexInFront");
  const static SG::AuxElement::Decorator<std::size_t> decIndexBehind("indexBehind");
} // namespace

namespace LVL1
{
  JGTowerBuilder::JGTowerBuilder(const std::string &name, ISvcLocator *pSvcLocator)
      : AthAlgorithm(name, pSvcLocator)
  {
    declareProperty("MappingMaker", m_mappingMaker);
    declareProperty("InputCells", m_cellsKey = "AllCalo");
    declareProperty("InputSuperCells", m_superCellsKey = "SCell");
    declareProperty("InputTriggerTowers", m_triggerTowersKey = "xAODTriggerTowers");
    declareProperty("UseAllCalo", m_useAllCalo = false);
    declareProperty("UseAllRun2TT", m_useAllRun2TT = false);
    declareProperty("OutputTowers", m_towersKey);
    declareProperty("EmulateSuperCellTiming", m_emulateSCTiming = false);
    declareProperty("UseSCQuality", m_useSCQuality = true);
    declareProperty("SuperCellQuality", m_scQuality = 0x200);
    declareProperty("MinSCETp", m_minSCETp = -1, "Minimum Et for positive energy scells");
    declareProperty("MaxSCETm", m_maxSCETm = 1, "Maximum Et for negative energy scells");
    declareProperty("MinTowerET", m_minTowerEt = -9e9);
  }

  JGTowerBuilder::~JGTowerBuilder() {}

  StatusCode JGTowerBuilder::initialize()
  {
    ATH_CHECK(m_mappingMaker.retrieve());
    ATH_CHECK(m_superCellsKey.initialize());
    if (m_useAllCalo)
      ATH_CHECK(m_cellsKey.initialize());
    else
      ATH_CHECK(m_triggerTowersKey.initialize());
    ATH_CHECK(m_towersKey.initialize());

    ATH_CHECK(detStore()->retrieve(m_scid));
    return StatusCode::SUCCESS;
  }

  StatusCode JGTowerBuilder::execute()
  {
    ATH_MSG_DEBUG("Executing " << name());
    auto superCells = SG::makeHandle(m_superCellsKey);
    ATH_CHECK(superCells.isValid());
    const CaloCellContainer *cells = nullptr;
    const xAOD::TriggerTowerContainer *triggerTowers = nullptr;
    if (m_useAllCalo)
    {
      auto cellsHandle = SG::makeHandle(m_cellsKey);
      ATH_CHECK(cellsHandle.isValid());
      cells = cellsHandle.ptr();
    }
    else
    {
      auto triggerTowersHandle = SG::makeHandle(m_triggerTowersKey);
      ATH_CHECK(triggerTowersHandle.isValid());
      triggerTowers = triggerTowersHandle.ptr();
    }

    auto towerHandle = SG::makeHandle(m_towersKey);
    auto towers = std::make_unique<xAOD::JGTowerContainer>();
    auto towersAux = std::make_unique<xAOD::JGTowerAuxContainer>();
    towers->setStore(towersAux.get());

    for (const JGTowerHelper &towerHelper : m_mappingMaker->getMapping())
    {
      float LArEt = 0;
      float tileEt = 0;
      if (m_useAllRun2TT)
      {
        for (const xAOD::TriggerTower *triggerTower : *triggerTowers)
        {
          if (triggerTower->sampling() != towerHelper.sampling() && std::abs(towerHelper.Eta()) < 3.1)
            continue;
          if (!towerHelper.inBox(triggerTower->eta(), triggerTower->phi()))
            continue;
          tileEt += 500 * triggerTower->cpET();
        }
      }
      else
      {
        // Fill towers from SC
        for (unsigned int scIdx : towerHelper.GetSCIndices())
        {
          Identifier scID = m_scid->cell_id(scIdx);
          IdentifierHash scHash = m_scid->calo_cell_hash(scID);
          const CaloCell *superCell = superCells->findCell(scHash);
          if (!superCell)
            // TODO: Should this at least be a warning?
            continue;
          float et = superCell->et();
          float time = superCell->time();
          if (std::isnan(superCell->et()))
            // TODO: Should this only be a warning, or else a job failure?
            ATH_MSG_ERROR("Supercell ET is nan. Likely due to BCID correction or something else upstream");
          if (m_emulateSCTiming)
          {
            if (
                (et < 10e3 && std::abs(time > 8)) ||
                (et >= 10e3 && (time > 16 || time < -8)))
              continue;
          }
          else
          {
            if (
                (m_useSCQuality && !(superCell->provenance() & m_scQuality)) ||
                (et > 0 && et < m_minSCETp) ||
                (et < 0 && et > m_maxSCETm))
              continue;
          }
          LArEt += et;
        }

        if (towerHelper.sampling() == 1)
        {
          if (m_useAllCalo)
            for (unsigned int tileIdx : towerHelper.GetTileIndices())
            {
              const CaloCell *cell = cells->findCell(tileIdx);
              if (!cell)
                // TODO: Should this be at least a warning?
                continue;
              tileEt += cell->e() * cell->sinTh();
            }
          else
            for (const xAOD::TriggerTower *triggerTower : *triggerTowers)
              if (
                  triggerTower->sampling() == 1 &&
                  std::abs(triggerTower->eta()) < 1.5 &&
                  towerHelper.inBox(triggerTower->eta(), triggerTower->phi()))
                tileEt += 500 * triggerTower->cpET();
        }
      }
      float towerEt = LArEt + tileEt;
      if (towerEt < m_minTowerEt)
        towerEt = 0;
      xAOD::JGTower *tower = new xAOD::JGTower();
      towers->push_back(tower);
      tower->initialize(towers->size() - 1, towerHelper.Eta(), towerHelper.Phi());
      tower->setdEta(towerHelper.dEta());
      tower->setdPhi(towerHelper.dPhi());
      tower->setEt(towerEt);
      tower->setSCIndex(towerHelper.GetSCIndices());
      tower->setTileIndex(towerHelper.GetTileIndices());
      tower->setSampling(towerHelper.sampling());
      decArea(*tower) = towerHelper.area();
      // Decorate the relations
      decNextEtaIndex(*tower) = towerHelper.nextEtaIndex();
      decPreviousEtaIndex(*tower) = towerHelper.previousEtaIndex();
      decNextPhiIndex(*tower) = towerHelper.nextPhiIndex();
      decPreviousPhiIndex(*tower) = towerHelper.previousPhiIndex();
      decIndexInFront(*tower) = towerHelper.indexInFront();
      decIndexBehind(*tower) = towerHelper.indexBehind();
    } //> end loop over tower helpers

    ATH_CHECK(towerHandle.record(std::move(towers), std::move(towersAux)));
    return StatusCode::SUCCESS;
  }
} // namespace LVL1