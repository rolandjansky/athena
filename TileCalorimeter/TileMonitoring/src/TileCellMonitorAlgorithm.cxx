/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCellMonitorAlgorithm.h"
#include "TileMonitoring/PairBuilder.h"
#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "CaloIdentifier/TileID.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;
using Athena::Units::ns;

StatusCode TileCellMonitorAlgorithm::initialize() {

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( detStore()->retrieve(m_tileID) );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_tileBadChanTool.retrieve() );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();

  ATH_CHECK( m_DQstatusKey.initialize() );
  ATH_CHECK( m_badChannelsKey.initialize() );
  ATH_CHECK( m_caloCellContainerKey.initialize() );

  using Tile = TileCalibUtils;
  using namespace Monitored;

  int nL1Triggers = getNumberOfL1Triggers();

  m_cellSynchGroups = buildToolMap<int>(m_tools, "TileCellSynch", nL1Triggers);
  m_energyBalModPartGroups = buildToolMap<int>(m_tools, "TileCellEneBalModPart", nL1Triggers);
  m_timeBalModPartGroups = buildToolMap<int>(m_tools, "TileCellTimeBalModPart", nL1Triggers);
  m_maskedOnFlyGroups = buildToolMap<int>(m_tools, "TileCellStatusOnFly", Tile::MAX_ROS - 1);
  m_maskedDueDQGroups = buildToolMap<int>(m_tools, "TileMaskChannelDueDQvsLB", Tile::MAX_ROS - 1);
  m_maskedCellsDueDQGroups = buildToolMap<int>(m_tools, "TileMaskedCellDueDQvsLB", Tile::MAX_ROS - 1);

  m_maskedOnFlyLBGroups = buildToolMap<int>(m_tools, "TileMaskChannelOnFlyLB", Tile::MAX_ROS - 1);
  m_maskedCellsLBGroups = buildToolMap<int>(m_tools, "TileMaskCellLB", Tile::MAX_ROS - 1);

  m_maskedGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellStatusInDB",
                                                  Tile::MAX_ROS - 1, Tile::MAX_GAIN);

  m_energySampEGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellEventEnergySampE",
                                                       Tile::MAX_ROS, nL1Triggers);

  m_moduleCorrGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellModuleCorrelation",
                                                      Tile::MAX_ROS, nL1Triggers);

  m_chanTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileChanPartTime",
                                                    Tile::MAX_ROS - 1, nL1Triggers);

  m_digiTimeGroups = buildToolMap<std::vector<int>>(m_tools, "TileDigiPartTime",
                                                    Tile::MAX_ROS - 1, nL1Triggers);

  m_detailOccupGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellDetailOccMap",
                                                       Tile::MAX_ROS - 1, nL1Triggers);

  m_overThrOccupGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellDetailOccMapOvThr",
                                                        Tile::MAX_ROS - 1, nL1Triggers);

  m_overThr30GeVOccupGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellDetailOccMapOvThr30GeV",
                                                             Tile::MAX_ROS - 1, nL1Triggers);

  m_overThr300GeVOccupGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellDetailOccMapOvThr300GeV",
                                                              Tile::MAX_ROS - 1, nL1Triggers);

  m_overThrOccupGainGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileCellDetailOccMapOvThrGain",
                                                                         Tile::MAX_ROS - 1, Tile::MAX_GAIN, nL1Triggers);

  m_nCellsGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellsNumberLB",
                                                  Tile::MAX_ROS, nL1Triggers);

  m_nCellsOverThrGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellOccOvThrBCID",
                                                         Tile::MAX_ROS, nL1Triggers);

  m_eneEtaPhiGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellEneEtaPhi",
                                                     MAX_SAMP, nL1Triggers);

  m_overThrEtaPhiGroups = buildToolMap<std::vector<int>>(m_tools, "TileCellEtaPhiOvThr",
                                                         MAX_SAMP, nL1Triggers);

  if (m_fillChannelTimeHistograms) {
    m_chanTimeSampGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileChannelTime",
                                                                       Tile::MAX_ROS - 1, MAX_SAMP, nL1Triggers);
  }

  m_eneDiffSampGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileCellEneDiff",
                                                                    Tile::MAX_ROS - 1, MAX_SAMP, nL1Triggers);

  m_timeDiffSampGroups = buildToolMap<std::vector<std::vector<int>>>(m_tools, "TileCellTimeDiff",
                                                                     Tile::MAX_ROS - 1, MAX_SAMP, nL1Triggers);

  m_negOccupGroups = buildToolMap<int>(m_tools, "TileCellDetailNegOccMap", Tile::MAX_ROS - 1);
  m_timeBalGroups = buildToolMap<int>(m_tools, "TileCellTimeBalance", Tile::MAX_ROS - 1);
  m_energyBalGroups = buildToolMap<int>(m_tools, "TileCellEnergyBalance", Tile::MAX_ROS - 1);

  return TileMonitorAlgorithm::initialize();
}


