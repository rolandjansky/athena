/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTBCellMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHORS:   Alexander Solodkov
//	      Luca Fiorini (Luca.Fiorini@cern.ch)
//
// ********************************************************************
#ifndef TILETBMONTOOL_H
#define TILETBMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class ITileBadChanTool;
class TileCell;

/** @class TileTBCellMonTool
 *  @brief Class for TileCal TestBeam monitoring at cell level
 */

class TileTBCellMonTool: public TileFatherMonTool {

  public:

    TileTBCellMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileTBCellMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHistograms();
    StatusCode fillHistograms();
    StatusCode procHistograms();
    StatusCode checkHists(bool fromFinalize);

    StatusCode bookHistTrig(int trig);
    StatusCode bookHistTrigPart(int trig, int part);

    void cleanHistVec();

  private:

    void calculateSynch();
    short isCellBad(const TileCell* tile_cell);
    short isAdcBad(int partition, int module, int channel, int gain);
    short nAdcBad(int partition);
    void initFirstEvent();
    void shiftLumiHist(TProfile2D*, int32_t);
    void fillHitMap(TH2F *hHitMap, int cellHitMap[], double energy);
    //Double_t langaufun(Double_t *x, Double_t *par);
    TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi,
        Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
    //Double_t langaufun(Double_t *x, Double_t *par);
    Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

//    bool m_isFirstEvent;
    bool m_doOnline;
    double m_energyThreshold;
    double m_negThreshold;
    double m_energyThresholdForTime;
    double m_eneBalThreshold;
    double m_timBalThreshold;
//    int32_t m_tileCellTrig;
//    int32_t m_oldLumiblock;
//    int32_t m_deltaLumiblock;
//    int32_t m_oldLumiArray1[4][64][4];
//    int32_t m_oldLumiArray2[4][64][4];
    int m_nEventsProcessed[9]; // number of processed events per trigger
    std::string m_cellsContName;

    TH1F* m_tileChannelTimeLBC01[48];
    TH1F* m_tileChannelTimeEBC02[48];
    TH1F* m_tileChannelEnergyLBC01[48];
    TH1F* m_tileChannelEnergyEBC02[48];
    TH2F* m_tileChannelEnergyVsTimeLBC01[48];
    TH2F* m_tileChannelEnergyVsTimeEBC02[48];
    //  TH2D* m_test;

    int m_cellsInPartition[5] = { 1151, 1472, 1408, 1151, 5182 }; // EBA, LBA, LBC, EBC, ALL
    bool m_fillTimeHistograms;

    TH1F* m_tileTBTotalEnergyEBA;
    TH2F* m_tileTBHitMapEBA;
    TH1F* m_tileTBCellEneSumEBA[14];
    TH1F* m_tileTBCellEneDiffEBA[14];
    TH1F* m_tileTBCellTimeSumEBA[14];
    TH1F* m_tileTBCellTimeDiffEBA[14];

    TH1F* m_tileTBTotalEnergyEBC;
    TH2F* m_tileTBHitMapEBC;
    TH1F* m_tileTBCellEneSumEBC[14];
    TH1F* m_tileTBCellEneDiffEBC[14];
    TH1F* m_tileTBCellTimeSumEBC[14];
    TH1F* m_tileTBCellTimeDiffEBC[14];

    TH1F* m_tileTBTotalEnergyLBA;
    TH2F* m_tileTBHitMapLBA;
    TH1F* m_tileTBCellEneSumLBAD0;
    TH1F* m_tileTBCellEneSumLBA[22];
    TH1F* m_tileTBCellEneDiffLBA[22];
    TH1F* m_tileTBCellTimeSumLBAD0;
    TH1F* m_tileTBCellTimeSumLBA[22];
    TH1F* m_tileTBCellTimeDiffLBA[22];

    TH1F* m_tileTBTotalEnergyLBC;
    TH2F* m_tileTBHitMapLBC;
    TH1F* m_tileTBCellEneSumLBCD0;
    TH1F* m_tileTBCellEneSumLBC[22];
    TH1F* m_tileTBCellEneDiffLBC[22];
    TH1F* m_tileTBCellTimeSumLBCD0;
    TH1F* m_tileTBCellTimeSumLBC[22];
    TH1F* m_tileTBCellTimeDiffLBC[22];
    //this currently only works for extended barrel
    /*cellHitMap[channel][bins]
     *  bins=0 is the lower x bin
     *  bins=1 is the lower y bin
     *  bins=2 is the upper x bin
     *  bins=3 is the upper y bin
     *  bins=4 corresponding name in cellHitMap
     *  bins=5 whether or not there is C cell for long barrel
     */
    int m_cellHitMapEB[48][5];
    int m_cellHitMapLB[48][6];
    int m_cellHitMapLB_C[8][4];
    std::string m_cellHitMapNameEB[14];
    std::string m_cellHitMapNameLB[22];

};

#endif
