/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileDigitsMonTool.cxx
// PACKAGE:  
//
// AUTHOR:   Alexander Solodkov
//	     Luca.Fiorini@cern.ch
//
// ********************************************************************

#include "TileMonitoring/TileDigitsMonTool.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

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
#include "TTree.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>



/*---------------------------------------------------------*/
TileDigitsMonTool::TileDigitsMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TilePaterMonTool(type, name, parent)
  , m_beamInfo("TileBeamInfoProvider")
  , m_tileToolNoiseSample("TileCondToolNoiseSample")
  , m_cispar(0)
  , m_nEvents(0)
  , m_nSamples(0)
  , m_sumPed1{}
  , m_sumPed2{}
  , m_sumRms1{}
  , m_sumRms2{}
  , m_meanAmp{}
  , m_meanAmp_ij{}
  , m_nEvents_i{}
  , m_nEvents_ij{}
  , m_cov_ratio{}
  , m_stuck_probs{}
  , m_allHistsFilled(false)
  //, hp(-1)
  //, hb(-1)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);
  declareInterface<ITileStuckBitsProbsTool>(this);

  declareProperty("bookAllDrawers", m_bookAll = false);
  declareProperty("book2D", m_book2D = false);

  // run type 1 - phys, 2 - las, 4 - ped, 8 - cis, 9- mono
  declareProperty("runType", m_runType = 8);
  declareProperty("bigain", m_bigain = true);
  declareProperty("TileRawChannelContainerDSP", m_contNameDSP = "TileRawChannelCnt");
  declareProperty("TileDigitsContainer", m_digitsContainerName = "TileDigitsCnt");
  declareProperty("FillPedestalDifference", m_fillPedestalDifference = true);

  m_path = "/Tile/Digits"; //ROOT file directory
}

/*---------------------------------------------------------*/
TileDigitsMonTool::~TileDigitsMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TileDigitsMonTool::initialize()
/*---------------------------------------------------------*/
{
  ATH_MSG_INFO("in initialize()");

  CHECK(m_beamInfo.retrieve());

  if (m_fillPedestalDifference) CHECK(m_tileToolNoiseSample.retrieve());

  m_nEvents = 0;
  m_allHistsFilled = false;
  memset(m_sumPed1, 0, sizeof(m_sumPed1));
  memset(m_sumPed2, 0, sizeof(m_sumPed2));
  memset(m_sumRms1, 0, sizeof(m_sumRms1));
  memset(m_sumRms2, 0, sizeof(m_sumRms2));
  memset(m_meanAmp, 0, sizeof(m_meanAmp));
  memset(m_meanAmp_ij, 0, sizeof(m_meanAmp_ij));
  memset(m_nEvents_i, 0, sizeof(m_nEvents_i));
  memset(m_nEvents_ij, 0, sizeof(m_nEvents_ij));
  memset(m_stuck_probs, 0, sizeof(m_stuck_probs));

  //For test stuck_bits_maker
  //hp = 1;
  //hb = -1;

  return TilePaterMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileDigitsMonTool::bookHists()
/*---------------------------------------------------------*/
{

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "in bookHists()" << endmsg;
    msg(MSG::DEBUG) << "Using base path " << m_path << endmsg;
  }

  if (m_bookAll) {
    for (int ros = 1; ros < 5; ++ros) {
      for (int drawer = 0; drawer < 64; ++drawer) {
        bookHists(ros, drawer);
      }
    }
  }

  m_outInHighGain.push_back(book1D("Summary", "OverFlowInHighGain", "Injected charge with overflow in high gain", 10010, -0.5, 1000.5));

  //SetBookStatus(true);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileDigitsMonTool::bookHists(int ros, int drawer)
/*---------------------------------------------------------*/
{
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

  MsgStream log(msgSvc(), name());

  std::ostringstream sStr;
  sStr << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
  std::string moduleName = sStr.str();
  std::string subDir = moduleName;
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

    m_hist2[ros][drawer][adc].push_back(book2F(subDir, histName, histTitle, 16, 0., 16., 8, -0.5, 7.5));
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

      m_hist1[ros][drawer][ch][adc].push_back(book1S(subDir, histName, histTitle, 151, -0.5, 150.5));

      // RMS over samples in event
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_meanRMS";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " mean RMS in event";
      histTitle = sStr.str();

      m_hist1[ros][drawer][ch][adc].push_back(book1S(subDir, histName, histTitle, 101, -0.05, 10.05));

      //all samples to find stuck bits
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_samples";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " all samples";
      histTitle = sStr.str();

      m_hist1[ros][drawer][ch][adc].push_back(book1S(subDir, histName, histTitle, 1024, -0.5, 1023.5));

      // average profile for a given channel/gain
      sStr.str("");
      sStr << moduleName << "_ch_" << sCh << gain[gn] << "_prof";
      histName = sStr.str();

      sStr.str("");
      sStr << moduleName << " CH " << ch << gain[3 + gn] << " average profile";
      histTitle = sStr.str();

      m_histP[ros][drawer][ch][adc].push_back(bookProfile(subDir, histName, histTitle, m_nSamples, 0.0, m_nSamples * 1.0, -0.5, 1023.5));

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

      m_shifted_hist[ros][drawer][ch][adc] = book1S(subDir, histName, histTitle, shiftnbins, shiftxmin, shiftxmax);

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

        m_hist1[ros][drawer][ch][adc].push_back(book1S(subDir, histName, histTitle, 100, 2050.5, 2150.5));

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << gain[gn] << "_BCID_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << gain[3 + gn] << " BCID errors";
        histTitle = sStr.str();

        m_hist_DMUerr[ros][drawer][ch][adc].push_back(book1I(subDir, histName, histTitle, 3, -0.5, 2.5));

        if (adc) continue; // don't book CRC for high gain

        sStr.str("");
        sStr << moduleName << "_dmu_" << sDmu << "_CRC_err";
        histName = sStr.str();

        sStr.str("");
        sStr << moduleName << " DMU0 " << ch << " CRC errors";
        histTitle = sStr.str();

        m_hist_DMUerr[ros][drawer][ch][0].push_back(book1I(subDir, histName, histTitle, 5, -0.5, 4.5));
      }
    }
  }

// and now global histograms for all channels in a drawer

  // global CRC
  sStr.str("");
  sStr << moduleName << "_CRC_global";
  histName = sStr.str();

  sStr.str("");
  sStr << moduleName << " global CRC errors";
  histTitle = sStr.str();

  m_hist0[ros][drawer].push_back(book1S("Summary", histName, histTitle, 3, -0.5, 2.5));

}

