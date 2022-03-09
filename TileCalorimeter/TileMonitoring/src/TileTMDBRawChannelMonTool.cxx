/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "TileTMDBRawChannelMonTool.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolTMDB.h"

#include "TProfile2D.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cstring>

/*---------------------------------------------------------*/
TileTMDBRawChannelMonTool::TileTMDBRawChannelMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TileFatherMonTool(type, name, parent)
  , m_nEventsProcessed(0)
  , m_tileToolTMDB("TileCondToolTMDB")
    //  , m_histogramsNotBooked(true)
  , m_isNotDSP(false)
  , m_eff(false)
  , m_TMDB_D6_amplitude{{}}
  , m_TMDB_D56_amplitude{{}}
  , m_partition_amplitude{}
  , m_partition_time{}
  , m_drawer{} //m_drawer[ros][ch][thr]
  , m_sector{} //m_sector[ros]
  , m_amplitude_map{}
  , m_time_map{}

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("TileRawChannelContainer", m_rawChannelContainerName = "MuRcvRawChCnt");
  declareProperty("MuonContainer", m_muonContainer = "Muons");
  declareProperty("TrigMuonContainer", m_muRoi = "LVL1MuonRoIs");
  declareProperty("AmplitudeThresholdForTime", m_amplitudeThreshold = 80);
  declareProperty("NotDSP", m_isNotDSP = false);
  declareProperty("Efficiency", m_eff = false);
  declareProperty("TileCondToolTMDB", m_tileToolTMDB);

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

  if (m_eff) {
    CHECK(m_tileToolTMDB.retrieve());  
  }

  return TileFatherMonTool::initialize();
}

