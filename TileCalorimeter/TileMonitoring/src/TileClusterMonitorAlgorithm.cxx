/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileClusterMonitorAlgorithm.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "CaloIdentifier/TileID.h"

#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;
using Athena::Units::ns;

StatusCode TileClusterMonitorAlgorithm::initialize() {

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( m_caloClusterContainerKey.initialize() );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  int nL1Triggers = getNumberOfL1Triggers();

  m_clusterEtaPhiGroups = buildToolMap<int>(m_tools, "TileClusterEtaPhi", nL1Triggers);
  m_clusterEtGroups = buildToolMap<int>(m_tools, "TileClusterEt", nL1Triggers);
  m_clusterNCellsGroups = buildToolMap<int>(m_tools, "TileClusterNCells", nL1Triggers);
  m_allClusterEnergyGroups = buildToolMap<int>(m_tools, "TileAllClusterEnergy", nL1Triggers);
  m_allClusterEtaPhiGroups = buildToolMap<int>(m_tools, "TileAllClusterEtaPhi", nL1Triggers);
  m_allClusterEneEtaPhiGroups = buildToolMap<int>(m_tools, "TileAllClusterEneEtaPhi", nL1Triggers);
  m_nClustersGroups = buildToolMap<int>(m_tools, "TileNClusters", nL1Triggers);
  m_clusterSumPxGroups = buildToolMap<int>(m_tools, "TileClusterSumPx", nL1Triggers);
  m_clusterSumPyGroups = buildToolMap<int>(m_tools, "TileClusterSumPy", nL1Triggers);
  m_clusterSumEtGroups = buildToolMap<int>(m_tools, "TileClusterSumEt", nL1Triggers);
  m_clusterTimeDiffGroups = buildToolMap<int>(m_tools, "TileClusterTimeDiff", nL1Triggers);
  m_clusterEneDiffGroups = buildToolMap<int>(m_tools, "TileClusterEneDiff", nL1Triggers);
  m_clusterEtaPhiDiffGroups = buildToolMap<int>(m_tools, "TileClusterEtaPhiDiff", nL1Triggers);
  m_clusterEnergyGroups = buildToolMap<std::vector<int>>(m_tools, "TileClusterEnergy",
                                                         Tile::MAX_ROS, nL1Triggers);

  if (m_fillTimingHistograms) {
    m_partitionTimeLBGroups = buildToolMap<int>(m_tools, "TilePartitionTimeLB", MAX_PART);
  }

  //=== TileID
  ATH_CHECK( detStore()->retrieve(m_tileID) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  return TileMonitorAlgorithm::initialize();
}


StatusCode TileClusterMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {


    // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();


  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  SG::ReadHandle<xAOD::CaloClusterContainer> caloClusterContainer(m_caloClusterContainerKey, ctx);
  ATH_CHECK( caloClusterContainer.isValid() );

  int nClusters = caloClusterContainer->size();
  ATH_MSG_DEBUG( "Number of clusters in the event: " << nClusters );

  auto monNClusters = Monitored::Scalar<int>("nClusters", nClusters);
  for (int l1TriggerIdx : l1TriggersIndices) {
    fill(m_tools[m_nClustersGroups[l1TriggerIdx]], monNClusters);
  }

  if (nClusters == 0) {
    return StatusCode::SUCCESS;
  }

  const xAOD::CaloCluster* mostEnCluster = *std::max_element(
     caloClusterContainer->begin(), caloClusterContainer->end(),
     [] (const xAOD::CaloCluster* cluster1, const xAOD::CaloCluster* cluster2) {
       return cluster1->e() < cluster2->e();
     });

  const CaloCell* mostEnCell = nullptr;
  double mostEnClusterPhi = 0.0;

  if (mostEnCluster->e() > 0.0) {
    mostEnClusterPhi = mostEnCluster->phi();

    auto monEta = Monitored::Scalar<float>("eta", mostEnCluster->eta());
    auto monPhi = Monitored::Scalar<float>("phi", mostEnCluster->phi());
    auto monEnergy = Monitored::Scalar<float>("energy", mostEnCluster->e());
    auto monEt = Monitored::Scalar<float>("et", mostEnCluster->et());
    auto monNCells = Monitored::Scalar<int>("nCells", mostEnCluster->size());
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_clusterEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
      fill(m_tools[m_clusterEtGroups[l1TriggerIdx]], monEt);
      fill(m_tools[m_clusterNCellsGroups[l1TriggerIdx]], monNCells);
    }

    if (mostEnCluster->getCellLinks()) {
      mostEnCell = *std::max_element(mostEnCluster->begin(), mostEnCluster->end(),
                                     [] (const CaloCell* cell1, const CaloCell* cell2) {
                                       return cell1->energy() < cell2->energy();
                                     });

      int partition = getPartition(mostEnCell, m_tileID);
      if (partition < PART_ALL) {
        auto monEt = Monitored::Scalar<float>("Et", mostEnCluster->et());
        for (int l1TriggerIdx : l1TriggersIndices) {
          fill(m_tools[m_clusterEnergyGroups[partition][l1TriggerIdx]], monEnergy);
          fill(m_tools[m_clusterEnergyGroups[PART_ALL][l1TriggerIdx]], monEnergy);
        }
      }
    }
  }


  float sumPx = 0.;
  float sumPy = 0.;

  const xAOD::CaloCluster* correlCluster = nullptr;
  double correlClusterEnergy = 0.0;

  for (const xAOD::CaloCluster* cluster : *caloClusterContainer) {

    float energy = cluster->e();
    float phi = cluster->phi();
    float pt = cluster->pt();
    float px = pt * std::cos(phi);
    float py = pt * std::sin(phi);

    sumPx += px;
    sumPy += py;

    if (phi * mostEnClusterPhi < 0.0       // Opposite to the most energetic cluster
        && energy > correlClusterEnergy) { // With maximum energy
      correlCluster = cluster;
      correlClusterEnergy = energy;
    }

    ATH_MSG_VERBOSE( "Cluster: nCells= " << cluster->size()
                     << ", Energy= " << energy
                     << ", Et()= " << cluster->et()
                     << ", Eta= " << cluster->eta()
                     << ", Phi= " << cluster->phi() );

  }

  auto monEta = Monitored::Collection("eta", *caloClusterContainer,
                                      [] (const xAOD::CaloCluster* cluster) {
                                        return cluster->eta();
                                      });

  auto monPhi = Monitored::Collection("phi", *caloClusterContainer,
                                      [] (const xAOD::CaloCluster* cluster) {
                                        return cluster->phi();
                                      });

  auto monEnergy = Monitored::Collection("energy", *caloClusterContainer,
                                         [] (const xAOD::CaloCluster* cluster) {
                                           return cluster->e();
                                         });

  float sumEt = sqrt(sumPx * sumPx + sumPy * sumPy);
  auto monSumPx = Monitored::Scalar<float>("sumPx", sumPx);
  auto monSumPy = Monitored::Scalar<float>("sumPy", sumPy);
  auto monSumEt = Monitored::Scalar<float>("sumEt", sumEt);

  for (int l1TriggerIdx : l1TriggersIndices) {
    fill(m_tools[m_allClusterEnergyGroups[l1TriggerIdx]], monEnergy);
    fill(m_tools[m_allClusterEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
    fill(m_tools[m_allClusterEneEtaPhiGroups[l1TriggerIdx]], monEta, monPhi, monEnergy);
    fill(m_tools[m_clusterSumPxGroups[l1TriggerIdx]], monSumPx);
    fill(m_tools[m_clusterSumPyGroups[l1TriggerIdx]], monSumPy);
    fill(m_tools[m_clusterSumEtGroups[l1TriggerIdx]], monSumEt);
  }



  if (mostEnCluster->e() > 0.0 && correlCluster) {

    float energyDiff = (mostEnClusterPhi > 0.0) ? mostEnCluster->e() - correlCluster->e()
                                                : correlCluster->e() - mostEnCluster->e();

    float etaDelta = std::abs(correlCluster->eta()) - std::abs(mostEnCluster->eta());
    float phiDelta = std::abs(correlCluster->phi() - mostEnCluster->phi());

    auto monEta = Monitored::Scalar<float>("eta", etaDelta);
    auto monPhi = Monitored::Scalar<float>("phi", phiDelta);
    auto monEnergyDiff = Monitored::Scalar<float>("energyDiff", energyDiff);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_clusterEneDiffGroups[l1TriggerIdx]], monEnergyDiff);
      fill(m_tools[m_clusterEtaPhiDiffGroups[l1TriggerIdx]], monEta, monPhi);
    }

    const CaloCell* correlCell = nullptr;
    if (correlCluster->getCellLinks()) {
      correlCell = *std::max_element( correlCluster->begin(), correlCluster->end(),
                                      [] (const CaloCell* cell1, const CaloCell* cell2) {
                                        return cell1->energy() < cell2->energy();
                                      });
    }

    if (mostEnCell && correlCell) {
      float timeDiff = mostEnCell->time() - correlCell->time();
      auto monTimeDiff = Monitored::Scalar<float>("timeDiff", timeDiff);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_clusterTimeDiffGroups[l1TriggerIdx]], monTimeDiff);
      }
    }

  }


  if (m_fillTimingHistograms
      && (eventInfo->errorState(xAOD::EventInfo::Tile) != xAOD::EventInfo::Error)) {

    std::set<Identifier> usedCells;
    std::vector<float> partitionTime[MAX_PART];

    for (const xAOD::CaloCluster* cluster : *caloClusterContainer) {
      if (cluster->getCellLinks()) {
        for (const CaloCell* cell : *cluster) {

          Identifier id = cell->ID();

          if (cell->badcell()
              || cell->energy() < m_cellEnergyThresholdForTiming
              || usedCells.find(id) != usedCells.end() ) continue;

          usedCells.insert(id);

          int sample = m_tileID->sample(id);
          bool single_PMT_scin = (sample == TileID::SAMP_E);
          bool single_PMT_C10 = (m_tileID->section(id) == TileID::GAPDET
                                 && sample == TileID::SAMP_C
                                 && (!m_cabling->C10_connected(m_tileID->module(id))) );

          // distinguish cells with one or two PMTs
          bool single_PMT = single_PMT_C10 || single_PMT_scin;

          if (!single_PMT && !(sample == TileID::SAMP_D && m_tileID->tower(id) == 0)) {
            int partition = getPartition(id, m_tileID);
            if (partition < MAX_PART) {
              partitionTime[partition].push_back(cell->time());
            }
          }
        }
      }
    }

    auto lumiBlock = Monitored::Scalar<int>("lumiBlock", eventInfo->lumiBlock());
    for (int partition = 0; partition < MAX_PART; ++partition) {
      if (!partitionTime[partition].empty()) {
        auto monTime = Monitored::Collection("time", partitionTime[partition]);
        fill(m_tools[m_partitionTimeLBGroups[partition]], lumiBlock, monTime);
        fill(m_tools[m_partitionTimeLBGroups[PART_ALL]], lumiBlock, monTime);
      }
    }
  }


  fill("TileClusterMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