/*---------------------------------------------------------*/
StatusCode TileDigitsMonTool::fillHists()
/*---------------------------------------------------------*/
{
//  static counter = 0;
//  bool extra = ((counter%m_updateCounter) == 0);
//  ++counter;

  ATH_MSG_DEBUG("in fillHists()");

  // array of 16 CIS parameters
  m_cispar = m_beamInfo->cispar();
  ++m_nEvents;

  //  std::cout << "Calib Mode=" << m_beamInfo->calibMode() << "\n";

  const TileDigitsContainer* digitsContainer;
  CHECK(evtStore()->retrieve(digitsContainer, m_digitsContainerName));

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

    if (digitsCollection->empty()) continue;

    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);

    if (m_hist1[ros][drawer][0][0].size() == 0) {
      m_bigain = true;
      m_nSamples = digitsCollection->front()->NtimeSamples();
      if (!m_bookAll) bookHists(ros, drawer);
    }
    
    uint32_t status = digitsCollection->getFragStatus();
    if (status != TileFragStatus::ALL_OK) {
      float bin = 99.;
      if (status & (TileFragStatus::ALL_FF || TileFragStatus::ALL_00)) {
        bin = 6.;
      } else if (status & (TileFragStatus::NO_FRAG || TileFragStatus::NO_ROB)) {
        bin = 7.;
      }
      for (int dmu = 0; dmu < 16; dmu++) {
        m_corrup[ros][drawer][0][dmu] = true;
        m_corrup[ros][drawer][1][dmu] = true;
        m_hist2[ros][drawer][0][0]->Fill(dmu + 0.5, bin, 1.);
        m_hist2[ros][drawer][1][0]->Fill(dmu + 0.5, bin, 1.);
      }
      continue;
    }

    std::vector<uint32_t> headerVec = digitsCollection->getFragChipHeaderWords();

    int headsize = headerVec.size();
    if (headsize > 16) {
      ATH_MSG_ERROR("length of BCID vector " << headsize << " - greater than 16 !");
      headsize = 16;
    }
    
    for (int dmu = 0; dmu < headsize; dmu++) {
      m_corrup[ros][drawer][0][dmu] = DMUheaderCheck(&headerVec, ros, drawer, 0, dmu); //tests and fills
      m_corrup[ros][drawer][1][dmu] = DMUheaderCheck(&headerVec, ros, drawer, 1, dmu);
    }
  
    for (int dmu = headsize; dmu < 16; dmu++) {
      m_corrup[ros][drawer][0][dmu] = false;
      m_corrup[ros][drawer][1][dmu] = false;
    }
  }

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

    if (digitsCollection->empty()) continue;

    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);

    if (m_hist1[ros][drawer][0][0].size() == 0) {
      //m_bigain = (m_beamInfo->calibMode() == 1); // true if bi-gain run
      // For the time being, we fill both high and low gain plots, as it was requiered by Tomas
      m_bigain = true;
      m_nSamples = digitsCollection->front()->NtimeSamples(); // number of samples
      //        bookHists(ros,drawer);
    }
    
    std::vector<uint32_t> headerVec = digitsCollection->getFragChipHeaderWords();
    
    int headsize = headerVec.size();
    //if (headsize > 16 ) {
    //   log << MSG::ERROR << "legth of BCID vector " << headsize << " - greater than 16 !" << endreq;
    //   headsize = 16;
    //}
    
    double mean_tmp[48][2][16];
    memset(mean_tmp, 0, sizeof(mean_tmp));
    
    double charge = m_cispar[6] * m_cispar[7] * (2. * 4.096 / 1023.);
    
    for (const TileDigits* tileDigits : *digitsCollection) {

      adc_id = tileDigits->adc_HWID();
      int chan = m_tileHWID->channel(adc_id);
      int gain = (m_bigain) ? m_tileHWID->adc(adc_id) : 0; // ignore gain in monogain run
      
      std::vector<float> vdigits = tileDigits->samples();
      
      double meansamp = 0.0;
      double rmssamp = 0.0;
      unsigned int dsize = vdigits.size();
      if (dsize > 16) {
        ATH_MSG_ERROR("length of digits vector " << dsize << " - greater than 16 !");
        dsize = 16;
      }
      //std::cout << "m_runType= "<< m_runType << "\n";
      for (unsigned int i = 0; i < dsize; ++i) {
        double dig = vdigits[i];
        meansamp += dig;
        rmssamp += dig * dig;
        mean_tmp[chan][gain][i] = dig;
        
        if (!m_corrup[ros][drawer][gain][chan / 3]) {
          if (m_runType != CisRun) {
            m_histP[ros][drawer][chan][gain][0]->Fill(i + 0.1, dig, 1.0);
            m_hist1[ros][drawer][chan][gain][2]->Fill(dig, 1.0);
          } else if (m_cispar[6] > 0. && (dig > 0 || gain < 1 || charge < 12.)) { //LF If CIS run, Protection to avoid zero amplitudes due to 0 injected charge
            m_histP[ros][drawer][chan][gain][0]->Fill(i + 0.1, dig, 1.0);
            m_hist1[ros][drawer][chan][gain][2]->Fill(dig, 1.0);
          }
          
          if (gain == 1 && dig > 1022.5) // overflow in high gain, find charge is it
            m_outInHighGain[0]->Fill(charge);
        }
      }

      if (dsize > 0 && !m_corrup[ros][drawer][gain][chan / 3]) {
        double ped = vdigits[0];
        m_hist1[ros][drawer][chan][gain][0]->Fill(ped, 1.0);
        m_sumPed1[ros][drawer][chan][gain] += ped;
        m_sumPed2[ros][drawer][chan][gain] += ped * ped;
        //if ( (chan==23)&&(ros==1)&&(drawer==18)&&(gain==0)) {
        //std::cout << "ped=" << ped << "\tm_sumPed1=" << m_sumPed1[ros][drawer][chan][gain] << "\n";
        //std::cout << "ped^2=" << ped*ped << "\tm_sumPed2=" << m_sumPed2[ros][drawer][chan][gain] << "\n";
        //}
        if (dsize > 1) {
          meansamp /= dsize;
          rmssamp = rmssamp / dsize - meansamp * meansamp;
          rmssamp = (rmssamp > 0.0) ? sqrt(rmssamp * dsize / (dsize - 1)) : 0.0;
          m_hist1[ros][drawer][chan][gain][1]->Fill(rmssamp, 1.0);
          m_sumRms1[ros][drawer][chan][gain] += rmssamp;
          m_sumRms2[ros][drawer][chan][gain] += rmssamp * rmssamp;
        }
      }
    } // digits
    
      //For cor&cov
    for (int sample = 0; sample < m_nSamples; ++sample) {
      for (int gain = 0; gain < 2; ++gain) {
        for (int ch_i = 0; ch_i < 48; ++ch_i)
	  if (!m_corrup[ros][drawer][gain][ch_i / 3]) {
	    m_nEvents_i[ros][drawer][gain][ch_i]++;
	    m_meanAmp[ros][drawer][gain][ch_i] += mean_tmp[ch_i][gain][sample];
	    for (int ch_j = 0; ch_j < 48; ++ch_j)
	      if (!m_corrup[ros][drawer][gain][ch_j / 3]) {
		m_nEvents_ij[ros][drawer][gain][ch_i][ch_j]++;
		m_meanAmp_ij[ros][drawer][gain][ch_i][ch_j] += mean_tmp[ch_i][gain][sample] * mean_tmp[ch_j][gain][sample];
	      }
	  }
      }
    }
    
    // BCID
    uint32_t bcid = digitsCollection->getRODBCID();
    
    for (int ch = 0; ch < headsize; ++ch) {
      uint32_t bcid_ch = (headerVec[ch] & 0xFFF);
      m_hist1[ros][drawer][ch][0][3]->Fill(bcid_ch, 1.0);
      if ((bcid_ch == bcid) || (bcid_ch == bcid - 1)) 	// Conservative condition to be consistent with both run before Feb07 and
        m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(1.0, 1.0);  	// runs after Feb07. Introducing a RunNum variable it could be more strict.
      else if ((bcid == 0) && ((bcid_ch == 3563) || (bcid_ch == 3564)))	// if bcid==0 then bcid_ch should be 3563 (0xDEB)
        m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(1.0, 1.0);			// but sometimes 3564 (0xDEC) is observed.
      //        if (bcid_ch == bcid) 				// Now allow only exact bcid: ROD BCID = DMU BCID+1
      //          m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(1.0,1.0);  	// 	Apr 2013
      //        else if (bcid_ch == 0)
      //          m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(0.0,1.0);
      //        else 
      //          m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(2.0,1.0);
    }
    
    //DMUheaderCheck(&headerVec,headsize,ros,drawer,0); 
    
    if ((m_bigain) && ((digitsCollection->getFragChipHeaderWordsHigh()).size() > 0)) {// LF we need to avoid that headsize is set to zero in monogain runs
      
      headerVec = digitsCollection->getFragChipHeaderWordsHigh();
      headsize = headerVec.size();
      if (headsize > 16) {
        ATH_MSG_ERROR("length of BCIDhigh vector " << headsize << " - greater than 16 !");
        headsize = 16;
      }
    }
    // if monogain run with m_bigain==1, we fill the BCID plots we the same info
    for (int ch = 0; ch < headsize; ++ch) {
      uint32_t bcid_ch = (headerVec[ch] & 0xFFF);
      m_hist1[ros][drawer][ch][1][3]->Fill(bcid_ch, 1.0);
      if ((bcid_ch == bcid) || (bcid_ch == bcid - 1))  		// BCID from TileDMU should match BCID from ROD header or
        m_hist_DMUerr[ros][drawer][ch][1][0]->Fill(1.0, 1.0);        		// bcid-1 for runs after Feb07.
      else if ((bcid == 0) && ((bcid_ch == 3563) || (bcid_ch == 3564)))	// if bcid==0 then bcid_ch should be 3563 (0xDEB)
        m_hist_DMUerr[ros][drawer][ch][1][0]->Fill(1.0, 1.0);			// but sometimes 3564 (0xDEC) is observed.
      //        if (bcid_ch == bcid) 				// Now allow only exact bcid
      //          m_hist_DMUerr[ros][drawer][ch][0][0]->Fill(1.0,1.0);  	// 	Apr 2013
      //	  else if (bcid_ch == 0)
      //	    m_hist_DMUerr[ros][drawer][ch][1][0]->Fill(0.0,1.0);
      //	  else
      //	    m_hist_DMUerr[ros][drawer][ch][1][0]->Fill(2.0,1.0);
    }
    
    //DMUheaderCheck(&headerVec,headsize,ros,drawer,1);
    
    if (m_beamInfo->calibMode() == 1) {
      // global and DMU CRC check
      uint32_t crc32 = digitsCollection->getFragCRC();
      uint32_t CRCmask = digitsCollection->getFragDMUMask();
      
      CRCcheck(crc32, CRCmask, headsize, ros, drawer);
    }
  } //loop over drawers
  // 
 
  if (m_beamInfo->calibMode() == 0) {
    if (RODCRCcalc().isFailure()) ATH_MSG_WARNING("Not possible to check CRC from ROD");
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileDigitsMonTool::finalHists()
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in finalHists()");
  if (m_allHistsFilled) return StatusCode::SUCCESS;
  m_allHistsFilled = true;

  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "_lo", "_hi", "", " low gain", " high gain", "" };

  std::string pedestalTitile(" Pedestal[0]");
  if (m_fillPedestalDifference) pedestalTitile += " - pedestal from DB";

  const char *HistName[8] = { "_ped", "_rms_lfr", "_rms_hfr", "_bits"
                              , pedestalTitile.c_str(), " RMS noise low frequency", " RMS noise high frequency", " Stuck bits and zero amp" };

  const char *ErrName[4] = { "_bcid", "_crc", " BCID errors", " CRC errors" };
  const char *HistName2[4] = { "_covar", "_corr", " covariance", " correlation" };

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer) {
      if (m_hist1[ros][drawer][0][0].size() != 0) {
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

        std::ostringstream sStr;
        std::string moduleName = TileCalibUtils::getDrawerString(ros, drawer);
        std::string subDir = "Summary";
        std::string histName, histTitle;

        for (int gn = mingain; gn < maxgain; ++gn) {

          int adc = gn & 1;

          for (int type = 0; type < 4; ++type) {
            sStr.str("");
            sStr << moduleName << gain[gn] << HistName[type];
            histName = sStr.str();
            sStr.str("");
            sStr << moduleName << gain[3 + gn] << HistName[4 + type];
            histTitle = sStr.str();
            m_final_hist1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 48, 0.0, 48.0));
          }

          // stuck bits and saturations / 0s
          sStr.str("");
          sStr << moduleName << gain[gn] << "_stucks";
          histName = sStr.str();
          sStr.str("");
          sStr << moduleName << gain[3 + gn] << " Stuck bits and saturation";
          histTitle = sStr.str();
          m_final_hist_stucks[ros][drawer][adc] = book2C(subDir, histName, histTitle, 48, 0.0, 48.0, 6, 0., 6.);
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(1, "SB 0");
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(2, "SB 1,2");
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(3, "SB 3,4");
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(4, "SB 5-9");
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(5, "zeros");
          m_final_hist_stucks[ros][drawer][adc]->GetYaxis()->SetBinLabel(6, "saturation");

          for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

            //            int pmt = abs(m_cabling->channel2hole(ros,channel));

            double Ped = 0.0, PedRMS = 0.0, RMSHi = 0.0;

            int nevents = int(m_hist1[ros][drawer][channel][adc][0]->GetEntries()); //ROOT GetEntries return a Double_t.
            bool fillPedAndRms = (nevents > 0);

            if ((nevents != 0) && (nevents != m_nEvents)) {
              /// LF: when we have a monogain run, like Laser, it might happen
              /// that occasionally some entries have not the expected gain.
              /// In this case we cannot use m_nEvents to calculate average and
              /// RMS. The most simple minded solution would be to use the
              /// a counter for each single chan (5x64x48x2 counters!).
              /// I prefered the following 'escamotage': the pedestal
              /// histograms already have the correct number of entries and,
              /// unless there are some overflow, even the correct mean and RMS.
              /// So, if  m_nEntries doesn't match the entries in the histo,
              /// I use the latter for calculation porpouse. I could even use
              /// the histogram mean and RMS, but it is not necessary and I
              /// avoid problems in case of overflows.
              ATH_MSG_VERBOSE( "Number of entries in histo " << m_hist1[ros][drawer][channel][adc][0]->GetTitle()
                              << " doesnt match n. of events! Using the first one in mean and RMS calculation" );

              ATH_MSG_VERBOSE( "Number of entries in histo =" << nevents << "\tNumber of events= " << m_nEvents );

            } else {	      //all ok
              nevents = m_nEvents;
            }

            if (fillPedAndRms) {

              if (nevents > 0) {
                Ped = m_sumPed1[ros][drawer][channel][adc] / nevents;
                RMSHi = m_sumRms1[ros][drawer][channel][adc] / nevents;
                
                if (nevents > 1) {
                  PedRMS = m_sumPed2[ros][drawer][channel][adc] / nevents - Ped * Ped;
                  PedRMS = (PedRMS > 0.0) ? sqrt(PedRMS * nevents / (nevents - 1)) : 0.0;
                }
              }
              
              //	    if ( (pmt==24)&&(ros==1)&&(drawer==18)&&(adc==0)) {
              //std::cout << "Evt = " << m_nEvents << "\t channel=" << channel << "  pmt=" << pmt << "\n";
              //std::cout << "Ped = " << Ped << "\n";
              //std::cout << "E(x^2) = " << m_sumPed2[ros][drawer][channel][adc] << "\n";
              //std::cout << "PedRMS = " << PedRMS << "\n";
              
              //}
              
              if (m_fillPedestalDifference) {

                if (!isDisconnected(ros, drawer, channel)) {
                  m_final_hist1[ros][drawer][adc][0]->SetBinContent(channel + 1, Ped - m_tileToolNoiseSample->getPed(drawerIdx, channel, adc));
                  m_final_hist1[ros][drawer][adc][0]->SetBinError(channel + 1, PedRMS);
                }

              } else {
                m_final_hist1[ros][drawer][adc][0]->SetBinContent(channel + 1, Ped);
                m_final_hist1[ros][drawer][adc][0]->SetBinError(channel + 1, PedRMS);
              }
              
              m_final_hist1[ros][drawer][adc][1]->SetBinContent(channel + 1, PedRMS);
              m_final_hist1[ros][drawer][adc][2]->SetBinContent(channel + 1, RMSHi);
              
            }

            // For tests
            //stuckBits_maker(m_hist1[ros][drawer][channel][adc][2]);

            TH1S * hist = m_hist1[ros][drawer][channel][adc][2];
            double weight = 0.0;
            for (int i = 2; i < 1025; ++i) { // first bin is excluded
              weight += hist->GetBinContent(i);
            }
            // if we have weight=0 after the loop, then there is no non-zero amplitudes
            if (weight > 0.0) {
              if (hist->GetBinContent(1) > 0) {
                weight = 0.5; // sometimes zeros
              } else {
                weight = 1.0; // assume that all OK
              }

              int stuck_bit = stuckBits(hist, adc);
              if (stuck_bit > 0) {
                weight += 1.0; // some stuck bits
              }
            }
            m_final_hist1[ros][drawer][adc][3]->SetBinContent(channel + 1, weight);
            stuckBits_Amp2(hist, adc, m_final_hist_stucks[ros][drawer][adc], channel, m_stuck_probs[ros][drawer][channel][adc]);
          } // end of loop over channels

          // BCID
          sStr.str("");
          sStr << moduleName << gain[gn] << ErrName[0];
          histName = sStr.str();
          sStr.str("");
          sStr << moduleName << gain[3 + gn] << ErrName[2];
          histTitle = sStr.str();
          m_final_hist1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 16, 0.0, 16.0));

          // CRC only for one gain
          if (adc == 0) {
            sStr.str("");
            sStr << moduleName << ErrName[1];
            histName = sStr.str();
            sStr.str("");
            sStr << moduleName << ErrName[3];
            histTitle = sStr.str();
            m_final_hist1[ros][drawer][adc].push_back(book1F(subDir, histName, histTitle, 16, 0.0, 16.0));
          }

          for (int ch = 0; ch < 16; ++ch) {

            int dmu = ch + 1;

            // BCID and CRC errors
            for (int id = 0; id < 2; ++id) {
              int bin0 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(0));
              int bin1 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(1));
              int bin2 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(2));
              int bin3 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(3));
              int bin4 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(4));
              int bin5 = lround(m_hist_DMUerr[ros][drawer][ch][adc][id]->GetBinContent(5));
              double weight = -1.0;

              if (bin0 + bin3 + bin4 + bin5 > 0) {
                if ((bin4 == 0) && (bin5 == 0)) {
                  weight = 2.0; // BCID: all other errors; CRC: FE only errors
                } else if ((bin3 == 0) && (bin5 == 0)) {
                  weight = 3.0; // CRC only: ROD only errors
                } else { // if there are ROD and FE errors (not necessary at the same time),
                         //weight is 2.5
                  weight = 2.5; // CRC only: ROD and FE errors
                }
              } else {
                if (bin2 == 0) {
                  weight = 0.0; // all zeros
                } else if (bin1 == 0) {
                  weight = 1.0; // all OK
                } else {
                  weight = 1.0 - (double) bin1 / m_nEvents; // percentage of good events
                  if (weight > 0.8) weight = 0.8; // to see clearly even one event with zeros
                }
              }
              m_final_hist1[ros][drawer][adc][id + 4]->SetBinContent(dmu, weight);

              if (adc) break; // no CRC histogram in high gain
            }
          } // end of loop over 16 DMU

          //Correlation and Covariance
          if (m_runType == PedRun) {

            for (int type = 0; type < 2; ++type) {
              sStr.str("");
              sStr << moduleName << gain[gn] << HistName2[type];
              histName = sStr.str();
              sStr.str("");
              sStr << moduleName << gain[3 + gn] << HistName2[2 + type];
              histTitle = sStr.str();
              m_final_hist2[ros][drawer][adc].push_back(book2F(subDir, histName, histTitle, 48, 0.0, 48.0, 48, 0.0, 48.0));
            }

            if (m_nEvents * m_nSamples > 0) {

              for (int ch_i = 0; ch_i < 48; ++ch_i) {
		if (m_nEvents_i[ros][drawer][adc][ch_i] > 0)
		  m_meanAmp[ros][drawer][adc][ch_i] /= m_nEvents_i[ros][drawer][adc][ch_i];
                for (int ch_j = 0; ch_j < 48; ++ch_j)
		  if (m_nEvents_ij[ros][drawer][adc][ch_i][ch_j] > 0)
		    m_meanAmp_ij[ros][drawer][adc][ch_i][ch_j] /= m_nEvents_ij[ros][drawer][adc][ch_i][ch_j];
              }

              double covar[48][48];
              double corr[48][48];
              double mean_rms[48];
              double mean_cov_ii = 0.; // mean values of covar in diag terms
              double mean_cov_ij = 0.;  // mean values of covar in off diag terms

              for (int ch_i = 0; ch_i < 48; ++ch_i) {
                //                int pmt_i = abs(m_cabling->channel2hole(ros,ch_i))-1;
                for (int ch_j = 0; ch_j < 48; ++ch_j) {
                  //                  int pmt_j = abs(m_cabling->channel2hole(ros,ch_j))-1;
                  covar[ch_i][ch_j] = m_meanAmp_ij[ros][drawer][adc][ch_i][ch_j] - m_meanAmp[ros][drawer][adc][ch_i] * m_meanAmp[ros][drawer][adc][ch_j];

                  if (ch_j < ch_i) {
                    mean_cov_ij += covar[ch_i][ch_j]; //LF: we take C_ij with its sign
                  }
                }

                mean_cov_ii += covar[ch_i][ch_i];

                if (covar[ch_i][ch_i] != 0.0)
                  mean_rms[ch_i] = covar[ch_i][ch_i] / sqrt(TMath::Abs(covar[ch_i][ch_i]));
                else
                  mean_rms[ch_i] = 0.;
              }

              if (mean_cov_ii != 0.) {
                m_cov_ratio[ros][drawer][adc] = (2. * mean_cov_ij) / (mean_cov_ii * 47.); //(2*cov_ij/(48*47))/(cov_ii/48)
              } else {
                m_cov_ratio[ros][drawer][adc] = 0.;
              }
              //	      std::cout << "m_cov_ratio["<<adc<<"]= " <<  m_cov_ratio[ros][drawer][adc] << "\n";

              for (int i = 0; i < 48; ++i) {
                for (int j = 0; j < 48; ++j) {
                  if ((mean_rms[i] == 0.) || (mean_rms[j] == 0.)) {
                    corr[i][j] = 0.;
                  } else {
                    corr[i][j] = covar[i][j] / mean_rms[i] / mean_rms[j];
                  }
                  m_final_hist2[ros][drawer][adc][0]->SetBinContent(i + 1, j + 1, covar[i][j]);
                  m_final_hist2[ros][drawer][adc][1]->SetBinContent(i + 1, j + 1, corr[i][j]);
                }
              }
            } // end if m_nEvents > 0

          } // end if PedRun
        } // end of loop over gn

        drawHists(ros, drawer, moduleName);

        //if RunType is Cis, the Kolmogorov test of the sample distribution is performed
        if (m_runType == CisRun) {

          for (int gn = mingain; gn < maxgain; ++gn) {
            for (int i = 0; i < 48; ++i) {
              TH1S *h = (TH1S*) m_hist1[ros][drawer][i][gn][2]->Clone("temphist");
              h->SetDirectory(0);
              shiftHisto(h, ros, drawer, i, gn);
              delete h;
            }
            double shiftxmin = 0.;
            double shiftxmax = 796.;
            int shiftnbins = 796;

            std::ostringstream s;
            s << part[ros] << std::setfill('0') << std::setw(2) << drawer + 1 << std::setfill(' ');
            std::string moduleName = s.str();
            std::string subDir = moduleName;

            s.str("");
            s << moduleName << gain[gn] << "_samples_ref";
            std::string histName = s.str();

            s.str("");
            s << moduleName << gain[3 + gn] << " all samples reference";
            std::string histTitle = s.str();

            m_shifted_hist[ros][drawer][48][gn] = book1S(subDir, histName, histTitle, shiftnbins, shiftxmin, shiftxmax);

            statTestHistos(ros, drawer, gn);
          } // if runTYpe==CisRun
        }
      }

    } //loop over drawer
  } //loop over ros


  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
