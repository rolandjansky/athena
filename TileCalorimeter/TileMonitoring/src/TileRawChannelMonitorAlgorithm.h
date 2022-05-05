/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H
#define TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H

#include "TileEvent/TileDQstatus.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileEMScale.h"
#include "TileIdentifier/TileRawChannelUnit.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileHWID;
class TileInfo;

/** @class TileRawChannelMonitorAlgorithm
 *  @brief Class for Tile raw channel based monitoring
 */

class TileRawChannelMonitorAlgorithm : public AthMonitorAlgorithm {

  public:

    using  AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileRawChannelMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    enum RunType {
       UNKNOWN_RUN  = 0, // expect monogain
       PHYS_RUN = 1, // expect monogain
       LAS_RUN  = 2, // expect monogain
       LED_RUN  = 3, // expect monogain
       PED_RUN  = 4, // expect bigain
       CIS_RUN  = 8, // expect bigain
       MONO_RUN = 9, // expect monogain
       CIS_RAMP_RUN = 10 // expect monogain
    };

    bool isDmuHeaderError(uint32_t header) const;

   /*
    * Function to check that the DMU header format is correct
    * bit_31 of the DMU header must be 1 and
    * bit_17 of the DMU header must be 0
    * Return true in case of error
    */
    bool isHeaderFormatError(uint32_t header) const {
      return (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0)) ? false : true;
    };

   /*
    * Function to check that the DMU header parity is correct
    * Parity of the DMU header should be odd
    * Return true in case of error
    */
    bool isHeaderParityError(uint32_t header) const {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i) {
        parity += ((header >> i) & 0x1);
      }
      return ((parity % 2) == 1) ? false : true;
    };

    Gaudi::Property<std::string> m_tileInfoName{this,
        "TileInfo", "TileInfo", "Name of TileInfo object in Detector Store"};

    Gaudi::Property<std::vector<int>> m_fragIDsToIgnoreDMUerrors{this,
        "FragIDsToIgnoreDMUErrors", {}, "List of Tile frag IDs for which ignore DMU errors"};

    Gaudi::Property<std::vector<int>> m_fragIDsDemonstrators{this,
        "FragIDsDemonstrators", {}, "List of Tile frag IDs of Demonstrators"};

    Gaudi::Property<int> m_runType{this,
        "RunType", 0, "Run type: 1 - phys, 2 - las, 4 - ped, 8 - cis, 9- mono"};

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

    SG::ReadHandleKey<TileDQstatus> m_dqStatusKey{this,
        "TileDQstatus", "TileDQstatus", "Tile DQ status name"};

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

   /**
    * @brief Name of Tile cabling service
    */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

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

    // TileInfo
    const TileInfo* m_tileInfo{nullptr};
    float m_ADCmaxMinusEpsilon;
    double m_dac2Charge[2] = {};
    bool m_runTypeIsNotCIS{true};
};


#endif // TILEMONITORING_TILERAWCHANNELMONITORALGORITHM_H
