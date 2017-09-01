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
class TileCondToolNoiseSample;

/** @class TileDigitsNoiseMonTool
 *  @brief Class for TileCal noise monitoring at digits level
 */

class TileDigiNoiseMonTool : public TileFatherMonTool {

  public:

    TileDigiNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileDigiNoiseMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    StatusCode bookNoiseHistograms();
    StatusCode updateSummaryHistograms();

    std::string m_digitsContainerName;

    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;
    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values

    const TileDQstatus* m_DQstatus;

    bool m_bigain;

    //int m_nSamples;
    double m_sumPed1[5][64][48][2];
    double m_sumPed2[5][64][48][2];
    double m_sumRms1[5][64][48][2];

    int m_nPedEvents[5][64][48][2];
    int m_nRmsEvents[5][64][48][2];

    // histogram to store 4 maps of module vs channel
    TH2F* m_finalNoiseMap[5][2][3];

    int m_summaryUpdateFrequency;
    int m_nEventsProcessed;
    bool m_fillEmtyFromDB;
    bool m_fillPedestalDifference;
    std::vector<uint32_t> m_triggerTypes;
    bool m_histogramsNotBooked;
    unsigned int m_nChannelsInDrawerThreshold;
};

#endif
