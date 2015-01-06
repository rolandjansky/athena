/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseMonTool.h
// PACKAGE:  
//
// AUTHOR:   Christophe Clement
//
//
// ********************************************************************
#ifndef TILEDIGINOISEMONTOOL_H
#define TILEDIGINOISEMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class TileBeamInfoProvider;
class ITileBadChanTool;
class TileDQstatus;

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileDigiNoiseMonTool : public TilePaterMonTool {

  public:

    TileDigiNoiseMonTool(const std::string & type, const std::string & name,
        const IInterface* parent);

    ~TileDigiNoiseMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHists();

    StatusCode fillHists();
    StatusCode finalHists();
    StatusCode checkHists(bool fromFinalize);


  private:


    bool m_bookAll;
    bool m_book2D;
    int m_runType;
    std::string m_contNameDSP;


    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;

    const TileDQstatus* m_DQstatus;

    bool m_bigain;
    int m_nEvents;
    int m_nSamples;
    double SumPed1[5][64][48][2];
    double SumPed2[5][64][48][2];
    double SumRms1[5][64][48][2];
    double SumRms2[5][64][48][2];

    // histogram to store 4 maps of module vs channel
    TH2F * final_noise_map[5][2][2];

    int m_summaryUpdateFrequency;
    int m_nEventsProcessed;

};

#endif
