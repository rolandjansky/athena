/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTowerMonitorAlgorithm.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "CaloIdentifier/TileID.h"

#include "StoreGate/ReadHandle.h"

#include <algorithm>

StatusCode TileTowerMonitorAlgorithm::initialize() {

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( m_caloTowerContainerKey.initialize() );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  int nL1Triggers = getNumberOfL1Triggers();

  m_towerEtaPhiGroups = buildToolMap<int>(m_tools, "TileTowerEtaPhi", nL1Triggers);
  m_towerEtaPhiDiffGroups = buildToolMap<int>(m_tools, "TileTowerEtaPhiDiff", nL1Triggers);
  m_towerEtGroups = buildToolMap<std::vector<int>>(m_tools, "TileTowerEt",
                                                   Tile::MAX_ROS, nL1Triggers);

  m_towerEnergyGroups = buildToolMap<std::vector<int>>(m_tools, "TileAllTowerEnergy",
                                                       Tile::MAX_ROS, nL1Triggers);

  //=== TileID
  ATH_CHECK( detStore()->retrieve(m_tileID) );

  return TileMonitorAlgorithm::initialize();
}


StatusCode TileTowerMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {


  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  int nTowers[MAX_PART] = { 0, 0, 0, 0, 0 };

  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  SG::ReadHandle<CaloTowerContainer> caloTowerContainer(m_caloTowerContainerKey, ctx);
  ATH_CHECK( caloTowerContainer.isValid() );

  const CaloTower* mostEnTower = *std::max_element(caloTowerContainer->begin(), caloTowerContainer->end(),
                                                   [] (const CaloTower* tower1, const CaloTower* tower2) {
                                                     return tower1->energy() < tower2->energy();
                                                   });


  double mostEnTowerPhi = 0.0;

  if (mostEnTower->energy() > 0.0) {
    mostEnTowerPhi = mostEnTower->phi();

    auto monEta = Monitored::Scalar<float>("eta", mostEnTower->eta());
    auto monPhi = Monitored::Scalar<float>("phi", mostEnTower->phi());
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_towerEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
    }

    int partition = getPartition(*(mostEnTower->cell_begin()), m_tileID);
    if (partition < PART_ALL) {
      auto monEt = Monitored::Scalar<float>("Et", mostEnTower->et());
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_towerEtGroups[partition][l1TriggerIdx]], monEt);
        fill(m_tools[m_towerEtGroups[PART_ALL][l1TriggerIdx]], monEt);
      }
    }
  }


  const CaloTower* correlTower = nullptr;
  double correlTowerEnergy = 0.0;

  for (const CaloTower* tower : *caloTowerContainer) {

    double energy = tower->energy();
    if (energy > 0.0) {
      if (tower->getNumberOfCells() > 0) {
        int partition = getPartition(*(tower->cell_begin()), m_tileID);
        if (partition < PART_ALL) {
          ++nTowers[partition];
          ++nTowers[PART_ALL];

          auto monEnergy = Monitored::Scalar<float>("energy", energy);
          for (int l1TriggerIdx : l1TriggersIndices) {
            fill(m_tools[m_towerEnergyGroups[partition][l1TriggerIdx]], monEnergy);
            fill(m_tools[m_towerEnergyGroups[PART_ALL][l1TriggerIdx]], monEnergy);
          }
        }
      }

      if (tower->phi() * mostEnTowerPhi < 0.0       // Opposite to the most energetic tower
          && tower->energy() > correlTowerEnergy) { // With maximum energy
        correlTower = tower;
        correlTowerEnergy = tower->energy();
      }


      ATH_MSG_VERBOSE( "Tower: nCells= " << tower->getNumberOfCells()
                       << ", Energy= " << energy
                       << ", Et()= " << tower->et()
                       << ", Eta= " << tower->eta()
                       << ", Phi= " << tower->phi() );

    }
  }

  ATH_MSG_DEBUG( "Total Number of towers in the event: " << nTowers[PART_ALL] );

  if (mostEnTower->energy() > 0.0 && correlTower) {
    float etaDelta = std::abs(correlTower->eta()) - std::abs(mostEnTower->eta());
    float phiDelta = std::abs(correlTower->phi() - mostEnTower->phi());

    auto monEta = Monitored::Scalar<float>("eta", etaDelta);
    auto monPhi = Monitored::Scalar<float>("phi", phiDelta);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_towerEtaPhiDiffGroups[l1TriggerIdx]], monEta, monPhi);
    }
  }


  fill("TileTowerMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
