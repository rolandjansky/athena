/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

#include "TileFatherMonTool.h"

class ITileBadChanTool;
class TileCell;

/** @class TileTBCellMonTool
 *  @brief Class for TileCal monitoring at cell level
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
	bool m_isFirstEv;
  private:

    void calculateSynch();
    short isCellBad(const TileCell* tile_cell);
    short isAdcBad(int partition, int module, int channel, int gain);
    short nAdcBad(int partition);
    void FirstEvInit();
    void ShiftLumiHist(TProfile2D*, int32_t);
	void fillHitMap(TH2F *hHitMap,int cellHitMap[],double energy);
	//Double_t langaufun(Double_t *x, Double_t *par);
	TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
	//Double_t langaufun(Double_t *x, Double_t *par);
	Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM);


    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool

    bool m_doOnline{};
    double m_Threshold{};
    double m_NegThreshold{};
    double m_energyThresholdForTime{};
    double m_EneBalThreshold{};
    double m_TimBalThreshold{};
    int32_t m_TileCellTrig{};
    int32_t m_old_lumiblock{};
    int32_t m_delta_lumiblock{};
    int32_t m_OldLumiArray1[4][64][4]={0};
    int32_t m_OldLumiArray2[4][64][4]={0};
    int m_nEventsProcessed[9]={0}; // number of processed events per trigger
    std::string m_cellsContName;

    double m_maxEnergy{};
    double m_maxTotalEnergy{};

    TH1F* m_tileChannelTimeLBA[64][48]={};
    TH1F* m_tileChannelTimeLBC[64][48]={};
    TH1F* m_tileChannelTimeEBA[64][48]={};
    TH1F* m_tileChannelTimeEBC[64][48]={};
    TH1F* m_tileChannelEnergyLBA[64][48]={};
    TH1F* m_tileChannelEnergyLBC[64][48]={};
    TH1F* m_tileChannelEnergyEBA[64][48]={};
    TH1F* m_tileChannelEnergyEBC[64][48]={};
    //TH2F* m_tileChannelEnergyVsTimeLBC01[48];
    //TH2F* m_tileChannelEnergyVsTimeEBC02[48];
    //  TH2D* m_test;

    int m_cellsInPartition[5] = {1151, 1472, 1408, 1151, 5182}; // EBA, LBA, LBC, EBC, ALL
    bool m_fillTimeHistograms{};

	TH1F* m_TileTBTotalEnergyEBA[64]={};
    TH2F* m_TileTBHitMapEBA[64]={};
    TH1F* m_TileTBCellEneSumEBA[64][14]={};
    TH1F* m_TileTBCellEneDiffEBA[64][14]={};
    TH1F* m_TileTBCellTimeSumEBA[64][14]={};
    TH1F* m_TileTBCellTimeDiffEBA[64][14]={};

    TH2F* m_TileTBCellEneLeftVsRightPMTEBA[64][14]={};
    TH2F* m_TileTBCellTimeLeftVsRightPMTEBA[64][14]={};
    TH2F* m_TileTBSampleBCvsAEneEBA[64]={};
    TH1F* m_TileTBSampleDEneEBA[64]={};

	TH1F* m_TileTBTotalEnergyEBC[64]={};
    TH2F* m_TileTBHitMapEBC[64]={};
    TH1F* m_TileTBCellEneSumEBC[64][14]={};
    TH1F* m_TileTBCellEneDiffEBC[64][14]={};
    TH1F* m_TileTBCellTimeSumEBC[64][14]={};
    TH1F* m_TileTBCellTimeDiffEBC[64][14]={};

	TH2F* m_TileTBCellEneLeftVsRightPMTEBC[64][14]={};
	TH2F* m_TileTBCellTimeLeftVsRightPMTEBC[64][14]={};
    TH2F* m_TileTBSampleBCvsAEneEBC[64]={};
    TH1F* m_TileTBSampleDEneEBC[64]={};

	TH1F* m_TileTBTotalEnergyLBA[64]={};
	TH2F* m_TileTBHitMapLBA[64]={};
	TH1F* m_TileTBCellEneSumLBAD0[64]={};
    TH1F* m_TileTBCellEneSumLBA[64][23]={};
    TH1F* m_TileTBCellEneDiffLBA[64][23]={};
    TH1F* m_TileTBCellTimeSumLBAD0[64]={};
    TH1F* m_TileTBCellTimeSumLBA[64][23]={};
    TH1F* m_TileTBCellTimeDiffLBA[64][23]={};

 	TH2F* m_TileTBCellEneLeftVsRightPMTLBA[64][23]={};
 	TH2F* m_TileTBCellTimeLeftVsRightPMTLBA[64][23]={};
    TH2F* m_TileTBSampleBCvsAEneLBA[64]={};
    TH1F* m_TileTBSampleDEneLBA[64]={};

	TH1F* m_TileTBTotalEnergyLBC[64]={};
	TH2F* m_TileTBHitMapLBC[64]={};
	TH1F* m_TileTBCellEneSumLBCD0[64]={};
    TH1F* m_TileTBCellEneSumLBC[64][23]={};
    TH1F* m_TileTBCellEneDiffLBC[64][23]={};
    TH1F* m_TileTBCellTimeSumLBCD0[64]={};
    TH1F* m_TileTBCellTimeSumLBC[64][23]={};
    TH1F* m_TileTBCellTimeDiffLBC[64][23]={};

 	TH2F* m_TileTBCellEneLeftVsRightPMTLBC[64][23]={};
 	TH2F* m_TileTBCellTimeLeftVsRightPMTLBC[64][23]={};
    TH2F* m_TileTBSampleBCvsAEneLBC[64]={};
    TH1F* m_TileTBSampleDEneLBC[64]={};
	//this currently only works for extended barrel
	/*cellHitMap[channel][bins]
	*  bins=0 is the lower x bin
	*  bins=1 is the lower y bin
	*  bins=2 is the upper x bin
	*  bins=3 is the upper y bin
	*  bins=4 corresponding name in cellHitMap
	*  bins=5 whether or not there is C cell for long barrel
	*/
	int m_cellHitMapEB[48][5]={0};
	int m_cellHitMapLB[48][6]={0};
	int m_cellHitMapLB_C[8][4]={0};
	std::string m_cellHitMapNameEB[14];
	std::string m_cellHitMapNameLB[23];