const uint8_t * TileDigitsMonTool::stuckBitProb (int ros, int module, int channel, int gain) const
/*---------------------------------------------------------*/
{
  return m_stuck_probs[ros][module][channel][gain];
}


/*---------------------------------------------------------*/
StatusCode TileDigitsMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{

  ATH_MSG_INFO("in checkHists()");

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
int TileDigitsMonTool::define_palette(int ncolors, int *colors) {
  /*---------------------------------------------------------*/
  Float_t alpha = 1.; // 0- transparent, 1- opaque
  Int_t ind;
  // set Deep Sea Blue palette (dark to light)
  if (ncolors == 51 && colors == 0) {
    TColor::InitializeColors();
    const int nRGBs = 5;
    Double_t stops[nRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[nRGBs] = { 0.00, 0.09, 0.18, 0.09, 0.00 };
    Double_t green[nRGBs] = { 0.01, 0.02, 0.39, 0.68, 0.97 };
    Double_t blue[nRGBs] = { 0.17, 0.39, 0.62, 0.79, 0.97 };
    ind = TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, m_NCont, alpha);
  }

  // set Grey Scale palette (light to dark)
  else if (ncolors == 52 && colors == 0) {
    TColor::InitializeColors();
    const int nRGBs = 3;
    Double_t stops[nRGBs] = { 0.00, 0.50, 1.00 };
    Double_t red[nRGBs] = { 0.90, 0.50, 0.10 };
    Double_t green[nRGBs] = { 0.90, 0.50, 0.10 };
    Double_t blue[nRGBs] = { 0.90, 0.50, 0.10 };
    ind = TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, m_NCont, alpha);
  } else
    return -1;
  return ind;
}

