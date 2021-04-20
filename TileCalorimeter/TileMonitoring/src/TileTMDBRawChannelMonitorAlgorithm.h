/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H
#define TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/ITileCondToolTMDB.h"
#include "TileConditions/TileCablingSvc.h"

// Atlas includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;

class TileTMDBRawChannelMonitorAlgorithm : public AthMonitorAlgorithm {
  public:

    using AthMonitorAlgorithm::AthMonitorAlgorithm;
    virtual ~TileTMDBRawChannelMonitorAlgorithm() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  private:

    int tilemodule_to_check(int sector) const;
    float calc_dR(float dEta, float dPhi) const;

    Gaudi::Property<std::vector<int>> m_nChannels{this,
        "NumberOfChannels", {8, 8, 4, 4}, "Number of TMDB channels per module in (LBA, LBC, EBA, EBC)"};

    Gaudi::Property<float> m_amplitudeThreshold{this,
        "AmplitudeThresholdForTime", 80, "Amplitude threashold for timing histograms"};

    Gaudi::Property<bool> m_isDSP{this,
        "DSP", true, "TMDB Raw Channels container comes from DSP"};

    Gaudi::Property<bool> m_fillRawChannelHistograms{this,
        "FillRawChannelHistograms", true, "Fill TMDB raw channel histograms"};

    Gaudi::Property<bool> m_fillEfficiencyHistograms{this,
        "FillEfficiencyHistograms", false, "Fill TMDB efficiency histograms"};

    Gaudi::Property<bool> m_numberOfThresholds{this,
        "NumberOfThresholds", false, "Number of thresholds to check (Run2: 2, Run3: 1)"};

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
        "MuRcvRawChCnt", "MuRcvRawChCnt", "TMDB raw container"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey{this,
        "MuonContainer", "Muons", "Input Muons container key"};

    SG::ReadHandleKey<xAOD::MuonRoIContainer> m_muonRoIsContainerKey{this,
        "TrigMuonContainer", "LVL1MuonRoIs", "Input LVL1 Muon RoIs container key"};

    ToolHandle<ITileCondToolTMDB> m_tileCondToolTMDB{this,
        "TileCondToolTMDB", "TileCondToolTMDB", "Tile TMDB conditions tool"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    const TileHWID* m_tileHWID{nullptr};

    std::vector<int> m_ampGroups;
    std::vector<int> m_timeGroups;
    std::vector<std::vector<int>> m_ampCell;
    std::vector<std::vector<int>> m_timeCell;

    std::vector<int> m_tgcSectorGroup;
    std::vector<std::vector<std::vector<int>>> m_coinThrDGroup;
};
#endif // TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H
