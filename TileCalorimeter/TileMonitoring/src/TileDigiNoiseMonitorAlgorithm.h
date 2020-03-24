/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEDIGINOISEMONITORALGORITHM_H
#define TILEMONITORING_TILEDIGINOISEMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileDigitsContainer.h"
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

/** @class TileDigiNoiseMonitorAlgorithm
 *  @brief Class for Tile digi noise based monitoring
 */

class TileDigiNoiseMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using  AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileDigiNoiseMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    Gaudi::Property<bool> m_checkDCS{this, "CheckDCS", false, "Check Tile DCS status"};
    Gaudi::Property<std::vector<unsigned int>> m_triggerTypes{this,
        "TriggerTypes", {}, "Given trigger types only events with these TT will be used, otherwise all"};

    Gaudi::Property<bool> m_fillPedestalDifference{this,
        "fillPedestalDifference", true, "Fill pedestal difference between monitored pedestal and one from DB"};

    Gaudi::Property<std::vector<int>> m_fragIDsToIgnoreDMUerrors{this,
        "FragIDsToIgnoreDMUErrors", {}, "List of Tile frag IDs for which ignore DMU errors"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    /**
     * @brief Name of TileDCSState object in condition store
     */
    SG::ReadCondHandleKey<TileDCSState> m_DCSStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Input Tile digits container key"};

    /**
     * @brief Name of TileBadChannels in condition store
     */
    SG::ReadCondHandleKey<TileBadChannels> m_badChannelsKey{this,
        "TileBadChannels", "TileBadChannels", "Input Tile bad channel status"};

    ToolHandle<TileCondToolNoiseSample> m_tileCondToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    std::vector<std::vector<int>> m_pedGroups;
    std::vector<std::vector<int>> m_hfnGroups;
};


#endif // TILEMONITORING_TILEDIGINOISEMONITORALGORITHM_H