/*---------------------------------------------------------*/
void TileDigitsMonTool::drawHists(int ros, int drawer, std::string moduleName)
/*---------------------------------------------------------*/
{

  ATH_MSG_VERBOSE("in drawHists()");

  int maxgain = (m_bigain) ? 2 : 1;
  double ms = (m_bigain) ? 0.75 : 1.0; // marker size

  static bool palette_init_done = false;
  static int pal51[m_NCont];
  static int pal52[m_NCont];
  static TExec *ex_pal51;
  static TExec *ex_pal52;
  bool do_plots = m_savePng || m_savePs || m_saveSvg;

  if (palette_init_done == false) {
    palette_init_done = true;
    int i;
    int ind = define_palette(51);
    if (ind < 0) ind = 0; // fall-back colours
    for (i = 0; i < m_NCont; i++)
      pal51[i] = ind + i;
    ind = define_palette(52);
    if (ind < 0) ind = 0; // fall-back colours
    for (i = 0; i < m_NCont; i++)
      pal52[i] = ind + i;
    TString s;
    s.Form("gStyle->SetPalette(%d,(int*)%ld);", m_NCont, (long) pal51);
    ex_pal51 = new TExec("ex_pal51", s.Data());
    s.Form("gStyle->SetPalette(%d,(int*)%ld);", m_NCont, (long) pal52);
    ex_pal52 = new TExec("ex_pal52", s.Data());
  }

  TLine * maxline[4];
  double line[8] = { 1.2, 2.5, 1.0, 1.0, 48.0, 48.0, 48.0, 16.0 };
  double miny[12] = { 20.0, 20.0, 0.0, 0.0, 0.0, 0.0, -0.1, -0.1, -0.1, -0.1, -0.1, -0.1 };
  double maxy[12] = { 80.0, 80.0, 1.5, 3.0, 1.5, 3.0, 2.2, 2.2, 2.2, 2.2, 3.2, 2.2 };
  //                   ?  ,  ?  , ?  , ?  , ?  , ?  ,  ?  ,  ? , BC hi,BC lo, CRC ,  ?
  if (m_fillPedestalDifference) {
    miny[0] = miny[1] = -5.0;
    maxy[0] = maxy[1] = 5.0;
  }

  TText *t = new TText();
  t->SetTextAlign(32);
  t->SetTextSize(0.06);
  t->SetTextFont(72);

  if (maxgain == 1) {
    line[0] = line[1];
    for (int ind = 0; ind < 12; ind += 2) {
      miny[ind] = miny[ind + 1] = std::min(miny[ind], miny[ind + 1]);
      maxy[ind] = maxy[ind + 1] = std::max(maxy[ind], maxy[ind + 1]);
    }
  }

  for (int type = 0; type < 4; ++type) {
    maxline[type] = new TLine(0.0, line[type], line[4 + type], line[type]);
    maxline[type]->SetLineColor(2 + type / 2);
  }

  TCanvas * Can = NULL;
  if (do_plots) {
    Can = new TCanvas("dig_noise", "dig_noise", 402 * maxgain, 588);
    Can->Divide(maxgain, 3);
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
  }

  for (int gain = 0; gain < maxgain; ++gain) {
    for (int type = 0; type < 3; ++type) {

      if (do_plots) {
        TVirtualPad * pad = Can->cd(type * maxgain + gain + 1);
        pad->SetTopMargin(0.15);
        pad->SetGridx();
      }
      int ind = type * 2 + gain;

      if (m_final_hist1[ros][drawer][gain][type]->GetMaximum() < 0.9 * maxy[ind]) //if maximum is below reasonable limit use a default scale
      m_final_hist1[ros][drawer][gain][type]->SetMaximum(maxy[ind]);
      if (m_final_hist1[ros][drawer][gain][type]->GetMinimum() > (miny[ind] + 0.1 * TMath::Abs(miny[ind]))) // if minimum is above reasonable limit
        m_final_hist1[ros][drawer][gain][type]->SetMinimum(miny[ind]);		              // use a default scale

      m_final_hist1[ros][drawer][gain][type]->SetMarkerStyle(21);
      m_final_hist1[ros][drawer][gain][type]->SetMarkerSize(ms);
      m_final_hist1[ros][drawer][gain][type]->SetLabelSize(0.08, "X");
      m_final_hist1[ros][drawer][gain][type]->SetLabelSize(0.08, "Y");
      if (do_plots) {
        m_final_hist1[ros][drawer][gain][type]->Draw("P0");
        if (type > 0) maxline[gain]->Draw();
      }
    }
  }

  if (m_savePng) {
    Can->Print(TString(moduleName + "_dig_noise.png"), "png");
  }
  if (m_savePs) {
    Can->Print(TString(moduleName + "_dig_noise.ps"), "ps");
  }
  if (m_saveSvg) {
    Can->Print(TString(moduleName + "_dig_noise.svg"), "svg");
  }
  if (do_plots) {
    delete Can;

    Can = new TCanvas("dig_errors", "dig_errors", 402 * maxgain, 588);
    Can->Divide(maxgain, 4);
    gStyle->SetOptStat(1);
    gStyle->SetTitleFontSize(0.1);
  }

  for (int gain = 0; gain < maxgain; ++gain) {
    for (int type = 3; type < 6; ++type) {

      TVirtualPad * pad = NULL;
      if (do_plots) {
        pad = Can->cd((type - 3) * maxgain + gain + 1);
        pad->SetTopMargin(0.15);
      }
      int ind = type * 2 + gain;

      if (type == 5 && gain == 1) {
        if (do_plots) pad->SetLogy();

        m_hist0[ros][drawer][0]->SetStats(kFALSE);
        gStyle->SetStatFontSize(0.1);
        m_hist0[ros][drawer][0]->SetStats(kTRUE);
        m_hist0[ros][drawer][0]->SetLabelSize(0.08, "X");
        m_hist0[ros][drawer][0]->SetLabelSize(0.08, "Y");
        if (do_plots) m_hist0[ros][drawer][0]->Draw(); // global CRC in bottom right corner

      } else {
        if (do_plots) pad->SetGridx();

        if (m_final_hist1[ros][drawer][gain][type]->GetMaximum() < 0.9 * maxy[ind]) //if maximum is below reasonable limit use a default scale
        m_final_hist1[ros][drawer][gain][type]->SetMaximum(maxy[ind]);
        if (m_final_hist1[ros][drawer][gain][type]->GetMinimum() > (miny[ind] + 0.1 * TMath::Abs(miny[ind]))) // if minimum is above reasonable limit
          m_final_hist1[ros][drawer][gain][type]->SetMinimum(miny[ind]);                           // use a default scale

        m_final_hist1[ros][drawer][gain][type]->SetMarkerStyle(21);
        m_final_hist1[ros][drawer][gain][type]->SetMarkerSize(ms);
        m_final_hist1[ros][drawer][gain][type]->SetStats(kFALSE);
        m_final_hist1[ros][drawer][gain][type]->SetLabelSize(0.08, "X");
        m_final_hist1[ros][drawer][gain][type]->SetLabelSize(0.08, "Y");
        if (type != 3) {
          if (do_plots) m_final_hist1[ros][drawer][gain][type]->Draw("P0");
        } else {
          m_final_hist_stucks[ros][drawer][gain]->SetStats(kFALSE);
          m_final_hist_stucks[ros][drawer][gain]->SetMarkerSize(3.);
          m_final_hist_stucks[ros][drawer][gain]->SetMarkerColor(4);
          m_final_hist_stucks[ros][drawer][gain]->SetLabelSize(0.08, "X");
          m_final_hist_stucks[ros][drawer][gain]->SetLabelSize(0.1, "Y");
          gStyle->SetNumberContours(m_NCont);
          if (do_plots) {
            m_final_hist_stucks[ros][drawer][gain]->Draw("textzcol");
            ex_pal52->Draw();
            m_final_hist_stucks[ros][drawer][gain]->Draw("textcolzsame");
            pad->Update();
            TPaletteAxis *palette = (TPaletteAxis*) m_final_hist_stucks[ros][drawer][gain]->GetListOfFunctions()->FindObject("palette");
            if (palette != NULL) palette->SetLabelSize(0.07);
            pad->Modified();
          }
        }
        if (type == 4 || (type == 5 && gain == 0)) {
          m_final_hist1[ros][drawer][gain][type]->GetYaxis()->SetLabelOffset(-0.85);	// do not draw default lables
          if (do_plots) {
            t->DrawText(-0.2, 1., "all OK");
            t->DrawText(-0.2, .0, "wrong");
            if (type == 4)
              t->DrawText(-0.2, 2., "mismatch");
            else {
              t->DrawText(-0.2, 2., "FE fail");
              t->DrawText(-0.2, 2.5, "FE+ROD fail");
              t->DrawText(-0.2, 3., "ROD fail");
            }
          }
        }
        /* if (type == 3)
         maxline[2]->Draw();
         else */
        if (type != 3) if (do_plots) maxline[3]->Draw();
      }
    }
    //LF Now plot DMU Header Errors histograms
    TVirtualPad * pad = NULL;
    if (do_plots) {
      pad = Can->cd(6 + gain + 1);
      pad->SetTopMargin(0.15);
      pad->SetGridx();
    }
    m_hist2[ros][drawer][gain][0]->SetMarkerSize(3.);
    m_hist2[ros][drawer][gain][0]->SetMarkerColor(2);
    m_hist2[ros][drawer][gain][0]->SetLabelSize(0.11, "Y");
    m_hist2[ros][drawer][gain][0]->SetLabelSize(0.08, "X");
    m_hist2[ros][drawer][gain][0]->SetStats(kFALSE);
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(1, "OK ");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(2, "Format");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(3, "Parity");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(4, "Memory");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(5, "SingleStr");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(6, "DbleStr");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(7, "DummyFrag");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetBinLabel(8, "NoDataFrag");
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetTickLength(0.01);
    m_hist2[ros][drawer][gain][0]->GetYaxis()->SetLabelOffset(0.001);
    if (do_plots) {
      gStyle->SetNumberContours(m_NCont);
      m_hist2[ros][drawer][gain][0]->Draw("textzcol");
      ex_pal51->Draw();
      m_hist2[ros][drawer][gain][0]->Draw("textzcolsame");
      pad->Update();
      TPaletteAxis *palette = (TPaletteAxis*) m_hist2[ros][drawer][gain][0]->GetListOfFunctions()->FindObject("palette");
      if (palette != NULL) palette->SetLabelSize(0.07);
      pad->Modified();
    }

  }

  if (m_savePng) {
    Can->Print(TString(moduleName + "_dig_errors.png"), "png");
  }
  if (m_savePs) {
    Can->Print(TString(moduleName + "_dig_errors.ps"), "ps");
  }
  if (m_saveSvg) {
    Can->Print(TString(moduleName + "_dig_errors.svg"), "svg");
  }
  if (do_plots) delete Can;

  for (int type = 0; type < 4; ++type) {
    delete maxline[type];
  }

  delete t;

  if (m_runType == PedRun) {

    if (do_plots) {
      Can = new TCanvas("correlation", "correlation", 402 * maxgain, 588);
      Can->Divide(maxgain, 2);
      gStyle->SetOptStat(0);
      gStyle->SetPalette(1);
      gStyle->SetTitleFontSize(0.1);
    }

    TPaveLabel covar_label[2];
    covar_label[0] = TPaveLabel(37., 49., 54., 56., "");
    covar_label[1] = TPaveLabel(37., 49., 54., 56., "");

    for (int gain = 0; gain < maxgain; ++gain) {
      for (int type = 0; type < 2; ++type) {
        if (do_plots) {
          TVirtualPad * pad = Can->cd(type * maxgain + gain + 1);
          pad->SetTopMargin(0.15);
        }
        m_final_hist2[ros][drawer][gain][type]->SetLabelSize(0.06, "X");
        m_final_hist2[ros][drawer][gain][type]->SetLabelSize(0.06, "Y");
        if (do_plots) {
          m_final_hist2[ros][drawer][gain][type]->Draw("COLZ");
          if (type == 0) {
            std::ostringstream label_text;
            label_text << "<C_ij>/<C_ii>= " << std::setprecision(4) << m_cov_ratio[ros][drawer][gain] << std::setprecision(4);
            covar_label[gain].SetLabel(label_text.str().c_str());
            covar_label[gain].SetTextSize(0.4);
            covar_label[gain].Draw();
          }
        }
      }
    }

    if (m_savePng) {
      Can->Print(TString(moduleName + "_correlation.png"), "png");
    }
    if (m_savePs) {
      Can->Print(TString(moduleName + "_correlation.ps"), "ps");
    }
    if (do_plots) delete Can;
  }

  gStyle->SetOptStat(1111);
}

