/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileCellMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHORS:   Alexander Solodkov
//	      Luca Fiorini (Luca.Fiorini@cern.ch)
//
// ********************************************************************
#ifndef TILECELLMONTOOL_H
#define TILECELLMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"

class ITileBadChanTool;
class TileBeamInfoProvider;
class TileCell;

#include <array>

/** @class TileCellMonTool
 *  @brief Class for TileCal monitoring at cell level
 */

class TileCellMonTool: public TileFatherMonTool {

  public:

    TileCellMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileCellMonTool();

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
    void FirstEvInit();
    void ShiftLumiHist(TProfile2D*, int32_t);

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileBeamInfoProvider> m_beamInfo;

    bool m_doOnline;
    double m_Threshold;
    double m_NegThreshold;
    double m_ThresholdForTime;
    double m_EneBalThreshold;
    double m_TimBalThreshold;
    int32_t m_TileCellTrig;
    int32_t m_old_lumiblock;
    int32_t m_delta_lumiblock;
    int32_t m_OldLumiArray1[4][64][4];
    int32_t m_OldLumiArray2[4][64][4];
    int m_nEventsProcessed[9]; // number of processed events per trigger
    std::string m_cellsContName;

    std::vector<const CaloCell*> m_muonCells;

    std::vector<std::string> m_cellchLabel[NumPart]; // array of cell-channels names
    std::vector<std::string> m_moduleLabel[NPartHisto]; // array of modules names

    bool m_isFirstEv;

    // histograms per samplings
    std::vector<TH2I*> m_TileCellEtaPhiOvThrSamp[TotalSamp];
    std::vector<TProfile2D*> m_TileCellEneEtaPhiSamp[TotalSamp];

    //detailed occupancy plots at the channel level
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapHiGainOvThr[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapLowGainOvThr[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr30GeV[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr300GeV[NPartHisto];
    std::vector<TProfile2D*> m_TileCellDetailOccMap[NPartHisto];

    // energy and time balance in channel vs module
    std::vector<TProfile2D*> m_TileCellEneDiffChanMod[NPartHisto];


    std::vector<TH2S*> m_TileCellEneBalModPart;
    std::vector<TH2S*> m_TileCellTimBalModPart;

    TProfile* m_TileCellEneBal[NumPart];
    TProfile* m_TileCellTimBal[NumPart];

    TH2S* m_TileCellStatFromDB[NumPart][2];  //partitions and gains
    TH2F* m_TileCellStatOnFly[NumPart];  //partitions
    TH2F* m_TileCellStatOnFlyLastLumiblocks[NumPart];  //partitions
    std::vector<TH2F*> m_TileCellStatOnFlyLastLumiblocksShadow[NumPart];  //partitions
    TH2F* m_TileCellDetailNegOccMap[NumPart];  //partitions
    TProfile* m_TileBadCell;  //number of bad cells per partition

    TProfile* m_TileMaskCellonFlyLumi[NPartHisto]; // number of masked cells as function of lumi_block
    TProfile* m_TileMaskChannonFlyLumi[NPartHisto]; // number of masked channels as function of lumi_block
    TProfile* m_TileMaskChannfromDBLumi[NPartHisto]; // number of masked channels as function of lumi_block

    // number of masked cells due to bad DQ as function of lumi_block
    std::array<TProfile*, NPartHisto> m_maskedCellsOnFlyDueToDQvsLumiblock;
    // number of masked channels due to bad DQ as function of lumi_block
    std::array<TProfile*, NPartHisto> m_maskedChannelsOnFlyDueToDQvsLumiblock;

    std::vector<TH1F*> m_TileCellSynch;

    //Pointers to 1DHistograms of Event Energy divided per sample and integrated
    std::vector<TH1F*> m_TileCellEneDiffSamp[NPartHisto][TotalSamp];
    std::vector<TH1F*> m_TileCellTimeDiffSamp[NPartHisto][TotalSamp];
    std::vector<TH1F*> m_TileCellEvEneSamp[NPartHisto][TotalSamp];
    std::vector<TH1F*> m_TileChannelTimeSamp[NPartHisto][TotalSamp];

    std::vector<TProfile*> m_TilenCellsLB[NPartHisto];
    std::vector<TH2F*> m_TileCellModuleCorr[NPartHisto];
    std::vector<TProfile*> m_TileCellEvEneTim[NPartHisto];
    std::vector<TProfile*> m_TileCellEvEneLumi[NPartHisto];

    std::vector<TProfile*> m_TileCellOccOvThrBCID[NPartHisto]; // last element not used, but we keep the same size of the others histograms to speed-up loops and avoid

    std::vector<TProfile2D*> m_TileChanPartTime[4];   // One for each partition.
    std::vector<TProfile2D*> m_TileDigiPartTime[4];   // One for each partition.

    // in each partition.
    std::vector<TProfile2D*> m_TileDigiEnergyLB[4][64];
    std::vector<TProfile2D*> m_TileDigiTimeLB[4][64];

    //  TH2D* m_test;

    int m_cellsInPartition[5] = {1151, 1472, 1408, 1151, 5182}; // EBA, LBA, LBC, EBC, ALL
    bool m_fillTimeHistograms;
    bool m_fillChannelTimeSampHistograms;
    bool m_fillTimeAndEnergyDiffHistograms;
    bool m_fillDigitizerTimeLBHistograms;
    bool m_fillDigitizerEnergyLBHistograms;
    int m_nLumiblocks;
    int m_nLastLumiblocks;
    bool m_fillMaskedOnFly4LastLumiblocks;
    unsigned int m_nEventsLastLumiblocks;
    std::vector<unsigned int> m_nEventsLastLumiblocksShadow;
    bool m_skipNotPhysicsEvents;
};

#endif
