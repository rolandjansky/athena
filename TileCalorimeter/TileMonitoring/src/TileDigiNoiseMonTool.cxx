/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigiNoiseMonTool
// PACKAGE:  TileMonitoring  
//
//           Produce DigiNoise folder in tilemon.root for monitoring digital noise
//           Input is RAW
//           Output are the noisemap 2D histograms per partition/gain/LFN and HFN
//
// AUTHOR:   Christophe Clement  (largely based on TileDigiMonTool.cxx)
//
// TODO
// -- filter bad channels from the noise maps
//
// ********************************************************************

#include "AthenaKernel/errorcheck.h"

#include "TileMonitoring/TileDigiNoiseMonTool.h"

#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"

#include "TH1S.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TVirtualPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPaveLabel.h"
#include "TObjArray.h"
#include "TSpectrum.h"
#include "TText.h"
#include "TExec.h"
#include "TColor.h"
#include "TPaletteAxis.h"
#include "TList.h"


#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


/*---------------------------------------------------------*/
TileDigiNoiseMonTool::TileDigiNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_beamInfo("TileBeamInfoProvider")
  , m_cispar(0)
  , m_nEvents(0)
  , m_nSamples(0)
  , hp(1)
  , hb(-1)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("bookAllDrawers",m_bookAll = false);
  declareProperty("book2D", m_book2D  = false);

  // run type 1 - phys, 2 - las, 4 - ped, 8 - cis, 9- mono
  declareProperty("runType",m_runType = 8);
  declareProperty("bigain",m_bigain = true);
  declareProperty("TileRawChannelContainerDSP", m_contNameDSP = "TileRawChannelCnt");
  declareProperty("DigiNoiseHistoPath"        , m_diginoise_histo_path ); //ROOT File relative directory
  //declareProperty("histoPathBase"             , m_path = "/Tile/DigiNoise"); //ROOT File relative directory   
}

/*---------------------------------------------------------*/
TileDigiNoiseMonTool::~TileDigiNoiseMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );

  CHECK( m_beamInfo.retrieve() );

  m_nEvents = 0;
  memset(SumPed1,    0, sizeof(SumPed1));
  memset(SumPed2,    0, sizeof(SumPed2));
  memset(SumRms1,    0, sizeof(SumRms1));
  memset(SumRms2,    0, sizeof(SumRms2));
  memset(MeanAmp,    0, sizeof(MeanAmp));
  memset(MeanAmp_ij, 0, sizeof(MeanAmp_ij));

  //For test stuck_bits_maker
  //hp = 1;
  //hb = -1;

  //return TilePaterMonTool::initialize();
  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::bookHists()
/*---------------------------------------------------------*/
{ 

  ATH_MSG_WARNING( "TileDigiNoiseMonTool::bookHists() :  m_path =  " << m_path  );
  
  if (m_bookAll) {
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        bookHistsOld(ros, drawer);
      }
    }
  }
  
  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };
  const char *type[4] = { "lfn", "hfn", "Low Freq. Noise", "High Freq. Noise" };
  char histName[50];
  char histTitle[50];
  char modName[10];

  int mingain=(m_bigain) ? 0 : 2;
  int maxgain=(m_bigain) ? 2 : 3;

  for (int ros = 1; ros < 5; ++ros) {
    for (int gn = mingain; gn < maxgain; ++gn) {
      int adc = gn & 1;
      for (int noisetype = 0; noisetype < 2; ++noisetype) {

        sprintf(histName, "noisemap_%s_%s_%s ", part[ros], gain[adc], type[noisetype]);

        sprintf(histTitle, "Noise map %s %s %s ", part[ros], gain[3 + gn], type[noisetype + 2]);

        ATH_MSG_WARNING( "in bookHists() :: booking noise_map histo : " << histName );

        //final_noise_map[ros][gn][noisetype] = book2F("/Summary",histName , histTitle, 49,0.,49.,65,0,65);
        final_noise_map[ros][gn][noisetype] = book2F(m_diginoise_histo_path, histName, histTitle, 49, 0., 49., 65, 0, 65);

        for (int imod = 1; imod < 65; imod = imod + 2) {
          sprintf(modName, "%s%s%i ", part[ros], (imod < 10 ? "0" : ""), imod);
          final_noise_map[ros][gn][noisetype]->GetYaxis()->SetBinLabel(imod + 1, modName);
        }

        final_noise_map[ros][gn][noisetype]->GetXaxis()->SetTitle("Channel #");
        final_noise_map[ros][gn][noisetype]->GetYaxis()->SetTitle("Module #");
        final_noise_map[ros][gn][noisetype]->GetYaxis()->SetTitle("Module #");
        final_noise_map[ros][gn][noisetype]->GetZaxis()->SetTitle("Noise [ADC]");
        final_noise_map[ros][gn][noisetype]->GetZaxis()->SetRangeUser(0, 10);
        final_noise_map[ros][gn][noisetype]->SetTitle(histTitle);

      } // end loop over noise type (hfn, lfn)
    } // end gain loop
  } // end loop over ros/partitions

  //SetBookStatus(true);
  
  return StatusCode::SUCCESS;
}




