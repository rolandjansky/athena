/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTMDBMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"


#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode TileTMDBMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("in initialize()");

  // initialize superclass
  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_muRcvRawChannelContainerKey.initialize() );
  ATH_CHECK( m_muRcvDigitsContainerKey.initialize() );
  ATH_CHECK( m_tileCondToolTMDB.retrieve() );

  if (m_pulseEnergyRange.size() != 2) {
    ATH_MSG_FATAL( "Size of PulseEnergyRange should be 2 (from,to), but is " << m_pulseEnergyRange.size() );
    return StatusCode::FAILURE;
  }

  if (m_energyRange.size() != 2) {
    ATH_MSG_FATAL( "Size of EnergyRange should be 2 (from,to), but is " << m_energyRange.size() );
    return StatusCode::FAILURE;
  }

  using namespace Monitored;

  m_peakGroups = buildToolMap<int>(m_tools, "TMDB_PeakPosition", Tile::MAX_ROS - 1);
  m_energyGroups = buildToolMap<int>(m_tools, "TMDB_Energy", Tile::MAX_ROS - 1);

  std::vector<std::string> partitionName = {"LBA", "LBC", "EBA", "EBC"}; // ROS - 1 to partition name map
  int nChannels[] = {8, 8, 4, 4};
  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {

    m_pulseGroups.push_back(buildToolMap<std::vector<int>>(m_tools,
                                                           "TMDB_MeanPulse_" + partitionName[partition],
                                                           Tile::MAX_DRAWER, nChannels[partition]));

    m_chanEnergyGroups.push_back(buildToolMap<std::vector<int>>(m_tools,
                                                                "TMDB_Energy_" + partitionName[partition],
                                                                Tile::MAX_DRAWER, nChannels[partition]));

    m_calibErrorGroups.push_back(buildToolMap<std::vector<int>>(m_tools,
                                                                "TMDB_CalibrationError_" + partitionName[partition],
                                                                Tile::MAX_DRAWER, nChannels[partition]));

    m_chanChannelNoiseGroups.push_back(buildToolMap<std::vector<std::vector<int>>>(m_tools,
                                                                      "TMDB_ChanNoise_" + partitionName[partition],
                                                                      Tile::MAX_DRAWER, nChannels[partition], nChannels[partition]));

    m_chanPeakPosGroups.push_back(buildToolMap<std::vector<int>>(m_tools,
                                                                 "TMDB_Peak_" + partitionName[partition],
                                                                 Tile::MAX_DRAWER, nChannels[partition]));
    
  }

  return StatusCode::SUCCESS;
}


void TileTMDBMonitorAlgorithm::fillNoiseHistograms(const TileDigitsCollection* muRcvDigitsCollection, const int drawer, const int partition) const {

  for (unsigned int i = 0; i < muRcvDigitsCollection->size(); i++){
    const std::vector<float> &samplesX = muRcvDigitsCollection->at(i)->samples();
    float pedestalX = samplesX[0];
    auto monPedestalX = Monitored::Scalar<float>("channX", pedestalX);
    for (unsigned int j = 0; j < muRcvDigitsCollection->size(); j++){
      const std::vector<float> &samplesY = muRcvDigitsCollection->at(j)->samples();
      float pedestalY = samplesY[0];
      auto monPedestalY = Monitored::Scalar<float>("channY", pedestalY);

      fill(m_tools[m_chanChannelNoiseGroups[partition][drawer][i][j]], monPedestalX, monPedestalY);
    }  
  }
}

