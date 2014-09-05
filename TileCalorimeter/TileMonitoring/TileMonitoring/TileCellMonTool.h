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

/** @class TileCellMonTool
 *  @brief Class for TileCell based monitoring
 */

class TProfile;
class TProfile2D;
class TileCell;
class ITileBadChanTool;

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
    std::string m_cellsContName;

    std::vector<const CaloCell*> m_muonCells;

    std::vector<std::string> m_cellchLabel[NumPart]; // array of cell-channels names
    std::vector<std::string> m_moduleLabel[NumPart]; // array of modules names

    bool m_isFirstEv;

    //std::vector<TH2I*> m_TileCellEtaPhiOvThr;
    //std::vector<TH1I*> m_TileCellPhiOvThr;
    //std::vector<TH1I*> m_TileCellEtaOvThr;
    //std::vector<TProfile2D*> m_TileCellEneEtaPhi;
    //std::vector<TProfile*> m_TileCellEneEta;
    //std::vector<TProfile*> m_TileCellEnePhi;
    // histograms per samplings
    std::vector<TH2I*> m_TileCellEtaPhiOvThrSamp[TotalSamp];
    std::vector<TProfile2D*> m_TileCellEneEtaPhiSamp[TotalSamp];

    //detailed occupancy plots at the channel level
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr30GeV[NPartHisto];
    std::vector<TH2F*> m_TileCellDetailOccMapOvThr300GeV[NPartHisto];
    std::vector<TProfile2D*> m_TileCellDetailOccMap[NPartHisto];

    // energy and time balance in module number vs partition
    //std::vector<TProfile2D*> m_TileCellOccModPart;
    //std::vector<TH2I*> m_TileCellOccModPartOvThr;
    // histograms per samplings
    //std::vector<TProfile2D*> m_TileCellOccModPartSamp[TotalSamp];
    //std::vector<TH2I*> m_TileCellOccModPartOvThrSamp[TotalSamp];

    // energy and time balance in module number vs partition
    std::vector<TH2S*> m_TileCellEneBalModPart;
    std::vector<TH2S*> m_TileCellTimBalModPart;

    TProfile* m_TileCellEneBal[NumPart];
    TProfile* m_TileCellTimBal[NumPart];

    TH2S* m_TileCellStatFromDB[NumPart][2];  //partitions and gains
    TH2F* m_TileCellStatOnFly[NumPart];  //partitions
    TH2F* m_TileCellDetailNegOccMap[NumPart];  //partitions
    TProfile* m_TileBadCell;  //number of bad cells per partition

    TProfile* m_TileMaskCellonFlyLumi[NPartHisto]; // number of masked cells as function of lumi_block
    TProfile* m_TileMaskChannonFlyLumi[NPartHisto]; // number of masked channels as function of lumi_block
    TProfile* m_TileMaskChannfromDBLumi[NPartHisto]; // number of masked channels as function of lumi_block

    std::vector<TH1F*> m_TileCellSynch;

    //Pointers to 1DHistograms of Event Energy divided per sample and integrated
    std::vector<TH1F*> m_TileCellEneDiffSamp[NPartHisto][TotalSamp];
    //std::vector<TH1F*> m_TileCellEneRatSamp[NPartHisto][TotalSamp];
    std::vector<TH1F*> m_TileCellTimeDiffSamp[NPartHisto][TotalSamp];
    std::vector<TH1F*> m_TileCellEvEneSamp[NPartHisto][TotalSamp];

    //std::vector<TH1F*> m_TilenCells[NPartHisto];
    std::vector<TProfile*> m_TilenCellsLB[NPartHisto];
    std::vector<TH2F*> m_TileCellModuleCorr[NPartHisto];
    //std::vector<TH1F*> m_TileCellTime[NPartHisto];
    //std::vector<TH1F*> m_TileCellEnergy[NPartHisto];
    //std::vector<TH1F*> m_TileCellEnergyRebin[NPartHisto];
    std::vector<TProfile*> m_TileCellEvEneTim[NPartHisto];
    std::vector<TProfile*> m_TileCellEvEneLumi[NPartHisto];
    //std::vector<TH1F*> m_TileCellEvEneRebin[NPartHisto];

    std::vector<TProfile*> m_TileCellOccOvThrBCID[NPartHisto]; // last element not used, but we keep the same size of the others histograms to speed-up loops and avoid
    //std::vector<TProfile*> m_TileCellOccEneBCID[NPartHisto];   // to have too many 'ifs' that are error-prone.
    //std::vector<TH1F*> m_TileEventsPerBCID;

    //std::vector<TH1F*>       m_TileChanDetTime;       // One for entire detector.
    std::vector<TProfile2D*> m_TileChanPartTime[4];   // One for each partition.
    std::vector<TProfile2D*> m_TileDigiPartTime[4];   // One for each partition.
    //std::vector<TProfile*>   m_TileChanModTime[4];    // One for each partition.
    //std::vector<TProfile*>   m_TileChanChTime[4][64]; // One for each module
    // in each partition.
    std::vector<TProfile2D*> m_TileDigiEnergyLB[4][64];
    std::vector<TProfile2D*> m_TileDigiTimeLB[4][64];

    //  TH2D* m_test;
};

#endif

