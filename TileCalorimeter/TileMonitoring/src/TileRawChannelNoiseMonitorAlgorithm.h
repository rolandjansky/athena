/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERAWCHANNELNOISEMONITORALGORITHM_H
#define TILEMONITORING_TILERAWCHANNELNOISEMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCablingSvc.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileHWID;
class TileCablingService;

/** @class TileRawChannelNoiseMonitorAlgorithm
 *  @brief Class for Tile digi noise based monitoring
 */

class TileRawChannelNoiseMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using  AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileRawChannelNoiseMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<int> m_gain{this, "Gain", 1, "Tile channel gain to monitor"};
    Gaudi::Property<bool> m_checkDCS{this, "CheckDCS", false, "Check Tile DCS status"};
    Gaudi::Property<std::vector<unsigned int>> m_triggerTypes{this,
        "TriggerTypes", {}, "Given trigger types only events with these TT will be used, otherwise all"};

    Gaudi::Property<std::vector<int>> m_fragIDsToIgnoreDMUerrors{this,
         "FragIDsToIgnoreDMUErrors", {}, "List of Tile frag IDs for which ignore DMU errors"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    /**
     * @brief Name of TileDCSState object in condition store
     */
    SG::ReadCondHandleKey<TileDCSState> m_DCSStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TileRawChannelOpt2", "Input Tile raw channel container key"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    ToolHandle<TileCondToolEmscale> m_tileCondToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    std::vector<std::vector<std::vector<int>>> m_ampGroups;
};


#endif // TILEMONITORING_TILERAWCHANNELNOISEMONITORALGORITHM_H
