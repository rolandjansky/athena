/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileTMDBRawChannelMonTool
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

#include "TileMonitoring/TileTMDBRawChannelMonTool.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
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
TileTMDBRawChannelMonTool::TileTMDBRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_partition_amplitude{}
  , m_partition_time{}
  , m_amplitude_map{}
  , m_time_map{}
  , m_nEventsProcessed(0)
  , m_histogramsNotBooked(true)
  , m_isNotDSP(false)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileRawChannelContainer", m_rawChannelContainerName = "MuRcvRawChCnt");
  declareProperty("AmplitudeThresholdForTime", m_amplitudeThreshold = 10);
  declareProperty("NotDSP", m_isNotDSP = false);

  m_path = "/Tile/TMDBRawChannel"; //ROOT File relative directory
}

/*---------------------------------------------------------*/
TileTMDBRawChannelMonTool::~TileTMDBRawChannelMonTool() {
/*---------------------------------------------------------*/

}

/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool:: initialize() {
/*---------------------------------------------------------*/


  ATH_MSG_INFO( "in initialize()" );


  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );
  

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::fillHistograms() {
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


  const TileRawChannelContainer* rawChannelContainer;
  CHECK( evtStore()->retrieve(rawChannelContainer, m_rawChannelContainerName) );

  
  if (m_histogramsNotBooked) { // find TMDBs and book histograms for them

    //  if(rawChannelContainer->get_type() != TileFragHash::OptFilterDsp) m_isNotDSP = true;
    //  else m_isNotDSP = false;

    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
      if (rawChannelCollection->empty()) continue;
      
      HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      if (!m_TMDB_names[ros].empty()) continue; // already booked TMDB histograms for this ros
      for (const TileRawChannel* rawChannel : *rawChannelCollection) {
        adc_id = rawChannel->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        CHECK( bookTMDBHistograms(ros, channel) );
      }
    }
    
    if (m_TMDB_names[1].empty() 
        && m_TMDB_names[2].empty() 
        && m_TMDB_names[3].empty() 
        && m_TMDB_names[4].empty()) return StatusCode::SUCCESS; // no TMDBs found
    
    CHECK( bookTMDBSummaryHistograms() );
    m_histogramsNotBooked = false;
    
  }

  
  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    
    if (rawChannelCollection->empty()) continue;
    
    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    
    for (const TileRawChannel* rawChannel : *rawChannelCollection) {
      
      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);

      float amplitude = rawChannel->amplitude();
      m_amplitude[ros][channel]->Fill(amplitude);
      m_partition_amplitude[ros]->Fill(amplitude);
      m_amplitude_map[ros]->Fill(drawer + 1, channel, amplitude);

      if (m_isNotDSP) {
        if (amplitude > m_amplitudeThreshold) {
          float time = rawChannel->time();
          m_time[ros][channel]->Fill(time);
          m_partition_time[ros]->Fill(time);
          m_time_map[ros]->Fill(drawer + 1, channel, time);
        }
      }
    }
  }
  

  ++m_nEventsProcessed;
  
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    if (m_TMDB_names[ros].empty()) continue;
    m_amplitude_map[ros]->SetEntries(m_nEventsProcessed);
    if (m_isNotDSP) m_time_map[ros]->SetEntries(m_nEventsProcessed);
  }


  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookTMDBHistograms(unsigned int ros, unsigned int channel) {
/*---------------------------------------------------------*/

  std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

  std::string TMDB_name(ros_names[ros] + "_" + getTMDBCellName(ros, channel));

  double min_amplitude = (m_isNotDSP) ? -20.5 : -50000;
  double max_amplitude = (m_isNotDSP) ? 255.5 : 50000;

  m_amplitude[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_amplitude", "Amplitude in " + TMDB_name, 276, min_amplitude, max_amplitude) );
  m_amplitude[ros].back()->GetXaxis()->SetTitle("Amplitude [ADC]");

  if (m_isNotDSP) {
    m_time[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_time", "Time in " + TMDB_name, 141, -70.5, 70.5) );
    m_time[ros].back()->GetXaxis()->SetTitle("Time [ns]");
  }

  m_TMDB_names[ros].push_back(TMDB_name);

  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookTMDBSummaryHistograms() {
/*---------------------------------------------------------*/

  std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    if (m_TMDB_names[ros].empty()) continue; // no TMDBs in this ros

    unsigned int n_TMDB = m_TMDB_names[ros].size();
    
    double min_amplitude = (m_isNotDSP) ? -20.5 : -50000;
    double max_amplitude = (m_isNotDSP) ? 255.5 : 50000;
    m_partition_amplitude[ros] = book1F("", "TMDB_" + ros_names[ros] + "_amplitude", "Amplitude in TMDB " + ros_names[ros]
                                        , 276, min_amplitude, max_amplitude); 
    m_partition_amplitude[ros]->GetXaxis()->SetTitle("Amplitude [ADC]");

    m_amplitude_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_amplitude_map", "Amplitude in TMDB " + ros_names[ros]
                                         , 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, min_amplitude, max_amplitude);
    
    SetBinLabel(m_amplitude_map[ros]->GetYaxis(), m_TMDB_names[ros]);
    m_amplitude_map[ros]->GetZaxis()->SetTitle("Amplitude [ADC]");

    if (m_isNotDSP) {
      m_time_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_time_map", "Time in TMDB " + ros_names[ros]
                                      , 64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, -70, 70);
      
      SetBinLabel(m_time_map[ros]->GetYaxis(), m_TMDB_names[ros]);
      m_time_map[ros]->GetZaxis()->SetTitle("Time [ns]");

      m_partition_time[ros] = book1F("", "TMDB_" + ros_names[ros] + "_time", "Time in TMDB " + ros_names[ros], 141, -70.5, 70.5); 
      m_partition_time[ros]->GetXaxis()->SetTitle("Time [ns]");
    }

    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
      int module = drawer + 1;
      std::string module_name = TileCalibUtils::getDrawerString(ros, drawer);

      m_amplitude_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
      if (m_isNotDSP) m_time_map[ros]->GetXaxis()->SetBinLabel(module, module_name.c_str());
    }

  }

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::procHistograms() {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in procHistograms()" );

  return StatusCode::SUCCESS;
}

