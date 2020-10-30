/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEMONITORING_TILETMDBDIGITSMONITORALGORITHM_H
#define TILEMONITORING_TILETMDBDIGITSMONITORALGORITHM_H

// Tile includes
#include "TileEvent/TileDigitsContainer.h"


// Atlas includes
#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

class TileHWID;

class TileTMDBDigitsMonitorAlgorithm : public AthMonitorAlgorithm {
  public:
    TileTMDBDigitsMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TileTMDBDigitsMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;
  private:

    std::string m_digitsContainerName;
    const TileHWID* m_tileHWID;
    //int m_nSamples;

    std::vector<int> m_pedGroups;
    std::vector<int> m_hfnGroups;
    std::vector<int> m_ampGroups;

    SG::ReadHandleKey<TileDigitsContainer> m_digitsContainerKey{this,
    "MuRcvDigitsCnt", "MuRcvDigitsCnt", "Muon digits container"};

    Gaudi::Property<std::vector<int>> m_nChannels{this,
    "NumberOfChannels", {8, 8, 4, 4}, "Number of TMDB channels per module in (LBA, LBC, EBA, EBC)"};

    std::vector<std::vector<int>> m_cellPedGroups;
    std::vector<std::vector<int>> m_cellHFNGroups;
    std::vector<std::vector<int>> m_cellAmpGroups;
    
};
#endif // TILEMONITORING_TILETMDBDIGITSMONITORALGORITHM_H
