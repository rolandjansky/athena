/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileTMDBRawChannelMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCondToolTMDB.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

#include <math.h>
// Athena includes
#include "StoreGate/ReadHandle.h"

TileTMDBRawChannelMonitorAlgorithm::TileTMDBRawChannelMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
  , m_eff(false)
  , m_isDSP(true)
  , m_numberOfThresholds(2)
  , m_amplitudeThreshold(80)
{
    declareProperty("AmplitudeThresholdForTime", m_amplitudeThreshold = 80);
    declareProperty("DSP", m_isDSP = true);
    declareProperty("Efficiency", m_eff = true);
    declareProperty("numberOfThresholds", m_numberOfThresholds = 2);
}

TileTMDBRawChannelMonitorAlgorithm::~TileTMDBRawChannelMonitorAlgorithm() {
}

StatusCode TileTMDBRawChannelMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  ATH_MSG_DEBUG("in initialize()");

    ATH_CHECK( detStore()->retrieve(m_tileHWID) );
    ATH_CHECK( m_rawChannelContainerKey.initialize() );
    ATH_CHECK( m_muonContainerKey.initialize(m_eff) );
    ATH_CHECK( m_muonRoIsContainerKey.initialize(m_eff) );
    ATH_CHECK( m_tileCondToolTMDB.retrieve(EnableTool{m_eff}) );

    using Tile = TileCalibUtils;
    using namespace Monitored;

    if (m_eff) {
      m_tgcSectorGroup = buildToolMap<int>(m_tools, "TGC_TrSec_number_Good_Muons", 2);  
      m_coinThrDGroup = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TMDB_coincidence_threshold", 2, m_numberOfThresholds, 2);
    }

    m_isDSP = (m_rawChannelContainerKey.key() == "MuRcvRawChCnt");
    m_ampGroups = buildToolMap<int>(m_tools, "TMDB_RawAmplitude", Tile::MAX_ROS - 1);
    m_timeGroups = buildToolMap<int>(m_tools, "TMDB_RawTime", Tile::MAX_ROS - 1);

    if (m_nChannels.size() != (Tile::MAX_ROS - 1)) {
      ATH_MSG_FATAL( "Size of NumberOfChannels should be 4 (LBA, LBC, EBA, EBC), but is " << m_nChannels.size() );
      return StatusCode::FAILURE;
    }

    std::vector<std::string> partitionName = {"LBA", "LBC", "EBA", "EBC"}; // ROS - 1 to partition name map

    for (unsigned int partition = 0; partition < Tile::MAX_ROS-1; ++partition) {
      m_ampCell.push_back(buildToolMap<int>(m_tools,
                                              "TMDB_RawCellAmplitude_" + partitionName[partition],
                                              m_nChannels[partition]));
      m_timeCell.push_back(buildToolMap<int>(m_tools,
                                              "TMDB_RawCellTime_" + partitionName[partition],
                                              m_nChannels[partition]));
    }

  return StatusCode::SUCCESS;
}


