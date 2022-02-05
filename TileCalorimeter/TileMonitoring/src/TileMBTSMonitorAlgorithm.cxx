/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMBTSMonitorAlgorithm.h"

#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"

#include "CaloIdentifier/TileTBID.h"

#include "StoreGate/ReadHandle.h"

#include "TrigT1Result/CTP_Decoder.h"

StatusCode TileMBTSMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_MSG_INFO("in initialize()");

  if (m_energyCut.size() != MAX_MBTS_COUNTER) {
    int numberOfCounters{MAX_MBTS_COUNTER};
    ATH_MSG_ERROR("Energy cuts are not provided for all ("
                  << numberOfCounters << ") MBTS. Provided only "
                  << m_energyCut.size() << " energy cuts");

    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( detStore()->retrieve(m_tileTBID) );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_DQstatusKey.initialize() );
  ATH_CHECK( m_digitsContainerKey.initialize(m_fillHistogramsPerMBTS) );
  ATH_CHECK( m_mbtsCellContainerKey.initialize() );

  memset(m_MBTSchannels, -1, sizeof(m_MBTSchannels));
  memset(m_MBTScounters, -1, sizeof(m_MBTScounters));
  for (unsigned int ros = 3; ros < Tile::MAX_ROS ; ++ros) {
    for (unsigned int drawer = 0; drawer < Tile::MAX_DRAWER; ++drawer) {
      HWIdentifier drawerHWID = m_tileHWID->drawer_id(ros, drawer);
      Identifier mbtsID = m_cabling->drawer2MBTS_id(drawerHWID);
      if (mbtsID.is_valid()) {
        int channel = m_tileHWID->channel(m_cabling->s2h_channel_id(mbtsID));
        m_MBTSchannels[ros - 3][drawer] = channel;

        int counter = getMBTSCounter(mbtsID);
        m_MBTScounters[ros - 3][drawer] = counter;

        ATH_MSG_DEBUG( "Found MBTS: drawer/channel (counter)=> " << drawer << "/" << channel << "(" << counter << ")" );

      }
    }
  }

  if (m_fillHistogramsPerMBTS) {
    m_energyGroups = Monitored::buildToolMap<int>(m_tools, "TileEnergyMBTS", MAX_MBTS_COUNTER);
    m_energyLBGroups = Monitored::buildToolMap<int>(m_tools, "TileEnergyLBMBTS", MAX_MBTS_COUNTER);

    m_timeGroups = Monitored::buildToolMap<int>(m_tools, "TileTimeMBTS", MAX_MBTS_COUNTER);

    m_hfnGroups = Monitored::buildToolMap<int>(m_tools, "TileHFNoiseMBTS", MAX_MBTS_COUNTER);
    m_pedestalGroups = Monitored::buildToolMap<int>(m_tools, "TilePedestalMBTS", MAX_MBTS_COUNTER);

    m_pulseGroups = Monitored::buildToolMap<int>(m_tools, "TileAveragePulseMBTS", MAX_MBTS_COUNTER);
  }

  if (m_useTrigger && (m_ctpID.size() != MAX_MBTS_COUNTER)) {
    if (m_l1Triggers.size() != MAX_MBTS_COUNTER) {
      int numberOfCounters{MAX_MBTS_COUNTER};
      ATH_MSG_WARNING("Names are not provided for all ("
                      << numberOfCounters << ") L1 MBTS triggers. Provided only "
                      << m_l1Triggers.size() << " L1 MBTS");

      ATH_MSG_WARNING("Autoconfiguration of CTP ID to L1 MBTS mapping is not possible: force useTrigger=false");
      m_useTrigger = false;
    } else {
      ATH_CHECK( m_L1MenuKey.initialize() );
    }
  }

  ATH_CHECK( m_ctpRdoKey.initialize(m_useTrigger) );
  if (m_useTrigger) {
    m_energyTrigGroups = Monitored::buildToolMap<int>(m_tools, "TileEnergyTrigMBTS", MAX_MBTS_COUNTER);
    m_energyTrigLBGroups = Monitored::buildToolMap<int>(m_tools, "TileEnergyTrigLBMBTS", MAX_MBTS_COUNTER);
    m_timeTrigGroups = Monitored::buildToolMap<int>(m_tools, "TileTimeTrigMBTS", MAX_MBTS_COUNTER);
    m_pulseTrigGroups = Monitored::buildToolMap<int>(m_tools, "TileAveragePulseTrigMBTS", MAX_MBTS_COUNTER);
    m_effTrigGroups = Monitored::buildToolMap<int>(m_tools, "TileEfficiencyTrigMBTS", MAX_MBTS_COUNTER);

    int nTriggers = 3;
    m_trigGroups = Monitored::buildToolMap<int>(m_tools, "MBTS_Triggers", nTriggers);
    m_trigInWinGroups = Monitored::buildToolMap<int>(m_tools, "MBTS_TriggersInWindow", nTriggers);
    m_trigSumGroups = Monitored::buildToolMap<int>(m_tools, "MBTS_TriggersSum", nTriggers);
    m_coinTrigGroups = Monitored::buildToolMap<int>(m_tools, "MBTS_CoincidentTriggers", nTriggers);
    m_deltaBCIDSumGroups = Monitored::buildToolMap<int>(m_tools, "MBTS_DeltaBCID_Summary", nTriggers);
    m_deltaBCIDGroups = Monitored::buildToolMap<std::vector<int>>(m_tools, "MBTS_DeltaBCID", nTriggers, MAX_MBTS_COUNTER);
  }

  return StatusCode::SUCCESS;
}

