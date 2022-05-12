/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTMDBDigiMonTool.h
// PACKAGE:  
//
// AUTHOR:   Andrey Ryzhov
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILETMDBDIGITSMONTOOL_H
#define TILEMONITORING_TILETMDBDIGITSMONTOOL_H

#include "TileFatherMonTool.h"

class ITileBadChanTool;
class TileDQstatus;
class TileCondToolNoiseSample;
class TileDigitsCollection;


#include <vector>
#include <string>

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileTMDBDigitsMonTool : public TileFatherMonTool {

  public:

    TileTMDBDigitsMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTMDBDigitsMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    StatusCode bookTMDBHistograms(const TileDigitsCollection* digitsCollection);
    StatusCode bookTMDBSummaryHistograms(unsigned int ros);
    StatusCode updateSummaryHistograms();

    std::string m_digitsContainerName;

    //int m_nSamples;
    double m_sumPed1[5][64][8];
    double m_sumPed2[5][64][8];
    int m_nPedEvents[5][64][8];

    std::vector<std::string> m_TMDB_names[5];

    TProfile2D* m_pedestal_map[5];
    TProfile2D* m_amplitude_map[5];
    TProfile2D* m_lfn_map[5];
    TProfile2D* m_hfn_map[5];

    std::vector<TH1F*> m_pedestal[5];
    std::vector<TH1F*> m_hfn[5];
    std::vector<TH1F*> m_amplitude[5]; // temporary until TMDB raw channel monitoring tool be ready


    int m_summaryUpdateFrequency;
    int m_nEventsProcessed;

};

#endif