StatusCode TileTMDBMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  //  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  float referenceEnergies[Tile::MAX_ROS - 1][Tile::MAX_DRAWER][8] = {{{0}}};

  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer(m_rawChannelContainerKey, ctx);
  ATH_CHECK( rawChannelContainer.isValid() );

  for (const TileRawChannelCollection* rawChannelCollection : *rawChannelContainer) {
    if (rawChannelCollection->empty() ) continue;

    int fragId = rawChannelCollection->identify();
    int drawer = fragId & 0x3F;
    int ros = fragId >> 8;
    int partition = ros - 1;

    for (const TileRawChannel* rawChannel : *rawChannelCollection) {

      HWIdentifier adc_id = rawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      float energy = rawChannel->amplitude();

      std::map<int, int>::const_iterator it = m_cellTMDB[partition].get().find(channel);
      if (it != m_cellTMDB[partition].get().end()) {
        int channelTMDB = (*it).second;
        if ((ros < 3) && (drawer % 2 == 0)) {
          ++channelTMDB; // In LB TMDB channels in even drawers are shifted by one
        }
        referenceEnergies[partition][drawer][channelTMDB] = energy;
      }

    }
  }

  std::vector<float> energies[Tile::MAX_ROS - 1];
  std::vector<int> energyDrawers[Tile::MAX_ROS - 1];
  std::vector<int> energyChannels[Tile::MAX_ROS - 1];

  SG::ReadHandle<TileRawChannelContainer> muRcvRawChannelContainer(m_muRcvRawChannelContainerKey, ctx);
  ATH_CHECK( muRcvRawChannelContainer.isValid() );

  // Collecting energy information
  for (const TileRawChannelCollection* muRcvRawChannelCollection : *muRcvRawChannelContainer) {
    if (muRcvRawChannelCollection->empty() ) continue;

    int fragId = muRcvRawChannelCollection->identify();
    int drawer = fragId & 0x3F;
    int ros = fragId >> 8;
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    int partition = ros - 1;
    
    for (const TileRawChannel* muRcvRawChannel : *muRcvRawChannelCollection) {
      HWIdentifier adc_id = muRcvRawChannel->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);
      float amplitude = muRcvRawChannel->amplitude();
      float energy = m_tileCondToolTMDB->channelCalib(drawerIdx, channel, amplitude);

      energyDrawers[partition].push_back(drawer);
      energyChannels[partition].push_back(channel);
      energies[partition].push_back(energy);

      auto monEnergy = Monitored::Scalar<float>("energy", energy);
      fill(m_tools[m_chanEnergyGroups[partition][drawer][channel]], monEnergy);

      auto monCalibError = Monitored::Scalar<float>("error", referenceEnergies[partition][drawer][channel] - energy);
      fill(m_tools[m_calibErrorGroups[partition][drawer][channel]], monCalibError);

      ATH_MSG_DEBUG(Tile::getDrawerString(ros, drawer)
                      << " TMDB channel " << channel
                      << ": energy [MeV] = " << energy);
    }
  }

  //Collecting channel noise information


  std::vector<float> peakPositions[Tile::MAX_ROS - 1];
  std::vector<int> peakPositionDrawers[Tile::MAX_ROS - 1];
  std::vector<int> peakPositionChannels[Tile::MAX_ROS - 1];

  SG::ReadHandle<TileDigitsContainer> muRcvDigitsContainer(m_muRcvDigitsContainerKey, ctx);
  ATH_CHECK( muRcvDigitsContainer.isValid() );

  // Collecting peak and samples information
  for (const TileDigitsCollection* muRcvDigitsCollection : *muRcvDigitsContainer) {
    if (muRcvDigitsCollection->empty() ) continue;

    int fragId = muRcvDigitsCollection->identify();
    int drawer = fragId & 0x3F;
    int ros = fragId >> 8;
    int partition = ros - 1;

    fillNoiseHistograms(muRcvDigitsCollection, drawer, partition);
    for (const TileDigits* muRcvDigits : *muRcvDigitsCollection) {
      HWIdentifier adc_id = muRcvDigits->adc_HWID();
      int channel = m_tileHWID->channel(adc_id);

      float energy = referenceEnergies[partition][drawer][channel];
      if ((energy > m_pulseEnergyRange[0]) && (energy < m_pulseEnergyRange[1])) {
        peakPositionDrawers[partition].push_back(drawer);
        peakPositionChannels[partition].push_back(channel);

        std::vector<float> samples = muRcvDigits->samples();
        unsigned int nSamples = samples.size();
        std::vector<int> sampleNumbers(nSamples, 0);
        std::iota(sampleNumbers.begin(), sampleNumbers.end(), 0);

        auto monSample = Monitored::Collection("sample", samples);
        auto monSampleNumber = Monitored::Collection("sampleNumber", sampleNumbers);

        fill(m_tools[m_pulseGroups[partition][drawer][channel]], monSampleNumber, monSample);

        float peakPosition = std::distance(samples.begin(), std::max_element(samples.begin(), samples.end()));
        peakPositions[partition].push_back(peakPosition);

        auto monPeakPos = Monitored::Scalar<float>("peak", peakPosition);
        fill(m_tools[m_chanPeakPosGroups[partition][drawer][channel]], monPeakPos);

        ATH_MSG_DEBUG(Tile::getDrawerString(ros, drawer)
                        << " TMDB channel " << channel
                        << ": peak position = " << peakPosition);
      }
    }
  }



  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (!energies[partition].empty()) {
      auto monModule = Monitored::Collection("module", energyDrawers[partition]);
      auto monChannel = Monitored::Collection("channel", energyChannels[partition]);
      auto monEnergy = Monitored::Collection("energy", energies[partition]);
      fill(m_tools[m_energyGroups[partition]], monModule, monChannel, monEnergy);
    }

    if (!peakPositions[partition].empty()) {
      auto monModule = Monitored::Collection("module", peakPositionDrawers[partition]);
      auto monChannel = Monitored::Collection("channel", peakPositionChannels[partition]);
      auto monPeakPosition = Monitored::Collection("peakPosition", peakPositions[partition]);
      fill(m_tools[m_peakGroups[partition]], monModule, monChannel, monPeakPosition);
    }
  }


  fill("TileTMDBMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}

