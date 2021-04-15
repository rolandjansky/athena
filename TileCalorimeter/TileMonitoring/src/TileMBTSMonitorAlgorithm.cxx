/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMBTSMonitorAlgorithm.h"

#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"

#include "CaloIdentifier/TileTBID.h"

#include "StoreGate/ReadHandle.h"


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

  std::vector<int> energyHitCounters;

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
