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

    std::string m_rawChannelContainerName;
    
    std::vector<std::string> m_TMDB_names[5];

    std::vector<TH1F*> m_amplitude[5];
    std::vector<TH1F*> m_time[5]; // temporary until TMDB raw channel monitoring tool be ready

    TH1F* m_partition_amplitude[5]; 
    TH1F* m_partition_time[5]; 

    TProfile2D* m_amplitude_map[5];
    TProfile2D* m_time_map[5];


    int m_nEventsProcessed;
    bool m_histogramsNotBooked;
    bool m_isNotDSP;
    float m_amplitudeThreshold;
};

#endif