int TileDigitsMonTool::stuckBits(TH1S * hist, int adc) {
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

/* not used
 int TileDigitsMonTool::stuckBits_maker(TH1S * hist)
 {

 // If all bins at given hypothetical stuck_position and stuck_bit are empty then
 // this bin is stuck in this position. Do this procedure only for
 // bins less last bin
 hp = 1 - hp;
 ++hb;
 if (hb > 15) hb = 0;
 // Stuck bits maker
 hist->SetBinContent(1,0);
 if (hb < 9) {
 int hyp_pos = hp;
 int hyp_bit = hb;
 int max_Nsample = 1024/(1<<(hyp_bit+1));
 for (int bin_sample = 0; bin_sample < max_Nsample; ++bin_sample) {
 //beginning of sample in histogram
 int BeginBin = (1 - hyp_pos)*(1<<hyp_bit)+bin_sample*(1<<(hyp_bit+1));
 //end of sample in histogram
 int EndBin = BeginBin + (1<<hyp_bit);
 for (int bin = BeginBin; bin < EndBin; ++bin) {
 //variable bin from 0 to 1023 - must be changed in GetBunConent to (bin+1)
 if ((bin+1) < 1025) hist->SetBinContent(bin+1,0);
 }
 }
 }
 return -1;
 }
 */

// New function by Tibor Zenis
// It checks how many bit flips happen compared to the expected ones
// Only works for CIS and Laser because the adc range is not sufficiently excersized 
// in the other run types.
int TileDigitsMonTool::stuckBits_Amp(TH1S * hist, int /*adc*/) {
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
    if (((bc[b] > 2) && (std::abs(bs[b]) == bc[b])) || ((std::abs(bs[b]) > 7) && (std::abs(bs[b]) * 3 > bc[b]))) {
      NSB++;
    }

  return NSB;

}

/* version 2. */
int TileDigitsMonTool::stuckBits_Amp2(TH1S * hist, int /*adc*/, TH2C *outhist, int ch, uint8_t *stuck_probs) {

  if (hist->GetEntries() < 1000)  return 0; /* too few events (1000 / N_samples) in histogram, ignore */

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
  if (last_non0 < 0) // empty histogram
    return 0;
  for (i = 1; i <= last_non0; i++) {
    cm = cc;
    c = cc = cp;
    cp = (i < 1023) ? hist->GetBinContent(i + 2) : 0;
    if (first_non0 < 0) {
      if (cc > 0)
        first_non0 = i;
      else
        continue;
    }
    for (b = 0; b < 10; b++) { // count bits flipped at this position
      if ((i & bv[b]) != ((i - 1) & bv[b])) {
        bac[b]++;
        if (bas[b] > 0) {
          if ((i - 1) & bv[b])
            ba1[b]++;
          else
            ba0[b]++;
          bas[b] = 0;
        }
      }
      if (cc > 0) bas[b]++;
    }
    prob = 1.;
    cont = sqrt((double) (c + cm));
    if ((c == 0 || cm == 0) && (c > 0 || cm > 0)) {
      prob = erf((cont - sqrt(cont)) / sqrt(2 * cont));
    }
    /* bin content is lower than usual */
    if (c > 0 && c < 0.25 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm))) {
      prob = 1. - (double) c / (0.25 * (cp + cm - std::sqrt((double) cp) - std::sqrt((double) cm)));
      c = 0;
    }
    if (c || f) // count bit flips (bc[b]) in bins with non-zero contents or adjacent bins
      for (b = 0; b < 10; b++) {
        if ((i & bv[b]) != ((i - 1) & bv[b]))
          bc[b]++;
        else
          break; // a higher bit can flip only of the lower one does
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
  double sb_prob[4] = {0., 0., 0., 0.};
  static const int sb_map[10] = {0, 1, 1, 2, 2, 3, 3, 3, 3, 3}; // mapping of SB to histogram rows
  for (b = 0; b < 10; b++) {
    if ((ba0[b] == 0 || ba1[b] == 0) && bac[b] > 2 && (ba0[b] + ba1[b] >= bac[b] / 2 || ba0[b] + ba1[b] > 2)) {
      is_stack = 1;
      if (outhist != NULL) {
	  sb_prob[sb_map[b]] = 100.;
      }
      if (stuck_probs != NULL)
	stuck_probs[b] = ba0[b] == 0 ? 100u : 200u;
      continue;
    }
    double bs1 = std::fabs(bs[b]) - sqrt(std::fabs(bs[b]));
    if (bs1 < 0.) bs1 = 0.;
    if ((bs1 > 0.5 * bc[b]) || (bc[b] > 7 && bs1 * 3 > bc[b])) is_stack = 1;
    if (outhist != NULL && bc[b] > 0) {
      if (sb_prob[sb_map[b]] < 100. * bs1 / bc[b]) sb_prob[sb_map[b]] = 100. * bs1 / bc[b];
    }
    if (stuck_probs != NULL)
    {
      stuck_probs[b] = (uint8_t) (100. * bs1 / bc[b]);
      if (bs[b] > 0)
      {
	stuck_probs[b] += 100u;
	if (stuck_probs[b] == 100u)
	  stuck_probs[b] = 0u;
      }
    }
  }
  if ((first_non0 >= 512 && first_non0 < 1023) || (last_non0 == 511 && hist->GetBinContent(last_non0) > 3)) {
    is_stack = 1;
    sb_prob[3] = 100.;
    if (stuck_probs != NULL)
      stuck_probs[9] = first_non0 >= 512 ? 200u : 100u;
  }
  if (outhist != NULL) {
    outhist->Fill((double) ch, 0., sb_prob[0]);
    outhist->Fill((double) ch, 1., sb_prob[1]);
    outhist->Fill((double) ch, 2., sb_prob[2]);
    outhist->Fill((double) ch, 3., sb_prob[3]);
    if (first_non0 == 1023)
      outhist->Fill((double) ch, 5., 100.);
    else if (last_non0 == 1023) {
      double frac;
      frac = 100. * (double) hist->GetBinContent(1024) / hist->GetEntries();
      if (frac > 0. && frac < 1.) frac = 1.;
      if (frac > 99. && frac < 100.) frac = 99.;
      outhist->Fill((double) ch, 5., frac);
    }
    if (last_non0 == 0)
      outhist->Fill((double) ch, 4., 100.);
    else if (first_non0 == 0) {
      double frac;
      frac = 100. * (double) hist->GetBinContent(1) / hist->GetEntries();
      if (frac > 0. && frac < 1.) frac = 1.;
      if (frac > 99. && frac < 100.) frac = 99.;
      outhist->Fill((double) ch, 4., frac);
    }
    outhist->SetMaximum(100.);
  }
  return is_stack;
}


void TileDigitsMonTool::CRCcheck(uint32_t crc32, uint32_t crcMask, int headsize, int ros, int drawer) {
  //! Author: Luca. F. November 06
  //! This method provides CRC checks for Global and TileDMU chips.
  //! Global CRC is taken from the trailer and it is correct if 
  //! the first 16 bits and the last 16 bits of the word match.
  //! TileDMU CRC is taken from the DMU mask filled in the FPGA.
  //! At the moment it makes sense only for monogain runs 
  //! (Physics, LED, MonoCIS). The least significant 16 bits are the result of 
  //! the CRC check in the FE (0xFFFF means all OK).  The most significant 16 bits 
  //! are the result of the CRC check in the ROD (0xFFFF means all OK).

  // protection against non-exisitng histograms
  if (m_hist0[ros][drawer].size() == 0) return;

  //LF CRC check
  //std::string ros_name[2] = {"LBA","LBC"};
  //std::cout << "Module= " << ros_name[ros-1] << drawer+1 << " CRCmask= 0x" <<  std::setfill('0') << std::setw(8) << std::hex <<  crcMask  << std::setfill(' ') << std::dec <<"\n";

  //END CRC check
  // array to adjust order of DMU's of EB* FE CRC status in DMU chip mask word
  // this is needed because FE and ROD fill DMU mask word in different order
  int chFE_ext[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 8, 9, 10, 11, 14, 15 };
  int chFE_sp[16] = { 11, 0, 1, 2, 3, 4, 5, 6, 12, 13, 7, 8, 9, 10, 14, 15 };
  // global CRC
  //  uint32_t crc32 = (*collItr)->getFragCRC();
  uint32_t crc0 = crc32 & 0xFFFF;
  uint32_t crc1 = crc32 >> 16;
  if (crc32 == 0)
    m_hist0[ros][drawer][0]->Fill(0.0, 1.0);
  else if (crc0 == crc1) // two half-words should match
    m_hist0[ros][drawer][0]->Fill(1.0, 1.0);
  else
    m_hist0[ros][drawer][0]->Fill(2.0, 1.0);

  // CRC per TileDMU.For mono gain we have it from ROD and FE. 
  // In bi gain, it is just a placeholder with global CRC information.

  if (headsize < 16) {  // How to handle EB? All bits are filled anyway or not?
  }

  if (m_beamInfo->calibMode() == 1) { //!bigain: dummy information
    for (int ch = 0; ch < headsize; ++ch) {
      if (crc32 == 0)
        m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(0.0, 1.0);
      else if (crc0 == crc1)
        m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(1.0, 1.0);
      else
        m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(2.0, 1.0);

    }
  } else { //! monogain: meaningful information.
           //! Fill 0 if crc32 if 0
           //! Fill 1 if both FE and ROD crc is 1
           //! Fill 2 if DMU FE crc is 0
           //! Fill 3 if DMU ROD crc is 0
           //! Fill 4 is FE and ROD crc is 0
    if (crc32 == 0) {  //std::cout << "Global crc is zero\n";
      for (int ch = 0; ch < headsize; ++ch) {
        m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(0.0, 1.0);
      }
    } else if (crcMask == 0xFFFFFFFF) {
      for (int ch = 0; ch < headsize; ++ch) {
        m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(1.0, 1.0);
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
          if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17))  // Special Check for EBA15,EBC18
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
            m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(1.0, 1.0);
            break;
          case 1: //fe error only
            m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(2.0, 1.0);
            break;
          case 2: // rod errors
            m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(3.0, 1.0);
            break;
          default: // fe+rod errors
            m_hist_DMUerr[ros][drawer][ch][0][1]->Fill(4.0, 1.0);
        } // end switch case

      } // end loop on chips

    } // end if on crcMask

  } // end if on runtype
}