/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "::bookHistograms() :  m_path =  " << m_path  );
  


  ///////////////
  if (m_eff == true) {

    std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};
    std::vector<std::string> thr_names = {"500MeV", "600MeV"};
    std::vector<std::string> ch_names = {"D6", "D5+D6"};

    for (unsigned int ros = 3; ros < TileCalibUtils::MAX_ROS; ++ros) {
      m_sector[ros] = book1F("", "TGC_TrSec_" + ros_names[ros] + "_number_Good_Muons", 
                             "Number Good Muons in TGC trigger sectors " + ros_names[ros], 48, -0.5, 47.5); 
      
      m_sector[ros]->GetXaxis()->SetTitle("TGC trigger sector");
      
      for (unsigned int ch = 0; ch < 2; ++ch) {
        for (unsigned int thr = 0; thr < 2; ++thr) {
          
          m_drawer[ros][ch][thr] = book1F("", "TMDB_" + ros_names[ros] + "_" + ch_names[ch] + "_coincidence_" + thr_names[thr], 
                                          "Number coincidence from all Good Muons in TMDB " + ros_names[ros] + " "  + ch_names[ch] + 
                                          " with threshold: " + thr_names[thr], 
                                          64, 0.5, 64.5);
          
          for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; drawer += 2) {
            int module = drawer + 1;
            std::string module_name = TileCalibUtils::getDrawerString(ros, drawer);
            
            m_drawer[ros][ch][thr]->GetXaxis()->SetBinLabel(module, module_name.c_str());
          }
        }
      }
    }

  }
    /////////////////
    
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHists()" );

  if (m_eff == true) {
    memset(m_TMDB_D56_amplitude, 0, sizeof(m_TMDB_D56_amplitude));
    memset(m_TMDB_D6_amplitude, 0, sizeof(m_TMDB_D6_amplitude));
  }


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
	
  
  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    
    if (rawChannelCollection->empty()) continue;
    
    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);

    if (m_TMDB_names[ros].empty()) CHECK(bookTMDBHistograms(rawChannelCollection));
    
    for (const TileRawChannel* rawChannel : *rawChannelCollection) {
      
      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      
      float amplitude = rawChannel->amplitude();
      ////////////
      if (m_eff == true) {
        if (ros > 2) {
          m_TMDB_D56_amplitude[ros - 3][drawer] += amplitude;
          if (channel == 2 || channel == 3) { m_TMDB_D6_amplitude[ros - 3][drawer] += amplitude;}
        }

        //////////////////////////
      } else {
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
    } //end_loop
  }
  
  /////////////////////////////////////////////////
  if (m_eff == true) {
    
    const xAOD::MuonRoIContainer* trig_mu = 0;
    CHECK(evtStore()->retrieve(trig_mu, m_muRoi));
    
    const xAOD::MuonContainer* muons = 0;
    CHECK(evtStore()->retrieve(muons, m_muonContainer) );
    
    for ( const xAOD::MuonRoI* roi_mu : *trig_mu ) {
      float eta   = roi_mu->eta();
      float phi   = roi_mu->phi();
      int pt_thr  = roi_mu->getThrNumber();
      int sector  = (roi_mu->getSectorAddress()>>1) & (0x3F);
      
      if(!(roi_mu->getSource() == xAOD::MuonRoI::Endcap && (pt_thr == 5 || pt_thr == 6))) { continue; }
      if( fabs(eta) < 1.0 || 1.3 < fabs(eta) )  { continue; }


      int m1 = tilemodule_to_check(sector);
      
      for ( const xAOD::Muon* mu : *muons ) {
        float muon_eta = mu->eta();
        float muon_phi = mu->phi();
        float muon_pt  = mu->pt();
        float dR = calc_dR(eta-muon_eta, phi-muon_phi);
        
        if(dR > 0.1)       {continue;} // dR cut
        if(muon_pt < 15000) {continue;} // pt cut
        if(mu->muonType() != xAOD::Muon::Combined)   {continue;} // combined muon

        unsigned int tile_ros  = (muon_eta > 0.0) ? 3 : 4;
        
        m_sector[tile_ros]->Fill(sector);

        unsigned int drawerIdx;
        
        for(unsigned int ros = 3; ros < TileCalibUtils::MAX_ROS; ++ros) {
          for(int module = m1; module <= m1 + 1; ++module) {
            
            unsigned int rosIdx = ros - 3;
            
            drawerIdx = TileCalibUtils::getDrawerIdx(ros, module);
            
            if( (m_TMDB_D56_amplitude[rosIdx][module] > m_tileToolTMDB->getThreshold(drawerIdx, 3)  )) {
              m_drawer[ros][1][1]->Fill(module + 1);
            }

            if( (m_TMDB_D56_amplitude[rosIdx][module] > m_tileToolTMDB->getThreshold(drawerIdx, 2)  )) {
              m_drawer[ros][1][0]->Fill(module + 1);
            }
            
            if( (m_TMDB_D6_amplitude[rosIdx][module] > m_tileToolTMDB->getThreshold(drawerIdx, 1)  )) {
              m_drawer[ros][0][1]->Fill(module+1);
            }

            if( m_TMDB_D6_amplitude[rosIdx][module]  > m_tileToolTMDB->getThreshold(drawerIdx, 0) ) {
              m_drawer[ros][0][0]->Fill(module+1);
            }
          
          }

        }

      } //mu_loop
    } //trig_mu_loop
  }
  ////////////////////////////////////////////////
  
  
  ++m_nEventsProcessed;
  if (m_eff == false) {
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      if (m_TMDB_names[ros].empty()) continue;
      m_amplitude_map[ros]->SetEntries(m_nEventsProcessed);
      if (m_isNotDSP) m_time_map[ros]->SetEntries(m_nEventsProcessed);
    }
  }
  
  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookTMDBHistograms(const TileRawChannelCollection* rawChannelCollection) {
/*---------------------------------------------------------*/

  std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};


  if (rawChannelCollection->empty()) return StatusCode::SUCCESS;
      
  HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
  int ros = m_tileHWID->ros(adc_id);

  for (const TileRawChannel* rawChannel : *rawChannelCollection) {
    adc_id = rawChannel->adc_HWID();
    int channel = m_tileHWID->channel(adc_id);

    std::string TMDB_name(ros_names[ros] + "_" + getTMDBCellName(ros, channel));

    double min_amplitude = (m_isNotDSP) ? -20.5 : -50000;
    double max_amplitude = (m_isNotDSP) ? 255.5 : 50000;
  
    if (m_eff == false) {

      m_amplitude[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_amplitude", "Amplitude in " + TMDB_name, 276, min_amplitude, max_amplitude) );
      m_amplitude[ros].back()->GetXaxis()->SetTitle("Amplitude [ADC]");

      if (m_isNotDSP) {
        m_time[ros].push_back( book1F("", "TMDB_" + TMDB_name + "_time", "Time in " + TMDB_name, 141, -70.5, 70.5) );
        m_time[ros].back()->GetXaxis()->SetTitle("Time [ns]");
      }
    }

    m_TMDB_names[ros].push_back(TMDB_name);
  }

  if (!m_TMDB_names[ros].empty()) CHECK( bookTMDBSummaryHistograms(ros) );

 	 
  return StatusCode::SUCCESS;
}



