/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileCellNoiseMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHORS:   Christophe Clement
//
//
// ********************************************************************
#ifndef TILECELLNOISEMONTOOL_H
#define TILECELLNOISEMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class ITileBadChanTool;

/** @class TileCellNoiseMonTool
 *  @brief Class for TileCal noise monitoring at cell level
 */

class TileCellNoiseMonTool: public TileFatherMonTool {

  public:

    TileCellNoiseMonTool(const std::string & type, const std::string & name,
        const IInterface* parent);

    ~TileCellNoiseMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);
    StatusCode finalHists();

    StatusCode bookCellNoiseHistos();

    StatusCode fillHistoPerCell();
    void do2GFit();                  // perform double gaussian fit
    void do2GFit(TH1F* h, double *, TF1 *); // perform double gaussian fit

    void cleanHistVec();

  private:

    void FirstEvInit();

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

    bool m_doOnline;
    int32_t m_TileCellTrig;
    int32_t m_old_lumiblock;
    int32_t m_delta_lumiblock;
    std::string m_cellsContName;

    bool m_isFirstEv;

    // x-axis range for the individual cell noise histograms
    float m_xmin;
    float m_xmax;

    std::vector<TH1F*> m_TileCellEne[4][64]; // a 2D array of 4x64 partitions X modules, each containing a vector of cells

    TH2F* m_map_sigma1[4];
    TH2F* m_map_sigma2[4];
    TH2F* m_map_R[4];
    TH2F* m_map_chi2[4];
    TH2F* m_map_chi2prb[4];
    TH2F* m_map_rmsOsig[4];
    TH2F* m_map_rms[4];

    TH1F* h_partition1;
    TH1F* h_partition2;

    TH2F* h2_partition0;
    TH2F* h2_partition1;
    TH2F* h2_partition2;
    TH2F* h2_partition3;

    //  TH2D* m_test;
};

#endif