StatusCode TileMBTSMonitorAlgorithm::start() {

  if (m_useTrigger && (m_ctpID.size() != MAX_MBTS_COUNTER)) {
    m_ctpID.value().resize(MAX_MBTS_COUNTER, -1);
    ATH_MSG_INFO("Try to autoconfigure CTP ID to L1 MBTS trigger mapping from L1 menu");
    SG::ReadHandle<TrigConf::L1Menu>  l1MenuHandle = SG::makeHandle( m_L1MenuKey );
    if( l1MenuHandle.isValid() ) {
      for (const TrigConf::L1Item& item: *l1MenuHandle) {
        auto it = std::find(m_l1Triggers.begin(), m_l1Triggers.end(), item.name());
        if (it != m_l1Triggers.end()) {
          unsigned int counter = std::distance(m_l1Triggers.begin(), it);
          m_ctpID[counter] = item.ctpId();
          ATH_MSG_DEBUG("Found in L1 Menu: " << item.name() << ", CTP ID: " << item.ctpId() << ", counter: " << counter);
        }
      }
    } else {
      ATH_MSG_WARNING("L1 menu is not available: force useTrigger=false");
      m_useTrigger = false;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileMBTSMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");
  auto timeerGroup = Monitored::Group(getGroup("TileMBTSMonExecuteTime"), timer);

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  const int Trig_b7(7);
  uint32_t l1TriggerType(eventInfo->level1TriggerType());
  bool physicRun = (l1TriggerType == 0) || (((l1TriggerType >> Trig_b7) & 1) == 1);

  if (!physicRun) {
    ATH_MSG_DEBUG("Calibration Event found => skip filling histograms.");
    return StatusCode::SUCCESS;
  }

  auto lumiBlock = Monitored::Scalar<int>("lumiBlock", eventInfo->lumiBlock());

  int nHitsA(0);
  int nHitsC(0);
  double timeA(0.0);
  double timeC(0.0);

  //  unsigned int l1aBCID(0);
  std::vector<bool> hasTBP(MAX_MBTS_COUNTER, false);

  if (m_useTrigger) {
    SG::ReadHandle<CTP_RDO> ctpRdoContainer(m_ctpRdoKey, ctx);
    ATH_CHECK( ctpRdoContainer.isValid() );

    uint32_t nBunches(0);
    CTP_Decoder ctpDecoder;

    ctpDecoder.setRDO(ctpRdoContainer.get());
    nBunches = ctpRdoContainer->getNumberOfBunches();

    if (nBunches > 0) {
      short l1aBunch = ctpRdoContainer->getL1AcceptBunchPosition();
      const std::vector<CTP_BC>& ctpBunchCrossings = ctpDecoder.getBunchCrossings();
      const CTP_BC & bunch = ctpBunchCrossings[l1aBunch];
      auto  l1aBCID = Monitored::Scalar<unsigned int>("bcid", bunch.getBCID());
      unsigned int bcid;
      auto triggerMultiplicityA = Monitored::Scalar<int>("multiplicityA", 0);
      auto triggerMultiplicityC = Monitored::Scalar<int>("multiplicityC", 0);

      const std::bitset<512> currentTBP(bunch.getTBP());
      for (size_t counter = 0; counter < m_ctpID.size(); ++counter) {
        if (m_ctpID[counter] < 0) continue;
        if (currentTBP.test(m_ctpID[counter])) {
          hasTBP[counter] = true;
          if (counter < 16) {
            ++triggerMultiplicityA;
          } else {
            ++triggerMultiplicityC;
          }
        }
      }

      fill("TileTriggerMultiplicityMBTSA", l1aBCID, triggerMultiplicityA);
      fill("TileTriggerMultiplicityMBTSC", l1aBCID, triggerMultiplicityC);
      fill("TileTriggerMultiplicityPerSideMBTS", triggerMultiplicityA, triggerMultiplicityC);

      std::vector<int> triggerTypes; // A-Inner, A-Outer, A-Total, C-Inner, C-Outer, C-Total
      std::vector<int> triggerCounters;
      std::vector<int> triggerInWindowCounters;
      std::vector<int> deltaBCIDs;

      //int deltaBCID;
      // Loop over bunch crossings in CTP window
      for (const CTP_BC& ctpBunchCrossing : ctpBunchCrossings) {
        bcid = ctpBunchCrossing.getBCID();
        int deltaBCID = l1aBCID - bcid;

        std::vector<std::reference_wrapper<const std::bitset<512>>> triggers;
        triggers.push_back(ctpBunchCrossing.getTBP());
        triggers.push_back(ctpBunchCrossing.getTAP());
        triggers.push_back(ctpBunchCrossing.getTAV());

        int triggerIdx = 0;
        for (const std::bitset<512>& currentTrigger : triggers) {
          triggerTypes.clear();
          triggerCounters.clear();
          triggerInWindowCounters.clear();
          deltaBCIDs.clear();

          for (size_t counter = 0; counter < m_ctpID.size(); ++counter) {
            if (m_ctpID[counter] < 0) continue;
            if (currentTrigger.test(m_ctpID[counter])) {
              triggerInWindowCounters.push_back(counter);
              deltaBCIDs.push_back(deltaBCID);
              auto monDeltaBCID = Monitored::Scalar<float>("DeltaBCID", deltaBCID);
              fill(m_tools[m_deltaBCIDGroups[triggerIdx][counter]], monDeltaBCID);
              if (bcid == l1aBCID) {
                triggerCounters.push_back(counter);
                int mbtsType = counter / 8; // A-Inner, A-Outer, C-Inner, C-Outer
                if (counter > 15) {
                  mbtsType += 1; // Shift C-Side to allow A-Total
                }
                triggerTypes.push_back(mbtsType);
                triggerTypes.push_back(counter < 16 ? 2 : 5); // A-Total, C-Total
              }
            }
          }

          auto monTriggerInWindowCounter = Monitored::Collection("TriggerCounter", triggerInWindowCounters);
          fill(m_tools[m_trigInWinGroups[triggerIdx]], monTriggerInWindowCounter);

          auto monDeltaBCID = Monitored::Collection("DeltaBCID", deltaBCIDs);
          fill(m_tools[m_deltaBCIDSumGroups[triggerIdx]], monTriggerInWindowCounter, monDeltaBCID);

          if (bcid == l1aBCID) {
            auto monTriggerType = Monitored::Collection("TriggerType", triggerTypes);
            fill(m_tools[m_trigSumGroups[triggerIdx]], monTriggerType);

            auto monTriggerCounter = Monitored::Collection("TriggerCounter", triggerCounters);
            fill(m_tools[m_trigGroups[triggerIdx]], monTriggerCounter);

            std::vector<int> coincidentTriggers1;
            std::vector<int> coincidentTriggers2;
            for (int counter1 : triggerCounters) {
              for (int counter2 : triggerCounters) {
                coincidentTriggers1.push_back(counter1);
                coincidentTriggers2.push_back(counter2);
              }
            }
            auto monCoincidentTrigger1 = Monitored::Collection("CoincidentTrigger1", coincidentTriggers1);
            auto monCoincidentTrigger2 = Monitored::Collection("CoincidentTrigger2", coincidentTriggers2);
            fill(m_tools[m_coinTrigGroups[triggerIdx]], monCoincidentTrigger1, monCoincidentTrigger2);
          }
          ++triggerIdx;
        }
      }

    }
  }

  auto monEnergy = Monitored::Scalar<float>("Energy", 0.0F);
  auto monTime = Monitored::Scalar<float>("Time", 0.0F);

  std::vector<float> energyCounters;
  energyCounters.reserve(MAX_MBTS_COUNTER);
  auto monEnergyCounter = Monitored::Collection("EnergyCounter", energyCounters);

  std::vector<float> energies;
  energies.reserve(MAX_MBTS_COUNTER);
  auto monSummaryEnergy = Monitored::Collection("SummaryEnergy", energies);

  std::vector<float> timeCounters;
  auto monTimeCounter = Monitored::Collection("TimeCounter", timeCounters);

  std::vector<float> times;
  auto monSummaryTime = Monitored::Collection("SummaryTime", times);

  std::vector<float> triggerEnergies;
  auto monTriggerSummaryEnergy = Monitored::Collection("SummaryEnergy", triggerEnergies);

  std::vector<float> triggerTimes;
  auto monTriggerSummaryTime = Monitored::Collection("SummaryTime", triggerTimes);

  std::vector<int> energyHitCounters;
  std::vector<int> triggerHitCounters;

  SG::ReadHandle<TileCellContainer> mbtsContainer(m_mbtsCellContainerKey, ctx);
  ATH_CHECK( mbtsContainer.isValid() );

  for (const TileCell* cell : *mbtsContainer) {

    Identifier id = cell->ID();

    // Calculate MBTS counter from "side", "tower" and "module"
    // Counter goes 0-31.
    // EBA-inner:0-7,EBA-outer:8-15,EBC-inner:16-23,EBC-outer:24-31
    // tower is 1 for outer counter (lower eta) and 0 for inner counter (higher eta)
    // module counts from 0-7 in increasing phi
    // side is -1 for EBC 1 for EBA

    int counter = getMBTSCounter(id);
    energyCounters.push_back(counter);

    double energy = cell->energy();
    monEnergy = energy;
    energies.push_back(energy);

    double time = cell->time();
    monTime = time;

    float quality = cell->quality();

    ATH_MSG_VERBOSE("Counter: " << counter
                    << ", energy= " << energy << " pCb"
                    << ", time= " << time
                    << ", quality= " << quality);


    if (m_fillHistogramsPerMBTS) {
      fill(m_tools[m_energyGroups[counter]], monEnergy);
      fill(m_tools[m_energyLBGroups[counter]], lumiBlock, monEnergy);
    }

    if (m_useTrigger) {
      auto monEfficiency = Monitored::Scalar<float>("Efficiency", 0.0F);
      if (hasTBP[counter]) {
        monEfficiency = 1.0F;
        triggerHitCounters.push_back(counter);
        triggerEnergies.push_back(energy);
        triggerTimes.push_back(time);

        fill(m_tools[m_energyTrigGroups[counter]], monEnergy);
        fill(m_tools[m_energyTrigLBGroups[counter]], lumiBlock, monEnergy);
        fill(m_tools[m_timeTrigGroups[counter]], monTime);
      }
      fill(m_tools[m_effTrigGroups[counter]], monEnergy, monEfficiency);
    }

    if (energy > m_energyCut[counter]) {

      timeCounters.push_back(counter);
      times.push_back(time);

      energyHitCounters.push_back(counter);

      if (m_fillHistogramsPerMBTS) {
        fill(m_tools[m_timeGroups[counter]], monTime);
      }

      if (std::abs(time) > 10e-4) {
        if (counter < 16) { // MBTS A side
          timeA += time;
          ++nHitsA;
        } else { // MBTS C side
          timeC += time;
          ++nHitsC;
        }
      }

    }
  }

  auto monHitCounter = Monitored::Collection("HitCounter", energyHitCounters);
  fill("TileOccupancyMBTS", monHitCounter);
  fill("TileEnergySummaryMBTS", monEnergyCounter, monSummaryEnergy);

  fill("TileTimeSummaryMBTS", monTimeCounter, monSummaryTime);

  if (m_useTrigger) {
    auto monTriggerHitCounter = Monitored::Collection("HitCounter", triggerHitCounters);
    fill("TileTriggerOccupancyMBTS", monTriggerHitCounter);
    fill("TileTriggerEnergySummaryMBTS", monTriggerHitCounter, monTriggerSummaryEnergy);
    fill("TileTriggerTimeSummaryMBTS", monTriggerHitCounter, monTriggerSummaryTime);
  }

  if (nHitsA > 0) {
    timeA /= nHitsA;
    auto monTimeA = Monitored::Scalar<float>("TimeA", timeA);
    fill("TileTimeMBTSA", monTimeA);
  }

  if (nHitsC > 0) {
    timeC /= nHitsC;
    auto monTimeC = Monitored::Scalar<float>("TimeC", timeC);
    fill("TileTimeMBTSC", monTimeC);
  }

  if (nHitsA > 0 && nHitsC > 0) {
    double timeDifference = timeA - timeC;
    auto monTimeDifference = Monitored::Scalar<float>("TimeDifference", timeDifference);
    fill("TileTimeDifferenceMBTS", monTimeDifference);
    fill("TileTimeDifferenceMBTSLB", lumiBlock, monTimeDifference);
  }


  std::vector<int> coincidentCounters1;
  auto monCoincidentCounters1 = Monitored::Collection("CoincidentCounter1", coincidentCounters1);

  std::vector<int> coincidentCounters2;
  auto monCoincidentCounters2 = Monitored::Collection("CoincidentCounter2", coincidentCounters2);

  for (int counter1 : energyHitCounters) {
    for (int counter2 : energyHitCounters) {
      coincidentCounters1.push_back(counter1);
      coincidentCounters2.push_back(counter2);
    }
  }

  fill("TileCoincidentHitsMBTS", monCoincidentCounters1, monCoincidentCounters2);

  std::vector<int> errors;
  auto monErrors = Monitored::Collection("Error", errors);

  std::vector<int> counters;
  auto monErrorCounters = Monitored::Collection("ErrorCounter", counters);

  // Create instance of TileDQstatus used to check for readout errors in Tile
  const TileDQstatus* dqStatus = SG::makeHandle (m_DQstatusKey, ctx).get();

 // Check only low gain (in physics runs low and high gain status should identical)
  const int gain{0};
  for (unsigned int ros = 3; ros < Tile::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < Tile::MAX_DRAWER; ++drawer) {
      int channel = m_MBTSchannels[ros - 3][drawer];
      int counter = m_MBTScounters[ros - 3][drawer];

      if (channel > 0) {
        // Fill Readout Error histogram
        if (!(dqStatus->isChanDQgood(ros, drawer, channel))) {
          setDigiError(counters, errors, counter, GENERAL);
        }
        if (dqStatus->checkGlobalCRCErr(ros, drawer, gain)) {
          setDigiError(counters, errors, counter, GLOBAL_CRC);
        }
        if (dqStatus->checkROD_CRCErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, ROD_CRC);
        }
        if (dqStatus->checkFE_CRCErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, FRONTEND_CRC);
        }
        if (dqStatus->checkBCIDErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, BCID);
        }
        if (dqStatus->checkHeaderFormatErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, HEADER_FORMAT);
        }
        if (dqStatus->checkHeaderParityErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, HEADER_PARITY);
        }
        if (dqStatus->checkSampleFormatErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, SAMPLE_FORMAT);
        }
        if (dqStatus->checkSampleParityErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, SAMPLE_PARITY);
        }
        if (dqStatus->checkMemoryParityErr(ros, drawer, MBTS_DMU, gain)) {
          setDigiError(counters, errors, counter, MEMORY_PARITY);
        }
      }
    }
  }

  fill("TileErrorsMBTS", monErrorCounters, monErrors);


  if (m_fillHistogramsPerMBTS) {
    SG::ReadHandle<TileDigitsContainer> digitsContainer(m_digitsContainerKey, ctx);
    ATH_CHECK( digitsContainer.isValid() );

    auto monPedestal = Monitored::Scalar<float>("Pedestal", 0.0F);
    auto monHFN = Monitored::Scalar<float>("HFN", 0.0F);

    for (const TileDigitsCollection* digitsCollection : *digitsContainer) {
      if (digitsCollection->empty()) continue;
      HWIdentifier adc_id = digitsCollection->front()->adc_HWID();
      int ros = m_tileHWID->ros(adc_id);
      if (ros > 2) { // Extended barrel
        int drawer = m_tileHWID->drawer(adc_id);
        int MBTSchannel = m_MBTSchannels[ros - 3][drawer];

        if (MBTSchannel >= 0) {
          for (const TileDigits* tile_digits: *digitsCollection) {

            adc_id = tile_digits->adc_HWID();
            int channel = m_tileHWID->channel(adc_id);
            if (channel == MBTSchannel) {  // MBTS channel found
              int counter = m_MBTScounters[ros - 3][drawer];

              double sampleMean = 0;
              double sampleRMS = 0;

              std::vector<float> samples = tile_digits->samples();
              unsigned int nSamples = samples.size();
              if (nSamples > 0) {

                for (float sample : samples) {
                  sampleMean += sample;
                  sampleRMS += sample * sample;
                }

                auto min_max = std::minmax_element(samples.begin(), samples.end());
                float minSample = *min_max.first;
                float maxSample = *min_max.second;

                if ((maxSample - minSample) > 10) {

                  auto monSamples = Monitored::Collection("Samples", samples);

                  std::vector<int> sampleNumbers;
                  sampleNumbers.reserve(nSamples);
                  for (unsigned int i = 0; i < nSamples; ++i) sampleNumbers.push_back(i);
                  auto monSampleNumbers = Monitored::Collection("SampleNumbers", sampleNumbers);

                  fill(m_tools[m_pulseGroups[counter]], monSampleNumbers, monSamples);
                  if (hasTBP[counter]) {
                    fill(m_tools[m_pulseTrigGroups[counter]], monSampleNumbers, monSamples);
                  }
                }

                sampleMean /= nSamples;
                sampleRMS = sampleRMS / nSamples - sampleMean * sampleMean;
                sampleRMS = (sampleRMS > 0.0 && nSamples > 1u) ? sqrt(sampleRMS * nSamples / (nSamples - 1u)) : 0.0;

                monHFN = sampleRMS;
                fill(m_tools[m_hfnGroups[counter]], monHFN);

                monPedestal = samples.front();
                fill(m_tools[m_pedestalGroups[counter]], monPedestal);

              }
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}



int TileMBTSMonitorAlgorithm::getMBTSCounter(Identifier mbtsID) const {
  int counter = m_tileTBID->phi(mbtsID) + 8 * m_tileTBID->eta(mbtsID);
  if (m_tileTBID->side(mbtsID) < 0) counter += 16;// EBC side
  return counter;
}


void TileMBTSMonitorAlgorithm::setDigiError(std::vector<int>& counters, std::vector<int>& errors,
                                            int counter, int error) const {

  counters.push_back(counter);
  errors.push_back(error);
}
