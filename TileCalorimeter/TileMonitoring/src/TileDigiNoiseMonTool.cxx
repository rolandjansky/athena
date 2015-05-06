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

#include "xAODEventInfo/EventInfo.h"

#include "TileMonitoring/TileDigiNoiseMonTool.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


/*---------------------------------------------------------*/
TileDigiNoiseMonTool::TileDigiNoiseMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_beamInfo("TileBeamInfoProvider")
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileToolNoiseSample("TileCondToolNoiseSample")
  , m_DQstatus(0)
  , m_nEventsProcessed(0)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileBeamInfoProvider", m_beamInfo);
  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileCondToolNoiseSample", m_tileToolNoiseSample);

  declareProperty("bigain", m_bigain = true);
  declareProperty("TileDigitsContainer", m_digitsContainerName = "TileDigitsCnt");
  declareProperty("SummaryUpdateFrequency", m_summaryUpdateFrequency = 0);
  declareProperty("FillEmptyFromDB", m_fillEmtyFromDB = false);
  declareProperty("FillPedestalDifference", m_fillPedestalDifference = true);
  declareProperty("TriggerTypes", m_triggerTypes);

  m_path = "/Tile/DigiNoise"; //ROOT File relative directory
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
  CHECK(m_tileBadChanTool.retrieve());
  if (m_fillEmtyFromDB || m_fillPedestalDifference) CHECK(m_tileToolNoiseSample.retrieve());

  memset(m_sumPed1, 0, sizeof(m_sumPed1));
  memset(m_sumPed2, 0, sizeof(m_sumPed2));
  memset(m_sumRms1, 0, sizeof(m_sumRms1));

  memset(m_nPedEvents, 0, sizeof(m_nPedEvents));
  memset(m_nRmsEvents, 0, sizeof(m_nRmsEvents));

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
StatusCode TileDigiNoiseMonTool::bookHistograms()
/*---------------------------------------------------------*/
{ 

  ATH_MSG_DEBUG( "TileDigiNoiseMonTool::bookHistograms() :  m_path =  " << m_path  );
  
  
  const char *part[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };
  const char *gain[6] = { "lo", "hi", "", " low gain", " high gain", "" };
  const char *type[6] = { "lfn", "hfn", "ped", "Low Freq. Noise", "High Freq. Noise", "Pedestal[0]" };
  char histName[50];
  char histTitle[70];

  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  std::string module_name;
  std::string cell_name;
  std::string channel_name;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (int gn = mingain; gn < maxgain; ++gn) {
      int adc = gn & 1;
      for (int noisetype = 0; noisetype < 3; ++noisetype) {

        sprintf(histName, "noisemap_%s_%s_%s", part[ros], gain[adc], type[noisetype]);
        if (noisetype == 2 && m_fillPedestalDifference) {
          sprintf(histTitle, "Noise map %s %s %s %s (entries = events)", part[ros], gain[3 + gn], type[noisetype + 3], "- pedestal in DB");
        } else {
          sprintf(histTitle, "Noise map %s %s %s (entries = events)", part[ros], gain[3 + gn], type[noisetype + 3]);
        }

        ATH_MSG_DEBUG( "in bookHists() :: booking noise_map histo : " << histName );

        m_finalNoiseMap[ros][gn][noisetype] = book2F("", histName, histTitle, 64, 0.5, 64.5, 48, -0.5, 47.5);

        for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
          module_name = TileCalibUtils::getDrawerString(ros, drawer);
          m_finalNoiseMap[ros][gn][noisetype]->GetXaxis()->SetBinLabel(drawer + 1, module_name.c_str());
        }

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          cell_name = getCellName(ros, channel);

          channel_name = cell_name + (cell_name.empty() ? "ch" : "_ch") + std::to_string(channel);
          m_finalNoiseMap[ros][gn][noisetype]->GetYaxis()->SetBinLabel(channel + 1, channel_name.c_str());
        }
        
        if (noisetype < 2) {
          m_finalNoiseMap[ros][gn][noisetype]->GetZaxis()->SetTitle("Noise [ADC]");
          m_finalNoiseMap[ros][gn][noisetype]->GetZaxis()->SetRangeUser(0, 10);
        } else {
          m_finalNoiseMap[ros][gn][noisetype]->GetZaxis()->SetTitle("Pedestal [ADC]");
          if (m_fillPedestalDifference) m_finalNoiseMap[ros][gn][noisetype]->GetZaxis()->SetRangeUser(-5, 5);
          else m_finalNoiseMap[ros][gn][noisetype]->GetZaxis()->SetRangeUser(20, 80);
        }
        m_finalNoiseMap[ros][gn][noisetype]->SetTitle(histTitle);

      } // end loop over noise type (hfn, lfn)
    } // end gain loop
  } // end loop over ros/partitions

  //SetBookStatus(true);
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHists()" );

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

    m_DQstatus = m_beamInfo->getDQstatus();

    const TileDigitsContainer* digitsContainer;
    CHECK( evtStore()->retrieve(digitsContainer, m_digitsContainerName) );
    
    for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
      
      if (digitsCollection->empty()) continue;
      
      HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      
      for (const TileDigits* tile_digits : *digitsCollection) {
        
        adc_id = tile_digits->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int gain = (m_bigain) ? m_tileHWID->adc(adc_id) : 0; // ignore gain in monogain run
        
        //        if (isDisconnected(ros, drawer, channel)) continue;
        
        if ( !(m_DQstatus->isAdcDQgood(ros, drawer, channel, gain) && m_beamInfo->isChanDCSgood(ros, drawer, channel)) ) continue;
        if ( m_tileBadChanTool->getAdcStatus(drawerIdx, channel, gain).isBad() ) continue;
        
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
          
          m_sumPed1[ros][drawer][channel][gain] += ped;
          m_sumPed2[ros][drawer][channel][gain] += ped * ped;
          
          ++m_nPedEvents[ros][drawer][channel][gain];
          
          if (n_digits > 1) {
            mean_samp /= n_digits;
            rms_samp = rms_samp / n_digits - mean_samp * mean_samp;
            rms_samp = (rms_samp > 0.0) ? sqrt(rms_samp * n_digits / (n_digits - 1)) : 0.0;
            m_sumRms1[ros][drawer][channel][gain] += rms_samp;
            
            ++m_nRmsEvents[ros][drawer][channel][gain];
          }
        }
      } // digits
    }
    
    ++m_nEventsProcessed;
  }
  
  if ((m_summaryUpdateFrequency > 0) && (m_nEvents % m_summaryUpdateFrequency == 0)) return updateSummaryHistograms();

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::updateSummaryHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_INFO( "in finalHists()" );

  // for bigain run book 2 histograms per channel
  // for monogain run book just one histogram per channel
  int mingain = (m_bigain) ? 0 : 2;
  int maxgain = (m_bigain) ? 2 : 3;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      for (int gain = mingain; gain < maxgain; ++gain) {
        int adc = gain & 1;

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
          double pedestal = 0.0;
          double pedestal_rms = 0.0;
          double hi_rms = 0.0;

          int n_ped_events = m_nPedEvents[ros][drawer][channel][adc];
          int n_rms_events = m_nRmsEvents[ros][drawer][channel][adc];

          if (n_rms_events > 0) {
            hi_rms = m_sumRms1[ros][drawer][channel][adc] / n_rms_events;
          } else if ( m_fillEmtyFromDB && !(m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad()) ) {
            hi_rms = m_tileToolNoiseSample->getHfn(drawerIdx, channel, adc);
          }

          if (n_ped_events > 1) {
            pedestal = m_sumPed1[ros][drawer][channel][adc] / n_ped_events;
            
            pedestal_rms = m_sumPed2[ros][drawer][channel][adc] / n_ped_events - pedestal * pedestal;
            pedestal_rms = (pedestal_rms > 0.0) ? sqrt(pedestal_rms * n_ped_events / (n_ped_events - 1)) : 0.0;

            if(m_fillPedestalDifference) {
              if (!isDisconnected(ros, drawer, channel)) pedestal -= m_tileToolNoiseSample->getPed(drawerIdx, channel, adc);
              else pedestal = 0.0;
            }

          } else if ( m_fillEmtyFromDB && !(m_tileBadChanTool->getAdcStatus(drawerIdx, channel, adc).isBad()) ) {
            pedestal_rms = m_tileToolNoiseSample->getLfn(drawerIdx, channel, adc);
          }

            
          // fill the 2D noise map  CC 2014-07-14
          // LFN : type=1, HFN : type=2

          m_finalNoiseMap[ros][adc][0]->SetBinContent(drawer + 1, channel + 1, pedestal_rms); // this is the LFN (RMS of DIGI0)
          m_finalNoiseMap[ros][adc][0]->SetEntries(m_nEventsProcessed);

          m_finalNoiseMap[ros][adc][1]->SetBinContent(drawer + 1, channel + 1, hi_rms); // this is the HFN (MeanOf(RMS over the 7 digis) )
          m_finalNoiseMap[ros][adc][1]->SetEntries(m_nEventsProcessed);

          m_finalNoiseMap[ros][adc][2]->SetBinContent(drawer + 1, channel + 1, pedestal); // this is the Pedestal (DIGI0) )
          m_finalNoiseMap[ros][adc][2]->SetEntries(m_nEventsProcessed);

        } // channnel loop for (int channel = 0; channel < 48; ++channel) {
      } // gain loop                             
    } //loop over drawer
  } //loop over ros

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileDigiNoiseMonTool::procHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in procHistograms()" );

  return updateSummaryHistograms();
}

