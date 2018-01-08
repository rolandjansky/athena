/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRawChannelNoiseMonTool.cxx
// PACKAGE:  TileMonitoring  
//
//           Produce CellNoise folder in tilemon.root for monitoring cell noise
//           Input is RAW
//           Output:
//                -- Cell noise histos for each individual cell (possibly do not keep in the final tilemon.root)
//                -- 2D map histograms per partition/gain/ for sigma1, sigma2, R, chi2, chi2prb, RMS, RMS/Sigma1 
// 
// AUTHOR:   Christophe Clement  largely inspired by code from S. Solodkov,  S. Harkusha and L. Fiorini
//
//
// ********************************************************************

#include "xAODEventInfo/EventInfo.h"

#include "TileMonitoring/TileRawChannelNoiseMonTool.h"
#include "TileMonitoring/PairBuilder.h"


#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileRawChannelUnit.h"

#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"

#include "TileEvent/TileCell.h"
#include "TileEvent/TileRawChannelContainer.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TString.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <algorithm>


/*---------------------------------------------------------*/
TileRawChannelNoiseMonTool::TileRawChannelNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_beamInfo("TileBeamInfoProvider")
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_DQstatus(0)
  , m_map_sigma{}
  , m_map_sigma2{}
  , m_map_R{}
  , m_map_chi2{}
  , m_map_chi2prb{}
  , m_map_rmsOsig{}
  , m_map_rms{}
  , m_gain(1)
  , m_nEventsProcessed(0)
  , m_minimumEventsNumberToFit(100)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("doOnline", m_doOnline = false); //online mode
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("Xmin", m_xmin = -20.25); //xmin for the single cell noise histos
  declareProperty("Xmax", m_xmax = 20.25); //xmax for the single cell noise histos
  declareProperty("NBins", m_nbins = 81); //xmax for the single cell noise histos
  declareProperty("do2GFit", m_do2GFit = false); // perform the 2G fit per channel
  declareProperty("doFit", m_doFit = false); // perform the 2G fit per channel
  // update frequency for the histograms that are filled in finalize step, essentially only the 2G fit results
  declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);

  declareProperty("TileRawChannelContainer", m_rawChannelContainerName = "TileRawChannelOpt2"); //SG RC Container
  declareProperty("TileRawChannelContainerDSP", m_rawChannelContainerDspName = "TileRawChannelCnt"); //SG RC Container
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("Gain", m_gainName = "HG"); // gain to be processed
  declareProperty("TriggerTypes", m_triggerTypes);
  declareProperty("MinimumEventsNumberToFit", m_minimumEventsNumberToFit);

  m_path = "/Tile/RawChannelNoise";
}