/*---------------------------------------------------------*/
void TileDigiNoiseMonTool::bookHists(int ros, int drawer) {
/*---------------------------------------------------------*/

  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };

//! index of hists per one channel per gain
//! 0 - digit[0] (first sample only)
//! 1 - mean RMS over 7(9) samples in event
//! 2 - all samples
//!
//! index of hists per TileDMU (16 DMUs, first 16 channels used)
//! 3 - BCID
//! 4 - BCID errors
//! 5 - CRC  errors (only gain=0 used, so should be the last histogram in a vector)


  std::ostringstream sStr;
  sStr << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
  std::string moduleName = sStr.str();
  std::string subDir = "/" + moduleName;
  std::string histName, histTitle;

  ATH_MSG_DEBUG("in bookHists() for module " << moduleName);

  //! for bigain run book 2 histograms per channel
  //! for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int gn = mingain; gn < maxgain; ++gn) {
    //Book DMU header errors histograms
    int adc = gn & 1;

    sStr.str("");
    sStr << moduleName << "_DMU_head_" << gain[gn] << "_err";
    histName = sStr.str();

    sStr.str("");
    sStr << moduleName << " DMU Header " << gain[3 + gn] << " errors";
    histTitle = sStr.str();

    hist2[ros][drawer][adc].push_back(book2F(subDir, histName, histTitle, 16, 0., 16., 6, -0.5, 5.5));
  } // loop over gains

  for (int ch = 0; ch < 48; ++ch) {

    //    int pmt=m_cabling->channel2hole(ros,ch); // 1:48, negative for non-connected channels
    //    int pmt0 = ((pmt > 0)? pmt-1 : pmt+1); // 0:47, negaitve for non-connected channels
    sStr.str("");
    sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
    std::string sCh = sStr.str(); // 00:47, always positive

    for (int gn = mingain; gn < maxgain; ++gn) {

      int adc = gn & 1;

      // first sample
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_sample0";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " sample[0]";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              151,-0.5,150.5));

      // RMS over samples in event
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_meanRMS";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " mean RMS in event";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              101,-0.05,10.05));

      //all samples to find stuck bits
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_samples";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " all samples";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              1024,-0.5,1023.5));

      // average profile for a given channel/gain
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_prof";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " average profile";
      histTitle = sStr.str();

      histP[ros][drawer][ch][adc].push_back(bookProfile(subDir, histName, histTitle, m_nSamples, 0.0, m_nSamples * 1.0, -0.5, 1023.5));

      // shifted samples histograms

      double shiftxmin = 0.;
      double shiftxmax = 796.;
      int shiftnbins = 796;

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_samples_shifted";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " all samples shifted";
      histTitle = sStr.str();

      shifted_hist[ros][drawer][ch][adc] = book1S(subDir, histName, histTitle, shiftnbins, shiftxmin, shiftxmax);

      if (ch < 16) { // use first 16 channels to store BCID/CRC errors (one per TileDMU)

        sStr.str("");
        sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
        std::string sDmu = sStr.str();

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << gain[gn] << "_BCID";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << gain[3 + gn] << " BCID";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
        //                                              100,2050.5,2150.5));

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << gain[gn] << "_BCID_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << gain[3 + gn] << " BCID errors";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
        //                                              3,-0.5,2.5));

        if (adc) continue; // don't book CRC for high gain

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << "_CRC_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << " CRC errors";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][0].push_back(book1S(subDir,histName,histTitle,
        //                                            5,-0.5,4.5));
      } // if ch<16
    } // gain loop
  } // channel loop

  // and now global histograms for all channels in a drawer


} // bookHistos




/*---------------------------------------------------------*/
void TileDigiNoiseMonTool::bookHistsOld(int ros, int drawer) {
/*---------------------------------------------------------*/

  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };

