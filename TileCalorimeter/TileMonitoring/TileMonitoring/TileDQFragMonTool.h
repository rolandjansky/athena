/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDQFragMonTool.h
// PACKAGE:  
//
// AUTHOR:   Natanael Nunes de Moura Junior
//
//
// ********************************************************************
#ifndef TILERAWCHANNELMONTOOL_H
#define TILETAWCHANNELMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"
//
#include <stdint.h>
#include <vector>

#define NERR 13
#define NCORRUPTED 13
#define NDMU 16

class TileBeamInfoProvider;
class TileDigitsCollection;
class TileRawChannelCollection;
class TileDQstatus;
class ITileBadChanTool;
class TileDCSSvc;
class TileRawChannel;
class TileDigits;

/** @class TileDQFragMonTool
 *  @brief Class for commissioning: dq fragment monitoring
 */

class TileDQFragMonTool: public TileFatherMonTool {

  public:

    TileDQFragMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileDQFragMonTool();

    StatusCode initialize();

    //pure virtual methods

    /*---------------------------------------------------------*/
    /* njunior@cern.ch */
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    void bookErrHist(int ros, int drawer);
    void fillErrHist(int ros, int drawer);
    void drawErrHist(int ros, int drawer);
    void fillGlobalHist(int ros, int drawer);
    void drawGlobalHist();
    void updateHistograms();
    void endErrHist();
    void rebin(TH2I*);

    /*---------------------------------------------------------*/

  private:

    void bookFirstHist();
    void fillMasking();
    void fillOneErrHist(int, int, int, int);
    bool CheckhasErr(int ros, int drawer, int dmu);
    bool isModuleDCSgood(int partition, int drawer) const;
    int findDrawerErrors(const TileRawChannelCollection *coll);
    int findChannelErrors(const TileRawChannel *rch, int & gain);
    void fillBadDrawer(void);

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ServiceHandle<TileDCSSvc> m_tileDCSSvc; //!< Pointer to TileDCSSvc
    ToolHandle<TileBeamInfoProvider> m_beamInfo;

    /*---------------------------------------------------------*/
    /* properties*/
    std::string m_contNameDSP;
    std::string m_contNameOffline;
    std::string m_contNameDigits;
    float m_negAmpHG;
    float m_negAmpLG;
    float m_jumpDeltaHG;
    float m_jumpDeltaLG;
    float m_pedDeltaHG;
    float m_pedDeltaLG;

    bool m_skipMasked;
    bool m_skipGapCells;
    bool m_doPlots;
    bool m_doOnline;
    bool m_checkDCS;  //!< if false, do not use TileDCSSvc at all

    /* njunior@cern.ch */
    const TileDQstatus* dqStatus;
    int m_UpdateCount;
    int m_UpdateCount_shadow[10];
    int m_Update;
    int m_UpdateTotal;
    // int m_dmu_map[16][3]; // not used
    uint32_t m_last_lb;
    //bool m_UpFlag = false;
    int globalErrCount[4][64]; //global CRC errors
    std::string errorDir;

    /* njunior@cern.ch */
    TH2I* hist_error[4][64];
    TH2I* hist_error_shadow[4][64][10];
    TH2I* hist_global[2];
    //TH1I* hist_summary[4];
    //TH1I* hist_BadDrawerBCID;
    //TProfile* hist_BadDrawerBCID_lb;
    //TH1I* hist_BadChanNeg[2];
    //TProfile* hist_BadChanNeg_lb[2];
    //TH1I* hist_BadDrawerJump;
    //TProfile* hist_BadDrawerJump_lb;
    TH2S* hist_BadChannelJump2D[4];
    TH2S* hist_BadChannelNeg2D[4];
    //TH1I* hist_BadDrawerBCID_nonmask;
    //TProfile* hist_BadDrawerBCID_lb_nonmask;
    //TH1I* hist_BadChanNeg_nonmask[2];
    //TProfile* hist_BadChanNeg_lb_nonmask[2];
    //TH1I* hist_BadDrawerJump_nonmask;
    //TProfile* hist_BadDrawerJump_lb_nonmask;
    TH2S* hist_BadChannelJump2D_nonmask[4];
    TH2S* hist_BadChannelNeg2D_nonmask[4];
    std::vector<std::string> m_ErrorsLabels, m_PartitionsLabels;
    std::vector<std::string> m_moduleLabel[NumPart]; // array of module names
    std::vector<std::string> m_cellchLabel[NumPart]; // array of cell-channels names

    TProfile* hist_error_lb[4][64];

    /*---------------------------------------------------------*/

};

#endif

