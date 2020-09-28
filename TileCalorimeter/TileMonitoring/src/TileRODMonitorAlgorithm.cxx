/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRODMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileRODMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( TileMonitorAlgorithm::initialize() );

  std::sort(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end());

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_dspRawChannelContainerKey.initialize() );
  ATH_CHECK( m_DQstatusKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_emScaleKey.initialize() );
  ATH_CHECK( m_DCSStateKey.initialize(m_checkDCS) );

  if (m_comparisonUnit <= TileRawChannelUnit::OnlineMegaElectronVolts) {
    m_finalRawChannelUnit = static_cast<TileRawChannelUnit::UNIT>(m_comparisonUnit.value());
  }

  if (m_timeRange.size() != 2) {
    ATH_MSG_FATAL( "Size of TimeRange should be 2 (from,to), but is " << m_timeRange.size() );
    return StatusCode::FAILURE;
  }

  using namespace Monitored;
  using Tile = TileCalibUtils;

  int nL1Triggers = getNumberOfL1Triggers();

  m_energyDiffGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspEnergyDiff",
                                                      Tile::MAX_ROS - 1, nL1Triggers);

  m_energyDiffVsTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspEnergyDiffVsTime",
                                                            Tile::MAX_ROS - 1, nL1Triggers);

  m_energyDiffVsEnergyGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspEnergyDiffVsEnergy",
                                                              Tile::MAX_ROS - 1, nL1Triggers);

  m_timeDiffGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspTimeDiff",
                                                    Tile::MAX_ROS - 1, nL1Triggers);

  m_timeDiffVsTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspTimeDiffVsTime",
                                                          Tile::MAX_ROS - 1, nL1Triggers);

  m_timeDiffVsEnergyGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspTimeDiffVsEnergy",
                                                            Tile::MAX_ROS - 1, nL1Triggers);

  m_dspChanTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileDspChannelTime",
                                                       Tile::MAX_ROS - 1, nL1Triggers);

  if (m_fillRODfragSizeHistograms) {
    ATH_CHECK( m_robSvc.retrieve() );

    if (m_nROBs > MAX_TILE_ROBS) {
      int nROBsSetUp = m_nROBs;
      m_nROBs = MAX_TILE_ROBS;
      ATH_MSG_INFO( "Decreasing number of ROBs from "  << nROBsSetUp << " to " << m_nROBs );
    }

    m_tileRobIds.reserve((Tile::MAX_ROS - 1) * m_nROBs); // Partitions * fragments
    for (unsigned int rodId : {0x510000, 0x520000, 0x530000, 0x540000}) {
      for (unsigned int fragment = 0; fragment < m_nROBs; ++fragment) {
        m_tileRobIds.push_back(rodId + fragment);
      }
    }

    m_rodFragSizeMapGroups = buildToolMap<int>(m_tools, "TileRodFragmentMapSize", nL1Triggers);
    m_rodFragSizeLBGroups = buildToolMap<int>(m_tools, "TileRodFragmentSizeLB", nL1Triggers);
  }

  return StatusCode::SUCCESS;
}


