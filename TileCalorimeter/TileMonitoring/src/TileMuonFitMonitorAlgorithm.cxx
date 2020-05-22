/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMuonFitMonitorAlgorithm.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "CaloIdentifier/TileID.h"

#include "StoreGate/ReadHandle.h"

#include <algorithm>

StatusCode TileMuonFitMonitorAlgorithm::initialize() {

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( m_cosmicMuonContainerKey.initialize() );

  using namespace Monitored;
  int nL1Triggers = getNumberOfL1Triggers();

  m_nMuGroups = buildToolMap<int>(m_tools, "TileMuonFitNum", nL1Triggers);
  m_muEneGroups = buildToolMap<int>(m_tools, "TileMuonFitEnergy", nL1Triggers);
  m_muTimeGroups = buildToolMap<int>(m_tools, "TileMuonFitTime", nL1Triggers);
  m_muPathGroups = buildToolMap<int>(m_tools, "TileMuonFitPath", nL1Triggers);
  m_muNCellsGroups = buildToolMap<int>(m_tools, "TileMuonFitNCells", nL1Triggers);
  m_muPositionGroups = buildToolMap<int>(m_tools, "TileMuonFitPosition", nL1Triggers);
  m_muPositionTimeGroups = buildToolMap<int>(m_tools, "TileMuonFitPositionTime", nL1Triggers);
  m_muEneDensityGroups = buildToolMap<int>(m_tools, "TileMuonFitEneDensity", nL1Triggers);
  m_muDirectionPhiGroups = buildToolMap<int>(m_tools, "TileMuonFitDirectionPhi", nL1Triggers);
  m_muDirectionThetaGroups = buildToolMap<int>(m_tools, "TileMuonFitDirectionTheta", nL1Triggers);
  m_muPosDirectionThetaGroups = buildToolMap<int>(m_tools, "TileMuonFitPosDirectionTheta", nL1Triggers);
  m_muPartitionTimeGroups = buildToolMap<int>(m_tools, "TileMuonFitPartitionTime", nL1Triggers);

  //=== TileID
  ATH_CHECK( detStore()->retrieve(m_tileID) );

  return TileMonitorAlgorithm::initialize();
}


StatusCode TileMuonFitMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {


  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  std::vector<int> muNCells;
  std::vector<float> muTime;
  std::vector<float> muFullPath;
  std::vector<float> muFullEnergy;
  std::vector<float> muEnergyDensity;
  std::vector<float> muPositionX;
  std::vector<float> muPositionZ;
  std::vector<float> muDirectionPhi;
  std::vector<float> muDirectionTheta;
  std::vector<float> muPosTime;
  std::vector<float> muPosTimeZ;
  std::vector<float> muPartition;
  std::vector<float> muPartitionTime;

  SG::ReadHandle<TileCosmicMuonContainer> cosmicMuonContainer(m_cosmicMuonContainerKey, ctx);
  ATH_CHECK( cosmicMuonContainer.isValid() );

  for (const TileCosmicMuon* cosmicMuon : *cosmicMuonContainer) {
    int muTrackNCells = cosmicMuon->GetTrackNCells();

    if (muTrackNCells > 2) { // we want the number of cells in the track to be more than 2

      muNCells.push_back(muTrackNCells);
      muTime.push_back(cosmicMuon->GetTime());
      muPositionX.push_back(cosmicMuon->GetPositionX());
      muPositionZ.push_back(cosmicMuon->GetPositionZ());
      muDirectionPhi.push_back(std::sin(cosmicMuon->GetDirectionPhi()));
      muDirectionTheta.push_back(std::sin(cosmicMuon->GetDirectionTheta()));

      float fullPath = cosmicMuon->GetFullPath();
      float fullEnergy = cosmicMuon->GetFullEnergy();
      muFullPath.push_back(fullPath);
      muFullEnergy.push_back(fullEnergy);

      if (fullPath > 0.) {
        float eneDensity = fullEnergy / fullPath;
        muEnergyDensity.push_back(eneDensity);

        if ((fullPath < 10000.) && (eneDensity > 1.05) && (eneDensity < 7.5)) {
          muPosTime.push_back(cosmicMuon->GetTime());
          muPosTimeZ.push_back(cosmicMuon->GetPositionZ());

          // Find Tile cells partitions
          std::set<int> partitions;
          for (int cellIdx = 0; cellIdx < muTrackNCells; ++cellIdx) {
            IdentifierHash hash = cosmicMuon->GetTrackCellHash(cellIdx);
            int partition = getPartition(hash, m_tileID);
            if (partition != MAX_PART) {
              partitions.insert(partition);
            }
          }

          if (partitions.size() == 1) { // Check that all Tile cells belong to the same partiton
            int partition = *partitions.begin();
            muPartition.push_back(partition);
            muPartitionTime.push_back(cosmicMuon->GetTime());
          }

        }
      }
    }
  }


  if (!muTime.empty()) {
    auto nMuons = Monitored::Scalar<float>("nMuons", muTime.size());
    auto monNCells = Monitored::Collection("nCells", muNCells);
    auto monTime = Monitored::Collection("time", muTime);
    auto monPath = Monitored::Collection("path", muFullPath);
    auto monEnergy = Monitored::Collection("energy", muFullEnergy);
    auto monPositionX = Monitored::Collection("xPosition", muPositionX);
    auto monPositionZ = Monitored::Collection("zPosition", muPositionZ);
    auto monDirectionPhi = Monitored::Collection("phi", muDirectionPhi);
    auto monDirectionTheta = Monitored::Collection("theta", muDirectionTheta);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_nMuGroups[l1TriggerIdx]], nMuons);
      fill(m_tools[m_muTimeGroups[l1TriggerIdx]], monTime);
      fill(m_tools[m_muPathGroups[l1TriggerIdx]], monPath);
      fill(m_tools[m_muEneGroups[l1TriggerIdx]], monEnergy);
      fill(m_tools[m_muNCellsGroups[l1TriggerIdx]], monNCells);
      fill(m_tools[m_muDirectionPhiGroups[l1TriggerIdx]], monDirectionPhi);
      fill(m_tools[m_muDirectionThetaGroups[l1TriggerIdx]], monDirectionTheta);
      fill(m_tools[m_muPositionGroups[l1TriggerIdx]], monPositionZ, monPositionX);
      fill(m_tools[m_muPosDirectionThetaGroups[l1TriggerIdx]], monPositionZ, monDirectionTheta);
    }
  }

  if (!muPosTime.empty()) {
    auto monTime = Monitored::Collection("time", muPosTime);
    auto monPositionZ = Monitored::Collection("zPosition", muPosTimeZ);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_muPositionTimeGroups[l1TriggerIdx]], monPositionZ, monTime);
    }
  }

  if (!muPartitionTime.empty()) {
    auto monTime = Monitored::Collection("time", muPartitionTime);
    auto monPartition = Monitored::Collection("partition", muPartition);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_muPartitionTimeGroups[l1TriggerIdx]], monPartition, monTime);
    }
  }

  if (!muEnergyDensity.empty()) {
    auto monEneDensity = Monitored::Collection("eneDensity", muEnergyDensity);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_muEneDensityGroups[l1TriggerIdx]], monEneDensity);
    }
  }

  fill("TileMuonFitMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
