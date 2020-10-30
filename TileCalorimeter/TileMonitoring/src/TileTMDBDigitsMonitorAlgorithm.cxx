/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileTMDBDigitsMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <math.h> 
// Athena includes
#include "StoreGate/ReadHandle.h"


TileTMDBDigitsMonitorAlgorithm::TileTMDBDigitsMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
{

}

TileTMDBDigitsMonitorAlgorithm::~TileTMDBDigitsMonitorAlgorithm() {
}


StatusCode TileTMDBDigitsMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_MSG_DEBUG("in initialize()");

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_digitsContainerKey.initialize() );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  m_hfnGroups = buildToolMap<int>(m_tools, "TMDBDigitsHFN", Tile::MAX_ROS - 1);
  m_pedGroups = buildToolMap<int>(m_tools, "TMDBDigitsPedestal", Tile::MAX_ROS - 1);    
  m_ampGroups = buildToolMap<int>(m_tools, "TMDBDigitsAmplitude", Tile::MAX_ROS - 1);


    if (m_nChannels.size() != (Tile::MAX_ROS - 1)) {
       ATH_MSG_FATAL( "Size of NumberOfChannels should be 4 (LBA, LBC, EBA, EBC), but is " << m_nChannels.size() );
       return StatusCode::FAILURE;
    }

    std::vector<std::string> partitionName = {"LBA", "LBC", "EBA", "EBC"}; // ROS - 1 to partition name map
    for (unsigned int partition = 0; partition < Tile::MAX_ROS-1; ++partition) {
      m_cellPedGroups.push_back(buildToolMap<int>(m_tools,
                                                  "TMDBDigitsCellPedestal_" + partitionName[partition],
                                                  m_nChannels[partition]));
      m_cellHFNGroups.push_back(buildToolMap<int>(m_tools,
                                                  "TMDBDigitsCellHFN_" + partitionName[partition],
                                                  m_nChannels[partition]));
      m_cellAmpGroups.push_back(buildToolMap<int>(m_tools,
                                                  "TMDBDigitsCellAmplitude_" + partitionName[partition],
                                                  m_nChannels[partition]));                                                                                        
    }
    

  return StatusCode::SUCCESS;
}


StatusCode TileTMDBDigitsMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG( "in fillHistograms()" );
  using Tile = TileCalibUtils;
  std::vector<int> drawers[Tile::MAX_ROS - 1];
  std::vector<int> channels[Tile::MAX_ROS - 1];
  std::vector<float> pedestals[Tile::MAX_ROS - 1];
  std::vector<float> amplitudes[Tile::MAX_ROS - 1];
  std::vector<float> hfns[Tile::MAX_ROS - 1];
  static constexpr int TMDB_MAX_CHANNEL = 8;
  std::vector<float> cellPedestals[Tile::MAX_ROS - 1][TMDB_MAX_CHANNEL];
  std::vector<float> cellHFNs[Tile::MAX_ROS - 1][TMDB_MAX_CHANNEL];
  std::vector<float> cellAmplitudes[Tile::MAX_ROS - 1][TMDB_MAX_CHANNEL];

  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
  ATH_CHECK( digitsContainer.isValid() );

  for (IdentifierHash hash : digitsContainer->GetAllCurrentHashes()) {
    const TileDigitsCollection* digitsCollection = digitsContainer->indexFindPtr (hash);
 
    int fragId = digitsCollection->identify();
    unsigned int drawer = (fragId & 0x3F);
    unsigned int ros = fragId >> 8;
    unsigned int partition = ros - 1;

    for (const TileDigits* tile_digits : *digitsCollection) {

      HWIdentifier adc_id = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      std::vector<float> digits = tile_digits->samples();

      double mean_samp = 0.0;
      double rms_samp = 0.0;
      unsigned int n_digits = digits.size();

      for (double digit : digits) {
        mean_samp += digit;
        rms_samp += digit * digit;
      }

      if (n_digits > 1) {
        drawers[partition].push_back(drawer);
        channels[partition].push_back(channel);

        double ped = digits[0];
        pedestals[partition].push_back(ped);
  
        mean_samp /= n_digits;
        rms_samp = rms_samp / n_digits - mean_samp * mean_samp;
        rms_samp = (rms_samp > 0.0) ? sqrt(rms_samp * n_digits / (n_digits - 1)) : 0.0;
        hfns[partition].push_back(rms_samp);

        //-- temporary until TMDB raw channel monitoring tool be ready --//

        auto min_max = std::minmax_element(digits.begin(), digits.end());
        float min_digit = *min_max.first;
        float max_digit = *min_max.second;
        float amplitude = max_digit - min_digit;
        amplitudes[partition].push_back(amplitude);


        //---------------------------------------------------------------//
        if (channel < m_nChannels[partition]) {
         cellPedestals[partition][channel].push_back(ped);
         cellHFNs[partition][channel].push_back(rms_samp);
         cellAmplitudes[partition][channel].push_back(amplitude);
        }

      } 
    }
  }


  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (!pedestals[partition].empty()) {
      auto monModule = Monitored::Collection("module", drawers[partition]);
      auto monChannel = Monitored::Collection("channel", channels[partition]);
      auto monPedestal = Monitored::Collection("pedestal", pedestals[partition]);
      fill(m_tools[m_pedGroups[partition]], monModule, monChannel, monPedestal);

      auto monHFN = Monitored::Collection("HFN", hfns[partition]);
      fill(m_tools[m_hfnGroups[partition]], monModule, monChannel, monHFN);
    
      auto monAmplitude = Monitored::Collection("amplitude", amplitudes[partition]);
      fill(m_tools[m_ampGroups[partition]], monModule, monChannel, monAmplitude);   

      for (int channel = 0; channel < int(m_nChannels[partition]); ++channel) {
        if (!cellPedestals[partition][channel].empty()) {
          auto monPedestal = Monitored::Collection("pedestal", cellPedestals[partition][channel]);
          fill(m_tools[m_cellPedGroups[partition][channel]], monPedestal);          
        }
        if (!cellHFNs[partition][channel].empty()) {
          auto monHFN = Monitored::Collection("HFN", cellHFNs[partition][channel]);
          fill(m_tools[m_cellHFNGroups[partition][channel]], monHFN);          
        }
        if (!cellAmplitudes[partition][channel].empty()) {
          auto monAmplitude = Monitored::Collection("amplitude", cellAmplitudes[partition][channel]);
          fill(m_tools[m_cellAmpGroups[partition][channel]], monAmplitude);          
        }
      }      
    }
  }   

    
  return StatusCode::SUCCESS;
}