//! index of hists per one channel per gain
//! 0 - digit[0] (first sample only)
//! 1 - mean RMS over 7(9) samples in event
//! 2 - all samples
//!
//! index of hists per TileDMU (16 DMUs, first 16 channels used)
//! 3 - BCID
//! 4 - BCID errors
//! 5 - CRC  errors (only gain=0 used, so should be the last histogram in a vector)

  std::ostringstream sStr;
  sStr << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
  std::string moduleName = sStr.str();
  std::string subDir = "/" + moduleName;
  std::string histName, histTitle;

  ATH_MSG_DEBUG( "in bookHists() for module " << moduleName );

  //! for bigain run book 2 histograms per channel
  //! for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int gn = mingain; gn < maxgain; ++gn) {
    //Book DMU header errors histograms
    int adc = gn & 1;

    sStr.str("");
    sStr << moduleName << "_DMU_head_" << gain[gn] << "_err";
    histName = sStr.str();

    sStr.str("");
    sStr << moduleName << " DMU Header " << gain[3 + gn] << " errors";
    histTitle = sStr.str();

    hist2[ros][drawer][adc].push_back(book2F(subDir, histName, histTitle, 16, 0., 16., 6, -0.5, 5.5));
  }

  for (int ch = 0; ch < 48; ++ch) {

    //    int pmt=m_cabling->channel2hole(ros,ch); // 1:48, negative for non-connected channels
    //    int pmt0 = ((pmt > 0)? pmt-1 : pmt+1); // 0:47, negaitve for non-connected channels
    sStr.str("");
    sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
    std::string sCh = sStr.str(); // 00:47, always positive

    for (int gn = mingain; gn < maxgain; ++gn) {

      int adc = gn & 1;

      // first sample
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_sample0";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " sample[0]";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              151,-0.5,150.5));

      // RMS over samples in event
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_meanRMS";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " mean RMS in event";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              101,-0.05,10.05));

      //all samples to find stuck bits
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_samples";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " all samples";
      histTitle = sStr.str();

      // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
      //                                              1024,-0.5,1023.5));

      // average profile for a given channel/gain
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_prof";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " average profile";
      histTitle = sStr.str();

      histP[ros][drawer][ch][adc].push_back(bookProfile(subDir, histName, histTitle, m_nSamples, 0.0, m_nSamples * 1.0, -0.5, 1023.5));

      // shifted samples histograms

      double shiftxmin = 0.;
      double shiftxmax = 796.;
      int shiftnbins = 796;

      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_samples_shifted";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " all samples shifted";
      histTitle = sStr.str();

      shifted_hist[ros][drawer][ch][adc] = book1S(subDir, histName, histTitle, shiftnbins,
          shiftxmin, shiftxmax);

      if (ch < 16) { // use first 16 channels to store BCID/CRC errors (one per TileDMU)

        sStr.str("");
        sStr << std::setfill('0') << std::setw(2) << ch << std::setfill(' ');
        std::string sDmu = sStr.str();

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << gain[gn] << "_BCID";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << gain[3 + gn] << " BCID";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
        //                                              100,2050.5,2150.5));

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << gain[gn] << "_BCID_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << gain[3 + gn] << " BCID errors";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][adc].push_back(book1S(subDir,histName,histTitle,
        //                                              3,-0.5,2.5));

        if (adc) continue; // don't book CRC for high gain

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << "_CRC_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << " CRC errors";
        histTitle = sStr.str();

        // hist1[ros][drawer][ch][0].push_back(book1S(subDir,histName,histTitle,
        //                                            5,-0.5,4.5));
      }
    }
  }


} // bookHistosOld



