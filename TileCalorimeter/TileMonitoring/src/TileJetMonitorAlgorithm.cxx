/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TileJetMonitorAlgorithm.h"

#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"
#include "CaloEvent/CaloCell.h"

#include "StoreGate/ReadHandle.h"
#include <xAODCore/ShallowCopy.h>
#include "xAODCaloEvent/CaloCluster.h"


TileJetMonitorAlgorithm::TileJetMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator)
  , m_tileID{nullptr}
  , m_tileHWID{nullptr}
  , m_cabling{nullptr}
{}


TileJetMonitorAlgorithm::~TileJetMonitorAlgorithm() {}


StatusCode TileJetMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( detStore()->retrieve(m_tileID) );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );

  //=== get TileCablingSvc
  ServiceHandle<TileCablingSvc> cablingSvc("TileCablingSvc", name());
  ATH_CHECK( cablingSvc.retrieve() );

  //=== cache pointers to cabling helpers
  m_cabling = cablingSvc->cablingService();
 
  if (!m_cabling) {
    ATH_MSG_ERROR( "Pointer to TileCablingService is zero: " << m_cabling);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("value of m_doJetCleaning: " << m_doJetCleaning);

  //=== get TileBadChanTool
  ATH_MSG_DEBUG("::Retrieving tile bad channel tool");
  ATH_CHECK(m_tileBadChanTool.retrieve());
  ATH_MSG_DEBUG("::Retrieved tile bad channel tool");

  if (m_doJetCleaning) {
    ATH_MSG_DEBUG("::initializing JVT updater");
    ATH_CHECK(m_jvt.retrieve());
    ATH_MSG_DEBUG("::initialized JVT updater");
    
    ATH_MSG_DEBUG("::initializing JetCleaningTool");
    ATH_CHECK(m_jetCleaningTool.retrieve());
    ATH_CHECK(m_eventCleaningTool.retrieve());
    ATH_MSG_DEBUG("::initialized JetCleaningTool");
  } else {
    m_jvt.disable();
    m_jetCleaningTool.disable();
    m_eventCleaningTool.disable();
  }

  ATH_CHECK( m_jetContainerKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TileJetMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  if (!isGoodEvent(ctx)) {
    ATH_MSG_DEBUG("::fillHistograms(), event skipped ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("::fillHistograms(), event accepted ");
  }

  SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetContainerKey);
  if (!jetContainer.isValid()) {
    ATH_MSG_WARNING("Can't retrieve Jet Container: " << m_jetContainerKey.key());
    return StatusCode::SUCCESS;
  }

  uint32_t lumiBlock = GetEventInfo(ctx)->lumiBlock();

  ATH_MSG_VERBOSE("::fillHistograms(), lumiblock " << lumiBlock);

  std::set<Identifier> usedCells;  // cells already used in the given event

  int iJet = 0;
  for (const xAOD::Jet* jet : *jetContainer) {
    if ((jet->pt() > m_jetPtMin) && (fabs(jet->eta()) < m_jetEtaMax)) {
      if (isGoodJet(*jet)) {
        ATH_MSG_DEBUG("::fillHistograms, jet " << iJet
			                << ", eta " << jet->eta()
			                << ", phi " << jet->phi()
			                << ", constituents " << jet->numConstituents());
        CHECK(fillTimeHistograms(*jet, lumiBlock, usedCells));
      } else {
        ATH_MSG_DEBUG("::fillHistogram, BAD jet " << iJet
                      << ", eta " << jet->eta()
                      << ", phi " << jet->phi()
                      << ", constituents " << jet->numConstituents());
      }
    }
    ++iJet;
  }


  fill("TileJetMonExecuteTime", timer);

  ATH_MSG_VERBOSE("::fillHistograms(), end-of-loops ");

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/  
StatusCode TileJetMonitorAlgorithm::fillTimeHistograms(const xAOD::Jet& jet, uint32_t lumiBlock, std::set<Identifier>& usedCells) const {
/*---------------------------------------------------------*/  

  ATH_MSG_VERBOSE( "in fillTimeHistograms()" );

  if( jet.numConstituents() == 0 || !jet.getConstituents().isValid()) return StatusCode::SUCCESS;
     
  int cellIndex(-1);

  ToolHandle<GenericMonitoringTool> tileJetChannTimeDQTool = getGroup("TileJetChanTimeDQ");

  std::array<std::string, 2> gainName{"LG", "HG"};
  std::array<std::string, 5> partitionName{"AUX", "LBA", "LBC", "EBA", "EBC"};

  for (const xAOD::JetConstituent* jet_constituent : jet.getConstituents()) {
    if( jet_constituent->type() == xAOD::Type::CaloCluster ){
      const xAOD::CaloCluster* calo_cluster = static_cast<const xAOD::CaloCluster*>(jet_constituent->rawConstituent());
      if (calo_cluster && calo_cluster->getCellLinks()) {
        for (const CaloCell* cell : *calo_cluster) {
          cellIndex++;
          if (cell->caloDDE()->getSubCalo() == CaloCell_ID::TILE) { // a Tile Cell
            ATH_MSG_DEBUG("Cell " << cellIndex << " IS TILECAL !!");
            const TileCell *tilecell = (TileCell*) cell;
            Identifier id = tilecell->ID();
            if (usedCells.find(id) == usedCells.end()) {
              usedCells.insert(id);
            } else {
              continue;
            }
            //      int section= m_tileID->section(id);
            // int module = m_tileID->module(id); // ranges 0..63
            auto module = Monitored::Scalar<int>("module", m_tileID->module(id));
            int sample = m_tileID->sample(id); // ranges 0..3 (A, BC, D, E)
            int ros1 = 0;
            int ros2 = 0;
            int chan1 = -1;
            int chan2 = -1;
            uint32_t bad1 = 0;
            uint32_t bad2 = 0;
            int gain1 = tilecell->gain1();
            int gain2 = tilecell->gain2();
            unsigned int qbit1 = tilecell->qbit1();
            unsigned int qbit2 = tilecell->qbit2();
            
            const CaloDetDescrElement * caloDDE = tilecell->caloDDE();
            IdentifierHash hash1 = caloDDE->onl1();
            if (hash1 != TileHWID::NOT_VALID_HASH) {
              HWIdentifier adc_id = m_tileHWID->adc_id(hash1, gain1);
              ros1 = m_tileHWID->ros(adc_id);
              chan1 = m_tileHWID->channel(adc_id);
              bad1 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
            }
            
            // How is it here with partition? D0 spans two partitions....
            // It should be ok to treat it in this way:
            IdentifierHash hash2 = caloDDE->onl2();
            if (hash2 != TileHWID::NOT_VALID_HASH) {
              HWIdentifier adc_id = m_tileHWID->adc_id(hash2, gain2);
              ros2 = m_tileHWID->ros(adc_id);
              chan2 = m_tileHWID->channel(adc_id);
              bad2 = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(adc_id));
            }

            bool is_good1 = isGoodChannel(ros1, module, chan1, bad1, qbit1, id);
            bool is_good2 = isGoodChannel(ros2, module, chan2, bad2, qbit2, id);
            float ene1 = is_good1 ? tilecell->ene1() : -1;
            float ene2 = is_good2 ? tilecell->ene2() : -1;
            
            ATH_MSG_DEBUG(".... " << TileCalibUtils::getDrawerString(ros1, module)
                          << ", ch1 " << chan1
                          << ", ch2 " << chan2
                          << ", qbit " << qbit1 << "/" << qbit2
                          << ", is_bad " << bad1 << "/" << bad2
                          << ", isGood " << is_good1
                          << "/" << is_good2
                          << ", ene " << tilecell->energy());
            /*
              Now really fill the histograms time vs lumiblock and 1dim time
            */
            
            // first channel
            if (is_good1 && (ene1 > m_energyChanMin) && (ene1 < m_energyChanMax)) {
              if (m_do1DHistograms) {
                std::string name = TileCalibUtils::getDrawerString(ros1, module) + "_ch_" + std::to_string(chan1) + "_1d";
                auto channelTime = Monitored::Scalar<float>(name, tilecell->time1());
                fill("TileJetChanTime1D", channelTime);
              }

              if (m_do2DHistograms) {
                ATH_MSG_WARNING("This histograms are not implemented yet!");
              }

              // info for DQ histograms
              auto moduleDQ = Monitored::Scalar<int>("module" + partitionName[ros1], module + 1);
              auto channelDQ = Monitored::Scalar<int>("channel" + partitionName[ros1], chan1);
              auto timeDQ = Monitored::Scalar<float>("time" + partitionName[ros1], tilecell->time1());
              Monitored::fill(tileJetChannTimeDQTool, moduleDQ, channelDQ, timeDQ);

              // general histograms, only require non-affected channels
              if (bad1 < 2) {
                ATH_MSG_DEBUG( "Filling in time1 for " << TileCalibUtils::getDrawerString(ros1, module)
                               << ", ch " << chan1
                               << ", ene " << ene1
                               << ", LB " << lumiBlock
                               << ", time: " << tilecell->time1());

                std::string name("channelTime" + partitionName[ros1]);
                auto channelTime = Monitored::Scalar<float>(name, tilecell->time1());
                fill("TileJetChanTime", channelTime);

                if ((ros1 >= 3) && (sample < TileID::SAMP_E)) {
                  std::string nameNoScint("channelTime" + partitionName[ros1] + "_NoScint");
                  auto channelTimeNoScint = Monitored::Scalar<float>(nameNoScint, tilecell->time1());
                  fill("TileJetChanTime", channelTimeNoScint);
                }
              }
            }

            // second channel
            if (is_good2 && (ene2 > m_energyChanMin) && (ene2 < m_energyChanMax)) {
              if (m_do1DHistograms) {
                std::string name = TileCalibUtils::getDrawerString(ros2, module) + "_ch_" + std::to_string(chan2) + "_1d";
                auto channelTime = Monitored::Scalar<float>(name, tilecell->time2());
                fill("TileJetChanTime1D", channelTime);
              }

              if (m_do2DHistograms) {
                ATH_MSG_WARNING("This histograms are not implemented yet!");
              }

              // info for DQ histograms
              auto moduleDQ = Monitored::Scalar<int>("module" + partitionName[ros2], module + 1);
              auto channelDQ = Monitored::Scalar<int>("channel" + partitionName[ros2], chan2);
              auto timeDQ = Monitored::Scalar<float>("time" + partitionName[ros2], tilecell->time2());
              Monitored::fill(tileJetChannTimeDQTool, moduleDQ, channelDQ, timeDQ);

              // general histograms, only require non-affected channels
              if (bad2 < 2) {
                ATH_MSG_DEBUG( "Filling in time2 for " << TileCalibUtils::getDrawerString(ros2, module)
                               << ", ch " << chan2
                               << ", ene " << ene2
                               << ", LB " << lumiBlock
                               << ", time: " << tilecell->time2()
                               <<" (qbit2 " << qbit2 << ", ch1 " << chan1 << ", ene1 " << ene1 << ", bad1 " << bad1 << ", qbit1 " << qbit1 << ")" );

                std::string name("channelTime" + partitionName[ros2]);
                auto channelTime = Monitored::Scalar<float>(name, tilecell->time2());
                fill("TileJetChanTime", channelTime);

                if ((ros2 > 2) && (sample < TileID::SAMP_E)) {
                  std::string nameNoScint("channelTime" + partitionName[ros2] + "_NoScint");
                  auto channelTimeNoScint = Monitored::Scalar<float>(nameNoScint, tilecell->time2());
                  fill("TileJetChanTime", channelTimeNoScint);
                }
              }
            }

            /* 
               Now filling the cell-based histograms,
               HG-HG and LG-LG combinations only
            */
            if ((is_good1) && (is_good2) && (gain1 == gain2)) {

              if (m_doEnergyDiffHistograms && (tilecell->energy() > m_energyDiffThreshold))  { 
                // EneDiff histograms

                int evenChannnel = (chan1 % 2 == 0) ? chan1 : chan2;
                std::string name = TileCalibUtils::getDrawerString(ros1, module) + "_enediff_" 
                                   + gainName[gain1] + "_ch1_" + std::to_string(evenChannnel);
                auto energyDifference = Monitored::Scalar<float>(name, tilecell->eneDiff() / tilecell->energy());
                fill("TileJetEnergyDiff", energyDifference);
              }

              if ((bad1 < 2) && (bad2 < 2)) {

                // cell-time histograms, only overall, require not affected channels
                if ((ros1 < 3) || (sample != TileID::SAMP_E)) {
                  int index = findIndex(gain1, tilecell->energy());
                  ATH_MSG_DEBUG( "Filling in cell-time for " << TileCalibUtils::getDrawerString(ros1, module)
                                 << ", ch1 " << chan1  
                                 << ", ch2 " << chan2  
                                 << ", ene " << tilecell->energy()
                                 << ", index " << index
                                 << ", time: " << tilecell->time());
                  
                  std::string name("Cell_time_" + partitionName[ros1] + "_" + gainName[gain1] + "_slice_" + std::to_string(index));
                  auto cellTime = Monitored::Scalar<float>(name, tilecell->time());
                  fill("TileJetCellTime", cellTime);
                  
                  if (m_doEnergyProfiles) {
                    std::string indexName("index_" + partitionName[ros1] + "_" + gainName[gain1]);
                    auto energyIndex = Monitored::Scalar<float>(indexName, index);
                    
                    std::string energyName("energy_" + partitionName[ros1] + "_" + gainName[gain1]);
                    auto cellEnergy = Monitored::Scalar<float>(energyName, tilecell->energy());
                    
                    fill("TileJetCellEnergyProfile", energyIndex, cellEnergy);
                  } else {
                    std::string name("Cell_ene_" + partitionName[ros1] + "_" + gainName[gain1] + "_slice_" + std::to_string(index));
                    auto cellEnergy = Monitored::Scalar<float>(name, tilecell->energy());
                    fill("TileJetCellEnergy", cellEnergy);
                  }

                }
              }
            }
          } else {
            ATH_MSG_DEBUG("Cell " << cellIndex << " is NOT Tilecal");
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}



bool TileJetMonitorAlgorithm::isGoodChannel(int ros, int module, int channel, uint32_t bad, unsigned int qbit, Identifier id) const {

  if ((ros < 1) || (ros >= (int) TileCalibUtils::MAX_ROS)) return false; // invalid partition

  if ((module < 0) || (module >= (int) TileCalibUtils::MAX_DRAWER)) return false;  // invalid module number

  if (m_cabling->channel2hole(ros, channel) <= 0) 
    return false; // non-existing PMT (empty hole)

  if (((qbit & TileCell::MASK_BADCH) != 0) || // masked as bad
      ((qbit & TileCell::MASK_TIME) != TileCell::MASK_TIME) ||  // flagged
      ((qbit & TileCell::MASK_ALGO) == TileFragHash::OptFilterDsp)) // in DSP

    return false;
  /* 
   bad is the status in the DB (see http://alxr.usatlas.bnl.gov/lxr-stb6/source/atlas/TileCalorimeter/TileConditions/src/TileBadChanTool.cxx#390).
   Meaning:
   0 = good, 1 = noisy, 2 = affected, 3 = bad, 4 = otherwise
   */
  if (bad > 2) return false;

  /* 
     Now check for special C10, merged E1, E4'
     C10 spec is ok only if channel = 5 (i.e. pmt=6). The other is pmt=5
     E1 merged and E4' should be dropped if channel = 12 (i.e. pmt=13)
  */
  return ((( channel != 4) && (channel != 12)) || m_cabling->TileGap_connected(id));
}


bool TileJetMonitorAlgorithm::isGoodEvent(const EventContext& ctx) const {
  /* check for errors in LAr and Tile, see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DataPreparationCheckListForPhysicsAnalysis
   */
  if (! m_doEventCleaning) return true;

  ATH_MSG_DEBUG("::isGoodEvent()....");

  SG::ReadHandle<xAOD::EventInfo> eventInfo = GetEventInfo(ctx);

  if (eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) return false;
  if (eventInfo->errorState(xAOD::EventInfo::Tile) == xAOD::EventInfo::Error) return false;

  /* see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/HowToCleanJets2017
  */
  if (! m_doJetCleaning) return true;

  const xAOD::JetContainer* jetContainer = nullptr;
  if (! evtStore()->retrieve(jetContainer, "AntiKt4EMTopoJets").isSuccess()){
    ATH_MSG_INFO("Cannot retrieve AntiKt4EMTopoJets. However, returning true.");
    return true;
  }

  auto jetsSC = xAOD::shallowCopyContainer(*jetContainer);
  std::unique_ptr< xAOD::JetContainer > jetsCopy(jetsSC.first);
  std::unique_ptr< xAOD::ShallowAuxContainer > jetsCopyAux(jetsSC.second);
  int iJet = 0;
  for (auto jet : *jetsCopy) {
    ATH_MSG_DEBUG("Jet " << iJet << ", pT " << jet->pt()/1000.0 << " GeV, eta " 
		  << jet->eta());
    jet->auxdata<char>(m_jvtDecorator) = passesJvt(*jet);
    jet->auxdata<char>(m_orDecorator) = true;
    ATH_MSG_DEBUG("... done with jet " << iJet);
    ++iJet;
  }

  bool accept = m_eventCleaningTool->acceptEvent(&*jetsCopy);

  return accept;
}


bool TileJetMonitorAlgorithm::passesJvt(const xAOD::Jet& jet) const {

  if (jet.pt() > m_jetPtMin
      && jet.pt() < m_jetPtMax
      && fabs(jet.getAttribute<float>("DetectorEta")) < m_jetTrackingEtaLimit
      && m_jvt->updateJvt(jet) < m_jvtThreshold) {

    return false;

  } else {
    return true;
  }

}

bool TileJetMonitorAlgorithm::isGoodJet(const xAOD::Jet& jet) const {

  if (m_doJetCleaning) {

    if (jet.pt() >= m_jetPtMin && passesJvt(jet) && m_jetCleaningTool->keep(jet)) {
      return true;
    } else {
      return false;
    }

  } else {
    return true;
  }

}

unsigned int TileJetMonitorAlgorithm::findIndex(const int gain, const float energy) const {

  if (gain == 1) {
    return (std::upper_bound(m_cellEnergyUpperLimitsHG.begin(), m_cellEnergyUpperLimitsHG.end(), energy) 
            - m_cellEnergyUpperLimitsHG.begin());
  } else {
    return (std::upper_bound(m_cellEnergyUpperLimitsLG.begin(), m_cellEnergyUpperLimitsLG.end(), energy) 
            - m_cellEnergyUpperLimitsLG.begin());
  }

}