/*
	//E3
	cellHitMapEB[0][0]=-1;
	cellHitMapEB[0][1]=-1;
	cellHitMapEB[0][2]=-1;
	cellHitMapEB[0][3]=-1;
	//E4
	cellHitMapEB[1][0]=-1;
	cellHitMapEB[1][1]=-1;
	cellHitMapEB[1][2]=-1;
	cellHitMapEB[1][3]=-1;
	//D4
	cellHitMapEB[2][0]=1;
	cellHitMapEB[2][1]=5;
	cellHitMapEB[2][2]=3;
	cellHitMapEB[2][3]=5;
	//D4
	cellHitMapEB[3][0]=1;
	cellHitMapEB[3][1]=5;
	cellHitMapEB[3][2]=3;
	cellHitMapEB[3][3]=5;
	//C10
	cellHitMapEB[4][0]=3;
	cellHitMapEB[4][1]=3;
	cellHitMapEB[4][2]=3;
	cellHitMapEB[4][3]=4;
	//C10
	cellHitMapEB[5][0]=3;
	cellHitMapEB[5][1]=3;
	cellHitMapEB[5][2]=3;
	cellHitMapEB[5][3]=4;
	//A12
	cellHitMapEB[6][0]=4;
	cellHitMapEB[6][1]=1;
	cellHitMapEB[6][2]=5;
	cellHitMapEB[6][3]=1;
	//A12
	cellHitMapEB[7][0]=4;
	cellHitMapEB[7][1]=1;
	cellHitMapEB[7][2]=5;
	cellHitMapEB[7][3]=1;
	//B11
	cellHitMapEB[8][0]=4;
	cellHitMapEB[8][1]=2;
	cellHitMapEB[8][2]=6;
	cellHitMapEB[8][3]=3;
	//B11
	cellHitMapEB[9][0]=4;
	cellHitMapEB[9][1]=2;
	cellHitMapEB[9][2]=6;
	cellHitMapEB[9][3]=3;
	//A13
	cellHitMapEB[10][0]=6;
	cellHitMapEB[10][1]=1;
	cellHitMapEB[10][2]=8;
	cellHitMapEB[10][3]=1;
	//A13
	cellHitMapEB[11][0]=6;
	cellHitMapEB[11][1]=1;
	cellHitMapEB[11][2]=8;
	cellHitMapEB[11][3]=1;
	//E1
	cellHitMapEB[12][0]=-1;
	cellHitMapEB[12][1]=-1;
	cellHitMapEB[12][2]=-1;
	cellHitMapEB[12][3]=-1;
	//E2
	cellHitMapEB[13][0]=-1;
	cellHitMapEB[13][1]=-1;
	cellHitMapEB[13][2]=-1;
	cellHitMapEB[13][3]=-1;
	//B12
	cellHitMapEB[14][0]=7;
	cellHitMapEB[14][1]=2;
	cellHitMapEB[14][2]=9;
	cellHitMapEB[14][3]=3;
	//B12
	cellHitMapEB[15][0]=7;
	cellHitMapEB[15][1]=2;
	cellHitMapEB[15][2]=9;
	cellHitMapEB[15][3]=3;
	//D5
	cellHitMapEB[16][0]=4;
	cellHitMapEB[16][1]=4;
	cellHitMapEB[16][2]=11;
	cellHitMapEB[16][3]=5;
	//D5
	cellHitMapEB[17][0]=4;
	cellHitMapEB[17][1]=4;
	cellHitMapEB[17][2]=11;
	cellHitMapEB[17][3]=5;
	//E3*
	cellHitMapEB[18][0]=-1;
	cellHitMapEB[18][1]=-1;
	cellHitMapEB[18][2]=-1;
	cellHitMapEB[18][3]=-1;
	//E4*
	cellHitMapEB[19][0]=-1;
	cellHitMapEB[19][1]=-1;
	cellHitMapEB[19][2]=-1;
	cellHitMapEB[19][3]=-1;
	//A14
	cellHitMapEB[20][0]=9;
	cellHitMapEB[20][1]=1;
	cellHitMapEB[20][2]=11;
	cellHitMapEB[20][3]=1;
	//A14
	cellHitMapEB[21][0]=9;
	cellHitMapEB[21][1]=1;
	cellHitMapEB[21][2]=11;
	cellHitMapEB[21][3]=1;
	//B13
	cellHitMapEB[22][0]=10;
	cellHitMapEB[22][1]=2;
	cellHitMapEB[22][2]=12;
	cellHitMapEB[22][3]=3;
	//B13
	cellHitMapEB[23][0]=10;
	cellHitMapEB[23][1]=2;
	cellHitMapEB[23][2]=12;
	cellHitMapEB[23][3]=3;
	//NOTHINg
	cellHitMapEB[24][0]=-1;
	cellHitMapEB[24][1]=-1;
	cellHitMapEB[24][2]=-1;
	cellHitMapEB[24][3]=-1;
	//NOTHINg
	cellHitMapEB[25][0]=-1;
	cellHitMapEB[25][1]=-1;
	cellHitMapEB[25][2]=-1;
	cellHitMapEB[25][3]=-1;
	//NOTHINg
	cellHitMapEB[26][0]=-1;
	cellHitMapEB[26][1]=-1;
	cellHitMapEB[26][2]=-1;
	cellHitMapEB[26][3]=-1;
	//NOTHINg
	cellHitMapEB[27][0]=-1;
	cellHitMapEB[27][1]=-1;
	cellHitMapEB[27][2]=-1;
	cellHitMapEB[27][3]=-1;
	//NOTHINg
	cellHitMapEB[28][0]=-1;
	cellHitMapEB[28][1]=-1;
	cellHitMapEB[28][2]=-1;
	cellHitMapEB[28][3]=-1;
	//NOTHINg
	cellHitMapEB[29][0]=-1;
	cellHitMapEB[29][1]=-1;
	cellHitMapEB[29][2]=-1;
	cellHitMapEB[29][3]=-1;
	//B14
	cellHitMapEB[30][0]=13;
	cellHitMapEB[30][1]=2;
	cellHitMapEB[30][2]=15;
	cellHitMapEB[30][3]=3;
	//A15
	cellHitMapEB[31][0]=12;
	cellHitMapEB[31][1]=1;
	cellHitMapEB[31][2]=14;
	cellHitMapEB[31][3]=1;
	//A15
	cellHitMapEB[32][0]=12;
	cellHitMapEB[32][1]=1;
	cellHitMapEB[32][2]=14;
	cellHitMapEB[32][3]=1;
	//NOTHINg
	cellHitMapEB[33][0]=-1;
	cellHitMapEB[33][1]=-1;
	cellHitMapEB[33][2]=-1;
	cellHitMapEB[33][3]=-1;
	//NOTHINg
	cellHitMapEB[34][0]=-1;
	cellHitMapEB[34][1]=-1;
	cellHitMapEB[34][2]=-1;
	cellHitMapEB[34][3]=-1;
	//B14
	cellHitMapEB[35][0]=13;
	cellHitMapEB[35][1]=2;
	cellHitMapEB[35][2]=15;
	cellHitMapEB[35][3]=3;
	//B15
	cellHitMapEB[36][0]=16;
	cellHitMapEB[36][1]=2;
	cellHitMapEB[36][2]=18;
	cellHitMapEB[36][3]=3;
	//D6
	cellHitMapEB[37][0]=12;
	cellHitMapEB[37][1]=4;
	cellHitMapEB[37][2]=18;
	cellHitMapEB[37][3]=5;
	//D6
	cellHitMapEB[38][0]=12;
	cellHitMapEB[38][1]=4;
	cellHitMapEB[38][2]=18;
	cellHitMapEB[38][3]=5;
	//B15
	cellHitMapEB[39][0]=16;
	cellHitMapEB[39][1]=2;
	cellHitMapEB[39][2]=18;
	cellHitMapEB[39][3]=3;
	//A16
	cellHitMapEB[40][0]=15;
	cellHitMapEB[40][1]=1;
	cellHitMapEB[40][2]=18;
	cellHitMapEB[40][3]=1;
	//A16
	cellHitMapEB[41][0]=15;
	cellHitMapEB[41][1]=1;
	cellHitMapEB[41][2]=18;
	cellHitMapEB[41][3]=1;
	//NOTHINg
	cellHitMapEB[42][0]=-1;
	cellHitMapEB[42][1]=-1;
	cellHitMapEB[42][2]=-1;
	cellHitMapEB[42][3]=-1;
	//NOTHINg
	cellHitMapEB[43][0]=-1;
	cellHitMapEB[43][1]=-1;
	cellHitMapEB[43][2]=-1;
	cellHitMapEB[43][3]=-1;
	//NOTHINg
	cellHitMapEB[44][0]=-1;
	cellHitMapEB[44][1]=-1;
	cellHitMapEB[44][2]=-1;
	cellHitMapEB[44][3]=-1;
	//NOTHINg
	cellHitMapEB[45][0]=-1;
	cellHitMapEB[45][1]=-1;
	cellHitMapEB[45][2]=-1;
	cellHitMapEB[45][3]=-1;
	//NOTHINg
	cellHitMapEB[46][0]=-1;
	cellHitMapEB[46][1]=-1;
	cellHitMapEB[46][2]=-1;
	cellHitMapEB[46][3]=-1;
	//NOTHINg
	cellHitMapEB[47][0]=-1;
	cellHitMapEB[47][1]=-1;
	cellHitMapEB[47][2]=-1;
	cellHitMapEB[47][3]=-1;*/
/*
std::string m_EBcellName[48] = { "E3", "E4", "D4", "D4", "C10", "C10", "A12", "A12", "B11", "B11", "A13", "A13"
                                 , "E1", "E2", "B12", "B12", "D5", "D5", "E3*", "E4*", "A14", "A14", "B13", "B13"
                                 ,   "",   "",    "",    "",   "",   "", "B14", "A15", "A15",    "",    "", "B14"
                                 , "B15", "D6", "D6", "B15", "A16", "A16",  "",    "",    "",    "",    "", "" };

std::string m_LBcellName[48] = { "D0", "A1", "B1", "B1", "A1", "A2", "B2", "B2", "A2",  "A3",  "A3", "B3"
                                 , "B3", "D1", "D1", "A4", "B4", "B4", "A4", "A5", "A5",  "B5",  "B5", "A6"
                                 , "D2", "D2", "A6", "B6", "B6", "A7",   "",   "", "A7",  "B7",  "B7", "A8"
                                 , "A9", "A9", "A8", "B8", "B8", "D3", "B9",   "", "D3", "A10", "A10", "B9" };
*/
};

#endif
