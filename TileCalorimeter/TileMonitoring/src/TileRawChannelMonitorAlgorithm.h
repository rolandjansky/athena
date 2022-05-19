/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H
#define TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H

#include "TileCalibMonitorAlgorithm.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileEMScale.h"
#include "TileIdentifier/TileRawChannelUnit.h"

#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

/** @class TileRawChannelMonitorAlgorithm
 *  @brief Class for Tile raw channel based monitoring
 */

class TileRawChannelMonitorAlgorithm : public TileCalibMonitorAlgorithm {

  public:

    using  TileCalibMonitorAlgorithm::TileCalibMonitorAlgorithm;
    virtual ~TileRawChannelMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    bool isDmuHeaderError(uint32_t header) const;

    Gaudi::Property<float> m_minAmpForCorrectedTime{this,
        "MinAmpForCorrectedTime", 0.5, "Minimum amplitude to use channel for time correction calculation"};

    Gaudi::Property<float> m_minOfflineAmp{this,
        "MinOfflineAmp", 0.3, "Minimum offline amplitude for DSP check"};

    Gaudi::Property<int> m_calibUnit{this,
        "CalibUnit", TileRawChannelUnit::Invalid, ""};

    Gaudi::Property<bool> m_fillHistogramsForDSP{this,
        "fillHistogramsForDSP", false, "Create and fill histograms for raw channels from DSP"};

    Gaudi::Property<bool> m_fill2DHistograms{this,
        "fill2DHistograms", true, "Create and fill 2D histograms"};

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Input Tile digits container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "", "Input Tile raw channel container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_dspRawChannelContainerKey{this,
        "TileRawChannelContainerDSP", "TileRawChannelCnt", "Input DSP Tile raw channel container key"};

   /**
    * @brief Name of TileEMScale in condition store
    */
     SG::ReadCondHandleKey<TileEMScale> m_emScaleKey{this,
         "TileEMScale", "TileEMScale", "Input Tile EMS calibration constants"};

    std::vector<std::vector<int>> m_ampGroups;
    std::vector<std::vector<int>> m_timeGroups;
    std::vector<std::vector<int>> m_timeCorrGroups;

    // For 5 pF capacitor
    std::vector<std::vector<int>> m_ampOverQ5Groups;
    std::vector<std::vector<int>> m_ampVsQ5Groups;
    std::vector<std::vector<int>> m_time5Groups;
    std::vector<std::vector<int>> m_timeVsTime5Groups;

    // For 100 pF capacitor
    std::vector<std::vector<int>> m_ampOverQ100Groups;
    std::vector<std::vector<int>> m_ampVsQ100Groups;
    std::vector<std::vector<int>> m_time100Groups;
    std::vector<std::vector<int>> m_timeVsTime100Groups;

    std::vector<std::vector<int>> m_dspAmpGroups;
    std::vector<std::vector<int>> m_dspTimeGroups;
    std::vector<std::vector<int>> m_dspChi2Groups;
    std::vector<std::vector<int>> m_dspChi2VsAmpGroups;

    std::vector<std::vector<int>> m_ampDiffGroups;
    std::vector<std::vector<int>> m_timeDiffGroups;

    static const int MAX_DMU{16};
    TileRawChannelUnit::UNIT m_finalRawChannelUnit{TileRawChannelUnit::Invalid};

    double m_dac2Charge[2] = {};
    bool m_runTypeIsNotCIS{true};
};


#endif // TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H
