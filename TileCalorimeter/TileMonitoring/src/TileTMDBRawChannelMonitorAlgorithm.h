/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H
#define TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"
#include "TileMonitoring/TileFatherMonTool.h"

// Atlas includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;
class ITileCondToolTMDB;

class TileTMDBRawChannelMonitorAlgorithm : public AthMonitorAlgorithm {
  public:

    TileTMDBRawChannelMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TileTMDBRawChannelMonitorAlgorithm();

    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  private:
    
    int tilemodule_to_check(int sector) const;
    float calc_dR(float dEta, float dPhi) const;
    const TileHWID* m_tileHWID;
    bool m_eff;
    bool m_isDSP;
    ToolHandle<ITileCondToolTMDB> m_tileToolTMDB;
    unsigned int m_numberOfThresholds;
    float m_amplitudeThreshold;
    
    std::vector<int> m_ampGroups;
    std::vector<int> m_timeGroups;
    std::vector<std::vector<int>> m_ampCell;
    std::vector<std::vector<int>> m_timeCell;

    SG::ReadHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,
    "MuRcvRawChCnt", "MuRcvRawChCnt", "TMDB raw container"};

    Gaudi::Property<std::vector<int>> m_nChannels{this,
    "NumberOfChannels", {8, 8, 4, 4}, "Number of TMDB channels per module in (LBA, LBC, EBA, EBC)"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_muonContainerKey{this,
    "MuonContainer", "Muons", "Input Muons container key"};
    
    SG::ReadHandleKey<xAOD::MuonRoIContainer> m_muonRoIsContainerKey{this,
    "TrigMuonContainer", "LVL1MuonRoIs", "Input LVL1 Muon RoIs container key"};    
        
    ToolHandle<ITileCondToolTMDB> m_tileCondToolTMDB{this,
    "TileCondToolTMDB", "TileCondToolTMDB", "Tile TMDB conditions tool"}; 

    std::vector<int> m_tgcSectorGroup;
    std::vector<std::vector<std::vector<int>>> m_coinThrDGroup;
};
#endif // TILEMONITORING_TILETMDBRAWCHANNELMONITORALGORITHM_H