/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::fillHists() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in fillHists()" );

  // array of 16 CIS parameters
  m_cispar = m_beamInfo->cispar();
  ++m_nEvents;

  const TileDigitsContainer* DigitsCnt;
  CHECK( evtStore()->retrieve(DigitsCnt, "TileDigitsCnt") );

  TileDigitsContainer::const_iterator collItr = DigitsCnt->begin();
  TileDigitsContainer::const_iterator lastColl = DigitsCnt->end();

  for (; collItr != lastColl; ++collItr) {
    TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

    if (digitsItr != lastDigits) {

      //HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      //int ros    = m_tileHWID->ros(adc_id);
      //int drawer = m_tileHWID->drawer(adc_id);
      // CC 2014-07-17
      m_nSamples = (*digitsItr)->NtimeSamples();

      std::vector<uint32_t> headerVec = (*collItr)->getFragChipHeaderWords();

      int headsize = headerVec.size();
      if (headsize > 16) {
        ATH_MSG_ERROR(  "length of BCID vector " << headsize << " - greater than 16 !" );
        headsize = 16;
      }

    }
  }

  collItr = DigitsCnt->begin(); //reset iterator

  for (; collItr != lastColl; ++collItr) {

    TileDigitsCollection::const_iterator digitsItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigits = (*collItr)->end();

    if (digitsItr != lastDigits) {

      HWIdentifier adc_id = (*digitsItr)->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);

      std::vector<uint32_t> headerVec = (*collItr)->getFragChipHeaderWords();

      double mean_tmp[48][2][16];
      memset(mean_tmp, 0, sizeof(mean_tmp));

      for (; digitsItr != lastDigits; ++digitsItr) {

        adc_id = (*digitsItr)->adc_HWID();
        int chan = m_tileHWID->channel(adc_id);
        int gain = (m_bigain) ? m_tileHWID->adc(adc_id) : 0; // ignore gain in monogain run

        std::vector<float> vdigits = (*digitsItr)->samples();

        double meansamp = 0.0;
        double rmssamp = 0.0;
        unsigned int dsize = vdigits.size();
        if (dsize > 16) {
          ATH_MSG_ERROR( "length of digits vector " << dsize << " - greater than 16 !" );
          dsize = 16;
        }

        for (unsigned int i = 0; i < dsize; ++i) {
          double dig = vdigits[i];
          meansamp += dig;
          rmssamp += dig * dig;
          mean_tmp[chan][gain][i] = dig;

        }

        if (dsize > 0 && !corrup[ros][drawer][gain][chan / 3]) {
          double ped = vdigits[0];

          SumPed1[ros][drawer][chan][gain] += ped;
          SumPed2[ros][drawer][chan][gain] += ped * ped;

          if (dsize > 1) {
            meansamp /= dsize;
            rmssamp = rmssamp / dsize - meansamp * meansamp;
            rmssamp = (rmssamp > 0.0) ? sqrt(rmssamp * dsize / (dsize - 1)) : 0.0;

            SumRms1[ros][drawer][chan][gain] += rmssamp;
            SumRms2[ros][drawer][chan][gain] += rmssamp * rmssamp;
          }
        }
      } // digits

    } //check if Digits in module

  }
  // 
  // if  (  m_beamInfo->calibMode() == 0 ) 
  //   {
  //     sc = RODCRCcalc();
  //     if (sc.isFailure()) log << MSG::WARNING << "Not possible to check CRC from ROD" << endreq ;

  //   }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::finalHists() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in finalHists()" );

  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (int ros = 1; ros < 5; ++ros) {
    for (int drawer = 0; drawer < 64; ++drawer) {
      std::ostringstream sStr;
      sStr << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
      std::string moduleName = sStr.str();
      std::string subDir = "/Summary";
      std::string histName, histTitle;

      for (int gn = mingain; gn < maxgain; ++gn) {

        int adc = gn & 1;

        for (int ch = 0; ch < 48; ++ch) {

          //            int pmt = abs(m_cabling->channel2hole(ros,ch));

          double Ped = 0.0, PedRMS = 0.0, RMSHi = 0.0;

          int nevents = m_nEvents;

          if (nevents > 0) {
            Ped = SumPed1[ros][drawer][ch][adc] / nevents;
            RMSHi = SumRms1[ros][drawer][ch][adc] / nevents;

            if (nevents > 1) {
              PedRMS = SumPed2[ros][drawer][ch][adc] / nevents - Ped * Ped;
              PedRMS = (PedRMS > 0.0) ? sqrt(PedRMS * nevents / (nevents - 1)) : 0.0;
            }
          }

          // fill the 2D noise map  CC 2014-07-14
          // LFN : type=1, HFN : type=2
          final_noise_map[ros][adc][0]->Fill((float) ch + 1, (float) drawer + 1.5, PedRMS); // this is the LFN (RMS of DIGI0)
          final_noise_map[ros][adc][1]->Fill((float) ch + 1, (float) drawer + 1.5, RMSHi); // this is the HFN (MeanOf(RMS over the 7 digis) )
        } // channnel loop for (int ch = 0; ch < 48; ++ch) {
      } // gain loop                             
    } //loop over drawer
  } //loop over ros

  return StatusCode::SUCCESS;
} // finalHists()



/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO( "in checkHists()" );

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
int TileDigiNoiseMonTool::define_palette(int ncolors, int *colors) {
  /*---------------------------------------------------------*/
  Float_t alpha = 1.; // 0- transparent, 1- opaque
  Int_t ind;
  // set Deep Sea Blue palette (dark to light)
  if (ncolors == 51 && colors == 0) {
    TColor::InitializeColors();
    const int nRGBs = 5;
    Double_t stops[nRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[nRGBs] =   { 0.00, 0.09, 0.18, 0.09, 0.00 };
    Double_t green[nRGBs] = { 0.01, 0.02, 0.39, 0.68, 0.97 };
    Double_t blue[nRGBs] =  { 0.17, 0.39, 0.62, 0.79, 0.97 };
    ind = TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, m_NCont, alpha);
  } else if (ncolors == 52 && colors == 0) { // set Grey Scale palette (light to dark)

    TColor::InitializeColors();
    const int nRGBs = 3;
    Double_t stops[nRGBs] = { 0.00, 0.50, 1.00 };
    Double_t red[nRGBs] =   { 0.90, 0.50, 0.10 };
    Double_t green[nRGBs] = { 0.90, 0.50, 0.10 };
    Double_t blue[nRGBs] =  { 0.90, 0.50, 0.10 };
    ind = TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, m_NCont, alpha);
  } else
    return -1;
  return ind;
}