StatusCode TileRODMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  ATH_MSG_DEBUG(*eventInfo);

  const TileDQstatus* dqStatus = SG::makeHandle(m_DQstatusKey, ctx).get();
  const TileDCSState* dcsState = m_checkDCS ? SG::ReadCondHandle(m_DCSStateKey, ctx).cptr() : nullptr;

  SG::ReadCondHandle<TileBadChannels> badChannels(m_badChannelsKey, ctx);
  SG::ReadCondHandle<TileEMScale> emScale(m_emScaleKey, ctx);

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
  ATH_CHECK( rawChannelContainer.isValid() );

  TileRawChannelUnit::UNIT rawChannelUnit = rawChannelContainer->get_unit();

  float referenceTimes[Tile::MAX_ROS - 1][Tile::MAX_DRAWER][Tile::MAX_CHAN] = {{{0}}};
  float referenceEnergies[Tile::MAX_ROS - 1][Tile::MAX_DRAWER][Tile::MAX_CHAN] = {{{0}}};

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    if (rawChannelCollection->empty() ) continue;

    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;

    bool checkDQ = true;

    int fragId = rawChannelCollection->identify();
    if (std::binary_search(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end(), fragId)) {
      checkDQ = false;
    }

    for (const TileRawChannel* rawChannel : *rawChannelCollection) {

      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);

      if (m_cabling->isDisconnected(ros, drawer, channel)) {
        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": channlel is disconnected => skipping!");
        continue;
      }

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

      if (ros > 2) { // Check if channel is MBTS, no MBTS in LB
        int pmt;
        int index;
        m_cabling->h2s_cell_id_index(adc_id, index, pmt);
        if (index < -1) {
          ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": MBTS => skipping!");
          continue;
        }
      }

      float amplitude = rawChannel->amplitude();
      if (rawChannelUnit != m_comparisonUnit) {
        amplitude = emScale->calibrateChannel(drawerIdx, channel, adc, amplitude, rawChannelUnit, m_finalRawChannelUnit);
      }

      float time = rawChannel->uncorrTime();
      if (amplitude > m_energyThreshold && time > m_timeRange[0] && time < m_timeRange[1]) {
        referenceEnergies[partition][drawer][channel] = amplitude;
        referenceTimes[partition][drawer][channel] = time;
      }
    }
  }

  std::vector<float> timeDiffs[Tile::MAX_ROS - 1];
  std::vector<float> energyDiffs[Tile::MAX_ROS - 1];
  std::vector<float> offlineEnergies[Tile::MAX_ROS - 1];
  std::vector<float> offlineTimes[Tile::MAX_ROS - 1];

  std::vector<float> dspTimes[Tile::MAX_ROS - 1];
  std::vector<float> dspTimesChannels[Tile::MAX_ROS - 1];
  std::vector<float> dspTimesDrawers[Tile::MAX_ROS - 1];

  SG::ReadHandle<TileRawChannelContainer> dspRawChannelContainer(m_dspRawChannelContainerKey, ctx);
  ATH_CHECK( dspRawChannelContainer.isValid() );

  TileRawChannelUnit::UNIT dspRawChannelUnit = dspRawChannelContainer->get_unit();
  for (const TileRawChannelCollection* rawChannelCollection : *dspRawChannelContainer) {
    if (rawChannelCollection->empty() ) continue;

    HWIdentifier adc_id = rawChannelCollection->front()->adc_HWID();
    int ros = m_tileHWID->ros(adc_id);
    int drawer = m_tileHWID->drawer(adc_id);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;

    for (const TileRawChannel* rawChannel : *rawChannelCollection) {
      adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      int adc = m_tileHWID->adc(adc_id);

      float offlineEnergy = referenceEnergies[partition][drawer][channel];
      if (offlineEnergy > m_energyThreshold) {
        offlineEnergies[partition].push_back(offlineEnergy);

        float offlineTime = referenceTimes[partition][drawer][channel];
        offlineTimes[partition].push_back(offlineTime);

        float dspEnergy = rawChannel->amplitude();
        float dspTime = rawChannel->uncorrTime();
        dspTimes[partition].push_back(dspTime);
        dspTimesDrawers[partition].push_back(drawer);
        dspTimesChannels[partition].push_back(channel);

        if (dspRawChannelUnit != m_finalRawChannelUnit) {
          dspEnergy = emScale->calibrateChannel(drawerIdx, channel, adc, dspEnergy, dspRawChannelUnit, m_finalRawChannelUnit);
        }

        float energyDiff = (dspEnergy - offlineEnergy) / offlineEnergy;
        energyDiffs[partition].push_back(energyDiff);

        float timeDiff = dspTime - offlineTime;
        timeDiffs[partition].push_back(timeDiff);

        ATH_MSG_VERBOSE(m_tileHWID->to_string(adc_id) << ": (DSP/OFFLINE)"
                        << ", energy " << dspEnergy << "/" << offlineEnergy
                        << ", time " << dspTime << "/" << offlineTime);
      }
    }
  }

  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (!energyDiffs[partition].empty()) {
      auto monTime = Monitored::Collection("time", offlineTimes[partition]);
      auto monEnergy = Monitored::Collection("energy", offlineEnergies[partition]);
      auto monEnergyDiff = Monitored::Collection("energyDiff", energyDiffs[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_energyDiffGroups[partition][l1TriggerIdx]], monEnergyDiff);
        fill(m_tools[m_energyDiffVsTimeGroups[partition][l1TriggerIdx]], monTime, monEnergyDiff);
        fill(m_tools[m_energyDiffVsEnergyGroups[partition][l1TriggerIdx]], monEnergy, monEnergyDiff);
      }
    }

    if (!timeDiffs[partition].empty()) {
      auto monTime = Monitored::Collection("time", offlineTimes[partition]);
      auto monEnergy = Monitored::Collection("energy", offlineEnergies[partition]);
      auto monTimeDiff = Monitored::Collection("timeDiff", timeDiffs[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_timeDiffGroups[partition][l1TriggerIdx]], monTimeDiff);
        fill(m_tools[m_timeDiffVsTimeGroups[partition][l1TriggerIdx]], monTime, monTimeDiff);
        fill(m_tools[m_timeDiffVsEnergyGroups[partition][l1TriggerIdx]], monEnergy, monTimeDiff);
      }
    }
    if (!timeDiffs[partition].empty()) {
      auto monTime = Monitored::Collection("time", offlineTimes[partition]);
      auto monEnergy = Monitored::Collection("energy", offlineEnergies[partition]);
      auto monTimeDiff = Monitored::Collection("timeDiff", timeDiffs[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_timeDiffGroups[partition][l1TriggerIdx]], monTimeDiff);
        fill(m_tools[m_timeDiffVsTimeGroups[partition][l1TriggerIdx]], monTime, monTimeDiff);
        fill(m_tools[m_timeDiffVsEnergyGroups[partition][l1TriggerIdx]], monEnergy, monTimeDiff);
      }
    }

    if (!dspTimes[partition].empty()) {
      auto monTime = Monitored::Collection("time", dspTimes[partition]);
      auto monModule = Monitored::Collection("module", dspTimesDrawers[partition]);
      auto monChannel = Monitored::Collection("channel", dspTimesChannels[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_dspChanTimeGroups[partition][l1TriggerIdx]], monModule, monChannel, monTime);
      }
    }

  }


  if (m_fillRODfragSizeHistograms) {
    int allTileRodFragsSize = 0;
    std::vector<int> roses;
    std::vector<int> fragments;
    std::vector<int> fragmentSizes;

    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
    m_robSvc->getROBData(m_tileRobIds, robFragments);

    for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* robFragment : robFragments) {
      uint32_t rodSourceId = robFragment->rod_source_id();
      unsigned int ros = (rodSourceId & 0x0F0000) >> 16;
      unsigned int fragment = rodSourceId & 0x0000FF;
      int rodFragmentSize = robFragment->rod_fragment_size_word();

      allTileRodFragsSize += rodFragmentSize;
      if (ros > 0 && ros < Tile::MAX_ROS && fragment < m_nROBs) {
        roses.push_back(ros);
        fragments.push_back(fragment);
        fragmentSizes.push_back(rodFragmentSize);
        ATH_MSG_VERBOSE("ROS = " << ros << ", ROD fragment = " << fragment << ", size = " << rodFragmentSize);
      }
    }

    ATH_MSG_DEBUG( "All Tile ROD fragemsts size: " << allTileRodFragsSize << " in LB " << eventInfo->lumiBlock());

    auto lumiBlock = Monitored::Scalar<int>("lumiBlock", eventInfo->lumiBlock());
    auto monRodFragsSize = Monitored::Scalar<int>("allRodFragsSize", allTileRodFragsSize);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_rodFragSizeLBGroups[l1TriggerIdx]], lumiBlock, monRodFragsSize);
    }

    if (!fragmentSizes.empty()) {
      auto monPartition = Monitored::Collection("partition", roses);
      auto monFragment = Monitored::Collection("fragment", fragments);
      auto monFragmentSize = Monitored::Collection("rodFragSize", fragmentSizes);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_rodFragSizeMapGroups[l1TriggerIdx]], monFragment, monPartition, monFragmentSize);
      }
    }
  }


  fill("TileRODMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
