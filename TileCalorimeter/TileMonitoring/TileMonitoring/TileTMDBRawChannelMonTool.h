/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TileMonitoring/TileFatherMonTool.h"

class TileBeamInfoProvider;
class ITileBadChanTool;
class TileDQstatus;
class TileCondToolNoiseSample;
class ITileCondToolTMDB;

#include <vector>
#include <string>

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileTMDBRawChannelMonTool : public TileFatherMonTool {

  public:

    TileTMDBRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTMDBRawChannelMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    StatusCode bookTMDBHistograms(unsigned int ros, unsigned int channel);
    StatusCode bookTMDBSummaryHistograms();
	int tilemodule_to_check(int sector);
	float calc_dR(float dEta, float dPhi);
    void fillTMDBThresholds();
    
    int m_nEventsProcessed;
    ToolHandle<ITileCondToolTMDB> m_tileToolTMDB;
    bool m_histogramsNotBooked;
    bool m_isNotDSP;
    bool m_eff;
    std::vector<float> m_TMDB_A_D6_amplitude;
    std::vector<float> m_TMDB_A_D56_amplitude;
    std::vector<float> m_TMDB_C_D6_amplitude;
    std::vector<float> m_TMDB_C_D56_amplitude;
    
    
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
	
	
    int m_thre_a[256];
    int m_thre_c[256];
};

#endif