/// DMU header format as defined in http://www.sysf.physto.se/~klere/tile-dmu/header.html
/// Bits: 1pllllesdrvvvv0mmgggbbbbbbbbbbbb
///
/// The header starts with bit 31 set to indicate a header. The other bits as follows:
/// p	-	Parity (odd)
/// l	-	Derandomizer length (number of samples).
/// e	-	Parity error. A parity error was detected from the memory in the last readout.
/// s	-	SEstr Single Error Strobe recieved from the ttc.
/// d	-	DEstr Double Error Strobe recieved from the ttc.
/// r	-	Register parity. Parity from the registers in the chip.
/// v	-	Variable parity. Parity from the variables in the chip.
/// 0	-	Not used. (Set to 0)
/// m	-	Mode.	00 - Normal mode
///                     01 - Calibration mode
///                     10 - Test mode
///                     11 - Not used
/// g	-	High/low gain. Indicates high(1) or low(0) amplification from the 3-in-1 cards.
/// b	-	Bunch Crossing.
//}
//New Method to check for header errors. Checks per channel and returns True for data corrupted, or False for data not corrupted.

/*-------------------------------------------------------*/
bool TileDigitsMonTool::DMUheaderCheck(std::vector<uint32_t>* headerVec, int ros, int drawer, int gain, int dmu)
/*-------------------------------------------------------*/
{
  bool err = false;

  if (DMUheaderFormatCheck((*headerVec)[dmu])) {
    m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 1., 1.);
    err = true;
    return err;
  }
  if (DMUheaderParityCheck((*headerVec)[dmu])) {
    m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 2., 1.);
    err = true;
    return err;
  }
  if (((*headerVec)[dmu] >> 25) & 0x1) {
    //Memory Parity Error
    m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 3., 1.);
    err = true;
  }
  if (((*headerVec)[dmu] >> 24) & 0x1) {
    //Single Strobe Error
    m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 4., 1.);
    err = true;
  }
  if (((*headerVec)[dmu] >> 23) & 0x1) {
    //Double Strobe Error
    m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 5., 1.);
    err = true;
  }
  if (!err) m_hist2[ros][drawer][gain][0]->Fill(dmu + 0.5, 0., 1.);
  return err;
}

