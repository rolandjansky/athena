/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileDigitsFlxMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <math.h> 
// Athena includes
#include "StoreGate/ReadHandle.h"


TileDigitsFlxMonitorAlgorithm::TileDigitsFlxMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
{}


TileDigitsFlxMonitorAlgorithm::~TileDigitsFlxMonitorAlgorithm() {
}


StatusCode TileDigitsFlxMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_digitsContainerKey.initialize() );


  return StatusCode::SUCCESS;
}


StatusCode TileDigitsFlxMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  std::array<std::string, 2> gainName{"LG", "HG"};

  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
  ATH_CHECK( digitsContainer.isValid() );

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
 
    int fragId = digitsCollection->identify();
    unsigned int drawer = (fragId & 0x3F);
    unsigned int ros = fragId >> 8;

    for (const TileDigits* tile_digits : *digitsCollection) {

      HWIdentifier adcId = tile_digits->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);         
      std::vector<float> digits = tile_digits->samples();

      std::string nameSample = TileCalibUtils::getDrawerString(ros, drawer)
        + "_ch_" + std::to_string(channel) + "_" + gainName[gain] +  "_samples";
      auto channelSample = Monitored::Scalar<float>(nameSample, 0.0F);

      std::string channelName = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_channel";
      auto monitoredChannel = Monitored::Scalar<float>(channelName, channel);

      std::vector<float> digits_monitored;
      for(int i = m_firstSample; i<m_lastSample; i++){
        float sample = digits[i];


        channelSample = sample;
        fill("TileFlxMonSamples", channelSample);


        digits_monitored.push_back(sample);
      }
      
      float MonSamp_Mean = accumulate( digits_monitored.begin(), digits_monitored.end(), 0.0)/digits_monitored.size(); 
      double Sum_Distance = 0;
      for(int i = m_firstSample; i<m_lastSample; i++){
        float sample = digits[i];
        Sum_Distance += pow((sample-MonSamp_Mean),2);
      }
      int N = digits_monitored.size();
      double RMS_Mon_samples = sqrt(Sum_Distance/N);
      
      std::string HFN_name = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_HFN";
      auto HFN_samples = Monitored::Scalar<float>(HFN_name, RMS_Mon_samples);

      std::string Ped_name = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_Pedestal[0]";
      auto Pedestal_sample = Monitored::Scalar<float>(Ped_name, digits[m_firstSample]);

      fill("TileFlxMonPed",monitoredChannel,Pedestal_sample);
      
      fill("TileFlxMonHFN",monitoredChannel,HFN_samples);


    }


  }



  
  fill("TileDigitsFlxMonExecuteTime", timer);
    
  return StatusCode::SUCCESS;
}