StatusCode TileCellMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using Tile = TileCalibUtils;

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  const TileDQstatus* dqStatus = SG::makeHandle(m_DQstatusKey, ctx).get();
  auto lumiBlock = Monitored::Scalar<int>("lumiBlock", eventInfo->lumiBlock());


  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Run = "     << eventInfo->runNumber()
                    << " LB = "     << eventInfo->lumiBlock()
                    << " Evt = "    << eventInfo->eventNumber()
                    << " BCID = "   << eventInfo->bcid()
                    << " lvl1 = 0x" << std::hex << eventInfo->level1TriggerType() << std::dec;

    const std::vector<xAOD::EventInfo::StreamTag>& eventStreamTags = eventInfo->streamTags();
    if (!eventStreamTags.empty()) {
      msg(MSG::DEBUG) << " stream name/type:";
      for (const auto& eventStreamTag : eventStreamTags) {
        msg(MSG::DEBUG) << " " << eventStreamTag.name() << "/" << eventStreamTag.type();
      }
    }

    msg(MSG::DEBUG) << endmsg;
  }

  // Weights for E-cells, only towers 10..15
  static const float energyWeight[6] = {1./3., 1./6., 1./11., 1./11., 1./28., 1./28.};

  std::vector<const CaloCell*> muonCells;

  std::vector<int> energyBalModPartModule;
  std::vector<int> energyBalModPartPartition;

  std::vector<int> timeBalModPartModule;
  std::vector<int> timeBalModPartPartition;

  double energySample[MAX_PART][MAX_SAMP] = {{0.}};
  int nCells[MAX_PART] = {0};
  int nBadCells[MAX_PART] = {0};

  // Arrays for BCID plots
  int nCellsOverThreshold[MAX_PART] = {0};

  // Number of channels masked on the fly
  int nBadChannelsOnFly[MAX_PART] = {0};

  // Number of channels masked on the fly due to bad DQ status
  unsigned int nMaskedChannelsDueDQ[MAX_PART] = { 0 };

  // Number of cells masked on the fly due to bad DQ status
  unsigned int nMaskedCellsDueDQ[MAX_PART] = { 0 };

  std::vector<int> negOccupModule[Tile::MAX_ROS - 1];
  std::vector<int> negOccupChannel[Tile::MAX_ROS - 1];

  std::vector<float> timeBal[Tile::MAX_ROS - 1];
  std::vector<int> timeBalModule[Tile::MAX_ROS - 1];

  std::vector<float> energyBal[Tile::MAX_ROS - 1];
  std::vector<int> energyBalModule[Tile::MAX_ROS - 1];

  std::vector<int> detailOccupModule[Tile::MAX_ROS - 1];
  std::vector<int> detailOccupChannel[Tile::MAX_ROS - 1];
  std::vector<float> detailOccupEnergies[Tile::MAX_ROS - 1];

  std::vector<int> overThrOccupModule[Tile::MAX_ROS - 1];
  std::vector<int> overThrOccupChannel[Tile::MAX_ROS - 1];
  std::vector<float> overThrOccupWeight[Tile::MAX_ROS - 1];

  std::vector<int> overThrOccupGainModule[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
  std::vector<int> overThrOccupGainChannel[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
  std::vector<float> overThrOccupGainWeight[Tile::MAX_ROS - 1][Tile::MAX_GAIN];

  std::vector<int> overThr30GeVOccupModule[Tile::MAX_ROS - 1];
  std::vector<int> overThr30GeVOccupChannel[Tile::MAX_ROS - 1];

  std::vector<int> overThr300GeVOccupModule[Tile::MAX_ROS - 1];
  std::vector<int> overThr300GeVOccupChannel[Tile::MAX_ROS - 1];

  std::vector<int> maskedOnFlyDrawers[Tile::MAX_ROS - 1];
  std::vector<int> maskedOnFlyChannel[Tile::MAX_ROS - 1];

  std::vector<int> chanTime[Tile::MAX_ROS - 1];
  std::vector<int> chanTimeDrawer[Tile::MAX_ROS - 1];
  std::vector<int> chanTimeChannel[Tile::MAX_ROS - 1];
  std::vector<int> chanTimeDigitizer[Tile::MAX_ROS - 1];

  int nChannelsInPartition = 2880; // LB
  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    if (partition > 1) nChannelsInPartition = 2048; // EB
    detailOccupModule[partition].reserve(nChannelsInPartition);
    detailOccupChannel[partition].reserve(nChannelsInPartition);
    detailOccupEnergies[partition].reserve(nChannelsInPartition);
  }

  std::vector<float> occupEta[SAMP_ALL];
  std::vector<float> occupPhi[SAMP_ALL];
  std::vector<float> occupEnergy[SAMP_ALL];

  std::vector<float> overThrOccupEta[SAMP_ALL];
  std::vector<float> overThrOccupPhi[SAMP_ALL];

  std::vector<float> sampChanTime[Tile::MAX_ROS - 1][SAMP_ALL];
  std::vector<float> sampEnergyDiff[Tile::MAX_ROS - 1][SAMP_ALL];
  std::vector<float> sampTimeDiff[Tile::MAX_ROS - 1][SAMP_ALL];

  PairBuilder moduleCorr[MAX_PART];

  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  SG::ReadCondHandle<TileBadChannels> badChannelsHandle(m_badChannelsKey, ctx);
  const TileBadChannels* badChannels(badChannelsHandle.cptr());

  fillMaskedInDB(badChannels);

  bool isCollision = true;

  SG::ReadHandle<CaloCellContainer> caloCellContainer(m_caloCellContainerKey, ctx);
  ATH_CHECK( caloCellContainer.isValid() );


  for (const CaloCell* cell : *caloCellContainer) {
    Identifier id = cell->ID();
    if (m_tileID->is_tile(id)) {

      const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell);
      if (tile_cell == 0) continue;

      int drawer = 0;    // Drawer number, range 0-63, the same for both channels
      int channel1 = -1; // Channel number, range 0-47 or -1 for unknown
      int channel2 = -1; // Channel number, range 0-47 or -1 for unknown

      int ros1 = -1;
      int ros2 = -1;

      int partition1 = -1;
      int partition2 = -1;

      bool isMaskedChannel1 = false;
      bool isMaskedAdc1 = false;
      bool isMaskedAdc2 = false;

      int gain1 = tile_cell->gain1(); // Gain of first PMT
      int gain2 = tile_cell->gain2(); // Gain of second PMT

      const CaloDetDescrElement* caloDDE = tile_cell->caloDDE();

      IdentifierHash hash1 = caloDDE->onl1();
      if (hash1 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier channel1_id = m_tileHWID->channel_id(hash1);
        channel1 = m_tileHWID->channel(channel1_id);
        drawer = m_tileHWID->drawer(channel1_id);
        ros1 = m_tileHWID->ros(channel1_id);
        partition1 = ros1 - 1;
        HWIdentifier adc1_id = m_tileHWID->adc_id(hash1, gain1);
        isMaskedAdc1 = badChannels->getAdcStatus(adc1_id).isBad();
      }

      IdentifierHash hash2 = caloDDE->onl2();
      if (hash2 != TileHWID::NOT_VALID_HASH) {
        HWIdentifier channel2_id = m_tileHWID->channel_id(hash2);
        channel2 = m_tileHWID->channel(channel2_id);
        drawer = m_tileHWID->drawer(channel2_id);
        ros2 = m_tileHWID->ros(channel2_id);
        partition2 = ros2 - 1;
        HWIdentifier adc2_id = m_tileHWID->adc_id(hash2, gain2);
        isMaskedAdc2 = badChannels->getAdcStatus(adc2_id).isBad();
      }

      // Note that drawer from HWID and module from ID are different for E3 cells near MBTS
      int module = m_tileID->module(id); // Range from 0-63

      // int samp = std::min(m_tileID->sample(id),(int)AllSamp);
      int sample = m_tileID->sample(id);

      bool single_PMT_scin = (sample == TileID::SAMP_E);
      bool single_PMT_C10 = (m_tileID->section(id) == TileID::GAPDET
                             && sample == TileID::SAMP_C
                             && (!m_cabling->C10_connected(module)));

      // Distinguish cells with one or two PMTs
      bool single_PMT = single_PMT_C10 || single_PMT_scin;

      // Distinguish normal cells and fantoms (e.g. non-existing D4 in EBA15, EBC18
      // or non-existing E3/E4 - they might appear in CaloCellContainer)
      bool realCell  = single_PMT_C10 || m_cabling->TileGap_connected(id);

      double weight = 1.;
      if (single_PMT_scin) {
        int tower = m_tileID->tower(id);
        if (tower > 9 && tower < 16) {
          weight = energyWeight[tower - 10];
        }
      }

      // Note that in single PMT cell both badch1() and badch2() are changed together
      bool isBadChannel1 = tile_cell->badch1();
      bool isBadChannel2 = tile_cell->badch2();

      int isCellGood = !tile_cell->badcell();


      bool isOkChannel1 = (channel1 > -1 && gain1 != CaloGain::INVALIDGAIN);
      bool isOkChannel2 = (channel2 > -1 && gain2 != CaloGain::INVALIDGAIN);

      // Get the cell energy, time and position info
      double energy = cell->energy();
      double time = cell->time();
      double eta = cell->eta();
      double phi = cell->phi();
      double energy1 = tile_cell->ene1();
      double energy2 = tile_cell->ene2();
      double energyDiff = (single_PMT) ? 0.0 : tile_cell->eneDiff();
      double energyRatio = (energy != 0.0) ? energyDiff/energy : 0.0;
      double time1 = tile_cell->time1();
      double time2 = tile_cell->time2();
      double timeDiff = (single_PMT) ? 0.0 : 2. * tile_cell->timeDiff(); // Attention! factor of 2 is needed here


      if (msgLvl(MSG::VERBOSE)) {

        msg(MSG::VERBOSE) << "Identifier: " << id << " " << m_tileID->to_string(id,-2) << endmsg;
        msg(MSG::VERBOSE) << "  region: " << m_tileID->region(id)
                          << "  system: " << m_tileID->system(id)
                          << "  section: "<< m_tileID->section(id)
                          << "  side: "   << m_tileID->side(id)
                          << "  module: " << m_tileID->module(id)
                          << "  tower: "  << m_tileID->tower(id)
                          << "  sample: " << m_tileID->sample(id)  <<  endmsg;

        msg(MSG::VERBOSE) << "1st PMT ROS: " << ros1
                          << " drawer: " << drawer
                          << " ch1: " << channel1
                          << " gain1: " << gain1
                          << " qual1: " << (int)tile_cell->qual1()
                          << " qbit1: " << (int)tile_cell->qbit1()
                          << ((drawer != module) ? "   drawer != module":" ") << endmsg;

        msg(MSG::VERBOSE) << "2nd PMT ROS: " << ros2
                          << " drawer: " << drawer
                          << " ch2: " << channel2
                          << " gain2: " << gain2
                          << " qual2: " << (int)tile_cell->qual2()
                          << " qbit2: " << (int)tile_cell->qbit2()
                          << ((ros1 != ros2) ? "   ros1 != ros2" : " ") << endmsg;

        msg(MSG::VERBOSE) << "Bad status: " << !isCellGood
                          << " bad1: " << isBadChannel1
                          << " bad2: " << isBadChannel2
                          << ((single_PMT_scin && isBadChannel1 && !isMaskedChannel1)
                              ? "  NEW BAD E-cell" : " ") << endmsg ;

        msg(MSG::VERBOSE) << "  real_cell: " << ((realCell) ? "true" : "false")
                          << "  E-cell: " << ((single_PMT_scin) ? "true" : "false")
                          << "  specC10: " << ((single_PMT_C10) ? "true" : "false") << endmsg;

        msg(MSG::VERBOSE) << "Energy= " << energy << " = " << energy1 << " + " << energy2
                          << "  ediff= " << energyDiff
                          << "  eratio= " << energyRatio
                          << "\t Energy[GeV]= " << energy/GeV << endmsg ;

        msg(MSG::VERBOSE) << "Time= " << time << " = (" << time1 << " + " << time2 << ")/2  "
                          << "tdiff= " << timeDiff
                          << "\t time[ns]= " << time/ns << endmsg ;
      }


      if (realCell) {

        // When only one channel is bad, it might be that gain of masked channel is not correct
        // If two qualities are not identical, then gains are not the same (new feature introduced in rel 17.2)
        int quality1 = (int)tile_cell->qual1();
        int quality2 = (int)tile_cell->qual2();
        if (isBadChannel1 != isBadChannel2 && quality1 != quality2
            && quality1 < 255 && quality2 < 255) {
          if (isBadChannel1 && isOkChannel1) gain1 = 1 - gain1;
          if (isBadChannel2 && isOkChannel2) gain2 = 1 - gain2;
        }

        bool channel1MaskedDueDQ(false);
        if (isBadChannel1 && isOkChannel1 && !(single_PMT_C10 && channel1 == 4)) {
          if (!isMaskedAdc1) {
            ++nBadChannelsOnFly[partition1];
            maskedOnFlyDrawers[partition1].push_back(drawer);
            maskedOnFlyChannel[partition1].push_back(channel1);

            if (!dqStatus->isAdcDQgood(ros1, drawer, channel1, gain1)) {
              channel1MaskedDueDQ = true;
              ++nMaskedChannelsDueDQ[partition1];
            }
          }
        }

        bool channel2MaskedDueDQ(false);
        if (isBadChannel2 && isOkChannel2 && !(single_PMT_C10 && channel2 == 4)) {
          if (!isMaskedAdc2) {
            ++nBadChannelsOnFly[partition2];
            maskedOnFlyDrawers[partition2].push_back(drawer);
            maskedOnFlyChannel[partition2].push_back(channel2);

            if (!dqStatus->isAdcDQgood(ros2, drawer, channel2, gain2)) {
              channel2MaskedDueDQ = true;
              ++nMaskedChannelsDueDQ[partition2];
            }
          }
        }

        if (isCellGood) {

          if ((energy > m_energyRangeForMuon[0]) && (energy < m_energyRangeForMuon[1])
              && (time > m_timeRangeForMuon[0]) && (time < m_timeRangeForMuon[1])
              && (time != 0)) { // Cell has reconstructed time
            muonCells.push_back(cell);
          }

          occupEta[sample].push_back(eta);
          occupPhi[sample].push_back(phi);
          occupEnergy[sample].push_back(energy);

          if (isOkChannel1) {
            detailOccupModule[partition1].push_back(module);
            detailOccupChannel[partition1].push_back(channel1);
            detailOccupEnergies[partition1].push_back(energy1 * weight);
          }

          if (isOkChannel2) {
            detailOccupModule[partition2].push_back(module);
            detailOccupChannel[partition2].push_back(channel2);
            detailOccupEnergies[partition2].push_back(energy2);
          }

        } else {
          ++nBadCells[partition1];
          if (channel1MaskedDueDQ || channel2MaskedDueDQ) {
            ++nMaskedCellsDueDQ[partition1];
          }
        }

        // Check if energy is below negative threshold
        if (isOkChannel1 && energy1 < m_negativeEnergyThreshold && (!isMaskedAdc1)) {
          negOccupModule[partition1].push_back(module);
          negOccupChannel[partition1].push_back(channel1);
        }

        if (isOkChannel2 && energy2 < m_negativeEnergyThreshold && (!isMaskedAdc2)) {
          negOccupModule[partition2].push_back(module);
          negOccupChannel[partition2].push_back(channel2);
        }

        if (energy > m_energyThreshold) {

          // Fill channel timing histograms.
          if (isCollision || m_fillTimeHistograms) {
            if (isOkChannel1 && energy1 > m_energyThresholdForTime) {
              chanTime[partition1].push_back(time1);
              chanTimeDrawer[partition1].push_back(drawer);
              chanTimeChannel[partition1].push_back(channel1);
              chanTimeDigitizer[partition1].push_back(getDigitizer(channel1));
              if (m_fillChannelTimeHistograms) {
                sampChanTime[partition1][sample].push_back(time1);
              }
            }

            if (isOkChannel2 && energy2 > m_energyThresholdForTime) {
              chanTime[partition2].push_back(time2);
              chanTimeDrawer[partition2].push_back(drawer);
              chanTimeChannel[partition2].push_back(channel2);
              chanTimeDigitizer[partition2].push_back(getDigitizer(channel2));
              if (m_fillChannelTimeHistograms) {
                sampChanTime[partition2][sample].push_back(time2);
              }
            }
          }

          overThrOccupEta[sample].push_back(eta);
          overThrOccupPhi[sample].push_back(phi);

          if (isOkChannel1 && energy1 > m_energyThreshold && (!isBadChannel1)) {
            overThrOccupModule[partition1].push_back(module);
            overThrOccupChannel[partition1].push_back(channel1);
            overThrOccupWeight[partition1].push_back(weight);

            overThrOccupGainModule[partition1][gain1].push_back(module);
            overThrOccupGainChannel[partition1][gain1].push_back(channel1);
            overThrOccupGainWeight[partition1][gain1].push_back(weight);

            if (energy1 > 30000.) {
              overThr30GeVOccupModule[partition1].push_back(module);
              overThr30GeVOccupChannel[partition1].push_back(channel1);

              if (energy1 > 300000.) {
                overThr300GeVOccupModule[partition1].push_back(module);
                overThr300GeVOccupChannel[partition1].push_back(channel1);
              }
            }
          }

          if (isOkChannel2 && energy2 > m_energyThreshold && (!isBadChannel2)) {
            overThrOccupModule[partition2].push_back(module);
            overThrOccupChannel[partition2].push_back(channel2);
            overThrOccupWeight[partition2].push_back(1.0F);

            overThrOccupGainModule[partition2][gain2].push_back(module);
            overThrOccupGainChannel[partition2][gain2].push_back(channel2);
            overThrOccupGainWeight[partition2][gain2].push_back(1.0F);

            if (energy2 > 30000.) {
              overThr30GeVOccupModule[partition2].push_back(module);
              overThr30GeVOccupChannel[partition2].push_back(channel2);

              if (energy2 > 300000.) {
                overThr300GeVOccupModule[partition2].push_back(module);
                overThr300GeVOccupChannel[partition2].push_back(channel2);
              }
            }
          }

          bool fillEneAndTimeDiff(m_fillTimeAndEnergyDiffHistograms);

          // avoid double peak structure in energy and time balance histograms
          if ((gain1 == 0 && gain2 == 1 && (energy1 < 2000 || std::abs(energy1 / energy2) > 5))
              || (gain1 == 1 && gain2 == 0 && (energy2 < 2000 || std::abs(energy2 / energy1) > 5))) {

            fillEneAndTimeDiff = false;
          }

          // Fill these histograms only if the both PMTs are good
          if (!(isBadChannel1 || isBadChannel2 || single_PMT)) {

            if (std::abs(energyRatio) > m_energyBalanceThreshold) {
              energyBalModPartModule.push_back(module);
              energyBalModPartPartition.push_back(partition1);
            }

            if (std::abs(timeDiff) > m_timeBalanceThreshold
                && (energy1 > m_energyThresholdForTime || energy2 > m_energyThresholdForTime)
                && (isCollision || m_fillTimeHistograms)) {
              timeBalModPartModule.push_back(module);
              timeBalModPartPartition.push_back(partition1);
            }

            energyBal[partition1].push_back(energyRatio);
            energyBalModule[partition1].push_back(module);

            if (fillEneAndTimeDiff) {
              sampEnergyDiff[partition1][sample].push_back(energyDiff);
            }

            if ((energy1 > m_energyThresholdForTime || energy2 > m_energyThresholdForTime)
                && (isCollision || m_fillTimeHistograms)) {

              timeBal[partition1].push_back(timeDiff);
              timeBalModule[partition1].push_back(module);

              if (fillEneAndTimeDiff) {
                sampTimeDiff[partition1][sample].push_back(timeDiff);
              }
            }

            moduleCorr[partition1].inputxy(module);
            moduleCorr[PART_ALL].inputxy(module);
          }

          // Store info for BCID plots.
          // Count the number of cells over threshold per partition
          ++nCellsOverThreshold[partition1];
        }
      }

      // Count total number of cells in a partition
      ++nCells[partition1];

      // Accumulate total energy per sample per partition
      energySample[partition1][sample] += energy;
    }
  }

  // Calculate totals for all samples and all partitions
  for (int partition = 0; partition < PART_ALL; ++partition) {

    nCells[PART_ALL] += nCells[partition];
    nBadCells[PART_ALL] += nBadCells[partition];

    nBadChannelsOnFly[PART_ALL] += nBadChannelsOnFly[partition];
    nCellsOverThreshold[PART_ALL] += nCellsOverThreshold[partition];

    nMaskedChannelsDueDQ[PART_ALL] += nMaskedChannelsDueDQ[partition];
    nMaskedCellsDueDQ[PART_ALL] += nMaskedCellsDueDQ[partition];

    for (int sample = 0; sample < SAMP_ALL; ++sample) {
      energySample[partition][SAMP_ALL] += energySample[partition][sample];
      energySample[PART_ALL][sample] += energySample[partition][sample];
    }

    energySample[PART_ALL][SAMP_ALL] += energySample[partition][SAMP_ALL];
  }

  if (msgLvl(MSG::DEBUG)) {

    msg(MSG::DEBUG) << "N Cells:   "
                    << " " << nCells[PART_LBA]
                    << " " << nCells[PART_LBC]
                    << " " << nCells[PART_EBA]
                    << " " << nCells[PART_EBC]
                    << " " << nCells[PART_ALL] << endmsg;

    if (nCells[PART_ALL] > 0) {

      msg(MSG::DEBUG) << "AboveThr: "
                      << " " << nCellsOverThreshold[PART_LBA]
                      << " " << nCellsOverThreshold[PART_LBC]
                      << " " << nCellsOverThreshold[PART_EBA]
                      << " " << nCellsOverThreshold[PART_EBC]
                      << " " << nCellsOverThreshold[PART_ALL] << endmsg;

      msg(MSG::DEBUG) << "MaskOnFly:"
                      << " " << nBadChannelsOnFly[PART_LBA]
                      << " " << nBadChannelsOnFly[PART_LBC]
                      << " " << nBadChannelsOnFly[PART_EBA]
                      << " " << nBadChannelsOnFly[PART_EBC]
                      << " " << nBadChannelsOnFly[PART_ALL] << endmsg;

      msg(MSG::DEBUG) << "EneSampA:"
                      << " " << energySample[PART_LBA][TileID::SAMP_A]
                      << " " << energySample[PART_LBC][TileID::SAMP_A]
                      << " " << energySample[PART_EBA][TileID::SAMP_A]
                      << " " << energySample[PART_EBC][TileID::SAMP_A]
                      << " " << energySample[PART_ALL][TileID::SAMP_A] << endmsg;

      msg(MSG::DEBUG) << "EneSampB:"
                      << " " << energySample[PART_LBA][TileID::SAMP_B]
                      << " " << energySample[PART_LBC][TileID::SAMP_B]
                      << " " << energySample[PART_EBA][TileID::SAMP_B]
                      << " " << energySample[PART_EBC][TileID::SAMP_B]
                      << " " << energySample[PART_ALL][TileID::SAMP_B] << endmsg;

      msg(MSG::DEBUG) << "EneSampD:"
                      << " " << energySample[PART_LBA][TileID::SAMP_D]
                      << " " << energySample[PART_LBC][TileID::SAMP_D]
                      << " " << energySample[PART_EBA][TileID::SAMP_D]
                      << " " << energySample[PART_EBC][TileID::SAMP_D]
                      << " " << energySample[PART_ALL][TileID::SAMP_D] << endmsg;

      msg(MSG::DEBUG) << "EneSampE:"
                      << " " << energySample[PART_LBA][TileID::SAMP_E]
                      << " " << energySample[PART_LBC][TileID::SAMP_E]
                      << " " << energySample[PART_EBA][TileID::SAMP_E]
                      << " " << energySample[PART_EBC][TileID::SAMP_E]
                      << " " << energySample[PART_ALL][TileID::SAMP_E] << endmsg;

      msg(MSG::DEBUG) << "EneTotal:"
                      << " " << energySample[PART_LBA][SAMP_ALL]
                      << " " << energySample[PART_LBC][SAMP_ALL]
                      << " " << energySample[PART_EBA][SAMP_ALL]
                      << " " << energySample[PART_EBC][SAMP_ALL]
                      << " " << energySample[PART_ALL][SAMP_ALL] << endmsg;

    }
  }

  std::vector<int> partitions(Tile::MAX_ROS);
  std::iota(partitions.begin(), partitions.end(), 0);

  auto monPartition = Monitored::Collection("Partition", partitions);
  auto monBadCells = Monitored::Collection("nBadCells", nBadCells);
  fill("TileBadCell", monPartition, monBadCells);

  fillSynchronization(muonCells, l1TriggersIndices);

  if (!energyBalModPartPartition.empty()) {
    for (int l1TriggerIdx : l1TriggersIndices) {
      auto monModule = Monitored::Collection("module", energyBalModPartModule);
      auto monPartion = Monitored::Collection("partition", energyBalModPartPartition);
      fill(m_tools[m_energyBalModPartGroups[l1TriggerIdx]], monModule, monPartion);
    }
  }

  if (!timeBalModPartPartition.empty()) {
    for (int l1TriggerIdx : l1TriggersIndices) {
      auto monModule = Monitored::Collection("module", timeBalModPartModule);
      auto monPartion = Monitored::Collection("partition", timeBalModPartPartition);
      fill(m_tools[m_timeBalModPartGroups[l1TriggerIdx]], monModule, monPartion);
    }
  }

  unsigned int bcid = eventInfo->bcid();

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {

    auto monMaskedDueDQ = Monitored::Scalar<int>("nMaskedChannelsDueDQ", nMaskedChannelsDueDQ[partition]);
    fill(m_tools[m_maskedDueDQGroups[partition]], lumiBlock, monMaskedDueDQ);

    auto monMaskedCellsDueDQ = Monitored::Scalar<int>("nMaskedCellsDueDQ", nMaskedCellsDueDQ[partition]);
    fill(m_tools[m_maskedCellsDueDQGroups[partition]], lumiBlock, monMaskedCellsDueDQ);

    auto monMaskedOnFly = Monitored::Scalar<int>("nMaskedChannelsOnFly", nBadChannelsOnFly[partition]);
    fill(m_tools[m_maskedOnFlyLBGroups[partition]], lumiBlock, monMaskedOnFly);

    auto monMaskedCellsOnFly = Monitored::Scalar<int>("nMaskedCells", nBadCells[partition]);
    fill(m_tools[m_maskedCellsLBGroups[partition]], lumiBlock, monMaskedCellsOnFly);

    if (!maskedOnFlyDrawers[partition].empty()) {
      auto monModule = Monitored::Collection("module", maskedOnFlyDrawers[partition]);
      auto monChannel = Monitored::Collection("channel", maskedOnFlyChannel[partition]);
      fill(m_tools[m_maskedOnFlyGroups[partition]], monModule, monChannel);
    }

    if (!negOccupModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", negOccupModule[partition]);
      auto monChannel = Monitored::Collection("channel", negOccupChannel[partition]);
      fill(m_tools[m_negOccupGroups[partition]], monModule, monChannel);
    }

    if (!energyBalModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", energyBalModule[partition]);
      auto monEnergyBal = Monitored::Collection("energyBalance", energyBal[partition]);
      fill(m_tools[m_energyBalGroups[partition]], monModule, monEnergyBal);
    }

    if (!timeBalModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", timeBalModule[partition]);
      auto monTimeBal = Monitored::Collection("timeBalance", timeBal[partition]);
      fill(m_tools[m_timeBalGroups[partition]], monModule, monTimeBal);
    }

    if (!chanTimeDrawer[partition].empty()) {
      auto monTime = Monitored::Collection("time", chanTime[partition]);
      auto monModule = Monitored::Collection("module", chanTimeDrawer[partition]);
      auto monChannel = Monitored::Collection("channel", chanTimeChannel[partition]);
      auto monDigitizer = Monitored::Collection("digitizer", chanTimeDigitizer[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_chanTimeGroups[partition][l1TriggerIdx]], monModule, monChannel, monTime);
        fill(m_tools[m_digiTimeGroups[partition][l1TriggerIdx]], monModule, monDigitizer, monTime);
      }
    }

    if (!detailOccupModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", detailOccupModule[partition]);
      auto monChannel = Monitored::Collection("channel", detailOccupChannel[partition]);
      auto monEnergies = Monitored::Collection("energy", detailOccupEnergies[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_detailOccupGroups[partition][l1TriggerIdx]], monModule, monChannel, monEnergies);
      }
    }

    if (!overThrOccupModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", overThrOccupModule[partition]);
      auto monChannel = Monitored::Collection("channel", overThrOccupChannel[partition]);
      auto monWeight = Monitored::Collection("weight", overThrOccupWeight[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_overThrOccupGroups[partition][l1TriggerIdx]], monModule, monChannel, monWeight);
      }
    }

    if (!overThr30GeVOccupModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", overThr30GeVOccupModule[partition]);
      auto monChannel = Monitored::Collection("channel", overThr30GeVOccupChannel[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_overThr30GeVOccupGroups[partition][l1TriggerIdx]], monModule, monChannel);
      }
    }

    if (!overThr300GeVOccupModule[partition].empty()) {
      auto monModule = Monitored::Collection("module", overThr300GeVOccupModule[partition]);
      auto monChannel = Monitored::Collection("channel", overThr300GeVOccupChannel[partition]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_overThr300GeVOccupGroups[partition][l1TriggerIdx]], monModule, monChannel);
      }
    }

    for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
      if (!overThrOccupGainModule[partition][gain].empty()) {
        auto monModule = Monitored::Collection("module", overThrOccupGainModule[partition][gain]);
        auto monChannel = Monitored::Collection("channel", overThrOccupGainChannel[partition][gain]);
        auto monWeight = Monitored::Collection("weight", overThrOccupGainWeight[partition][gain]);
        for (int l1TriggerIdx : l1TriggersIndices) {
          fill(m_tools[m_overThrOccupGainGroups[partition][gain][l1TriggerIdx]], monModule, monChannel, monWeight);
        }
      }
    }

    if (m_fillChannelTimeHistograms) {
      for (unsigned int sample = 0; sample < SAMP_ALL; ++sample) {
        if (!sampChanTime[partition][sample].empty()) {
          auto monChanTime = Monitored::Collection("time", sampChanTime[partition][sample]);
          for (int l1TriggerIdx : l1TriggersIndices) {
            fill(m_tools[m_chanTimeSampGroups[partition][sample][l1TriggerIdx]], monChanTime);
            fill(m_tools[m_chanTimeSampGroups[partition][SAMP_ALL][l1TriggerIdx]], monChanTime);
          }
        }
      }
    }

    if (m_fillTimeAndEnergyDiffHistograms) {
      for (unsigned int sample = 0; sample < SAMP_ALL; ++sample) {
        if (!sampEnergyDiff[partition][sample].empty()) {
          auto monEneDiff = Monitored::Collection("energyDiff", sampEnergyDiff[partition][sample]);
          for (int l1TriggerIdx : l1TriggersIndices) {
            fill(m_tools[m_eneDiffSampGroups[partition][sample][l1TriggerIdx]], monEneDiff);
            fill(m_tools[m_eneDiffSampGroups[partition][SAMP_ALL][l1TriggerIdx]], monEneDiff);
          }
        }

        if (!sampTimeDiff[partition][sample].empty()) {
          auto monTimeDiff = Monitored::Collection("timeDiff", sampTimeDiff[partition][sample]);
          for (int l1TriggerIdx : l1TriggersIndices) {
            fill(m_tools[m_timeDiffSampGroups[partition][sample][l1TriggerIdx]], monTimeDiff);
            fill(m_tools[m_timeDiffSampGroups[partition][SAMP_ALL][l1TriggerIdx]], monTimeDiff);
          }
        }
      }
    }

  }

  for (int partition = 0; partition < MAX_PART; ++partition) {
    if (moduleCorr[partition].numberOfPairs() > 0) {
      const PairBuilder::PairVector pairs = moduleCorr[partition].pairs();
      auto monModule1 = Monitored::Collection("firstModule", pairs, [] (const PairBuilder::XYPair& p) {return (double) p.first;});
      auto monModule2 = Monitored::Collection("secondModule", pairs, [] (const PairBuilder::XYPair& p) {return (double) p.second;});
      std::vector<float> weight(moduleCorr[partition].numberOfPairs(), moduleCorr[partition].weight());
      auto monWeight = Monitored::Collection("weight", weight);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_moduleCorrGroups[partition][l1TriggerIdx]], monModule1, monModule2, monWeight);
      }
    }

    for (int l1TriggerIdx : l1TriggersIndices) {
      auto monCellsNumber = Monitored::Scalar<float>("nCells", nCells[partition]);
      fill(m_tools[m_nCellsGroups[partition][l1TriggerIdx]], lumiBlock, monCellsNumber);

      auto monBCID = Monitored::Scalar<unsigned int>("BCID", bcid);
      auto monCellsNumberOvThr = Monitored::Scalar<float>("nCells", nCellsOverThreshold[partition]);
      fill(m_tools[m_nCellsOverThrGroups[partition][l1TriggerIdx]], monBCID, monCellsNumberOvThr);
    }

    if (partition > 1) { // Fill SampE energy for EB
      for (int l1TriggerIdx : l1TriggersIndices) {
        auto monEnergy = Monitored::Scalar<float>("energy", energySample[partition][TileID::SAMP_E]);
        fill(m_tools[m_energySampEGroups[partition][l1TriggerIdx]], monEnergy);
      }
    }
  }

  for (unsigned int sample = 0; sample < SAMP_ALL; ++sample) {
    if (!occupEnergy[sample].empty()) {
      auto monEta = Monitored::Collection("eta", occupEta[sample]);
      auto monPhi = Monitored::Collection("phi", occupPhi[sample]);
      auto monEnergy = Monitored::Collection("energy", occupEnergy[sample]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_eneEtaPhiGroups[sample][l1TriggerIdx]], monEta, monPhi, monEnergy);
        fill(m_tools[m_eneEtaPhiGroups[SAMP_ALL][l1TriggerIdx]], monEta, monPhi, monEnergy);
      }
    }

    if (!overThrOccupEta[sample].empty()) {
      auto monEta = Monitored::Collection("eta", overThrOccupEta[sample]);
      auto monPhi = Monitored::Collection("phi", overThrOccupPhi[sample]);
      for (int l1TriggerIdx : l1TriggersIndices) {
        fill(m_tools[m_overThrEtaPhiGroups[sample][l1TriggerIdx]], monEta, monPhi);
        fill(m_tools[m_overThrEtaPhiGroups[SAMP_ALL][l1TriggerIdx]], monEta, monPhi);
      }
    }
  }



  fill("TileCellMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}


void TileCellMonitorAlgorithm::fillMaskedInDB(const TileBadChannels* badChannels) const {

  using Tile = TileCalibUtils;

  std::vector<HWIdentifier>::const_iterator adc_it = m_tileHWID->adc_begin();
  std::vector<HWIdentifier>::const_iterator adc_end = m_tileHWID->adc_end();

  std::vector<int> drawers[Tile::MAX_ROS - 1][Tile::MAX_GAIN];
  std::vector<int> channels[Tile::MAX_ROS - 1][Tile::MAX_GAIN];

  for (; adc_it != adc_end; ++adc_it) {
    HWIdentifier adc_id(*adc_it);

    if (badChannels->getAdcStatus(adc_id).isBad()) {

      unsigned int partition = m_tileHWID->ros(adc_id) - 1; // ROS - 1
      unsigned int drawer = m_tileHWID->drawer(adc_id);
      unsigned int channel = m_tileHWID->channel(adc_id);
      unsigned int adc = m_tileHWID->adc(adc_id);

      drawers[partition][adc].push_back(drawer);
      channels[partition][adc].push_back(channel);

    }
  }

  for (unsigned int partition = 0; partition < Tile::MAX_ROS - 1; ++partition) {
    for (unsigned int gain = 0; gain < Tile::MAX_GAIN; ++gain) {
      if (!drawers[partition][gain].empty()) {

        auto monModule = Monitored::Collection("module", drawers[partition][gain]);
        auto monChannel = Monitored::Collection("channel", channels[partition][gain]);
        fill(m_tools[m_maskedGroups[partition][gain]], monModule, monChannel);

      }
    }
  }
}


void TileCellMonitorAlgorithm::fillSynchronization(const std::vector<const CaloCell*>& cells,
                                                   const std::vector<int>& l1TriggersIndices) const {

  std::vector<float> timeDifference;

  int size = cells.size();
  if (size > 1) {
    for (int i = 1; i < size; ++i) {
      float x1 = cells[i]->x();
      float y1 = cells[i]->y();
      float z1 = cells[i]->z();
      float time1 = cells[i]->time();

      for (int j = 0; j < i; ++j) {
        float x2 = cells[j]->x();
        float y2 = cells[j]->y();
        float z2 = cells[j]->z();
        float time2 = cells[j]->time();

        float timeMeasured = (y2 > y1) ? time1 - time2 : time2 - time1;

        float distance = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
        float timeExpected = distance / 300.;

        timeDifference.push_back(timeExpected - timeMeasured);
      }
    }

    auto monTimeDifference = Monitored::Collection("timeDifference", timeDifference);
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_cellSynchGroups[l1TriggerIdx]], monTimeDifference);
    }
  }
}

int TileCellMonitorAlgorithm::getDigitizer(int channel) const {

  // Conversion from channel number to digitizer number
  static const int channel2digitizer[48] = {8, 8, 8,  8, 8, 8,
                                            7, 7, 7,  7, 7, 7,
                                            6, 6, 6,  6, 6, 6,
                                            5, 5, 5,  5, 5, 5,
                                            4, 4, 4,  4, 4, 4,
                                            3, 3, 3,  3, 3, 3,
                                            2, 2, 2,  2, 2, 2,
                                            1, 1, 1,  1, 1, 1};

  return channel2digitizer[channel];
}
