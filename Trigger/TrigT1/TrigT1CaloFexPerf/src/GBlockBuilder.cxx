/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GBlockBuilder.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODTrigL1Calo/GBlockAuxContainer.h"
#include "xAODTrigL1Calo/GBlock.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GTowerHelpers.h"
#include <memory>

namespace
{
  const static SG::AuxElement::ConstAccessor<std::size_t> accNextEtaIndex("nextEtaIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accPreviousEtaIndex("previousEtaIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accNextPhiIndex("nextPhiIndex");
  const static SG::AuxElement::ConstAccessor<std::size_t> accPreviousPhiIndex("previousPhiIndex");
  const static SG::AuxElement::ConstAccessor<float> accArea("area");
} // namespace

namespace LVL1
{
  GBlockBuilder::GBlockBuilder(const std::string &name, ISvcLocator *pSvcLocator)
      : AthAlgorithm(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_towersKey);
    declareProperty("OutputBlocks", m_gBlocksKey);
    declareProperty("ForwardEta", m_forwardEta = 3.2);
  }

  GBlockBuilder::~GBlockBuilder() {}

  StatusCode GBlockBuilder::initialize()
  {
    ATH_CHECK(m_towersKey.initialize());
    ATH_CHECK(m_gBlocksKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode GBlockBuilder::execute()
  {
    const EventContext &ctx = Gaudi::Hive::currentContext();
    auto towers = SG::makeHandle(m_towersKey, ctx);
    if (!towers.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_towersKey.key());
      return StatusCode::FAILURE;
    }

    auto outputGBlocks = std::make_unique<xAOD::GBlockContainer>();
    auto outputGBlocksAux = std::make_unique<xAOD::GBlockAuxContainer>();
    outputGBlocks->setStore(outputGBlocksAux.get());

    for (const xAOD::JGTower *seedTower : *towers)
    {
      float et = 0;
      float area = 0;
      float dEta = seedTower->deta();
      float dPhi = seedTower->dphi();
      gFEX::FPGA currentFPGA = gFEX::getFPGA(seedTower->eta());
      std::vector<const xAOD::JGTower *> inputs{seedTower};
      // The forward region only uses 1x1 gBlocks as the towers are already larger
      if (std::abs(seedTower->eta()) < m_forwardEta)
      {
        const xAOD::JGTower *nextPhiTower = towers->at(accNextPhiIndex(*seedTower));
        dPhi += nextPhiTower->dphi();
        const xAOD::JGTower *previousPhiTower = towers->at(accPreviousPhiIndex(*seedTower));
        dPhi += previousPhiTower->dphi();
        inputs.push_back(towers->at(accNextPhiIndex(*seedTower)));
        inputs.push_back(towers->at(accPreviousPhiIndex(*seedTower)));
        std::size_t nextEtaIndex = accNextEtaIndex(*seedTower);
        if (nextEtaIndex != SIZE_MAX)
        {
          const xAOD::JGTower *nextTower = towers->at(nextEtaIndex);
          dEta += nextTower->deta();
          if (currentFPGA == gFEX::getFPGA(nextTower->eta()))
          {
            inputs.push_back(nextTower);
            inputs.push_back(towers->at(accNextPhiIndex(*nextTower)));
            inputs.push_back(towers->at(accPreviousPhiIndex(*nextTower)));
          }
        }
        std::size_t previousEtaIndex = accPreviousEtaIndex(*seedTower);
        if (previousEtaIndex != SIZE_MAX)
        {
          const xAOD::JGTower *previousTower = towers->at(previousEtaIndex);
          dEta += previousTower->deta();
          if (currentFPGA == gFEX::getFPGA(previousTower->eta()))
          {
            inputs.push_back(previousTower);
            inputs.push_back(towers->at(accNextPhiIndex(*previousTower)));
            inputs.push_back(towers->at(accPreviousPhiIndex(*previousTower)));
          }
        }
      }
      std::vector<ElementLink<xAOD::JGTowerContainer>> links;
      for (const xAOD::JGTower *tower : inputs)
      {
        et += tower->et();
        area += accArea(*tower);
        links.emplace_back(m_towersKey.key(), tower->index());
      }
      xAOD::GBlock *block = new xAOD::GBlock();
      outputGBlocks->push_back(block);
      block->setEta(seedTower->eta());
      block->setPhi(seedTower->phi());
      block->setdEta(dEta);
      block->setdPhi(dPhi);
      block->setEt(et);
      block->setArea(area);
      block->setSeedTowerLink({m_towersKey.key(), seedTower->index()});
      block->setTowerLinks(links);
    }

    auto gBlockHandle = SG::makeHandle(m_gBlocksKey, ctx);
    ATH_CHECK(gBlockHandle.record(std::move(outputGBlocks), std::move(outputGBlocksAux)));
    return StatusCode::SUCCESS;
  }
} // namespace LVL1