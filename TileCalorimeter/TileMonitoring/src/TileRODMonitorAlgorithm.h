/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERODMONITORALGORITHM_H
#define TILEMONITORING_TILERODMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileBadChannels.h"
#include "TileConditions/TileEMScale.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileMonitorAlgorithm.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileHWID;
class TileCablingService;

/** @class TileRODMonitorAlgorithm
 *  @brief Class for Tile digi noise based monitoring
 */

class TileRODMonitorAlgorithm : public TileMonitorAlgorithm {

  public:

    using TileMonitorAlgorithm::TileMonitorAlgorithm;
    virtual ~TileRODMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<bool> m_checkDCS{this, "CheckDCS", false, "Check Tile DCS status"};
    Gaudi::Property<std::vector<unsigned int>> m_triggerTypes{this,
        "TriggerTypes", {}, "Given trigger types only events with these TT will be used, otherwise all"};

    Gaudi::Property<std::vector<int>> m_fragIDsToIgnoreDMUerrors{this,
        "FragIDsToIgnoreDMUErrors", {}, "List of Tile frag IDs for which ignore DMU errors"};

    Gaudi::Property<unsigned int> m_comparisonUnit{this,
        "ComparisonUnit", TileRawChannelUnit::MegaElectronVolts, "Units to compare Tile raw channel containers"};

    Gaudi::Property<float> m_energyThreshold{this,
        "EnergyThreshold", 300.0F, "Energy threshold in MeV"};

    Gaudi::Property<std::vector<float>> m_timeRange{this,
        "TimeRange", {-65.0F, 65.0F}, "Time range to be monitored, default: (-65,65)"};

    Gaudi::Property<unsigned int> m_nROBs{this,
        "NumberOfROBFragmets", MAX_TILE_ROBS, "Number of Tile ROB fragments"};

    Gaudi::Property<bool> m_fillRODfragSizeHistograms{this,
        "fillRODFragmentSizeHistograms", true, "Fill summary histograms with ROD fragment size"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    /**
     * @brief Name of TileDCSState object in condition store
     */
    SG::ReadCondHandleKey<TileDCSState> m_DCSStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TileRawChannelOpt2", "Input Tile reference raw channel container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_dspRawChannelContainerKey{this,
        "TileRawChannelContainerDSP", "TileRawChannelCnt", "Input Tile DSP raw channel container key"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    /**
     * @brief Name of TileEMScale in condition store
     */
    SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
        "TileEMScale", "TileEMScale", "Input Tile EMS calibration constants"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    /**
     * @brief Name of ROB data provider service
     */
    ServiceHandle<IROBDataProviderSvc> m_robSvc{this,
        "ROBDataProviderSvc", "ROBDataProviderSvc", "The ROB data provider service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};
    TileRawChannelUnit::UNIT m_finalRawChannelUnit{TileRawChannelUnit::Invalid};

    std::vector<int> m_rodFragSizeMapGroups;
    std::vector<int> m_rodFragSizeLBGroups;
    std::vector<std::vector<std::vector<int>>> m_rodFragSizeGroups;

    std::vector<std::vector<int>> m_energyDiffGroups;
    std::vector<std::vector<int>> m_energyDiffVsTimeGroups;
    std::vector<std::vector<int>> m_energyDiffVsEnergyGroups;
    std::vector<std::vector<int>> m_timeDiffGroups;
    std::vector<std::vector<int>> m_timeDiffVsTimeGroups;
    std::vector<std::vector<int>> m_timeDiffVsEnergyGroups;
    std::vector<std::vector<int>> m_dspChanTimeGroups;

    std::vector<uint32_t> m_tileRobIds;
    static constexpr int MAX_TILE_ROBS = 32;
};


#endif // TILEMONITORING_TILERODMONITORALGORITHM_H
