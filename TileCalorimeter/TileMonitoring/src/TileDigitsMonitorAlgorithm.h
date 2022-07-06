/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILEDIGITSMONITORALGORITHM_H
#define TILEMONITORING_TILEDIGITSMONITORALGORITHM_H

#include "TileCalibMonitorAlgorithm.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

/** @class TileDigitsMonitorAlgorithm
 *  @brief Class for Tile digi noise based monitoring
 */

class TileDigitsMonitorAlgorithm : public TileCalibMonitorAlgorithm {

  public:

    using  TileCalibMonitorAlgorithm::TileCalibMonitorAlgorithm;
    virtual ~TileDigitsMonitorAlgorithm() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  private:

    enum TileFragStatus {ALL_OK = 0, ALL_FF = 0x10, ALL_00 = 0x20, NO_FRAG = 0x40, NO_ROB = 0x80};

    enum TileDigiErrors {OK, HEADER_FORM, HEADER_PAR, MEMO_PAR, SINGLE_STB, DOUBLE_STB,
                         DUMMY_FRAG, NO_RECO_FRAG};

    void checkCRC(bool isCalibMode, uint32_t crc32, uint32_t crcMask, int nDMU, int ros, int drawer) const;
    StatusCode checkROD_CRC(const TileDQstatus* dqStatus, const EventContext& ctx) const;
    bool checkCorruptedData(const std::vector<std::reference_wrapper<const std::vector<uint32_t>>>& headerWordsAll,
                            uint32_t fragStatus, int ros, int drawer, std::vector<std::vector<bool>>& corruptedData) const;
    void checkBCID(const std::vector<std::reference_wrapper<const std::vector<uint32_t>>>& headerWordsAll,
                   uint32_t rodBCID, int ros, int drawer) const;

    Gaudi::Property<bool> m_fillPedestalDifference{this,
        "fillPedestalDifference", true, "Fill pedestal difference between monitored pedestal and one from DB"};

    Gaudi::Property<bool> m_fillErrorsHistograms{this,
        "fillErrorsHistograms", true, "Fill histograms per channel/gain with DMU, CRC, BCID errors"};

    Gaudi::Property<bool> m_fillPedestalHistograms{this,
        "fillPedestalHistograms", true, "Fill histograms per channel/gain with pedestals"};

    Gaudi::Property<bool> m_fillHighFrequencyNoiseHistograms{this,
        "fillHighFrequencyNoiseHistograms", true, "Fill histograms per channel/gain with HFN"};

    Gaudi::Property<bool> m_fillSamplesHistograms{this,
        "fillSamplesHistograms", true, "Fill histograms per channel/gain with samples"};

    Gaudi::Property<bool> m_fillProfileHistograms{this,
        "fillProfileHistograms", true, "Fill histograms per channel/gain with profile"};

    Gaudi::Property<bool> m_fillEventModule32Histograms{this,
        "fillEventModule32Histograms", true, "Fill histograms per channel/gain with event number % 32"};

    Gaudi::Property<bool> m_fillCorrelationsHistograms{this,
        "fillCorrelationsHistograms", true, "Fill histograms which are used to produce correlation and corvariance plots"};

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
        "TileDigitsContainer", "TileDigitsCnt", "Input Tile digits container key"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "TileRawChannelContainer", "", "Input Tile raw channel container key"};

    ToolHandle<TileCondToolNoiseSample> m_tileCondToolNoiseSample{this,
        "TileCondToolNoiseSample", "TileCondToolNoiseSample", "Tile sample noise tool"};

    std::vector<int> m_meanSampleGroups;
    std::vector<int> m_meanSampleProdGroups;
    std::vector<std::vector<int>> m_crcGlobalGroups;
    std::vector<std::vector<int>> m_crcErrGroups;
    std::vector<std::vector<std::vector<int>>> m_bcidGroups;
    std::vector<std::vector<std::vector<int>>> m_bcidErrGroups;
    std::vector<std::vector<std::vector<int>>> m_errorsGroups;

    std::vector<std::vector<int>> m_pedGroups;
    std::vector<std::vector<int>> m_hfnGroups;
    std::vector<std::vector<int>> m_samplesGroups;
    std::vector<std::vector<int>> m_profGroups;
    std::vector<std::vector<int>> m_evtMod32Groups;

    static const int MAX_DMU{16};
    bool m_is12bit{false};
    float m_ADCmaxMinusEpsilon;
};


#endif // TILEMONITORING_TILEDIGITSMONITORALGORITHM_H
