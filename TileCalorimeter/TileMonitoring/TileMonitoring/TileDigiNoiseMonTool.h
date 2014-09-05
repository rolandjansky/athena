/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseMonTool.h
// PACKAGE:  
//
// AUTHOR:   Christophe Clement
//
//
// ********************************************************************
#ifndef TILEDIGINOISEMONTOOL_H
#define TILEDIGINOISEMONTOOL_H

#include "GaudiKernel/ToolHandle.h"

//#include "TileMonitoring/TilePaterMonTool.h"
#include "TileMonitoring/TileFatherMonTool.h"
#include <vector>
#include <stdint.h>

class TileBeamInfoProvider;

/** @class TileDigitsMonTool
 *  @brief Class for commissioning: digits histograms
 */

//class TileDigiNoiseMonTool : public TilePaterMonTool
class TileDigiNoiseMonTool: public TileFatherMonTool {

  public:

    TileDigiNoiseMonTool(const std::string & type, const std::string & name,
        const IInterface* parent);

    ~TileDigiNoiseMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHists();

    StatusCode fillHists();
    StatusCode finalHists();
    StatusCode checkHists(bool fromFinalize);

    void bookHists(int ros, int drawer);
    void bookHistsOld(int ros, int drawer);

  private:

    /** Method to find stuckbits in Read-Out ADC channel.
     */
    int stuckBits(TH1S *hist, int adc);

    /** A crude method to check  Read-Out ADC channel stuckbits.
     */
    int stuckBits_Amp(TH1S *hist, int adc);
    int stuckBits_Amp2(TH1S *hist, int adc, TH2C *outhist = NULL, int ch = 0);
    /** Method to check global CRC and DMU CRC.
     */
    void CRCcheck(uint32_t crc32, uint32_t crcMask, int headsize, int ros, int drawer);

    bool DMUheaderCheck(std::vector<uint32_t>* headerVec, int ros, int drawer, int gain, int dmu);

    //methods to shift and stat test histos
    void shiftHisto(TH1S *hist, int ros, int drawer, int ch, int gain);
    void statTestHistos(int ros, int drawer, int gain);

    // method to define coloour palette
    static const int m_NCont = 10; //number of colors in palette
    int define_palette(int ncolors, int *colors = NULL);

    StatusCode RODCRCcalc();

    /// Function to check that the DMU header format is correct
    /// bit_31 of the DMU header must be 1 and
    /// bit_17 of the DMU header must be 0
    /// Return true in case of error
    inline bool DMUheaderFormatCheck(uint32_t header) {
      if (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0))
        return false; // no error
      else
        return true; //error
    }
    ;

    /// Function to check that the DMU header parity is correct
    /// Parity of the DMU header should be odd
    /// Return true in case of error
    inline bool DMUheaderParityCheck(uint32_t header) {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i)
        parity += ((header >> i) & 0x1);

      if ((parity % 2) == 1)
        return false; //no error
      else
        return true; //error
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

    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    const uint32_t * m_cispar;

    bool m_bigain;
    int m_nEvents;
    int m_nSamples;
    double SumPed1[5][64][48][2];
    double SumPed2[5][64][48][2];
    double SumRms1[5][64][48][2];
    double SumRms2[5][64][48][2];
    double MeanAmp[5][64][2][48];
    double MeanAmp_ij[5][64][2][48][48];
    double m_cov_ratio[5][64][2]; //covariance ratio printed in covariance plots

    //vector to hold data corruption information
    // std::vector<bool> corrup[5][64][2]; //ros, drawer, gain (index of each vector is channel)
    bool corrup[5][64][2][16]; //ros, drawer, gain, DMU

    //Pointers to Histograms
    std::vector<TH1S *> hist1[5][64][48][2]; // ros,drawer,channel,gain
    std::vector<TH2F *> hist2[5][64][2];
    std::vector<TProfile *> histP[5][64][48][2];

    // histogram to store 4 maps of module vs channel
    TH2F * final_noise_map[5][2][2];

    std::string m_diginoise_histo_path;

    //shifted histos for DQMF
    TH1S * shifted_hist[5][64][49][2]; // one extra histo for reference!

    //For test
    int hp;
    int hb;

};

#endif
