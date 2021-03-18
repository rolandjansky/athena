/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileDigiNoiseMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileDigiNoiseMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  std::sort(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end());

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( m_digitsContainerKey.initialize() );
  ATH_CHECK( m_DQstatusKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_DCSStateKey.initialize(m_checkDCS) );
  ATH_CHECK( m_tileCondToolNoiseSample.retrieve(EnableTool(m_fillPedestalDifference)) );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  m_pedGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigiNoisePed", Tile::MAX_ROS - 1, Tile::MAX_GAIN);
  m_hfnGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigiNoiseHFN", Tile::MAX_ROS - 1, Tile::MAX_GAIN);

  return StatusCode::SUCCESS;
}


StatusCode TileDigiNoiseMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

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

    std::vector<int> drawers[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
    std::vector<int> channels[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
    std::vector<float> pedestals[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
    std::vector<float> hfns[Tile::MAX_ROS - 1][Tile::MAX_GAIN];

    int nChannelsInPartition = 2880; // LB
    for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
      if (partition > 1) nChannelsInPartition = 2048; // EB
      for (unsigned int gain : {0, 1}) {
        drawers[partition][gain].reserve(nChannelsInPartition);
        channels[partition][gain].reserve(nChannelsInPartition);
        pedestals[partition][gain].reserve(nChannelsInPartition);
        hfns[partition][gain].reserve(nChannelsInPartition);
      }
    }

    const TileDQstatus* dqStatus = SG::makeHandle(m_DQstatusKey, ctx).get();
    const TileDCSState* dcsState = m_checkDCS ? SG::ReadCondHandle(m_DCSStateKey, ctx).cptr() : nullptr;

    SG::ReadCondHandle<TileBadChannels> badChannels(m_badChannelsKey, ctx);

    SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
    ATH_CHECK( digitsContainer.isValid() );

    for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
      if (digitsCollection->empty() ) continue;

      if (digitsCollection->getLvl1Type() != lvl1TriggerType) {
        ATH_MSG_DEBUG("Level1 Trigger Type in Tile digits [0x" << std::hex
                      << digitsCollection->getLvl1Type() << "] != [0x"
                      << lvl1TriggerType << std::dec << "] from the event info");
        continue;
      }

      HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
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
      if (digitsCollection->size() < nRequiredChannels) {
        ATH_MSG_DEBUG("Number of Tile channels with digits [" << digitsCollection->size()
                      << "] less than expected [" << nRequiredChannels << "]");
        continue;
      }

      bool checkDQ = true;

      int fragId = digitsCollection->identify();
      if (std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {
        checkDQ = false;
      }

      for (const TileDigits* tile_digits : *digitsCollection) {

        adc_id = tile_digits->adc_HWID();
        int channel = m_tileHWID->channel(adc_id);
        int adc = m_tileHWID->adc(adc_id);

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

        std::vector<float> digits = tile_digits->samples();

        double sampleMean = 0.0;
        double sampleRMS = 0.0;
        unsigned int nSamples = digits.size();

        for (double sample : digits) {
          sampleMean += sample;
          sampleRMS += sample * sample;
        }

        if (nSamples > 1) {
          double pedestal = digits[0];

          if(m_fillPedestalDifference) {
            if (!m_cabling->isDisconnected(ros, drawer, channel)) {
              pedestal -= m_tileCondToolNoiseSample->getPed(drawerIdx, channel, adc, TileRawChannelUnit::ADCcounts, ctx);
            } else {
              pedestal = 0.0;
            }
          }

          drawers[partition][adc].push_back(drawer);
          channels[partition][adc].push_back(channel);
          pedestals[partition][adc].push_back(pedestal);

          sampleMean /= nSamples;
          sampleRMS = sampleRMS / nSamples - sampleMean * sampleMean;
          sampleRMS = (sampleRMS > 0.0) ? sqrt(sampleRMS * nSamples / (nSamples - 1)) : 0.0;

          hfns[partition][adc].push_back(sampleRMS);

          ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": pedestal = " << pedestal << " HFN = " << sampleRMS);
        }
      } // digits
    }

    for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
      for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
        if (!pedestals[partition][gain].empty()) {
          auto monModule = Monitored::Collection("module", drawers[partition][gain]);
          auto monChannel = Monitored::Collection("channel", channels[partition][gain]);
          auto monPedestal = Monitored::Collection("pedestal", pedestals[partition][gain]);
          fill(m_tools[m_pedGroups[partition][gain]], monModule, monChannel, monPedestal);

          auto monHFN = Monitored::Collection("HFN", hfns[partition][gain]);
          fill(m_tools[m_hfnGroups[partition][gain]], monModule, monChannel, monHFN);
        }
      }
    }
  }

  fill("TileDigiNoiseMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
