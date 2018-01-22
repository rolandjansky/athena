/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// NAME:      TileRawChannelMonNoiseMonTool.h
// PACKAGE:   TileMonitoring
//
// AUTHORS:   Christophe Clement  - Created November 2014 - cclement@cern.ch
//
// Purpose:   Measure and monitors the channel at the RawChannel level (m_doRawChannel=true)
//            Computes the RMS of the amplitude at the channel level and
//
//            Performs double gaussian fit at the channel level
//            Also optionally (m_doRawChannel=false) the noise and double gaussian fit can be performed
//            from on the channel level but with the channels accessed indirectly from the cell container. 
//
//            To perform the double gaussian fit use the option: m_do2gfit=true
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TILERAWCHANNELMONTOOL_H
#define TILERAWCHANNELMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class ITileBadChanTool;
class TileCondToolEmscale;
class TileBeamInfoProvider;
class TileDQstatus;

/** @class TileRawChannelNoiseMonTool
 *  @brief Class for TileCal noise monitoring at cell level
 */

class TileRawChannelNoiseMonTool: public TileFatherMonTool {

  public:

    TileRawChannelNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileRawChannelNoiseMonTool();

    virtual StatusCode initialize();

    //pure virtual methods
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();

  private:

    StatusCode bookRawChannelNoiseHistos();
    StatusCode fillHistoPerRawChannel();  // work on the raw channels

    void cleanHistVec();

    // perform the 2G fit at the channel level, optional only if m_do2gfit=true
    void doFit();                         // perform  gaussian fit

    void fitGauss(TH1F* h, double*, TF1*); // perform  gaussian fit
    void fitDoubleGauss(TH1F* h, double*, TF1*); // perform  gaussian fit

    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;

    const TileDQstatus* m_DQstatus;

    bool m_doOnline;

    // x-axis range for the individual cell noise histograms
    float m_xmin;
    float m_xmax;

    Bool_t m_do2GFit;
    Bool_t m_doFit;
    std::string m_rawChannelContainerName;
    std::string m_rawChannelContainerDspName;
    // histograms
    std::vector<TH1F*> m_tileChannelEne[5][64]; // a 2D array of 5x64 partitions X modules, each containing a vector of channels

    TH2F* m_map_sigma[5];
    TH2F* m_map_sigma2[5];
    TH2F* m_map_R[5];
    TH2F* m_map_chi2[5];
    TH2F* m_map_chi2prb[5];
    TH2F* m_map_rmsOsig[5];
    TH2F* m_map_rms[5];

    int m_summaryUpdateFrequency;
    std::string m_gainName;
    int m_gain;

    int m_nEventsProcessed;
    std::vector<uint32_t> m_triggerTypes;
    int m_nbins;
    unsigned int m_minimumEventsNumberToFit;
};

#endif
