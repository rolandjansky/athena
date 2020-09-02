/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawChannelNoiseMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileRawChannelNoiseMonitorAlgorithm::initialize() {

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
  ATH_CHECK( m_tileCondToolEmscale.retrieve() );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  m_ampGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileRawChannelNoise",
                                                            Tile::MAX_ROS - 1, Tile::MAX_DRAWER, Tile::MAX_CHAN);

  return StatusCode::SUCCESS;
}


StatusCode TileRawChannelNoiseMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();


  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Run = " << eventInfo->runNumber()
                    << " LB = " << eventInfo->lumiBlock()
                    << " Evt = " << eventInfo->eventNumber()
                    << " BCID = " << eventInfo->bcid()
                    << " lvl1 = 0x" << std::hex << eventInfo->level1TriggerType() << std::dec;

    const std::vector<xAOD::EventInfo::StreamTag>& evtStreamTags = eventInfo->streamTags();
    if (!evtStreamTags.empty()) {
      msg(MSG::DEBUG) << " stream name/type:";
      for (const auto& evtStreamTag : evtStreamTags) {
        msg(MSG::DEBUG) << " " << evtStreamTag.name() << "/" << evtStreamTag.type();
      }
    }

    msg(MSG::DEBUG) << endmsg;
  }


  unsigned int lvl1TriggerType = eventInfo->level1TriggerType();

  if (m_triggerTypes.empty()
      || std::find( m_triggerTypes.begin(), m_triggerTypes.end(), lvl1TriggerType) != m_triggerTypes.end()) {

    const TileDQstatus* dqStatus = SG::makeHandle(m_DQstatusKey, ctx).get();
    const TileDCSState* dcsState = m_checkDCS ? SG::ReadCondHandle(m_DCSStateKey, ctx).cptr() : nullptr;

    SG::ReadCondHandle<TileBadChannels> badChannels(m_badChannelsKey, ctx);

    SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
    ATH_CHECK( rawChannelContainer.isValid() );

    // What is the unit used to store info in the RawChannelContainer ?
    TileRawChannelUnit::UNIT rawChannelUnit = rawChannelContainer->get_unit();
    bool recalibrate(false);
    if (rawChannelUnit != TileRawChannelUnit::ADCcounts) {
      ATH_MSG_VERBOSE( " RawChannel Units is = " << rawChannelUnit << " => recalibrating in ADC counts"  );
      recalibrate = true;
    }

    for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
      if (rawChannelCollection->empty() ) continue;

      if (rawChannelCollection->getLvl1Type() != lvl1TriggerType) {
        ATH_MSG_DEBUG("Level1 Trigger Type in Tile raw channels [0x" << std::hex
                      << rawChannelCollection->getLvl1Type() << "] != [0x"
                      << lvl1TriggerType << std::dec << "] from the event info");
        continue;
      }

      HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      int drawer = m_tileHWID->drawer(adc_id);
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      int partition = ros - 1;

      unsigned int nBadOrDisconnectedChannels = 0;
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {
        if (m_cabling->isDisconnected(ros, drawer, channel)) {
          ++nBadOrDisconnectedChannels;
        }
      }

      unsigned int nRequiredChannels = TileCalibUtils::MAX_CHAN - nBadOrDisconnectedChannels;
      if (rawChannelCollection->size() < nRequiredChannels) {
        ATH_MSG_DEBUG("Number of Tile channels with digits [" << rawChannelCollection->size()
                      << "] less than expected [" << nRequiredChannels << "]");
        continue;
      }

      bool checkDQ = true;

      int fragId = rawChannelCollection->identify();
      if (std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {
        checkDQ = false;
      }

      for (const TileRawChannel* rawChannel : *rawChannelCollection) {

        adc_id = rawChannel->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);

        if (adc != m_gain) continue;

        if (checkDQ && !(dqStatus->isAdcDQgood(ros, drawer, channel, adc))) {
          ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": DQ is BAD => skipping!");
          continue;
        }

        if (m_checkDCS && dcsState->isStatusBad(ros, drawer, channel)) {
          ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": DCS is Bad => skipping!");
          continue;
        }

        if (badChannels->getAdcStatus(adc_id).isBad()) {
          ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": Status is BAD => skipping!");
          continue;
        }

        float amplitude = rawChannel->amplitude();
        if (recalibrate) {
           amplitude = m_tileCondToolEmscale->channelCalib(drawerIdx, channel, adc, amplitude,
                                                           rawChannelUnit, TileRawChannelUnit::ADCcounts);
        }

        auto monAmplitude = Monitored::Scalar<float>("amplitude", amplitude);
        fill(m_tools[m_ampGroups[partition][drawer][channel]], monAmplitude);

      }
    }
  }


  fill("TileRawChanNoiseMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
