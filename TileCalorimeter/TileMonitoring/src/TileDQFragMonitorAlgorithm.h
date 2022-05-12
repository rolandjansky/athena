/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEDQFRAGMONITORALGORITHM_H
#define TILEMONITORING_TILEDQFRAGMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileDCSState.h"
#include "TileConditions/TileCablingSvc.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"


class TileHWID;
class TileCablingService;
class TileInfo;


/** @class TileDQFragMonitorAlgorithm
 *  @brief Class for Tile DQFrag based monitoring
 */

class TileDQFragMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileDQFragMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    enum TileFragStatus {ALL_OK = 0, CRC_ERR = 1, ALL_FF = 0x10, ALL_00 = 0x20, NO_FRAG = 0x40, NO_ROB = 0x80};

    enum TileDigiErrors {OK, HEADER_FORM, HEADER_PAR, MEMO_PAR, FE_CRC, ROD_CRC, BCID,
                         SAMPLE_FORM, SAMPLE_PAR, DOUBLE_STB, SINGLE_STB, GLOBAL_CRC,
                         DUMMY_FRAG, NO_RECO_FRAG, MASKED, ALL_M_BAD_DCS, ANY_CH_BAD_HV,
                         MAX_DIGI_ERROR};

    void setDigiError(std::vector<int>& dmus, std::vector<int>& errors, int dmu, int error) const;
    void clearDigiError(std::vector<int>& dmus, std::vector<int>& errors) const;

    Gaudi::Property<bool> m_checkDCS{this, "CheckDCS", false, "Check Tile DCS status"};
    Gaudi::Property<bool> m_skipMasked{this, "SkipMasked", true, "Ignore masked Tile channels"};
    Gaudi::Property<bool> m_skipGapCells{this, "SkipGapCells", true, "Ignore E1-E4 cells in negative amplitude detection"};
    Gaudi::Property<bool> m_ignoreNoRecoFragError{this, "IgnoreNoRecoFragError", false, "Ignore NO_RECO_FRAG error"};

    Gaudi::Property<float> m_negativeAmplitudeCutHG{this,
        "NegativeAmplitudeCutHG", -200.0, "Monitor Tile channels with negative amplitude in HG below this cut"};

    Gaudi::Property<float> m_negativeAmplitudeCutLG{this,
        "NegativeAmplitudeCutLG", -15.0, "Monitor Tile channels with negative amplitude in LG below this cut"};

    Gaudi::Property<float> m_qualityCut{this,
        "QualityCut", 254.0, "Monitor Tile channels reconstructed with quality below this cut"};


    ToolHandle<ITileBadChanTool> m_tileBadChanTool{this,
        "TileBadChanTool", "TileBadChanTool", "Tile bad channel tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

    /**
     * @brief Name of TileDCSState object in condition store
     */
    SG::ReadCondHandleKey<TileDCSState> m_DCSStateKey{this,
        "TileDCS", "TileDCS", "Input Tile DCS status"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "TileRawChannelCnt", "Input Tile raw channel container key"};

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Input Tile digits container key"};

    SG::ReadDecorHandleKey<xAOD::EventInfo> m_eventInfoTileStatusKey{this, "EventInfoTileStatus",
                                                                    "EventInfo.TileStatus",
                                                                    "Aid to MT scheduling, do not change"};

    std::vector<std::vector<int>> m_errorsGroups;
    std::vector<std::vector<int>> m_errorsVsLBGroups;

    std::vector<int> m_badChannelJumpGroups;
    std::vector<int> m_badChannelJumpNotMaskGroups;

    std::vector<int> m_badChannelNegGroups;
    std::vector<int> m_badChannelNegNotMaskGroups;

    std::vector<int> m_badPulseQualityGroups;

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    static const int MAX_DMU{16};
    static const int MAX_CORRUPTED_ERROR{13};

    // TileInfo
    std::string m_infoName = "TileInfo";
    const TileInfo* m_tileInfo{nullptr};
    float m_ADCmaxMinusEps;
    float m_ADCmaskValueMinusEps; //!< indicates channels which were masked in background dataset

};


#endif // TILEMONITORING_TILEDQFRAGMONITORALGORITHM_H
