/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILECLUSTERMONITORALGORITHM_H
#define TILEMONITORING_TILECLUSTERMONITORALGORITHM_H

#include "TileMonitorAlgorithm.h"
#include "TileConditions/TileCablingSvc.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileID;

/** @class TileClusterMonitorAlgorithm
 *  @brief Class for Tile Tower based monitoring
 */

class TileClusterMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileClusterMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<float> m_energyThreshold{this,
        "EnergyThreshold", 500.0F, "Energy threshold in MeV"};

    Gaudi::Property<bool> m_fillTimingHistograms{this,
       "fillTimingHistograms", false, "Fill timing histograms from cells associated with clusters"};

    Gaudi::Property<float> m_cellEnergyThresholdForTiming{this,
       "CellEnergyThresholdForTiming", 1500.0F, "Energy threshold in MeV"};

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_caloClusterContainerKey{this,
        "CaloClusterContainer", "TileTopoCluster", "Calo cluster container name"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
                "TileCablingSvc", "TileCablingSvc", "Tile cabling service"};

    std::vector<int> m_clusterEtaPhiGroups;
    std::vector<int> m_clusterEtGroups;
    std::vector<int> m_clusterNCellsGroups;
    std::vector<int> m_allClusterEnergyGroups;
    std::vector<int> m_allClusterEtaPhiGroups;
    std::vector<int> m_allClusterEneEtaPhiGroups;
    std::vector<int> m_nClustersGroups;
    std::vector<int> m_clusterSumPxGroups;
    std::vector<int> m_clusterSumPyGroups;
    std::vector<int> m_clusterSumEtGroups;
    std::vector<int> m_clusterTimeDiffGroups;
    std::vector<int> m_clusterEneDiffGroups;
    std::vector<int> m_clusterEtaPhiDiffGroups;
    std::vector<int> m_partitionTimeLBGroups;

    std::vector<std::vector<int>> m_clusterEnergyGroups;

    const TileCablingService* m_cabling{nullptr};
    const TileID* m_tileID{nullptr};
};


#endif // TILEMONITORING_TILECLUSTERMONITORALGORITHM_H
