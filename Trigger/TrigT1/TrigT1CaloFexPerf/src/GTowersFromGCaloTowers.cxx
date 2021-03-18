/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "GTowersFromGCaloTowers.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include <memory>
#include <cmath>

namespace
{
  const static SG::AuxElement::ConstAccessor<float> accArea("area");
  const static SG::AuxElement::Decorator<float> decArea("area");
  const static SG::AuxElement::ConstAccessor<std::size_t> accNextEtaIndex("nextEtaIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accPreviousEtaIndex("previousEtaIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accNextPhiIndex("nextPhiIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accPreviousPhiIndex("previousPhiIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accIndexInFront("indexInFront");
  const static SG::AuxElement::ConstAccessor<std::size_t> accIndexBehind("indexBehind");
  const static SG::AuxElement::Decorator<std::size_t> decNextEtaIndex("nextEtaIndex");
  const static SG::AuxElement::Decorator<std::size_t> decPreviousEtaIndex("previousEtaIndex");
  const static SG::AuxElement::Decorator<std::size_t> decNextPhiIndex("nextPhiIndex");
  const static SG::AuxElement::Decorator<std::size_t> decPreviousPhiIndex("previousPhiIndex");
  const static SG::AuxElement::Decorator<std::size_t> decIndexInFront("indexInFront");
  const static SG::AuxElement::Decorator<std::size_t> decIndexBehind("indexBehind");
  const static SG::AuxElement::Decorator<std::vector<std::size_t>> decMergedIndices("mergedIndices");

  std::size_t getRemappedIndex(const std::map<std::size_t, std::size_t> &remap, std::size_t originalIdx)
  {
    auto itr = remap.find(originalIdx);
    return itr == remap.end() ? SIZE_MAX : itr->second;
  }
} // namespace

namespace LVL1
{
  GTowersFromGCaloTowers::GTowersFromGCaloTowers(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_inputKey = "GTower");
    declareProperty("OutputTowers", m_outputKey = "GCaloTower");
    declareProperty("MergeEdgeTowers", m_mergeEdgeTowers = true);
  }

  GTowersFromGCaloTowers::~GTowersFromGCaloTowers() {}

  StatusCode GTowersFromGCaloTowers::initialize()
  {
    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_mappingKey.initialize());
    ATH_CHECK(detStore()->retrieve(m_gTowerID));
    return StatusCode::SUCCESS;
  }

