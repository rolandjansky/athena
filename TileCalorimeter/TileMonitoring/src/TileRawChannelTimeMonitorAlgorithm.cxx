/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawChannelTimeMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileRawChannelTimeMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_DQstatusKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_DCSStateKey.initialize(m_checkDCS) );

  using Tile = TileCalibUtils;
  using namespace Monitored;
  const int nDigitizers = 8;

  m_timeGroups = buildToolMap<int>(m_tools, "TileAverageTime", Tile::MAX_ROS - 1);
  m_timeLBGroups = buildToolMap<int>(m_tools, "TileAverageTimeLB", Tile::MAX_ROS - 1);
  m_timeDiffLBGroups = buildToolMap<int>(m_tools, "TileAverageTimeDifferenceLB", m_partitionTimeDifferencePairs.size());
  m_digiTimeLBGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileDigitizerTimeLB",
                                                                   Tile::MAX_ROS - 1, Tile::MAX_DRAWER, nDigitizers);

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelTimeMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  std::vector<int> drawers[Tile::MAX_ROS - 1];
  std::vector<int> channels[Tile::MAX_ROS - 1];
  std::vector<double> channelTimes[Tile::MAX_ROS - 1];

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  const TileDQstatus* dqStatus = SG::makeHandle(m_DQstatusKey, ctx).get();
  const TileDCSState* dcsState = m_checkDCS ? SG::ReadCondHandle(m_DCSStateKey, ctx).cptr() : nullptr;

  SG::ReadCondHandle<TileBadChannels> badChannels(m_badChannelsKey, ctx);

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
  ATH_CHECK( rawChannelContainer.isValid() );

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    if (rawChannelCollection->empty() ) continue;

    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    // unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;

    for (const TileRawChannel* rawChannel : *rawChannelCollection) {

      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);

      if (m_cabling->isDisconnected(ros, drawer, channel)) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": channlel is disconnected => skipping!");
        continue;
      }

      if (!(dqStatus->isAdcDQgood(ros, drawer, channel, adc))) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": DQ is BAD => skipping!");
        continue;
      }

      if (m_checkDCS && dcsState->isStatusBad(ros, drawer, channel)) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": DCS is Bad => skipping!");
        continue;
      }

      TileBchStatus status = badChannels->getAdcStatus(adc_id);
      if (status.isBad() || status.isBadTiming()) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": Status or Timing is BAD => skipping!");
        continue;
      }

      if (rawChannel->amplitude() < m_energyThresholds[adc]) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": Energy is below threshold => skipping!");
        continue;
      }

      drawers[partition].push_back(drawer);
      channels[partition].push_back(channel);
      channelTimes[partition].push_back(rawChannel->time());

    }
  }

  float partitionTime[4] = {0};

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    int nChannels = 0;
    double averagePartitionTime = 0.0;
    std::vector<double>& times = channelTimes[partition];

    for (unsigned int channelIdx = 0; channelIdx < channels[partition].size(); ++ channelIdx) {
      int channel = channels[partition][channelIdx];
      if ( (partition > 1) // EB, exclude some channels (most likely single PMT) from calculating average time
           && (channel < 6 || channel == 12 || channel == 13 || channel == 18 || channel == 19) ) continue;

      averagePartitionTime += times[channelIdx];
      ++nChannels;
    }

    if (nChannels > 0) {
      averagePartitionTime /= nChannels;
      partitionTime[partition] = averagePartitionTime - m_partitionTimeCorrection[partition];

      std::transform(times.begin(), times.end(), times.begin(),
                     [averagePartitionTime] (double time) {return time - averagePartitionTime;});
    }
  }

  static const int channel2digitizer[48] = {7, 7, 7,  7, 7, 7,
                                            6, 6, 6,  6, 6, 6,
                                            5, 5, 5,  5, 5, 5,
                                            4, 4, 4,  4, 4, 4,
                                            3, 3, 3,  3, 3, 3,
                                            2, 2, 2,  2, 2, 2,
                                            1, 1, 1,  1, 1, 1,
                                            0, 0, 0,  0, 0, 0};

  unsigned int lumiBlock = eventInfo->lumiBlock();
  auto monLumiBlock = Monitored::Scalar<double>("lumiBlock", lumiBlock);

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (!channelTimes[partition].empty()) {
      auto monModule = Monitored::Collection("module", drawers[partition]);
      auto monChannel = Monitored::Collection("channel", channels[partition]);
      auto monTime = Monitored::Collection("time", channelTimes[partition]);
      fill(m_tools[m_timeGroups[partition]], monModule, monChannel, monTime);

      auto monPartitionTime = Monitored::Scalar<float>("time", partitionTime[partition]);
      fill(m_tools[m_timeLBGroups[partition]], monLumiBlock, monPartitionTime);

      for (unsigned int channelIdx = 0; channelIdx < channels[partition].size(); ++ channelIdx) {
        int drawer = drawers[partition][channelIdx];
        int digitizer = channel2digitizer[ channels[partition][channelIdx] ];
        auto monTime = Monitored::Scalar<double>("time", channelTimes[partition][channelIdx]);
        fill(m_tools[m_digiTimeLBGroups[partition][drawer][digitizer]], monLumiBlock, monTime);
      }

    }
  }

  for (unsigned int pairIdx = 0; pairIdx < m_partitionTimeDifferencePairs.size(); ++pairIdx) {
    const std::pair<int, int>& partitionPair = m_partitionTimeDifferencePairs[pairIdx];
    int partition1 = partitionPair.first;
    int partition2 = partitionPair.second;
    auto monTime = Monitored::Scalar<double>("time", partitionTime[partition1] - partitionTime[partition2]);
    fill(m_tools[m_timeDiffLBGroups[pairIdx]], monLumiBlock, monTime);
  }

  fill("TileRawChanTimeMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
