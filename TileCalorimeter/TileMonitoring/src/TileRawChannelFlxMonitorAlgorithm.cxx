/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRawChannelFlxMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include <math.h> 
// Athena includes
#include "StoreGate/ReadHandle.h"


TileRawChannelFlxMonitorAlgorithm::TileRawChannelFlxMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name, pSvcLocator)
  , m_tileHWID(nullptr)
{}


TileRawChannelFlxMonitorAlgorithm::~TileRawChannelFlxMonitorAlgorithm() {
}


StatusCode TileRawChannelFlxMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_digitsContainerKey.initialize() );

  ATH_CHECK( m_rawChannelContainerKeyFlx.initialize() );

  ATH_CHECK( m_rawChannelContainerKeyLegacy.initialize() );


  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelFlxMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  std::array<std::string, 2> gainName{"LG", "HG"};

  SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
  ATH_CHECK( digitsContainer.isValid() );

  float amplitude[TileCalibUtils::MAX_CHAN][TileCalibUtils::MAX_GAIN];
  float amplitudeFlx[TileCalibUtils::MAX_CHAN][TileCalibUtils::MAX_GAIN];

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainerFlx(m_rawChannelContainerKeyFlx, ctx);
  ATH_CHECK( rawChannelContainerFlx.isValid() );

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainerLegacy(m_rawChannelContainerKeyLegacy, ctx);
  ATH_CHECK( rawChannelContainerLegacy.isValid() );

  const TileFragHash& hashFunc = rawChannelContainerFlx->hashFunc();
 
  for (int fragID : m_fragIDsToCompare) {

    IdentifierHash hash = static_cast<IdentifierHash>(hashFunc(fragID));

    unsigned int drawer = (fragID & 0x3F);
    unsigned int ros = fragID >> 8;

    // Legacy (FIT) amplitude calculation
    const TileRawChannelCollection* rawChannelCollectionLegacy = rawChannelContainerLegacy->indexFindPtr(hash);
    for (const TileRawChannel* rawChannel : *rawChannelCollectionLegacy) {
  
      HWIdentifier adcId = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);

      std::string channelName = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_channel";
      auto monitoredChannel = Monitored::Scalar<float>(channelName, channel);
  
      amplitude[channel][gain] = rawChannel->amplitude();
      

      std::string Summary_name_Legacy = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_Summary_Legacy";
      auto Summary_sample_Legacy = Monitored::Scalar<float>(Summary_name_Legacy, amplitude[channel][gain]);
      fill("TileRawChannelLegacySummary", monitoredChannel, Summary_sample_Legacy);

  
    }
 
    // Felix (Opt2) amplitude calcualtion
    const TileRawChannelCollection* rawChannelCollectionFlx = rawChannelContainerFlx->indexFindPtr(hash);
    for (const TileRawChannel* rawChannel : *rawChannelCollectionFlx) {
  
      HWIdentifier adcId = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adcId);
      int gain = m_tileHWID->adc(adcId);
  
      std::string channelName = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_channel";
      auto monitoredChannel = Monitored::Scalar<float>(channelName, channel);

      amplitudeFlx[channel][gain] = rawChannel->amplitude();
     
      std::string Summary_name_Felix = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[gain] + "_Summary_Felix";
      auto Summary_sample_Felix = Monitored::Scalar<float>(Summary_name_Felix, amplitudeFlx[channel][gain]);
      fill("TileRawChannelFlxSummary", monitoredChannel, Summary_sample_Felix);

    }

 
  // Compare amplitude and amplitudeFlx arrays here and put results into histograms
  for(int i = 0; i<2; i++){
    for(int j = 0; j<48; j++){

      std::string channelName = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[i] + "_channel";
      auto monitoredChannel = Monitored::Scalar<float>(channelName, j);

      double diff_abs = amplitude[j][i]-amplitudeFlx[j][i];


      std::string Diff_name = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[i] + "_Diff";
      auto Diff_sample = Monitored::Scalar<float>(Diff_name, diff_abs);
      

      std::string Legacy_name = TileCalibUtils::getDrawerString(ros, drawer) + "_" + gainName[i] + "_Legacy";
      auto Legacy_sample = Monitored::Scalar<float>(Legacy_name, amplitude[j][i]);

      fill("TileRawChannelDiffLegacyFlx", monitoredChannel, Diff_sample);

      fill("TileRawChannelDiffLegacyFlx_Legacy", Legacy_sample, Diff_sample);
    }

  }







  }





  
  fill("TileDigitsFlxMonExecuteTime", timer);
    
  return StatusCode::SUCCESS;
}
