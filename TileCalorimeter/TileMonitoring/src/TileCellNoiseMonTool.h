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

#include "TileFatherMonTool.h"

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

    void initFirstEvent();

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

    bool m_doOnline;
    //int32_t m_TileCellTrig;
    //int32_t m_oldLumiblock;
    //int32_t m_delta_lumiblock;
    std::string m_cellsContName;

    bool m_isFirstEvent;

    // x-axis range for the individual cell noise histograms
    float m_xMin;
    float m_xMax;

    std::vector<TH1F*> m_tileCellEne[4][64]; // a 2D array of 4x64 partitions X modules, each containing a vector of cells

    TH2F* m_mapSigma1[4];
    TH2F* m_mapSigma2[4];
    TH2F* m_mapR[4];
    TH2F* m_mapChi2[4];
    TH2F* m_mapChi2prb[4];
    TH2F* m_mapRmsOsig[4];
    TH2F* m_mapRms[4];

    TH1F* m_hPartition1;
    TH1F* m_hPartition2;

    TH2F* m_h2Partition0;
    TH2F* m_h2Partition1;
    TH2F* m_h2Partition2;
    TH2F* m_h2Partition3;

    //  TH2D* m_test;
};

#endif
