/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigitsMonTool.h
// PACKAGE:  
//
// AUTHOR:   Alexander Solodkov
//
//
// ********************************************************************
#ifndef TILEMONITORING_TILEDIGITSMONTOOL_H
#define TILEMONITORING_TILEDIGITSMONTOOL_H

#include "TileMonitoring/TilePaterMonTool.h"
#include "TileEvent/TileDQstatus.h"
#include "StoreGate/ReadHandleKey.h"
#include "TileMonitoring/ITileStuckBitsProbsTool.h"

class TileCondToolNoiseSample;


/** @class TileDigitsMonTool
 *  @brief Class for TileCal monitoring at digits level
 */

class TileDigitsMonTool: public TilePaterMonTool 
                       , virtual public ITileStuckBitsProbsTool {

  public:

    TileDigitsMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    virtual ~TileDigitsMonTool();

    virtual StatusCode initialize() override;

    //pure virtual methods
    virtual StatusCode bookHists() override;
    virtual StatusCode fillHists() override;
    virtual StatusCode finalHists() override;
    virtual StatusCode checkHists(bool fromFinalize) override;

    void bookHists(int ros, int drawer);
    void drawHists(int ros, int drawer, std::string moduleName);

    virtual const uint8_t* stuckBitProb (int ros, int module, int channel, int gain) const override;
    virtual void saveStuckBitsProbabilities(TTree* tree) override;

  private:

    /** Method to find stuckbits in Read-Out ADC channel.
     */
    int stuckBits(TH1S *hist, int adc);
    //int stuckBits_maker(TH1S *hist);
    /** A crude method to check  Read-Out ADC channel stuckbits.
     */
    int stuckBits_Amp(TH1S *hist, int adc);
    int stuckBits_Amp2(TH1S *hist, int adc, TH2C *outhist = NULL, int ch = 0, uint8_t *stuck_probs = NULL);
    /** Method to check global CRC and DMU CRC.
     */
    void CRCcheck(const TileDQstatus* dqStatus,
                  uint32_t crc32, uint32_t crcMask, int headsize, int ros, int drawer);

    //void DMUheaderCheck(std::vector<uint32_t>* headerVec, int headsize, int ros, int drawer, int gain);
    bool DMUheaderCheck(std::vector<uint32_t>* headerVec, int ros, int drawer, int gain, int dmu);

    //methods to shift and stat test histos
    void shiftHisto(TH1S *hist, int ros, int drawer, int ch, int gain);
    void statTestHistos(int ros, int drawer, int gain);

    // method to define coloour palette
    static const int m_NCont = 10; //number of colors in palette
    int define_palette(int ncolors, int *colors = NULL);

    StatusCode RODCRCcalc(const TileDQstatus* dqStatus);

    /// Function to check that the DMU header format is correct
    /// bit_31 of the DMU header must be 1 and
    /// bit_17 of the DMU header must be 0
    /// Return true in case of error
    inline bool DMUheaderFormatCheck(uint32_t header) {
      if (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0))
        return false; // no error
      else
        return true; //error
    };

    /// Function to check that the DMU header parity is correct
    /// Parity of the DMU header should be odd
    /// Return true in case of error
    inline bool DMUheaderParityCheck(uint32_t header) {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i)
        parity += ((header >> i) & 0x1);

      if ((parity % 2) == 1) return false; //no error
      else return true; //error
    };

    bool m_bookAll;
    bool m_book2D;
    int m_runType;
    std::string m_contNameDSP;

    enum RunType {
      Unknown = 0, // expect monogain
      PhysRun = 1, // expect monogain
      LasRun = 2, // expect monogain
      LedRun = 3, // expect monogain
      PedRun = 4, // expect bigain
      CisRun = 8, // expect bigain
      MonoRun = 9, // expect monogain
      CisRamp = 10 // expect monogain
    };

    ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provided noise values

    const uint32_t* m_cispar;

    bool m_bigain;
    int m_nEvents;
    int m_nSamples;
    // Factor these out to avoid triggering the ubsan sanity checks.
    struct Data {
      double m_sumPed1[5][64][48][2];
      double m_sumPed2[5][64][48][2];
      double m_sumRms1[5][64][48][2];
      double m_sumRms2[5][64][48][2];
      double m_meanAmp[5][64][2][48];
      double m_meanAmp_ij[5][64][2][48][48];
      int m_nEvents_i[5][64][2][48];
      int m_nEvents_ij[5][64][2][48][48];
      double m_cov_ratio[5][64][2]; //covariance ratio printed in covariance plots
      uint8_t m_stuck_probs[5][64][48][2][10];

      //vector to hold data corruption information
      // std::vector<bool> corrup[5][64][2]; //ros, drawer, gain (index of each vector is channel)
      bool m_corrup[5][64][2][16]; //ros, drawer, gain, DMU

      //Pointers to Histograms
      std::vector<TH1S *> m_hist0[5][64]; // ros,drawer
      std::vector<TH1S *> m_hist1[5][64][48][2]; // ros,drawer,channel,gain
      std::vector<TH1I *> m_hist_DMUerr[5][64][48][2]; // ros,drawer,channel,gain for DMU BCID/CRC errors
      std::vector<TH2F *> m_hist2[5][64][2];
      std::vector<TProfile *> m_histP[5][64][48][2];
      std::vector<TH1F *> m_final_hist1[5][64][2]; // ros, drawer, gain
      std::vector<TH2F *> m_final_hist2[5][64][2];
      TH2C* m_final_hist_stucks[5][64][2]; // stuck bits, saturation: ros, drawer. gain
      std::vector<TH1D *> m_outInHighGain;

      //shifted histos for DQMF
      TH1S * m_shifted_hist[5][64][49][2]; // one extra histo for reference!

    };
    std::unique_ptr<Data> m_data;
    bool m_allHistsFilled;

    //For test
    //int hp;
    //int hb;
    bool m_fillPedestalDifference;
    std::string m_digitsContainerName;
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey;
};

#endif
