/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRawChannelMonTool.h
// PACKAGE:  
//
// AUTHOR:   Alexander Solodkov
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILERAWCHANNELMONTOOL_H
#define TILEMONITORING_TILERAWCHANNELMONTOOL_H

#include "TilePaterMonTool.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileDQstatus.h"
#include "StoreGate/ReadHandleKey.h"

class TileRawChannel;
class TileCondToolEmscale;
class TileInfo;

/** @class TileRawChannelMonTool
 *  @brief Class for TileCal monitoring at channel level
 */

class TileRawChannelMonTool: public TilePaterMonTool {

  public:

    TileRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    virtual ~TileRawChannelMonTool();

    virtual StatusCode initialize() override;
    
    //pure virtual methods
    virtual StatusCode bookHists() override;
    virtual StatusCode fillHists() override;
    virtual StatusCode finalHists() override;
    virtual StatusCode checkHists(bool fromFinalize) override;

    void bookHists(int ros, int drawer);
    void drawHists(int ros, int drawer, const std::string& moduleName);
    void ratioErrorBar(TH1S* hist, double& xmin, double& xmax, double mean);
    void rangeErrorBar(double& xmin, double& max, double mean);
    void bookDsp(int ros, int drawer);
    StatusCode fillDsp(std::map<int, std::vector<double> > &efitMap, std::map<int, std::vector<double> > &tfitMap);
    StatusCode finalDsp(int ros, int drawer);

    void drawDsp(int ros, int drawer, const std::string& moduleName);
    void drawOptFilt(int ros, int drawer, const std::string& moduleName);
    TF1* GetTimeFitFunc(TH2S* hist2d);
    void LaserFancyPlotting(int ros, int drawer, int maxgain, const std::string& moduleName);

  private:

    void bookSummaryHistograms(int ros, int drawer);
    StatusCode fillSummaryHistograms();
    void resetSummaryHistograms();

    bool m_bookAll{};
    bool m_book2D{};
    bool m_overlaphists{};
    int m_runType{};
    std::string m_contName;
    std::string m_contNameDSP;
    std::string m_contNameOF;
    double m_DownLimit{}; // Down Threshold for Amp/Q ratio plots
    double m_UpLimit{};   // Up  Threshold for Amp/Q ratio plots
    double m_lo_IntegralLimit{}; // Warning error bar Low limit for  Amp/Q ratio plots
    double m_med_IntegralLimit{}; // Warning error bar Med limit for  Amp/Q ratio plots
    double m_hi_IntegralLimit{}; // Warning error bar High limit for  Amp/Q ratio plots
    bool m_useratioerror{}; //use ratioerror bar or rangeerror bar ?
    //bool m_plotOptFilt; //book Optimal Filter histograms?
    bool m_plotDsp{}; //book Optimal Filter histograms?
    bool m_storeGraph{}; //Store TGraph to file: necessary due to THistSvc bug
    std::map<int, std::vector<double> > m_efitMap;
    std::map<int, std::vector<double> > m_tfitMap;
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale;
    double m_efitThresh{};

    /// The following three functions are implemented to filter data corruption, copied from TileDigitsMonTool.h

    ///Function to check for data corruption. Modified from TileDigitsMonTool implementation

    bool checkDmuHeader(std::vector<uint32_t>* headerVec, int dmu);
    //vector to hold data corruption information
    // std::vector<bool> corrup[5][64][2]; //ros, drawer, gain (index of each vector is channel)
    bool m_corrup[5][64][2][16]={}; //ros, drawer, gain, DMU

    /// Function to check that the DMU header format is correct
    /// bit_31 of the DMU header must be 1 and
    /// bit_17 of the DMU header must be 0
    /// Return true in the case of error
    inline bool checkDmuHeaderFormat(uint32_t header) {
      if (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0))
        return false; //no error
      else
        return true; //error
    };

    /// Function to check that the DMU header parity is correct
    /// Parity of the DMU header should be odd
    /// Return true in case of error
    inline bool checkDmuHeaderParity(uint32_t header) {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i)
        parity += ((header >> i) & 0x1);

      if ((parity % 2) == 1) return false; //no error
      else                   return true; //error

    };

    enum RunType {
      Unknown = 0, PhysRun = 1, // expect monogain
      LasRun = 2, // expect monogain
      LedRun = 3, // expect monogain
      PedRun = 4, // expect bigain
      CisRun = 8, // expect bigain
      MonoRun = 9, // expect monogain
      CisRamp = 10 // expect monogain
    };

    enum DspPlot {
      Edsp = 0, Tdsp = 1, chi2dsp = 2, Edsp_fit = 3, Tdsp_fit = 4
    };

    enum sumDspPlot {
      sumEdsp_fit = 0, sumTdsp_fit = 1, sumEdsp = 2, NsumDsp = 3
    };

    const uint32_t* m_cispar;

    bool m_bigain{};
    int m_nEventsTileMon{};
    struct Data {
      float m_rangeQ[2][2][3][48];
      double m_timeCov[5][64][48][2][2][6];
      double m_timeCovCorr[5][64][48][2][2][3];	//Lukas

      //Pointers to Histograms
      std::vector<TH1S *> m_hist1[5][64][48][2]; // ros,drawer,channel,gain
      std::vector<TH2S *> m_hist2[5][64][48][2];
      std::vector<TH1F *> m_finalHist1[5][64][2][2]; //ros, drawer,capacitor, gain
      std::vector<TH2F *> m_finalHist2[256][2];

      std::vector<TH1F *> m_histDsp1[5][64][48][2]; // ros,drawer,channel,gain
      std::vector<TH2F *> m_histDsp2[5][64][48][2];
      std::vector<TH1F *> m_finalHistDsp1[5][64][2]; //ros,drawer,gain
      std::vector<TH2F *> m_finalHistDsp2[5][64][2];
      std::vector<TH1F *> m_hBarDsp1[5][64][2]; // ros, drawer,gain

      std::vector<TH1F *> m_summaryPmts[5][64][2][2];
    };
    std::unique_ptr<Data> m_data;

    TileRawChannelUnit::UNIT m_calibUnit{};

    int m_summaryUpdateFrequency{};
    bool m_resetAfterSummaryUpdate{};
    bool m_doLaserSummaryVsPMT{};
    bool m_drawHists{};
    float m_minAmpForCorrectedTime{};
    // TileInfo
    std::string m_infoName{};
    const TileInfo* m_tileInfo{};
    bool m_is12bit{};
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey;
    int m_intCalibUnit{};
};

#endif