/*---------------------------------------------------------*/
StatusCode TileTMDBRawChannelMonTool::bookTMDBSummaryHistograms(unsigned int ros) {
/*---------------------------------------------------------*/

  if (m_eff == false) {

    std::vector<std::string> ros_names = {"AUX", "LBA", "LBC", "EBA", "EBC"};
    
    unsigned int n_TMDB = m_TMDB_names[ros].size();
    double min_amplitude = (m_isNotDSP) ? -20.5 : -50000;
    double max_amplitude = (m_isNotDSP) ? 255.5 : 50000;
    
    m_partition_amplitude[ros] = book1F("", "TMDB_" + ros_names[ros] + "_amplitude", "Amplitude in TMDB " + ros_names[ros], 
                                        276, min_amplitude, max_amplitude); 
    
    m_partition_amplitude[ros]->GetXaxis()->SetTitle("Amplitude [ADC]");
    
    
    
    m_amplitude_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_amplitude_map", "Amplitude in TMDB " + ros_names[ros],
                                         64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, min_amplitude, max_amplitude);
    
    SetBinLabel(m_amplitude_map[ros]->GetYaxis(), m_TMDB_names[ros]);
    m_amplitude_map[ros]->GetZaxis()->SetTitle("Amplitude [ADC]");
    
    if (m_isNotDSP) {
      m_time_map[ros] = bookProfile2D("", "TMDB_" + ros_names[ros] + "_time_map", "Time in TMDB " + ros_names[ros],
                                      64, 0.5, 64.5, n_TMDB, -0.5, n_TMDB - 0.5, -70, 70);
      
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


/*---------------------------------------------------------*/
float TileTMDBRawChannelMonTool::calc_dR(float dEta, float dPhi){
/*---------------------------------------------------------*/
  float dR = sqrt( (dEta*dEta) + (dPhi*dPhi) );
  return(dR);
}


/*---------------------------------------------------------*/
int TileTMDBRawChannelMonTool::tilemodule_to_check(int sector){
/*---------------------------------------------------------*/
  int module = 0;
  switch ( sector )
    {
    case 0:
      module = 61;
      break;
    case 1:
      module = 62;
      break;
 
    case 2:
      module =  0;
      break;
    case 3:
      module =  1;
      break;
    case 4:
      module =  2;
      break;
 
    case 5:
      module =  4;
      break;
    case 6:
      module =  5;
      break;
    case 7:
      module =  6;
      break;
 
    case 8:
      module =  8;
      break;
    case 9:
      module =  9;
      break;
    case 10:
      module = 10;
      break;
 
    case 11:
      module = 12;
      break;
    case 12:
      module = 13;
      break;
    case 13:
      module = 14;
      break;
 
    case 14:
      module = 16;
      break;
    case 15:
      module = 17;
      break;
    case 16:
      module = 18;
      break;
 
    case 17:
      module = 20;
      break;
    case 18:
      module = 21;
      break;
    case 19:
      module = 22;
      break;
 
    case 20:
      module = 24;
      break;
    case 21:
      module = 25;
      break;
    case 22:
      module = 26;
      break;
 
    case 23:
      module = 28;
      break;
    case 24:
      module = 29;
      break;
    case 25:
      module = 30;
      break;
 
    case 26:
      module = 32;
      break;
    case 27:
      module = 33;
      break;
    case 28:
      module = 34;
      break;
 
    case 29:
      module = 36;
      break;
    case 30:
      module = 37;
      break;
    case 31:
      module = 38;
      break;
 
    case 32:
      module = 40;
      break;
    case 33:
      module = 41;
      break;
    case 34:
      module = 42;
      break;
 
    case 35:
      module = 44;
      break;
    case 36:
      module = 45;
      break;
    case 37:
      module = 46;
      break;
 
    case 38:
      module = 48;
      break;
    case 39:
      module = 49;
      break;
    case 40:
      module = 50;
      break;
 
    case 41:
      module = 52;
      break;
    case 42:
      module = 53;
      break;
    case 43:
      module = 54;
      break;
 
    case 44:
      module = 56;
      break;
    case 45:
      module = 57;
      break;
    case 46:
      module = 58;
      break;
 
    case 47:
      module = 60;
      break;
    }
 
  return module;
} // End of tilemoudle_to_check
