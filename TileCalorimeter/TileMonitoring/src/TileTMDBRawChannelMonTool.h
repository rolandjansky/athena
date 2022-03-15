/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTMDBRawChannelMonTool.h
// PACKAGE:  
//
// AUTHOR:   Andrey Ryzhov
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILETMDBRAWCHANNELMONTOOL_H
#define TILEMONITORING_TILETMDBRAWCHANNELMONTOOL_H

#include "TileFatherMonTool.h"

class ITileBadChanTool;
class TileDQstatus;
class TileCondToolNoiseSample;
class ITileCondToolTMDB;
class TileRawChannelCollection;

#include <vector>
#include <string>

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileTMDBRawChannelMonTool : public TileFatherMonTool {

  public:

    TileTMDBRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    virtual ~TileTMDBRawChannelMonTool();

    virtual StatusCode initialize() override;

    //pure virtual methods
    virtual StatusCode bookHistograms() override;
    virtual StatusCode fillHistograms() override;
    virtual StatusCode procHistograms() override;

  private:

    StatusCode bookTMDBHistograms(const TileRawChannelCollection* rawChannelCollection);
    StatusCode bookTMDBSummaryHistograms(unsigned int ros);
    int tilemodule_to_check(int sector);
    float calc_dR(float dEta, float dPhi);
    
    int m_nEventsProcessed;
    ToolHandle<ITileCondToolTMDB> m_tileToolTMDB;
    //    bool m_histogramsNotBooked;
    bool m_isNotDSP;
    bool m_eff;

    float m_TMDB_D6_amplitude[2][64]; // (EBA and EBC) x 64 modules
    float m_TMDB_D56_amplitude[2][64]; // (EBA and EBC) x 64 modules

    std::string m_rawChannelContainerName;
    std::string m_muonContainer;
    std::string m_muRoi;
    
    std::vector<std::string> m_TMDB_names[5];
	

    std::vector<TH1F*> m_amplitude[5];
    std::vector<TH1F*> m_time[5]; // temporary until TMDB raw channel monitoring tool be ready

    TH1F* m_partition_amplitude[5]; 
    TH1F* m_partition_time[5]; 
	
    TH1F* m_drawer[5][2][2];  //m_drawer[ros][ch][thr]
    TH1F* m_sector[5]; //m_sector[ros]

    TProfile2D* m_amplitude_map[5];
    TProfile2D* m_time_map[5];



    float m_amplitudeThreshold;
	
};

#endif