StatusCode TileTMDBRawChannelMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG( "in fillHistograms()" );
  using Tile = TileCalibUtils;
  std::vector<int> drawers[Tile::MAX_ROS - 1];
  std::vector<int> channels[Tile::MAX_ROS - 1];
  std::vector<float> amplitudes[Tile::MAX_ROS - 1];
 
  std::vector<int> timeDrawers[Tile::MAX_ROS - 1];
  std::vector<int> timeChannels[Tile::MAX_ROS - 1];  
  std::vector<float> times[Tile::MAX_ROS - 1];

  static constexpr int TMDB_MAX_CHANNEL = 8;
  std::vector<float> cellAmplitudes[Tile::MAX_ROS - 1][TMDB_MAX_CHANNEL];
  std::vector<float> cellTimes[Tile::MAX_ROS - 1][TMDB_MAX_CHANNEL];

  static constexpr int TMDB_NUMBER_LAYERS = 2;
  static constexpr int TMDB_NUMBER_BARRELS = 2;
  std::vector<float> tgcSectors[2];
  std::vector<float> coinThrModules_1Th[TMDB_NUMBER_BARRELS][1][TMDB_NUMBER_LAYERS]; // (EBAxEBC) x Thresholds x (D5 x D5+D6) 
  std::vector<float> coinThrModules_2Th[TMDB_NUMBER_BARRELS][2][TMDB_NUMBER_LAYERS]; // (EBAxEBC) x Thresholds x (D5 x D5+D6) 

  float TMDB_D6_amplitude[2][64]; // (EBA and EBC) x 64 modules
  float TMDB_D56_amplitude[2][64]; // (EBA and EBC) x 64 modules

  SG::ReadHandle<TileRawChannelContainer> rawContainer(m_rawChannelContainerKey, ctx);
  for (IdentifierHash hash : rawContainer->GetAllCurrentHashes()) {
    const TileRawChannelCollection* rawChannelCollection = rawContainer->indexFindPtr(hash);
    if (rawChannelCollection->empty()) continue;
    int fragId = rawChannelCollection->identify();
    unsigned int drawer = (fragId & 0x3F);
    unsigned int ros = fragId >> 8;
    unsigned int partition = ros - 1;
    
    for (const TileRawChannel* rawChannel : *rawChannelCollection) {
      HWIdentifier adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      float amplitude = rawChannel->amplitude();

      if(channel < m_nChannels[partition]){
        if(m_eff){
          if (partition > 1) {
            TMDB_D56_amplitude[partition - 2][drawer] += amplitude;
            if (channel == 2 || channel == 3) { TMDB_D6_amplitude[partition - 2][drawer] += amplitude;}
          }
        }else{
          channels[partition].push_back(channel);
          drawers[partition].push_back(drawer);
          amplitudes[partition].push_back(amplitude);
          cellAmplitudes[partition][channel].push_back(amplitude);
          if(m_isDSP == false){
            if (amplitude > m_amplitudeThreshold) {
              timeChannels[partition].push_back(channel);
              timeDrawers[partition].push_back(drawer);
              float time = rawChannel->time();
              times[partition].push_back(time);
              cellTimes[partition][channel].push_back(time);
            }
          }  
        }
      }
    }
  }

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (!amplitudes[partition].empty()) {
      auto monModule = Monitored::Collection("module", drawers[partition]);
      auto monChannel = Monitored::Collection("channel", channels[partition]);
      auto monAmplitude = Monitored::Collection("amplitude", amplitudes[partition]);
      fill(m_tools[m_ampGroups[partition]], monModule, monChannel, monAmplitude);
      for (int channel = 0; channel < int(m_nChannels[partition]); ++channel) {
        if (!cellAmplitudes[partition][channel].empty()) {
          auto monAmplitude = Monitored::Collection("amplitude", cellAmplitudes[partition][channel]);
          fill(m_tools[m_ampCell[partition][channel]], monAmplitude);
        }
      }
    }

    if (!times[partition].empty()) {
      auto monModule = Monitored::Collection("module", timeDrawers[partition]);
      auto monChannel = Monitored::Collection("channel", timeChannels[partition]);
      auto monTime = Monitored::Collection("time", times[partition]);
      fill(m_tools[m_timeGroups[partition]], monModule, monChannel, monTime);
      for (int channel = 0; channel < int(m_nChannels[partition]); ++channel) {
        if (!cellAmplitudes[partition][channel].empty()) {
          auto monTime = Monitored::Collection("time", cellTimes[partition][channel]);
          fill(m_tools[m_timeCell[partition][channel]], monTime);
        }
      }
    }
  }

  if (m_eff == true) {
  
    SG::ReadHandle<xAOD::MuonContainer> muonContainer(m_muonContainerKey, ctx);
    ATH_CHECK( muonContainer.isValid() );
     
    SG::ReadHandle<xAOD::MuonRoIContainer> muonRoIsContainer(m_muonRoIsContainerKey, ctx);
    ATH_CHECK( muonRoIsContainer.isValid() );

    for ( const xAOD::MuonRoI* roi_mu : *muonRoIsContainer ) {
      float eta   = roi_mu->eta();
      float phi   = roi_mu->phi();
      int pt_thr  = roi_mu->getThrNumber();
      int sector  = (roi_mu->getSectorAddress()>>1) & (0x3F);
      
      if(!(roi_mu->getSource() == xAOD::MuonRoI::Endcap && (pt_thr == 5 || pt_thr == 6))) { continue; }
      if( fabs(eta) < 1.0 || 1.3 < fabs(eta) )  { continue; }

      int m1 = tilemodule_to_check( sector );
      
      for ( const xAOD::Muon* mu : *muonContainer ) {
        float muon_eta = mu->eta();
        float muon_phi = mu->phi();
        float muon_pt  = mu->pt();
        float dR = calc_dR(eta-muon_eta, phi-muon_phi);
        
        if(dR > 0.1)       {continue;} // dR cut
        if(muon_pt < 15000) {continue;} // pt cut
        if(mu->muonType() != xAOD::Muon::Combined)   {continue;} // combined muon

        unsigned int partition  = (muon_eta > 0.0) ? 0 : 1;
        tgcSectors[partition].push_back(sector);

        unsigned int drawerIdx;

        for(unsigned int partition = 0; partition < 2; ++partition) {
          for(int module = m1; module <= m1 + 1; ++module) {
             
            drawerIdx = Tile::getDrawerIdx(partition+3, module);

            if( TMDB_D6_amplitude[partition][module]  > m_tileCondToolTMDB->getThreshold(drawerIdx, 0) ) {
              coinThrModules_1Th[partition][0][0].push_back(module+1);
              coinThrModules_2Th[partition][0][0].push_back(module+1);
            }
            if( (TMDB_D56_amplitude[partition][module] > m_tileCondToolTMDB->getThreshold(drawerIdx, 2)  )) {
              coinThrModules_1Th[partition][0][1].push_back(module+1);
              coinThrModules_2Th[partition][0][0].push_back(module+1);
            }

            if(m_numberOfThresholds == 2){
              if( (TMDB_D6_amplitude[partition][module] > m_tileCondToolTMDB->getThreshold(drawerIdx, 1)  )) {
                coinThrModules_2Th[partition][1][0].push_back(module+1);
              }
              if( (TMDB_D56_amplitude[partition][module] > m_tileCondToolTMDB->getThreshold(drawerIdx, 3)  )) {
                coinThrModules_2Th[partition][1][1].push_back(module+1);
              }  
            }           
          }
        }
      } //mu_loop
    } //trig_mu_loop


    for (unsigned int partition = 0; partition < 2; ++partition) {
      if (!tgcSectors[partition].empty()) {
        auto monSector = Monitored::Collection("sector", tgcSectors[partition]);
        fill(m_tools[m_tgcSectorGroup[partition]], monSector);
        for(unsigned int threshold = 0; threshold < m_numberOfThresholds; ++threshold){
          for(unsigned int cell = 0; cell <2; ++cell){
            if (!coinThrModules_1Th[partition][threshold][cell].empty()) {
              if(m_numberOfThresholds == 1){
                auto monModule = Monitored::Collection("module", coinThrModules_1Th[partition][threshold][cell]);
                fill(m_tools[m_coinThrDGroup[partition][threshold][cell]], monModule);
              }
              if(m_numberOfThresholds == 2){
                auto monModule = Monitored::Collection("module", coinThrModules_2Th[partition][threshold][cell]);
                fill(m_tools[m_coinThrDGroup[partition][threshold][cell]], monModule);
              }
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
float TileTMDBRawChannelMonitorAlgorithm::calc_dR(float dEta, float dPhi) const {
/*---------------------------------------------------------*/
  float dR = sqrt( (dEta*dEta) + (dPhi*dPhi) );
  return(dR);
}

/*---------------------------------------------------------*/
int TileTMDBRawChannelMonitorAlgorithm::tilemodule_to_check(int sector) const{
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