StatusCode TileDigitsMonTool::RODCRCcalc() {
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

  const TileRawChannelContainer* rawChannelContainer;
  CHECK(evtStore()->retrieve(rawChannelContainer, m_contNameDSP));

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

    if (rawChannelCollection->empty()) continue;

    HWIdentifier hwid = rawChannelCollection->front()->adc_HWID(); //take the first channel in the drawer
    int ros = m_tileHWID->ros(hwid);  //take the ros and drawer from the first channel
    int drawer = m_tileHWID->drawer(hwid);
    
    uint32_t crc32 = rawChannelCollection->getFragGlobalCRC() & 1;
    
    if (crc32 == 0) {
      crc32 = 0xFFFFFFFF;
    } else { // means OK CRC match
      crc32 = 0xFFFF;
    } //means NOT OK, CRC mismatch

    uint32_t CRCmask = rawChannelCollection->getFragRODChipMask();
    CRCmask = CRCmask << 16; // ROD mask is stored in the 16 most significant bits ce
    CRCmask += rawChannelCollection->getFragFEChipMask();
    
    CRCcheck(crc32, CRCmask, 16, ros, drawer); //reuse the same funcion used for digits
    
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileDigitsMonTool::shiftHisto(TH1S *hist, int ros, int drawer, int ch, int gain)
/*---------------------------------------------------------*/
{

  hist->GetXaxis()->SetRange(5, 100);

  TSpectrum s(8, 1.);

  s.Search(hist, 2, "goff");
  int shift = int(s.GetPositionX()[0]);
  if (shift > 0) {
    int xmax = std::max(1025, 846 + shift);
    int xmin = std::max(1, shift + 50);
    for (int bin = xmin; bin < xmax; ++bin) {
      double c = hist->GetBinContent(bin);
      if (c > 0) m_shifted_hist[ros][drawer][ch][gain]->SetBinContent(bin - shift - 50, c);
    }
  }
}

/*---------------------------------------------------------*/
void TileDigitsMonTool::statTestHistos(int ros, int drawer, int gain)
/*---------------------------------------------------------*/

{
  std::vector<TH1S*> refbld;
  std::vector<TH1S*> newrefbld;
  TH1F *ref = new TH1F("ref", "ref", 796, 0., 796.);
  ref->SetDirectory(0);
  TH1F *ref1 = new TH1F("ref1", "ref1", 796, 0., 796.);
  ref1->SetDirectory(0);

  for (int i = 0; i < 48; i++) {

    TH1S *h = m_shifted_hist[ros][drawer][i][gain];
    float integ = h->Integral(200, 600);
    if (integ > 0) {
      refbld.push_back(h);
    }

  }

  int entr = refbld.size();
  const double inv_entr = (entr > 0 ? 1. / static_cast<double>(entr) : 1);
  for (int i = 0; i < entr; i++) {
    TH1S *h = refbld.at(i);
    ref->Add(h, inv_entr);
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
  const double inv_ent = (ent > 0 ? 1. / static_cast<double>(ent) : 1);
  for (int i = 0; i < ent; i++) {
    obj = (TH1S*) newrefbld.at(i);
    ref1->Add(obj, inv_ent);
  }

  //  newrefbld->Add(ref1);      

  if (ent > 6)
    m_shifted_hist[ros][drawer][48][gain]->Add(ref1, 1.);
  else
    m_shifted_hist[ros][drawer][48][gain]->Add(ref, 1.);

  delete ref;
  delete ref1;

}


void TileDigitsMonTool::saveStuckBitsProbabilities(TTree* tree) {

  tree->Branch("StuckBitsProb", m_stuck_probs, "StuckBitsProb[5][64][48][2][10]/b");
}
