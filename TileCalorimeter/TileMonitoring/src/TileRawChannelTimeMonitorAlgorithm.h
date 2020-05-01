/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERAWCHANNELTIMEMONITORALGORITHM_H
#define TILEMONITORING_TILERAWCHANNELTIMEMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCablingSvc.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileHWID;
class TileCablingService;

#include <array>

/** @class TileRawChannelTimeMonitorAlgorithm
 *  @brief Class for Tile raw channel time based monitoring
 */

class TileRawChannelTimeMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using  AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileRawChannelTimeMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<bool> m_checkDCS{this, "CheckDCS", false, "Check Tile DCS status"};
    Gaudi::Property<std::vector<double>> m_energyThresholds{this,
         "EnergyThresholds", {10, 40}, "Energy thresholds [LG, HG]"};

    Gaudi::Property<std::vector<double>> m_partitionTimeCorrection{this,
         "PartitionTimeCorrections", {-15.18, -15.37, 47.65, 47.42},
         "Partition time corrections [LBA, LBC, EBA, EBC]"};

    Gaudi::Property<std::vector<std::pair<int, int>>> m_partitionTimeDifferencePairs{this,
         "PartitionTimeDiffferncePairs", {}, "Partition (ROS - 1) pairs to monitor time differnce"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TileRawChannelFit", "Input Tile raw channel container key"};

    /**
     * @brief Name of TileDCSState object in condition store
     */
    SG::ReadCondHandleKey<TileDCSState> m_DCSStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    std::vector<int> m_timeGroups;
    std::vector<int> m_timeLBGroups;
    std::vector<int> m_timeDiffLBGroups;
    std::vector<std::vector<std::vector<int>>> m_digiTimeLBGroups;
};


#endif // TILEMONITORING_TILERAWCHANNELTIMEMONITORALGORITHM_H