int TileDigiNoiseMonTool::stuckBits(TH1S* hist, int adc) {
  // function returns number of stuck bits
  int NSB = 0;

  // If all bins at given hypothetical stuck_position (0,1) and stuck_bit (0,...,9)
  // are empty then this bin is stuck in this position. Do this procedure only for
  // bins between the first and the last non-zeroth one.

  // Find first/last non-zero bin in the histogram
  int MinBin = 1;
  for (; MinBin < 1025; ++MinBin) {
    if (hist->GetBinContent(MinBin) > 0) {
      break;
    }
  }
  if (MinBin == 1025) return 0; // empty histogram, nothing to do

  int MaxBin = 1024;
  for (; MaxBin > 0; --MaxBin) {
    if (hist->GetBinContent(MaxBin) > 0) {
      break;
    }
  }

  // bins in hist are counted from 1 to 1024, but actual X value for those bins are 0-1023

  // if there is nothing in first half of histogram
  // or there is nothing in second half and there is a sharp edge at 512
  // it can be that upper most bit is stuck in 0 or 1
  if (MinBin == 513 || (MaxBin == 512 && hist->GetBinContent(MaxBin) > 3)) {
    ++NSB;
    // std::cout << "Bit 9 is stuck" << std::endl;
  }

  --MinBin; // shift to zero bin
  ++MaxBin; // shift to zero bin

  for (int hyp_pos = 0; hyp_pos < 2; ++hyp_pos) { // bit stuck at value 0 or 1
    for (int hyp_bit = 0; hyp_bit < 10; ++hyp_bit) { // which bit is stuck
      double sum = 0;
      int bin_counter = 0;

      int win_length = (1 << hyp_bit); // length of one interval with all zeros if bit is stuck
      int BeginBin = (1 - hyp_pos) * win_length; // beginning of first interval to check

      while (BeginBin + 1 < MaxBin) {
        int EndBin = BeginBin + win_length; // end of interval to check

        if (EndBin > MinBin) {
          for (int bin = BeginBin; bin < EndBin; ++bin) {
            //bin is from 0 to 1023 here - must be changed in GetBunConent to (bin+1)
            if (MinBin < (bin + 1) && (bin + 1) < MaxBin) {
              sum += hist->GetBinContent(bin + 1);
              ++bin_counter;
            }
          }
        }
        BeginBin = EndBin + win_length; // beginning of next interval
      }
      if (sum == 0 && bin_counter > 0) {
        // std::cout << "Bit "<<hyp_bit<<" is stuck at "<< hyp_pos << std::endl;
        ++NSB;
      }
    }
  }

  if ((NSB == 0) && ((m_runType == CisRun) || (m_runType == CisRamp))) {

    NSB = stuckBits_Amp(hist, adc); // if no stuck bits found, lets try another crude procedure
  }

  return NSB;
}


// New function by Tibor Zenis
// It checks how many bit flips happen compared to the expected ones
// Only works for CIS and Laser because the adc range is not sufficiently excersized 
// in the other run types.

int TileDigiNoiseMonTool::stuckBits_Amp(TH1S * hist, int /*adc*/) {
  int i, b, c, cc = 2;
  int bc[10], bv[10];
  double bs[10];
  int f; // flip-flop of non-zero contents of previous bin
  int cm, cp; // previous and next bin contents
  double prob; // probability

  for (b = 0; b < 10; b++) {
    bc[b] = 0;
    bs[b] = 0.;
    bv[b] = 1 << b;
  }

  cp = hist->GetBinContent(1);
  f = !!cp;
  for (i = 1; i < 1024; i++) {
    cm = cc;
    c = cc = cp;
    cp = (i < 1023) ? hist->GetBinContent(i + 2) : 0;
    prob = 1.;

    if (c > 0 && c < 0.125 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm))) {
      prob = 1. - (double) c / (0.125 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm)));
      c = 0;
    }

    if (c || f) // count bit flips (bc[b]) in bins with non-zero contents and adjacent bins
      for (b = 0; b < 10; b++) {
        if ((i & bv[b]) != ((i - 1) & bv[b]))
          bc[b]++;
        else
          break; // higher bit can flip only of the previous one does
      }

    if (f > 0 && c > 0) continue;
    if (f == 0 && c == 0) continue;
    if (c || f) // bin content flips (zero - non-zero)
      for (b = 0; b < 10; b++) { // count bits flipped at this position
        if ((i & bv[b]) != ((i - 1) & bv[b]))
          bs[b] += (i & bv[b]) ? (c ? prob : -prob) : (c ? -prob : prob);
        else
          break;
      }
    f = !f;
  }

  int NSB = 0;
  for (b = 0; b < 10; b++)
    // if (bc[b] > 2 && abs(bs[b]) * 2 > bc[b])
    if (((bc[b] > 2) && (abs(bs[b]) == bc[b])) || ((abs(bs[b]) > 7) && (abs(bs[b]) * 3 > bc[b]))) {
      NSB++;
    }

  return NSB;

}

