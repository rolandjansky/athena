/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTMDBDigitsMonTool
// PACKAGE:  TileMonitoring  
//
//           Produce DigiNoise folder in tilemon.root for monitoring digital noise
//           Input is RAW
//           Output are the noisemap 2D histograms per partition/gain/LFN and HFN
//
// AUTHOR:   Andrey Ryzhov  (largely based on TileDigiNoiseMonTool.cxx)
//
// TODO
// -- filter bad channels from the noise maps
//
// ********************************************************************

#include "xAODEventInfo/EventInfo.h"

#include "TileTMDBDigitsMonTool.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#include "TProfile2D.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

/*---------------------------------------------------------*/
TileTMDBDigitsMonTool::TileTMDBDigitsMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_sumPed1{}
  , m_sumPed2{}
  , m_nPedEvents{}
  , m_pedestal_map{}
  , m_amplitude_map{}
  , m_lfn_map{}
  , m_hfn_map{}
  , m_nEventsProcessed(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileDigitsContainer", m_digitsContainerName = "MuRcvDigitsCnt");
  declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);

  m_path = "/Tile/TMDBDigits"; //ROOT File relative directory
}

/*---------------------------------------------------------*/
TileTMDBDigitsMonTool::~TileTMDBDigitsMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );

  memset(m_sumPed1, 0, sizeof(m_sumPed1));
  memset(m_sumPed2, 0, sizeof(m_sumPed2));
  memset(m_nPedEvents, 0, sizeof(m_nPedEvents));

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );
  

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms()" );

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


  const TileDigitsContainer* digitsContainer;
  CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainerName) );
  
  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
    
    if (digitsCollection->empty()) continue;
    
    HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);

    if (m_TMDB_names[ros].empty()) CHECK(bookTMDBHistograms(digitsCollection));
    
    for (const TileDigits* tile_digits : *digitsCollection) {
      
      adc_id = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);

      std::vector<float> digits = tile_digits->samples();
      
      double mean_samp = 0.0;
      double rms_samp = 0.0;
      unsigned int n_digits = digits.size();
      
      for (double digit : digits) {
        mean_samp += digit;
        rms_samp += digit * digit;
      }

      if (n_digits > 0) {

        double ped = digits[0];
        m_pedestal[ros][channel]->Fill(ped);
        m_pedestal_map[ros]->Fill(drawer + 1, channel, ped); // this is the Pedestal (DIGI0) )

        m_sumPed1[ros][drawer][channel] += ped;
        m_sumPed2[ros][drawer][channel] += ped * ped;
        
        ++m_nPedEvents[ros][drawer][channel];
        
        if (n_digits > 1) {
          mean_samp /= n_digits;
          rms_samp = rms_samp / n_digits - mean_samp * mean_samp;
          rms_samp = (rms_samp > 0.0) ? sqrt(rms_samp * n_digits / (n_digits - 1)) : 0.0;

          m_hfn[ros][channel]->Fill(rms_samp);
          m_hfn_map[ros]->Fill(drawer + 1, channel, rms_samp); // this is the HFN (MeanOf(RMS over the 7 digis) )

          //-- temporary until TMDB raw channel monitoring tool be ready --//

          auto min_max = std::minmax_element(digits.begin(), digits.end());
          float min_digit = *min_max.first;
          float max_digit = *min_max.second;
          float amplitude = max_digit - min_digit;
          m_amplitude[ros][channel]->Fill(amplitude);
          m_amplitude_map[ros]->Fill(drawer + 1, channel, amplitude);

          //---------------------------------------------------------------//

        }
      }
    } // digits
  }
  
  ++m_nEventsProcessed;
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    if (m_TMDB_names[ros].empty()) continue;
    m_pedestal_map[ros]->SetEntries(m_nEventsProcessed);
    m_amplitude_map[ros]->SetEntries(m_nEventsProcessed);
    m_hfn_map[ros]->SetEntries(m_nEventsProcessed);
  }
  
  if ((m_summaryUpdateFrequency > 0) && (m_nEvents % m_summaryUpdateFrequency == 0)) return updateSummaryHistograms();

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::updateSummaryHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in updateSummaryHistograms()" );

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    unsigned int TMDB_max_channel = m_TMDB_names[ros].size();
    for (unsigned int channel = 0; channel < TMDB_max_channel; ++channel) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {

        double pedestal = 0.0;
        double pedestal_rms = 0.0;
        
        int n_ped_events = m_nPedEvents[ros][drawer][channel];
        
        if (n_ped_events > 1) {
          pedestal = m_sumPed1[ros][drawer][channel] / n_ped_events;
          
          pedestal_rms = m_sumPed2[ros][drawer][channel] / n_ped_events - pedestal * pedestal;
          pedestal_rms = (pedestal_rms > 0.0) ? sqrt(pedestal_rms * n_ped_events / (n_ped_events - 1)) : 0.0;
          
        }

        int bin = m_lfn_map[ros]->GetBin(drawer + 1, channel + 1);
        m_lfn_map[ros]->SetBinContent(bin, pedestal_rms * n_ped_events); // this is the LFN (RMS of DIGI0)
        m_lfn_map[ros]->SetBinEntries(bin, n_ped_events); // this is the LFN (RMS of DIGI0)
        m_lfn_map[ros]->SetEntries(m_nEventsProcessed);
        
        
      } // channnel loop for (int channel = 0; channel < 48; ++channel) {
    } //loop over drawer
  } //loop over ros

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::bookTMDBHistograms(const TileDigitsCollection* digitsCollection) {
/*---------------------------------------------------------*/

  std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

  if (digitsCollection->empty()) return StatusCode::SUCCESS;

  HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
  int ros = m_tileHWID->ros(adc_id);
  
  for (const TileDigits* digits: *digitsCollection) {

    adc_id = digits->adc_HWID();
    int channel = m_tileHWID->channel(adc_id);

    std::string TMDB_name(ros_names[ros] + "_" + getTMDBCellName(ros, channel));
    
    m_pedestal[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_pedestal", "Sample[0] in " + TMDB_name, 101, -0.5, 100.5) );
    m_pedestal[ros].back()->GetXaxis()->SetTitle("[ADC]");
    
    m_hfn[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_hfn", "RMS of 7 samples in " + TMDB_name, 180, -0.5, 179.5) );
    m_hfn[ros].back()->GetXaxis()->SetTitle("[ADC]");
    
    // temporary until TMDB raw channel monitoring tool be ready
    m_amplitude[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_amplitude", "Difference betwean maximum and minimum sample in " + TMDB_name, 256, -0.5, 255.5) );
    m_amplitude[ros].back()->GetXaxis()->SetTitle("[ADC]");
    
    m_TMDB_names[ros].push_back(TMDB_name);

  }

  if (!m_TMDB_names[ros].empty()) CHECK( bookTMDBSummaryHistograms(ros) );

  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::bookTMDBSummaryHistograms(unsigned int ros) {
/*---------------------------------------------------------*/

  std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};


  unsigned int n_TMDB = m_TMDB_names[ros].size();
  
  m_pedestal_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_pedestal_map", "Mean sample[0] in TMDB " + ros_names[ros],
                                      64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, 0, 100);
  
  SetBinLabel(m_pedestal_map[ros]->GetYaxis(), m_TMDB_names[ros]);
  m_pedestal_map[ros]->GetZaxis()->SetTitle("[ADC]");
  
  m_amplitude_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_amplitude_map", "Difference betwean maximum and minimum sample in TMDB " + ros_names[ros],
                                       64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, 0, 100);
  
  SetBinLabel(m_amplitude_map[ros]->GetYaxis(), m_TMDB_names[ros]);
  m_amplitude_map[ros]->GetZaxis()->SetTitle("[ADC]");
  
  m_hfn_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_hfn_map", "Mean RMS of 7 samples in TMDB " + ros_names[ros],
                                 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, 0, 180);
  
  SetBinLabel(m_hfn_map[ros]->GetYaxis(), m_TMDB_names[ros]);
  m_hfn_map[ros]->GetZaxis()->SetTitle("[ADC]");
  
  m_lfn_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_lfn_map", "RMS of sample[0] in TMDB " + ros_names[ros],
                                 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, 0, 10);
  
  SetBinLabel(m_lfn_map[ros]->GetYaxis(), m_TMDB_names[ros]);
  m_lfn_map[ros]->GetZaxis()->SetTitle("[ADC]");
  
  
  for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
    int module = drawer + 1;
    std::string module_name = TileCalibUtils::getDrawerString(ros, drawer);
    
    m_pedestal_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
    m_amplitude_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
    m_hfn_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
    m_lfn_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
  }
  
  

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBDigitsMonTool::procHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in procHistograms()" );

  return updateSummaryHistograms();
}