/*---------------------------------------------------------*/
TileRawChannelNoiseMonTool::~TileRawChannelNoiseMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::initialize() {
  /*---------------------------------------------------------*/

  ATH_MSG_INFO("in initialize() - m_path = " << m_path);

  CHECK(m_beamInfo.retrieve());
  //=== get TileBadChanTool
  CHECK(m_tileBadChanTool.retrieve());

  if (m_gainName == "HG"){
    m_gain = 1;
  } else if (m_gainName == "LG") {
    m_gain = 0;
  } else {
    ATH_MSG_FATAL("Unknown gain: " << m_gainName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO( "Gain to be processed: " << m_gainName);

  if (m_triggerTypes.empty()) {
    ATH_MSG_INFO("No trigger type specified,  all received trigger types will be processed");
  } else {
    msg(MSG::INFO) << "Only events of the following trigger type(s) will be processed: ";
    for (auto trigger: m_triggerTypes) {
      msg(MSG::INFO) << trigger << " (0x" << std::hex << trigger << ") " << std::dec;
    }
    msg(MSG::INFO) << endmsg;
  }


  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::bookRawChannelNoiseHistos() {
  /*---------------------------------------------------------*/

  ATH_MSG_DEBUG("in bookRawChannelNoiseHistos() - m_path = " << m_path);

  std::string PartitionName[] = {"AUX", "LBA", "LBC", "EBA", "EBC" };

  std::string module_name;
  std::string cell_name;
  std::string channel_name;
  std::string runNumStr = getRunNumStr();

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      module_name = TileCalibUtils::getDrawerString(ros, drawer);
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        cell_name = getCellName(ros, channel);

        m_tileChannelEne[ros][drawer].push_back( book1F(module_name, "ChannelNoise_" + module_name + "_" + cell_name + "_ch" + std::to_string(channel)
                                                        , "TileChannelNoise - Run " + runNumStr + " " + module_name + " " + cell_name + " ch" + std::to_string(channel) + " " + m_gainName
                                                        , m_nbins, m_xmin, m_xmax));

      } // channel
    } // drawer
  } // ros


  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    
    if (m_do2GFit) {
      m_map_sigma[ros] = book2F("Summary", "map_sigma1_" + PartitionName[ros], "Sigma 1 - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
      m_map_sigma2[ros] = book2F("Summary", "map_sigma2_" + PartitionName[ros], "Sigma 2 - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
      m_map_rmsOsig[ros] = book2F("Summary", "map_rmsOsig_" + PartitionName[ros], "RMS/ Sigma1 - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
      m_map_R[ros] = book2F("Summary", "map_R_" + PartitionName[ros], "R (A_{1}/A_{2})- " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
    } else {
      m_map_sigma[ros] = book2F("Summary", "map_sigma_" + PartitionName[ros], "Sigma - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
      m_map_rmsOsig[ros] = book2F("Summary", "map_rmsOsig_" + PartitionName[ros], "RMS/ Sigma - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
    }

    m_map_chi2[ros] = book2F("Summary", "map_chi2_" + PartitionName[ros], "chi2 - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);
    m_map_chi2prb[ros] = book2F("Summary", "map_chi2prb_" + PartitionName[ros], "chi2 prob. - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);

    m_map_rms[ros] = book2F("Summary", "map_rms_" + PartitionName[ros], "RMS - " + PartitionName[ros] + " " + m_gainName + " (entries = events)", 64, 0.5, 64.5, 48, -0.5, 47.5);


    //////////// SET THE MODULE NAMES ON THE X-AXIS ////////////
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
      module_name = TileCalibUtils::getDrawerString(ros, drawer);

      Int_t bin = drawer + 1;
      m_map_sigma[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());

      if (m_do2GFit) {
        m_map_sigma2[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
        m_map_R[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
      }

      m_map_chi2[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
      m_map_chi2prb[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
      m_map_rmsOsig[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
      m_map_rms[ros]->GetXaxis()->SetBinLabel(bin, module_name.c_str());
    } // drawer


    //////////// SET THE CHANNEL NAMES ON THE Y-AXIS ////////////
    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      cell_name = getCellName(ros, channel);
      channel_name = cell_name + (cell_name.empty() ? "ch" : "_ch") + std::to_string(channel);

      Int_t bin = channel + 1;
      m_map_sigma[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
      
      if (m_do2GFit) {
        m_map_sigma2[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
        m_map_R[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
      }

      m_map_chi2[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
      m_map_chi2prb[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
      m_map_rmsOsig[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
      m_map_rms[ros]->GetYaxis()->SetBinLabel(bin, channel_name.c_str());
    }

  } // ros

  return StatusCode::SUCCESS;
} // bookCellNoiseHistos

/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::bookHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("in bookHistograms() -  m_path = " << m_path);

  cleanHistVec(); //necessary to avoid problems at the run, evblock, lumi blocks boundaries

  // Use all triggers (if needs to be done per trigger type then move the booking to the fillhisto where we check the trigger)

  if (bookRawChannelNoiseHistos().isFailure()) {
    ATH_MSG_WARNING("Error with bookRawChannelNoiseHistos() ");
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void TileRawChannelNoiseMonTool::cleanHistVec() {
  /*---------------------------------------------------------*/

  for (int i = 0; i < 9; i++) {
    m_activeTrigs[i] = -1;
  }

  for (unsigned int ros = 0; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      m_tileChannelEne[ros][drawer].clear();
    } // drawer
  } // ros

} // cleanHistVec

/*---------------------------------------------------------*/
void TileRawChannelNoiseMonTool::doFit() {
  /*---------------------------------------------------------*/

  ATH_MSG_VERBOSE("in doFit() ");

  // check there is at least one histo to fit!
  float xmin = -1000.;
  float xmax = 1000.;

  if (m_do2GFit) {

    TF1 fitfunction = TF1("total", "gaus(0)+gaus(3)", xmin, xmax);
    fitfunction.SetLineColor(2);

    double fitresults[9] = {0};
    double R, rmsOsig;
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

          // fit the single cell energy distributions
          ATH_MSG_VERBOSE("in  doFit() : ros = " << ros << ", drawer = " << drawer << ", channel = " << channel);
          
          if (m_tileChannelEne[ros][drawer].at(channel)->GetEntries() > m_minimumEventsNumberToFit) {
            
            fitDoubleGauss(m_tileChannelEne[ros][drawer].at(channel), fitresults, &fitfunction);
            
            // then store the fitresults into a permanent container
            ATH_MSG_VERBOSE( "Fit results:"
                             << " sigma1  = " << fitresults[2]
                             << ", sigma2  = " << fitresults[5]
                             << ", amp1    = " << fitresults[0]
                             << ", amp2    = " << fitresults[3]
                             << ", chi2    = " << fitresults[6]
                             << ", chi2prb = " << fitresults[7]
                             << ", RMS = " << fitresults[8]);

            
            //////////// Store the results in 2D maps /////////////
            
            //////////// Store Sigma 1,2,R, chi2, RMS/sigma , RMS /////////////
            R = (fitresults[3] != 0) ? fitresults[0] / fitresults[3] : -1;
            
            Int_t bin = m_map_sigma[ros]->GetBin(drawer + 1, channel + 1);
            
            m_map_sigma[ros]->SetBinContent(bin, fitresults[2]); // sigma 1
            m_map_sigma[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_sigma2[ros]->SetBinContent(bin, fitresults[5]); // sigma 2
            m_map_sigma2[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_R[ros]->SetBinContent(bin, R); // R = Amp1/Amp2
            m_map_R[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_chi2[ros]->SetBinContent(bin, fitresults[6]);
            m_map_chi2[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_chi2prb[ros]->SetBinContent(bin, fitresults[7]);
            m_map_chi2prb[ros]->SetEntries(m_nEventsProcessed);
            
            rmsOsig = (fitresults[2] != 0) ? (fitresults[8] / fitresults[2]) : -1;
            m_map_rmsOsig[ros]->SetBinContent(bin, rmsOsig);
            m_map_rmsOsig[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_rms[ros]->SetBinContent(bin, fitresults[8]);
            m_map_rms[ros]->SetEntries(m_nEventsProcessed);
            
          } // if histogram exists and has entries
        } // channel
      } // drawer
    } // ros
    
  } else {
    
    TF1 fitfunction("total", "gaus(0)", xmin, xmax);
    fitfunction.SetLineColor(2);

    double fitresults[6] = {0};
    double rmsOsig;
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

          ATH_MSG_VERBOSE("in  doFit() : ros =  " << ros << ", drawer = " << drawer << ", channel = " << channel);
          
          if (m_tileChannelEne[ros][drawer].at(channel)->GetEntries() > m_minimumEventsNumberToFit) {

            fitGauss(m_tileChannelEne[ros][drawer].at(channel), fitresults, &fitfunction);

            // then store the fitresults into a permanent container
            ATH_MSG_VERBOSE( "Fit results:"
                             << " sigma  = " << fitresults[2]
                             << ", amp    = " << fitresults[0]
                             << ", chi2    = " << fitresults[3]
                             << ", chi2prb = " << fitresults[4]
                             << ", RMS = " << fitresults[5]);

            //////////// Store the results in 2D maps /////////////
            
            //////////// Store Sigma 1,2,R, chi2, RMS/sigma , RMS /////////////
            
            Int_t bin = m_map_sigma[ros]->GetBin(drawer + 1, channel + 1);
            
            m_map_sigma[ros]->SetBinContent(bin, fitresults[2]); // sigma 1
            m_map_sigma[ros]->SetEntries(m_nEventsProcessed);

            m_map_chi2[ros]->SetBinContent(bin, fitresults[3]);
            m_map_chi2[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_chi2prb[ros]->SetBinContent(bin, fitresults[4]);
            m_map_chi2prb[ros]->SetEntries(m_nEventsProcessed);
            
            rmsOsig = (fitresults[2] != 0) ? (fitresults[5] / fitresults[2]) : -1;
            m_map_rmsOsig[ros]->SetBinContent(bin, rmsOsig);
            m_map_rmsOsig[ros]->SetEntries(m_nEventsProcessed);
            
            m_map_rms[ros]->SetBinContent(bin, fitresults[5]);
            m_map_rms[ros]->SetEntries(m_nEventsProcessed);
            
          } // if histogram exists and has entries
        } // channel
      } // drawer
    } // ros

  }

  return;
} // doFit


/*---------------------------------------------------------*/
void TileRawChannelNoiseMonTool::fitGauss(TH1F* h, double* fitresults, TF1* fitfunction) {
  /*---------------------------------------------------------*/

  double par[3];

  // start values for fit parameters should be the same as in the main reconstruction
  float nentries = h->GetEntries();
  float rms = h->GetRMS();

  ATH_MSG_VERBOSE("in fitGauss(...)  : nentries = " << nentries << ", rms = " << rms);

  par[0] = 0.1 * nentries;
  par[1] = 0.;
  par[2] = 0.7 * rms;

  fitfunction->SetParameters(par);

  fitfunction->SetParName(0, "Amp");
  fitfunction->SetParName(1, "#mu");
  fitfunction->SetParName(2, "#sigma");

  float bin = h->GetBinWidth(0);
  float lim1 = bin * 0.5;
  float lim2 = std::max(rms * 1.05, bin * 2.0);    

  fitfunction->SetParLimits(0, 0., nentries);
  fitfunction->FixParameter(1, 0.);
  fitfunction->SetParLimits(2, lim1, lim2);

  h->Fit(fitfunction, "BQ");

  fitresults[0] = fitfunction->GetParameter(0);
  fitresults[1] = fitfunction->GetParameter(1);
  fitresults[2] = fitfunction->GetParameter(2);
  fitresults[3] = fitfunction->GetChisquare();
  fitresults[4] = fitfunction->GetProb();
  fitresults[5] = h->GetRMS();

  return;
}


/*---------------------------------------------------------*/
void TileRawChannelNoiseMonTool::fitDoubleGauss(TH1F* h, double* fitresults, TF1* fitfunction) {
  /*---------------------------------------------------------*/

  double par[6];

  float nentries = h->GetEntries();
  float rms = h->GetRMS();

  par[0] = 0.1 * nentries;
  par[1] = 0.;
  par[2] = 0.7 * rms;
  par[3] = 0.15 * par[0];
  par[4] = 0.;
  par[5] = 5. * par[2];

  fitfunction->SetParameters(par);

  fitfunction->SetParName(0, "Amp_{1}");
  fitfunction->SetParName(1, "#mu_{1}");
  fitfunction->SetParName(2, "#sigma_{1}");
  fitfunction->SetParName(3, "Amp_{2}");
  fitfunction->SetParName(4, "#mu_{2}");
  fitfunction->SetParName(5, "#sigma_{2}");

  float bin = h->GetBinWidth(0);
  float lim1 = bin * 0.5;
  float lim2 = std::max(rms * 1.05, bin * 2.0);    
  float lim3 = std::max(rms * 10.0, bin * 20.);

  fitfunction->SetParLimits(0, 0., nentries);
  fitfunction->FixParameter(1, 0.);
  fitfunction->SetParLimits(2, lim1, lim2);
  fitfunction->SetParLimits(3, 0., nentries);
  fitfunction->FixParameter(4, 0.);
  fitfunction->SetParLimits(5, lim2, lim3);

  h->Fit(fitfunction, "BQ");

  fitresults[0] = fitfunction->GetParameter(0);
  fitresults[1] = fitfunction->GetParameter(1);
  fitresults[2] = fitfunction->GetParameter(2);
  fitresults[3] = fitfunction->GetParameter(3);
  fitresults[4] = fitfunction->GetParameter(4);
  fitresults[5] = fitfunction->GetParameter(5);
  fitresults[6] = fitfunction->GetChisquare();
  fitresults[7] = fitfunction->GetProb();
  fitresults[8] = h->GetRMS();

  return;
}


///// This is dedicated to noise calibration data taken during physics, hence monogain is assumed /////

/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::fillHistoPerRawChannel() {
  /*---------------------------------------------------------*/

  ATH_MSG_VERBOSE("in fillHistoPerRawChannel() ");

  m_DQstatus = m_beamInfo->getDQstatus();

  const TileRawChannelContainer* rawChannelContainer;
  CHECK(evtStore()->retrieve(rawChannelContainer, m_rawChannelContainerName));

  // What is the unit used to store info in the RawChannelContainer ?
  TileRawChannelUnit::UNIT RChUnit = rawChannelContainer->get_unit();
  bool recalibrate(false);
  if (RChUnit != TileRawChannelUnit::ADCcounts) {
    ATH_MSG_VERBOSE( " RawChannel Units is = " << RChUnit << " - recalibrating in ADC counts ! "  );
    recalibrate = true;
  }

  std::string module_name;

  const TileRawChannelContainer* rawChannelContainerDSP(nullptr);
  CHECK(evtStore()->retrieve(rawChannelContainerDSP, m_rawChannelContainerDspName));


  // Loop over the containers
  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {

    if (rawChannelCollection->empty()) continue;

    int fragId = rawChannelCollection->identify();
    IdentifierHash fragHash = (rawChannelContainerDSP->hashFunc())(fragId);
    const TileRawChannelCollection* rawChannelCollectionDSP = *(rawChannelContainerDSP->indexFind(fragHash));

    if (rawChannelCollectionDSP->getLvl1Type() != getL1info()) continue;

    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

    unsigned int nBadOrDisconnectedChannels(0);
    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
      if (isDisconnected(ros, drawer, channel)) {
        ++nBadOrDisconnectedChannels;
      }
    }

    unsigned int nRequiredChannels(TileCalibUtils::MAX_CHAN - nBadOrDisconnectedChannels);
    if (rawChannelCollection->size() < nRequiredChannels) continue;

    /////// loop over raw channels in the container ///////
    for (const TileRawChannel* rawChannel : *rawChannelCollection) {

      HWIdentifier adc_id = rawChannel->adc_HWID();
      int adc = m_tileHWID->adc(adc_id);

      if (adc != m_gain) continue;

      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      unsigned int channel = m_tileHWID->channel(adc_id);

      // if (isDisconnected(ros, drawer, channel)) continue;

      if ( !(m_DQstatus->isAdcDQgood(ros, drawer, channel, adc) && m_beamInfo->isChanDCSgood(ros, drawer, channel)) ) continue;
      if ( m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad() ) continue;

      module_name = TileCalibUtils::getDrawerString(ros, drawer);
      double amplitude = rawChannel->amplitude();

      if (recalibrate) {
        amplitude = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, amplitude, RChUnit, TileRawChannelUnit::ADCcounts);
      }

      ATH_MSG_VERBOSE("in fillHistoPerRawChannel() : "
                      << " module   =  " << module_name
                      << "   channel = " << channel
                      << "   adc = " << adc
                      << "   amplitude = " << amplitude);


      m_tileChannelEne[ros][drawer][channel]->Fill(amplitude);


    }
  } 

  ++m_nEventsProcessed;

  return StatusCode::SUCCESS;
} // fillHistoPerRawChannel


/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::fillHistograms() {
  /*---------------------------------------------------------*/

  ATH_MSG_DEBUG("in fillHistograms() ");

  // fill event info like L1 trigger type, run number, etc...
  fillEvtInfo();

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Run = " << m_runNum
                    << " LB = " << m_lumiBlock
                    << " Evt = " << m_evtNum
                    << " BCID = " << m_evtBCID
                    << " lvl1 = 0x" << std::hex << m_lvl1info << std::dec;

    const DataHandle<xAOD::EventInfo> eventInfo;
    if (evtStore()->retrieve(eventInfo).isSuccess()) {
      const std::vector<xAOD::EventInfo::StreamTag>& evtStreamTags = eventInfo->streamTags();
      if (!evtStreamTags.empty()) {
        msg(MSG::DEBUG) << " stream name/type:";
        for (const auto& evtStreamTag : evtStreamTags) {
          msg(MSG::DEBUG) << " " << evtStreamTag.name() << "/" << evtStreamTag.type();
        }
      }
    }

    msg(MSG::DEBUG) << endmsg;
  }

  if (m_triggerTypes.empty()
      || std::find( m_triggerTypes.begin(), m_triggerTypes.end(), m_lvl1info) != m_triggerTypes.end()) {

    CHECK(fillHistoPerRawChannel());
  }

  if ((m_summaryUpdateFrequency > 0) 
      && (m_nEvents % m_summaryUpdateFrequency == 0) 
      && (m_nEvents > 10) ) doFit();

  return StatusCode::SUCCESS;

} // fillHistograms

/*---------------------------------------------------------*/
StatusCode TileRawChannelNoiseMonTool::procHistograms() {
  /*---------------------------------------------------------*/

  if (m_doFit) doFit();

  return StatusCode::SUCCESS;
}