/* version 2. */
int TileDigiNoiseMonTool::stuckBits_Amp2(TH1S * hist, int /*adc*/, TH2C *outhist, int ch) {

  int i, b, c, cc = 0; /* bin counter, but position, bin content */
  double cont;
  int last_non0, first_non0 = -1;
  int bc[10] = { 0 }, bv[10] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 }; // bit flips at signal regions, bit values
  double bs[10] = { 0. }; // bit flips at signal edges
  int bac[10] = { 0 }, ba0[10] = { 0 }, ba1[10] = { 0 }, bas[10] = { 0 }; // for stuck bits between first_non0, last_non0 using old technique
  int f; // flip-flop of non-zero contents of previous bin
  int cm, cp; // previous and next bin contents
  double prob; // probability
  cp = hist->GetBinContent(1);
  f = !!cp;
  if (f) first_non0 = 0;
  for (last_non0 = 1023; last_non0 >= 0; last_non0--)
    if (hist->GetBinContent(last_non0 + 1) > 0) break;
  if (last_non0 < 0) return 0;// empty histogram

  for (i = 1; i <= last_non0; i++) {
    cm = cc;
    c = cc = cp;
    cp = (i < 1023) ? hist->GetBinContent(i + 2) : 0;
    if (first_non0 < 0) {
      if (cc > 0) first_non0 = i;
      else continue;
    }

    for (b = 0; b < 10; b++) { // count bits flipped at this position
      if ((i & bv[b]) != ((i - 1) & bv[b])) {
        bac[b]++;
        if (bas[b] > 0) {
          if ((i - 1) & bv[b]) ba1[b]++;
          else ba0[b]++;
          bas[b] = 0;
        }
      }
      if (cc > 0) bas[b]++;
    }

    prob = 1.;
    if ((c == 0 || cm == 0) && (c > 0 || cm > 0)) {
      cont = sqrt((double) (c + cm));
      prob = erf((cont - sqrt(cont)) / sqrt(2 * cont));
    }

    /* bin content is lower than usual */
    cont = sqrt((double) (c + cm));
    if (c > 0 && c < 0.25 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm))) {
      prob = 1. - (double) c / (0.25 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm)));
      c = 0;
    }

    if (c || f) // count bit flips (bc[b]) in bins with non-zero contents or adjacent bins
      for (b = 0; b < 10; b++)
        if ((i & bv[b]) != ((i - 1) & bv[b]))
          bc[b]++;
        else
          break; // a higher bit can flip only of the lower one does
    if (f > 0 && c > 0) continue;
    if (f == 0 && c == 0) continue;
    if (c || f) // bin content flips (zero - non-zero)
      for (b = 0; b < 10; b++) // count bits flipped at this position
        if ((i & bv[b]) != ((i - 1) & bv[b]))
          bs[b] += (i & bv[b]) ? (c ? prob : -prob) : (c ? -prob : prob);
        else
          break;
    f = !f;
  }

  for (b = 0; b < 10; b++)  // count bits flipped at this position
    if ((i & bv[b]) != ((i - 1) & bv[b])) {
      bac[b]++;
      if (bas[b] > 0) {
        if ((i - 1) & bv[b])
          ba1[b]++;
        else
          ba0[b]++;
      }
    }

  int is_stack = 0;
  double prob_H = 0.;
  for (b = 0; b < 10; b++) {
    if ((ba0[b] == 0 || ba1[b] == 0) && bac[b] > 2
        && (ba0[b] + ba1[b] >= bac[b] / 2 || ba0[b] + ba1[b] > 2)) {
      is_stack = 1;
      if (outhist != NULL) {
        if (b < 3)
          outhist->Fill((double) ch, (double) b, 100.);
        else
          prob_H = 100.;
      }
      continue;
    }

    double bs1 = std::fabs(bs[b]) - sqrt(std::fabs(bs[b]));
    if (bs1 < 0.) bs1 = 0.;
    if ((bs1 > 0.5 * bc[b]) || (bc[b] > 7 && bs1 * 3 > bc[b])) is_stack = 1;
    if (outhist != NULL && bc[b] > 0) {
      if (b < 3)
        outhist->Fill((double) ch, (double) b, 100. * bs1 / bc[b]);
      else if (prob_H < 100. * bs1 / bc[b]) prob_H = 100. * bs1 / bc[b];
    }
  }

  if ((first_non0 >= 512 && first_non0 < 1023)
      || (last_non0 == 511 && hist->GetBinContent(last_non0) > 3)) {

    is_stack = 1;
    prob_H = 100.;
  }

  if (outhist != NULL) {
    outhist->Fill((double) ch, 3., prob_H);
    if (first_non0 == 1023)
      outhist->Fill((double) ch, 7., 100.);
    else if (last_non0 == 1023) outhist->Fill((double) ch, 6., 100.);
    if (last_non0 == 0)
      outhist->Fill((double) ch, 5., 100.);
    else if (first_non0 == 0) outhist->Fill((double) ch, 4., 100.);
    outhist->SetMaximum(100.);
  }

  return is_stack;
}




