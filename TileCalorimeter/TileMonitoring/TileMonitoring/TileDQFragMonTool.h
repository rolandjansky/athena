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
#ifndef TILEMONITORING_TILEDQFRAGMONTOOL_H
#define TILEMONITORING_TILEDQFRAGMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

#include <array>

class ITileBadChanTool;
class TileDCSSvc;
class TileBeamInfoProvider;
class TileDQstatus;
class TileRawChannel;
class TileRawChannelCollection;


/** @class TileDQFragMonTool
 *  @brief Class for TileCal monitoring of Data Quality fragment
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
    const TileDQstatus* m_dqStatus;
    int m_UpdateCount;
    int m_UpdateCount_shadow[10];
    int m_UpdateTotal;
    uint32_t m_last_lb;
    int m_globalErrCount[4][64]; //global CRC errors

    /* njunior@cern.ch */
    TH2I* m_hist_error[4][64];
    TH2I* m_hist_error_shadow[4][64][10];
    TH2I* m_hist_global[2];
    TH2I* m_mismatchedL1TriggerType;

    TH2S* m_hist_BadChannelJump2D[4];
    TH2S* m_hist_BadChannelNeg2D[4];

    TH2S* m_hist_BadChannelJump2D_nonmask[4];
    TH2S* m_hist_BadChannelNeg2D_nonmask[4];
    std::vector<std::string> m_ErrorsLabels, m_PartitionsLabels;
    std::vector<std::string> m_moduleLabel[NumPart]; // array of module names
    std::vector<std::string> m_cellchLabel[NumPart]; // array of cell-channels names

    TProfile* m_hist_error_lb[4][64];

    std::array<TH2I*, 4> m_badPulseQuality;
    TH2I* m_noAllDigitsInDrawer;

    static const int NERROR = 17;
    static const int MASKEDERROR = NERROR - 3;
    static const int DCSERROR = NERROR - 2;
    static const int HVERROR = NERROR - 1;
    static const int NCORRUPTED = 13;
    static const int NDMU = 16;

    int m_nLumiblocks;
    float m_qualityCut;
    unsigned int m_nEventsWithAllDigits;
    /*---------------------------------------------------------*/

};

#endif