  StatusCode GTowersFromGCaloTowers::execute(const EventContext &ctx) const
  {
    auto inputTowers = SG::makeHandle(m_inputKey, ctx);
    if (!inputTowers.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve input towers " << m_inputKey.key());
      return StatusCode::FAILURE;
    }

    auto outputTowers = std::make_unique<xAOD::JGTowerContainer>();
    auto outputTowersAux = std::make_unique<xAOD::JGTowerAuxContainer>();
    outputTowers->setStore(outputTowersAux.get());

    // Get the helpers
    SG::ReadCondHandle<JGTowerMappingData> mapping(m_mappingKey, ctx);
    if (!mapping.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve mapping " << m_mappingKey);
      return StatusCode::FAILURE;
    }
    const std::vector<JGTowerHelper> &helpers = **mapping;

    std::map<std::size_t, std::size_t> indexRemap;

    for (const xAOD::JGTower *tower : *inputTowers)
    {
      const JGTowerHelper &helper = helpers.at(tower->Id());
      if (!helper.isFrontTower())
        // Avoid double counting by only taking the front tower in any given eta/phi slice
        continue;
      Identifier towerID = m_gTowerID->tower_id(tower->Id());
      int iRegion = m_gTowerID->region(towerID);
      if (m_mergeEdgeTowers && iRegion == 1)
        // region 1 is the 2.4-2.5 region
        // There used to be a region 3, but we have just absorbed this into the FCAL towers
        continue;
      indexRemap[tower->index()] = outputTowers->size();
      std::vector<int> SCIndex = tower->SCIndex();
      std::vector<int> tileIndex = tower->TileIndex();
      float totalEt = tower->et();
      std::size_t nextTowerIdx = accIndexBehind(*tower);
      float area = accArea(*tower);
      float eta = tower->eta();
      std::size_t nextEtaIndex = accNextEtaIndex(*tower);
      std::size_t previousEtaIndex = accPreviousEtaIndex(*tower);
      std::vector<std::size_t> mergedIndices{tower->index()};
      while (nextTowerIdx != SIZE_MAX)
      {
        const xAOD::JGTower *nextTower = inputTowers->at(nextTowerIdx);
        totalEt += nextTower->et();
        SCIndex.insert(SCIndex.end(), nextTower->SCIndex().begin(), nextTower->SCIndex().end());
        tileIndex.insert(tileIndex.end(), nextTower->TileIndex().begin(), nextTower->TileIndex().end());
        mergedIndices.push_back(nextTowerIdx);
        nextTowerIdx = helpers.at(nextTower->Id()).indexBehind();
      }
      if (m_mergeEdgeTowers)
      {
        Identifier regionID = m_gTowerID->region_id(towerID);
        // Figure out if this is at the edge of region 0
        if (m_gTowerID->region(towerID) == 0 && m_gTowerID->eta(towerID) == m_gTowerID->eta_max(regionID))
        {
          if (tower->eta() > 0)
          {
            nextTowerIdx = nextEtaIndex;
            const xAOD::JGTower *nextTower = inputTowers->at(nextTowerIdx);
            // Now we need to correct the eta index that we will write out
            nextEtaIndex = accNextEtaIndex(*nextTower);
          }
          else
          {
            nextTowerIdx = previousEtaIndex;
            const xAOD::JGTower *nextTower = inputTowers->at(nextTowerIdx);
            // Now we need to correct the eta index that we will write out
            previousEtaIndex = accPreviousEtaIndex(*nextTower);
          }
          indexRemap[nextTowerIdx] = outputTowers->size();
          while (nextTowerIdx != SIZE_MAX)
          {
            const xAOD::JGTower *nextTower = inputTowers->at(nextTowerIdx);
            if (helpers.at(nextTowerIdx).isFrontTower())
            {
              area += accArea(*nextTower);
              eta += std::copysign(nextTower->deta() / 2, eta);
            }
            totalEt += nextTower->et();
            SCIndex.insert(SCIndex.end(), nextTower->SCIndex().begin(), nextTower->SCIndex().end());
            tileIndex.insert(tileIndex.end(), nextTower->TileIndex().begin(), nextTower->TileIndex().end());
            mergedIndices.push_back(nextTowerIdx);
            nextTowerIdx = helpers.at(nextTower->Id()).indexBehind();
          }
        }
      }
      // Now create the new tower
      xAOD::JGTower *newTower = new xAOD::JGTower();
      outputTowers->push_back(newTower);
      // Copy auxdata from the old tower to this one
      *newTower = *tower;
      decArea(*newTower) = area;
      // override the et and indices.
      newTower->setEt(totalEt);
      newTower->setEta(eta);
      // NB - in the old code these indices get set to the fairly meaningless [0, 0].
      // I've decied to set this to the union of the indices of the towers forming the gCaloTowers
      newTower->setSCIndex(SCIndex);
      newTower->setTileIndex(tileIndex);
      // Now no towers in front or behind
      decIndexInFront(*newTower) = SIZE_MAX;
      decIndexBehind(*newTower) = SIZE_MAX;
      decNextEtaIndex(*newTower) = nextEtaIndex;
      decPreviousEtaIndex(*newTower) = previousEtaIndex;
      decMergedIndices(*newTower) = mergedIndices;
    }

    // Remap the relational indices
    for (xAOD::JGTower *tower : *outputTowers)
    {
      decNextEtaIndex(*tower) = getRemappedIndex(indexRemap, accNextEtaIndex(*tower));
      decPreviousEtaIndex(*tower) = getRemappedIndex(indexRemap, accPreviousEtaIndex(*tower));
      decNextPhiIndex(*tower) = getRemappedIndex(indexRemap, accNextPhiIndex(*tower));
      decPreviousPhiIndex(*tower) = getRemappedIndex(indexRemap, accPreviousPhiIndex(*tower));
    }

    auto outputHandle = SG::makeHandle(m_outputKey, ctx);
    ATH_CHECK(outputHandle.record(std::move(outputTowers), std::move(outputTowersAux)));
    return StatusCode::SUCCESS;
  }
} // namespace LVL1