void TileDigiNoiseMonTool::CRCcheck(uint32_t crc32, uint32_t crcMask, int headsize, int ros, int drawer) {
  //! Author: Luca. F. November 06
  //! This method provides CRC checks for Global and TileDMU chips.
  //! Global CRC is taken from the trailer and it is correct if 
  //! the first 16 bits and the last 16 bits of the word match.
  //! TileDMU CRC is taken from the DMU mask filled in the FPGA.
  //! At the moment it makes sense only for monogain runs 
  //! (Physics, LED, MonoCIS). The least significant 16 bits are the result of 
  //! the CRC check in the FE (0xFFFF means all OK).  The most significant 16 bits 
  //! are the result of the CRC check in the ROD (0xFFFF means all OK).


  
  //LF CRC check
  //std::string ros_name[2] = {"LBA","LBC"};
  //std::cout << "Module= " << ros_name[ros-1] << drawer+1 << " CRCmask= 0x" <<  std::setfill('0') << std::setw(8) << std::hex <<  crcMask  << std::setfill(' ') << std::dec <<"\n";

  //END CRC check
  // array to adjust order of DMU's of EB* FE CRC status in DMU chip mask word
  // this is needed because FE and ROD fill DMU mask word in different order
  int chFE_ext[16]= { 0,1,2,3,4,5,6,7,12,13,8,9,10,11,14,15 };
  int chFE_sp[16] = { 11,0,1,2,3,4,5,6,12,13,7,8,9,10,14,15 };
  // global CRC
  //  uint32_t crc32 = (*collItr)->getFragCRC();
  uint32_t crc0 = crc32 & 0xFFFF;
  uint32_t crc1 = crc32>>16;
  // if (crc32 == 0)
  //   hist0[ros][drawer][0]->Fill(0.0,1.0);
  // else if (crc0 == crc1) // two half-words should match
  //   hist0[ros][drawer][0]->Fill(1.0,1.0);
  // else
  //   hist0[ros][drawer][0]->Fill(2.0,1.0);
  

  // CRC per TileDMU.For mono gain we have it from ROD and FE. 
  // In bi gain, it is just a placeholder with global CRC information.

  if (headsize < 16) {  // How to handle EB? All bits are filled anyway or not?
  }

  if (m_beamInfo->calibMode() == 1) { //!bigain: dummy information
    for (int ch = 0; ch < headsize; ++ch) {
      if (crc32 == 0)
        hist1[ros][drawer][ch][0][5]->Fill(0.0, 1.0);
      else if (crc0 == crc1)
        hist1[ros][drawer][ch][0][5]->Fill(1.0, 1.0);
      else
        hist1[ros][drawer][ch][0][5]->Fill(2.0, 1.0);

    }
  } else { //! monogain: meaningful information.
           //! Fill 0 if crc32 if 0
           //! Fill 1 if both FE and ROD crc is 1
           //! Fill 2 if DMU FE crc is 0
           //! Fill 3 if DMU ROD crc is 0
           //! Fill 4 is FE and ROD crc is 0
    if (crc32 == 0) {  //std::cout << "Global crc is zero\n";
      for (int ch = 0; ch < headsize; ++ch) {
        hist1[ros][drawer][ch][0][5]->Fill(0.0, 1.0);
      }
    } else if (crcMask == 0xFFFFFFFF) {
      for (int ch = 0; ch < headsize; ++ch) {
        hist1[ros][drawer][ch][0][5]->Fill(1.0, 1.0);
      }
    } else {
      uint32_t fe_crc = crcMask & 0xFFFF;
      uint32_t rod_crc = crcMask >> 16;
      for (int ch = 0; ch < headsize; ++ch) {
        int flag = 0;
        if (ros == 1 || ros == 2) // LB* use simple FECRC map
            {
          if ((fe_crc >> ch & 0x1) == 0x0) flag++;
        } else if (ros == 3 || ros == 4)  // EB* use different FECRC map
            {
          if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17)) // Special Check for EBA15,EBC18
              {
            if ((fe_crc >> chFE_sp[ch] & 0x1) == 0x0) flag++;
          } else {
            if ((fe_crc >> chFE_ext[ch] & 0x1) == 0x0) flag++;
          }
        }
        if ((rod_crc >> ch & 0x1) == 0x0) flag += 2;

        //std::cout << "flag for chan "<< ch <<  "=" << flag <<  "\n";

        switch (flag) {
          case 0: //TileDMU is fine
            hist1[ros][drawer][ch][0][5]->Fill(1.0, 1.0);
            break;
          case 1: //fe error only
            hist1[ros][drawer][ch][0][5]->Fill(2.0, 1.0);
            break;
          case 2: // rod errors
            hist1[ros][drawer][ch][0][5]->Fill(3.0, 1.0);
            break;
          default: // fe+rod errors
            hist1[ros][drawer][ch][0][5]->Fill(4.0, 1.0);
        } // end switch case

      } // end loop on chips

    } // end if on crcMask

  } // end if on runtype
}


//New Method to check for header errors. Checks per channel and returns True for data corrupted, or False for data not corrupted. Old method with relevant description is just above. Most important difference is that this is now a function of channel, so it should be called once per channel.
/*-------------------------------------------------------*/
bool TileDigiNoiseMonTool::DMUheaderCheck(std::vector<uint32_t>* headerVec, int ros, int drawer, int gain, int dmu) {
/*-------------------------------------------------------*/

  bool err = false;

  if (DMUheaderFormatCheck((*headerVec)[dmu])) {
    hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 1., 1.);
    err = true;
    return err;
  }

  if (DMUheaderParityCheck((*headerVec)[dmu])) {
    hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 2., 1.);
    err = true;
    return err;
  }

  if (((*headerVec)[dmu] >> 25) & 0x1) {
    //Memory Parity Error
    hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 3., 1.);
    err = true;
  }

  if (((*headerVec)[dmu] >> 24) & 0x1) {
    //Single Strobe Error
    hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 5., 1.);
    err = true;
  }

  if (((*headerVec)[dmu] >> 23) & 0x1) {
    //Double Strobe Error
    hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 0., 1.);
    err = true;
  }

  if (!err) hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 0., 1.);
  return err;
}

StatusCode TileDigiNoiseMonTool::RODCRCcalc() {

  //! Author: Luca. F. Mar 08
  //! This method provides CRC checks for Global and TileDMU chips.
  //! They are taken from the the ROD RawChannel Container
  //! TileDMU CRC is taken from the DMU mask filled in the FPGA.
  //! It makes sense only for monogain runs 
  //! (Physics, LED, MonoCIS). The least significant 16 bits are the result of 
  //! the CRC check in the FE (0xFFFF means all OK).  The most significant 16 bits 
  //! are the result of the CRC check in the ROD (0xFFFF means all OK).
  //! Global CRC is taken also from the DQ Fragment. '0' means OK,
  //! '1' means mismatch.


  const TileRawChannelContainer* RawChannelCnt;
  CHECK( evtStore()->retrieve(RawChannelCnt, m_contNameDSP) );

  TileRawChannelContainer::const_iterator collItr=RawChannelCnt->begin();
  TileRawChannelContainer::const_iterator lastColl=RawChannelCnt->end();
  
  for (; collItr != lastColl; ++collItr) {

    TileRawChannelCollection::const_iterator chItr = (*collItr)->begin();
    TileRawChannelCollection::const_iterator lastCh = (*collItr)->end();

    if (chItr != lastCh) {

      HWIdentifier hwid = (*chItr)->adc_HWID(); //take the first channel in the drawer
      int ros = m_tileHWID->ros(hwid);  //take the ros and drawer from the first channel
      int drawer = m_tileHWID->drawer(hwid);

      uint32_t crc32 = (*collItr)->getFragGlobalCRC() & 1;

      if (crc32 == 0) {
        crc32 = 0xFFFFFFFF;
      } // means OK CRC match
      else {
        crc32 = 0xFFFF;
      } //means NOT OK, CRC mismatch
      uint32_t CRCmask = (*collItr)->getFragRODChipMask();
      CRCmask = CRCmask << 16; // ROD mask is stored in the 16 most significant bits ce
      CRCmask += (*collItr)->getFragFEChipMask();

      CRCcheck(crc32, CRCmask, 16, ros, drawer); //reuse the same funcion used for digits
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileDigiNoiseMonTool::shiftHisto(TH1S *hist, int ros, int drawer, int ch, int gain) {
/*---------------------------------------------------------*/


  hist->GetXaxis()->SetRange(5, 100);

  TSpectrum s(8, 1.);

  s.Search(hist, 2, "goff");
  int shift = int(s.GetPositionX()[0]);
  if (shift > 0) {
    int xmax = std::max(1025, 846 + shift);
    int xmin = std::max(1, shift + 50);
    for (int bin = xmin; bin < xmax; ++bin) {
      double c = hist->GetBinContent(bin);
      if (c > 0) shifted_hist[ros][drawer][ch][gain]->SetBinContent(bin - shift - 50, c);
    }
  }
}

 

/*---------------------------------------------------------*/
void TileDigiNoiseMonTool::statTestHistos(int ros, int drawer, int gain) {
/*---------------------------------------------------------*/

  std::vector<TH1S*> refbld;
  std::vector<TH1S*> newrefbld;
  TH1F *ref = new TH1F("ref", "ref", 796, 0., 796.);
  ref->SetDirectory(0);
  TH1F *ref1 = new TH1F("ref1", "ref1", 796, 0., 796.);
  ref1->SetDirectory(0);

  for (int i = 0; i < 48; i++) {

    TH1S *h = shifted_hist[ros][drawer][i][gain];
    float integ = h->Integral(200, 600);
    if (integ > 0) {
      refbld.push_back(h);
    }

  }

  int entr = refbld.size();
  for (int i = 0; i < entr; i++) {
    TH1S *h = refbld.at(i);
    ref->Add(h, 1. / entr);
  }

  //      refbld.push_back(ref);
  TH1S *obj = 0;

  for (int i = 0; i < entr; i++) {
    obj = (TH1S*) refbld.at(i);
    float kol = obj->KolmogorovTest(ref);
    //std::cout<<"The value of the Kolmogorov test for "<< ros << drawer << gain << " pmt:" << i << " is "<< kol << std::endl;
    if (kol > 0.5) {
      newrefbld.push_back(obj);
    }
  }

  int ent = newrefbld.size();
  for (int i = 0; i < ent; i++) {
    obj = (TH1S*) newrefbld.at(i);
    ref1->Add(obj, 1. / ent);
  }

  //  newrefbld->Add(ref1);      

  if (ent > 6)
    shifted_hist[ros][drawer][48][gain]->Add(ref1, 1.);
  else
    shifted_hist[ros][drawer][48][gain]->Add(ref, 1.);

  delete ref;
  delete ref1;

}
