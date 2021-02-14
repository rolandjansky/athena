/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RoIBResultToAOD.cxx 784984 2016-11-18 05:53:57Z ssnyder $

// STL include(s):
#include <algorithm>
#include <cmath>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

// LVL1 trigger include(s):
#include "TrigT1Interfaces/TriggerTypeWord.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/CTPResult.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JetInput.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// Trigger configuration interface includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

// Include for the configuration service:
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// LVL1 AOD event include(s):
#include "AnalysisTriggerEvent/CTP_Decision.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "RoIBResultToAOD.h"

using namespace TrigConf;

RoIBResultToAOD::RoIBResultToAOD( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ),
    m_recRPCRoiSvc( LVL1::ID_RecRpcRoiSvc, name ),
    m_recTGCRoiSvc( LVL1::ID_RecTgcRoiSvc, name ),
    m_EmTauTool( "LVL1::L1EmTauTools/L1EmTauTools" ),
    m_JetTool( "LVL1::L1JetTools/L1JetTools" ) {

   // services
   declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
   declareProperty( "RecRpcRoiSvc", m_recRPCRoiSvc, "RPC Rec Roi Service");
   declareProperty( "RecTgcRoiSvc", m_recTGCRoiSvc, "TGC Rec Roi Service");

   // tools
   declareProperty( "L1EmTauTools", m_EmTauTool,
                    "Tool for calculation of EmTau trigger sums per RoI");
   declareProperty( "L1JetTools", m_JetTool,
                    "Tool for calculation of Jet cluster sums per RoI");

   // Properties: input selection
   declareProperty( "DoCalo", m_doCalo = true, "Use inputs from Calo system" );
   declareProperty( "DoMuon", m_doMuon = true, "Use inputs from Muon system" );

   // Properties: rebuilding trigger type
   declareProperty( "EGammaItem", m_egammaItem,
                    "List of LVL1 items for e/gamma trigger type" );
   declareProperty( "TauHadItem", m_tauhadItem,
                    "List of LVL1 items for tau trigger type" );
   declareProperty( "JetItem", m_jetItem,
                    "List of LVL1 items for jet trigger type" );
   declareProperty( "ESumItem", m_esumItem,
                    "List of LVL1 items for energy sum trigger type" );
   declareProperty( "HighMuItem", m_highmuItem,
                    "List of LVL1 items for high pt muon trigger type" );
   declareProperty( "LowMuItem", m_lowmuItem,
                    "List of LVL1 items for low pt muon trigger type" );

   // Properties: StoreGate keys
   m_TriggerTowerLocation       = LVL1::TrigT1CaloDefs::TriggerTowerLocation;
   m_JetElementLocation         = LVL1::TrigT1CaloDefs::JetElementLocation;
   declareProperty( "TriggerTowerLocation", m_TriggerTowerLocation ) ;
   declareProperty( "JetElementLocation",   m_JetElementLocation ) ;
   declareProperty( "RoIBResultInputKey", m_roibInputKey = "RoIBResult" );
   declareProperty( "LVL1_ROIOutputKey", m_lvl1RoIOutputKey = "LVL1_ROI" );

   m_retrievedEmTauTool = false;
   m_retrievedJetTool = false;

}

// Initialize
StatusCode RoIBResultToAOD::initialize() {

   ATH_MSG_INFO( "initializing " << name()
                 << " - package version " << PACKAGE_VERSION );

   // Print system info
   if( m_doCalo == false ) {
      ATH_MSG_WARNING( "Inputs from LVL1 Calo systems switched off" );
   }
   if( m_doMuon == false ) {
      ATH_MSG_WARNING( "Inputs from LVL1 Muon systems switched off" );
   }

   // Connect to the LVL1ConfigSvc for the trigger configuration:
   CHECK( m_configSvc.retrieve() );
   ATH_MSG_DEBUG( "Connected to " << m_configSvc.typeAndName() );

   if( m_doMuon ) {

      // Get the RPC RecRoI service
      CHECK( m_recRPCRoiSvc.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recRPCRoiSvc.typeAndName() );

      // Get the TGC RecRoI service
      CHECK( m_recTGCRoiSvc.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recTGCRoiSvc.typeAndName() );

   } // if (m_doMuon)

   if( m_doCalo ) {

      // Get tools
      CHECK( m_EmTauTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_EmTauTool.typeAndName() );
      m_retrievedEmTauTool = true;

      CHECK( m_JetTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_JetTool.typeAndName() );
      m_retrievedJetTool = true;

   } // if (m_doCalo)

   return StatusCode::SUCCESS;
}

// finalize
StatusCode RoIBResultToAOD::finalize() {

   ATH_MSG_INFO( "Finalizing " << name()
                 << " - package version " << PACKAGE_VERSION );

   return StatusCode::SUCCESS;
}

// execute
StatusCode RoIBResultToAOD::execute() {

   ATH_MSG_DEBUG( "in execute()" );

   ///////////////////////////////////////////////////////////////////////////
   // build CTP decision
   if( buildCTP_Decision().isFailure() ) {
      ATH_MSG_WARNING( "Problems building CTP_Decision" );
   }

   ///////////////////////////////////////////////////////////////////////////
   // Trigger Info

   // retrieve event info
   const DataHandle< EventInfo > eventInfoBeg;
   const DataHandle< EventInfo > eventInfoEnd;
   CHECK( evtStore()->retrieve( eventInfoBeg, eventInfoEnd ) );
   if( eventInfoBeg == eventInfoEnd ) {
      ATH_MSG_ERROR( "No event info objects" );
      return StatusCode::RECOVERABLE;
   }

   // get key
   const std::string eventInfoKey = eventInfoBeg.key();

   // build TriggerInfo
   EventInfo* eventInfo = const_cast< EventInfo* >( eventInfoBeg.cptr() );
   TriggerInfo* tInfo = eventInfo->trigger_info();
   TriggerInfo* triggerInfo;
   if( tInfo != 0 ) {
      triggerInfo = new TriggerInfo( *tInfo );
   } else {
      triggerInfo = new TriggerInfo( 0, 0, 0,
                                     std::vector< TriggerInfo::number_type>( 0 ),
                                     std::vector< TriggerInfo::number_type>( 0 ),
                                     std::vector< TriggerInfo::StreamTag>( 0 ) );
   }

   // set TriggerInfo
   eventInfo->setTriggerInfo( triggerInfo );

   // dump
   ATH_MSG_DEBUG( MSG::dec << "EventInfo:" << eventInfoKey );
   ATH_MSG_DEBUG( *( eventInfo->event_ID() ) );
   ATH_MSG_DEBUG( "[TimeStamp] = " << eventInfo->event_ID()->time_stamp() );
   ATH_MSG_DEBUG( "[Type] = " << eventInfo->event_type()->user_type() );
   ATH_MSG_DEBUG( "[BitMask]" );
   for( EventType::BitMaskIterator bitIt = eventInfo->event_type()->bit_mask_begin();
        bitIt != eventInfo->event_type()->bit_mask_end(); ++bitIt ) {
      ATH_MSG_DEBUG( *bitIt );
   }

   if( triggerInfo != 0 ) {
      ATH_MSG_DEBUG( MSG::hex << "[L1ID] = "<< triggerInfo->extendedLevel1ID() );
      ATH_MSG_DEBUG( "[TriggerType] = "<< triggerInfo->level1TriggerType() );
      ATH_MSG_DEBUG( "[L1Info] = "<< triggerInfo->level1TriggerInfo() );
      ATH_MSG_DEBUG( "[L2Info] = "<< triggerInfo->level2TriggerInfo() );
      ATH_MSG_DEBUG( "[EFInfo]" );
      const std::vector< TriggerInfo::number_type >& efInfo = triggerInfo->eventFilterInfo();
      for( std::vector< TriggerInfo::number_type >::const_iterator itEF = efInfo.begin();
           itEF != efInfo.end(); ++itEF ) {
         ATH_MSG_DEBUG( *itEF );
      }
   }

   ///////////////////////////////////////////////////////////////////////////
   // build LVL1 ROI

   LVL1_ROI *lvl1ROI = new LVL1_ROI;

   ///////////////////////////////////////////////////////////////////////////
   // retrive L1 result

   const ROIB::RoIBResult *result = 0;
   if( evtStore()->retrieve( result, m_roibInputKey ).isFailure() ) {
      ATH_MSG_WARNING( "Could not retrieve RoIBResult. Building empty LVL1_ROI" );
      result = 0;
   }

   ///////////////////////////////////////////////////////////////////////////
   // Muon ROI

   if( m_doMuon == true ) addMuonRoI( result, lvl1ROI );

   ///////////////////////////////////////////////////////////////////////////
   // EmTau ROI

   if( m_doCalo == true ) addEmTauRoI( result, lvl1ROI );

   ///////////////////////////////////////////////////////////////////////////
   // JetEnergy ROI

   if( m_doCalo == true ) addJetEnergyRoI( result, lvl1ROI );

   ///////////////////////////////////////////////////////////////////////////
   // record LVL1 ROI

   CHECK( evtStore()->record( lvl1ROI, m_lvl1RoIOutputKey ) );
   CHECK( evtStore()->setConst( lvl1ROI ) );

   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToAOD::buildCTP_Decision() {

   ATH_MSG_DEBUG( "building CTP decision" );

   CTP_Decision* ctpDecision = new CTP_Decision;

   // retrive L1 result
   const ROIB::RoIBResult *result = 0;
   if( evtStore()->retrieve( result, m_roibInputKey ).isSuccess() ) {

      // Trigger Type Word
      bool triggerTypeLegacyFlag = false;
      uint32_t triggerTypeWord = result->cTPResult().header().triggerType();

      if( triggerTypeWord == 0 ) { // switch to legacy mode
         ATH_MSG_DEBUG( "Building TriggerType word" );
         triggerTypeWord = LVL1::TriggerTypeWord::Physics |
            LVL1::TriggerTypeWord::PriorityLevel;
         triggerTypeLegacyFlag = true;
      }

      // store TAV vector in CTP decision object
      const std::vector< ROIB::CTPRoI > tav = result->cTPResult().TAV();
      for( size_t i( 0 ); i < tav.size(); ++i ) {
         ctpDecision->setWord( i, tav[ i ].roIWord() );
      }

      // search for passed items in TAV vector from CTP result
      const std::bitset< 512 > items = ROIB::convertToBitset( tav );

      for(TrigConf::ItemContainer::const_iterator item = m_configSvc->ctpConfig()->menu().items().begin(); 
          item != m_configSvc->ctpConfig()->menu().items().end(); ++item ) {
         if( items[ ( *item )->ctpId() ] ) {
            // add item name to CTP_Decision
            const std::string& itemName = ( *item )->name();
            ctpDecision->setItem( itemName );
            ATH_MSG_VERBOSE( "Item : " << itemName );

            if( triggerTypeLegacyFlag ) {
               ///////// compute Trigger Type Word
               ///////// This is temporary implementation for DC2 and will be moved to
               ///////// CTP simulation
               // e/gamma
               if( m_egammaItem.end() != std::find( m_egammaItem.begin(), m_egammaItem.end(),
                                                    itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::Egamma;

               // tau/hadron
               if( m_tauhadItem.end() != std::find( m_tauhadItem.begin(), m_tauhadItem.end(),
                                                    itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::TauHdr;

               // jet
               if( m_jetItem.end() != std::find( m_jetItem.begin(), m_jetItem.end(),
                                                 itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::Jet;

               // energy sum
               if( m_esumItem.end() != std::find( m_esumItem.begin(), m_esumItem.end(),
                                                  itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::EnergySum;

               // high pT muon
               if( m_highmuItem.end() != std::find( m_highmuItem.begin(), m_highmuItem.end(),
                                                    itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::HighPtMuon;

               // low pT muon
               if( m_lowmuItem.end() != std::find( m_lowmuItem.begin(), m_lowmuItem.end(),
                                                   itemName ) )
                  triggerTypeWord |= LVL1::TriggerTypeWord::LowPtMuon;
            } // if (triggerTypeLegacyFlag)
         } // if (items[(*item)->ctpId()])
      } // for

      // set Trigger Type Word
      ctpDecision->setTriggerTypeWord( triggerTypeWord );
      ATH_MSG_DEBUG( "CTP_Decision: \n" + ctpDecision->dump() );

   } else {
      ATH_MSG_WARNING( "Could not retrieve RoIBResult, building empty CTP_Decision" );
   }

   // record CTP Decision
   CHECK( evtStore()->record( ctpDecision, "CTP_Decision" ) );
   CHECK( evtStore()->setConst( ctpDecision ) );

   return StatusCode::SUCCESS;
}

void RoIBResultToAOD::addEmTauRoI( const ROIB::RoIBResult* result, LVL1_ROI* lvl1ROI ) {

   ATH_MSG_DEBUG( "building EmTauRoI" );

   // empty input
   if( result == 0 ) return;

   // Digit scale for calorimeter trigger
   float caloTrigScale = m_configSvc->thresholdConfig()->caloInfo().globalScale() * CLHEP::GeV;
   ATH_MSG_DEBUG( "caloTrigScale = " << caloTrigScale );

   /** Get EmTau Thresholds from configSvc. Also fill a map of threshold names while
       we are here - will be useful later */

   std::vector< TrigConf::TriggerThreshold* > thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
   std::vector< TrigConf::TriggerThreshold* > caloThresholds;
   std::map< int, std::string > thresholdNames;
   for( std::vector< TrigConf::TriggerThreshold* >::const_iterator it = thresholds.begin();
        it != thresholds.end(); ++it ) {
      if( ( *it )->type() == L1DataDef::emType() || ( *it )->type() == L1DataDef::tauType() ) {
         caloThresholds.push_back( *it );
         thresholdNames.insert( std::map< int, std::string >::value_type( ( *it )->thresholdNumber(), ( *it )->name() ) );
      }
   }

   // Tool to reconstruct EM/tau cluster & isolation sums
   //   - need to form tower map for RoI reconstruction
   const DataVector< LVL1::TriggerTower >* storedTTs;
   LVL1::CPMTowerMap_t cpmtowers;
   if( m_retrievedEmTauTool ) {
      if( evtStore()->contains< TriggerTowerCollection >( m_TriggerTowerLocation ) ) {
         StatusCode sc = evtStore()->retrieve( storedTTs, m_TriggerTowerLocation );
         if( sc.isSuccess() ) m_EmTauTool->mapTowers( storedTTs, &cpmtowers );
         else ATH_MSG_WARNING( "Error retrieving TriggerTowers" );
      }
      else ATH_MSG_DEBUG( "No TriggerTowerCollection found at "
                          << m_TriggerTowerLocation );
   }

   // get EmTau Result
   const std::vector< ROIB::EMTauResult >& emtauResult = result->eMTauResult();

   // reconstruct ROI
   ATH_MSG_DEBUG( "EmTau ROI" );
   std::vector< ROIB::EMTauResult >::const_iterator itEMR  = emtauResult.begin();
   std::vector< ROIB::EMTauResult >::const_iterator itEMRe = emtauResult.end();
   for( ; itEMR != itEMRe; ++itEMR ) {
      const std::vector< ROIB::EMTauRoI >& emtauRoIV = itEMR->roIVec();
      std::vector< ROIB::EMTauRoI >::const_iterator itEM  = emtauRoIV.begin();
      std::vector< ROIB::EMTauRoI >::const_iterator itEMe = emtauRoIV.end();
      for( ; itEM != itEMe; ++itEM ) {

         uint32_t roIWord = itEM->roIWord();
         ATH_MSG_DEBUG( "About to create RecEmTauRoI : " << MSG::hex
                        << std::setw( 8 ) << roIWord << MSG::dec );

         // RecRoI
         LVL1::RecEmTauRoI recRoI( roIWord, &caloThresholds );

         // AOD component
         // ATLAS standard phi convention differs from L1 hardware convention
         double roiPhi = recRoI.phi();
         if( roiPhi > M_PI ) roiPhi -= 2 * M_PI;
         EmTau_ROI roi( roIWord, recRoI.eta(), roiPhi, roIWord & 0xFFFF );

         // fired thresholds
         std::vector< unsigned int >* thrV = recRoI.thresholdsPassed();
         std::vector< unsigned int >::const_iterator itTh  = thrV->begin();
         std::vector< unsigned int >::const_iterator itThE = thrV->end();
         for( ; itTh != itThE; ++itTh ) {
            double thrValue = recRoI.triggerThreshold(*itTh)* CLHEP::GeV;
            std::string thrName = "NameNotFound";
            if ( thresholdNames.find( *itTh - 1 ) != thresholdNames.end() ) {
               thrName = thresholdNames[ *itTh - 1 ];
            }

            roi.addThresholdValue( thrValue );
            roi.addThresholdName( thrName );

            ATH_MSG_DEBUG( "EmTau Thr : " << thrName
                           << ", value = "   << thrValue );
         }
         delete thrV;

         // Cluster ET values, reconstructed from TriggerTowers
         if( m_retrievedEmTauTool ) {
            m_EmTauTool->formSums( roIWord, &cpmtowers );
            roi.setCore( m_EmTauTool->Core() * caloTrigScale );
            roi.setEMClus( m_EmTauTool->EMClus() * caloTrigScale );
            roi.setTauClus( m_EmTauTool->TauClus() * caloTrigScale );
            roi.setEMIsol( m_EmTauTool->EMIsol() * caloTrigScale );
            roi.setHadIsol( m_EmTauTool->HadIsol() * caloTrigScale );
            roi.setHadCore( m_EmTauTool->HadCore() * caloTrigScale );
         }

         lvl1ROI->addEmTauROI( roi );
      }
   }

   for( LVL1::CPMTowerMap_t::value_type& p : cpmtowers)
     delete p.second;

   return;
}

void RoIBResultToAOD::addJetEnergyRoI( const ROIB::RoIBResult* result, LVL1_ROI* lvl1ROI ) {

   ATH_MSG_DEBUG( "building JetEnergyRoI" );

   // empty input
   if( result == 0 ) return;

   // Digit scale for calorimeter trigger
   float caloTrigScale = m_configSvc->thresholdConfig()->caloInfo().globalScale() * CLHEP::GeV;
   ATH_MSG_DEBUG( "caloTrigScale = " << caloTrigScale );

   /** Get Jet/Energy Thresholds from configSvc. Also fill maps of threshold names while
       we are here - will be useful later.

       Unfortunately there are 3 types of jet threshold and 3 types of ET trigger threshold,
       so this bit doesn't look very elegant */

   std::vector< TrigConf::TriggerThreshold* > thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
   std::vector<TrigConf::TriggerThreshold*> jetThresholds;
   std::vector<TrigConf::TriggerThreshold*> energyThresholds;
   std::vector<TrigConf::TriggerThreshold*> jetEnergyThresholds;
   std::map<int, std::string> jetNames;
   std::map<int, std::string> jfNames;
   std::map<int, std::string> jbNames;
   std::map<int, std::string> xeNames;
   std::map<int, std::string> teNames;
   std::map<int, std::string> xsNames;
   std::map<int, std::string> jeNames;

   for( std::vector<TrigConf::TriggerThreshold*>::const_iterator it = thresholds.begin();
        it != thresholds.end(); ++it ) {
      if( ( *it )->type() == L1DataDef::jetType() ) {
         jetThresholds.push_back( *it );
         jetNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::jfType() ) {
         jetThresholds.push_back( *it );
         jfNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::jbType() ) {
         jetThresholds.push_back( *it );
         jbNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::xeType() ) {
         energyThresholds.push_back( *it );
         xeNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::teType() ) {
         energyThresholds.push_back( *it );
         teNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::xsType() ) {
         energyThresholds.push_back( *it );
         xsNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
      else if( ( *it )->type() == L1DataDef::jeType() ) {
         jetEnergyThresholds.push_back( *it );
         jeNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
   }

   // Tool to reconstruct Jet cluster ET sums
   //   - form input map ready for analysis
   const DataVector< LVL1::JetElement >* storedJEs;
   std::map< int, LVL1::JetInput* > jetInputs;
   if( m_retrievedJetTool ) {
      if( evtStore()->contains< JetElementCollection >( m_JetElementLocation ) ) {
         StatusCode sc = evtStore()->retrieve( storedJEs, m_JetElementLocation );
         if( sc.isSuccess() ) m_JetTool->mapJetInputs( storedJEs, &jetInputs );
         else ATH_MSG_WARNING( "Error retrieving JetElements" );
      }
      else ATH_MSG_DEBUG( "No JetElementCollection found at "
                          << m_JetElementLocation );
   }

   // get JetEnergy Result
   const std::vector< ROIB::JetEnergyResult >& jetenergyResult = result->jetEnergyResult();

   // reconstruct ROI
   std::vector< ROIB::JetEnergyResult >::const_iterator itJETR  = jetenergyResult.begin();
   std::vector< ROIB::JetEnergyResult >::const_iterator itJETRe = jetenergyResult.end();
   for( ; itJETR != itJETRe; ++itJETR ) {
      const std::vector< ROIB::JetEnergyRoI >& jetenergyRoIV = itJETR->roIVec();
      std::vector< ROIB::JetEnergyRoI >::const_iterator itJET  = jetenergyRoIV.begin();
      std::vector< ROIB::JetEnergyRoI >::const_iterator itJETe = jetenergyRoIV.end();
      for( ; itJET != itJETe; ++itJET ) {
         uint32_t roIWord = itJET->roIWord();

         ATH_MSG_DEBUG( "Jet RoI, RoIWord = " << MSG::hex << std::setw( 8 )
                        << roIWord << MSG::dec );

         // RoI type
         LVL1::JEPRoIDecoder conv;
         int roiType = conv.roiType( roIWord );

         // Jet ROI
         if( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType ) {
            // RecRoI
            LVL1::RecJetRoI recRoI( roIWord, &jetThresholds );

            // AOD component
            // Convert to ATLAS phi convention
            double roiPhi = recRoI.phi();
            if( roiPhi > M_PI ) roiPhi -= 2 * M_PI;
            Jet_ROI roi( roIWord, recRoI.eta(), roiPhi, roIWord & 0xFFF );

            // fired Jet thresholds
            for( const unsigned int thrMapping : recRoI.thresholdsPassed()) {
               double thrValue = recRoI.triggerThreshold(thrMapping) * CLHEP::GeV;
               std::string thrName = "NameNotFound";
               if (!recRoI.isForwardJet()) {
                  if (jetNames.find(thrMapping-1) != jetNames.end()) thrName = jetNames[thrMapping-1];
               }
               else if (recRoI.eta() > 0) {
                  if (jfNames.find(thrMapping-1) != jfNames.end()) thrName = jfNames[thrMapping-1];
               }
               else {
                  if (jbNames.find(thrMapping-1) != jbNames.end()) thrName = jbNames[thrMapping-1];
               }
               roi.addThresholdValue( thrValue );
               roi.addThresholdName( thrName );
               ATH_MSG_DEBUG( "Jet Thr : " << thrName << ", value = " << thrValue );
            }

            // Jet Cluster ET sums
            if (m_retrievedJetTool) {
               m_JetTool->formSums(roIWord, &jetInputs);
               roi.setET4x4(m_JetTool->ET4x4()*caloTrigScale);
               roi.setET6x6(m_JetTool->ET6x6()*caloTrigScale);
               roi.setET8x8(m_JetTool->ET8x8()*caloTrigScale);
            }

            lvl1ROI->addJetROI(roi);
         }
         // Jet ET ROI
         else if( roiType == LVL1::TrigT1CaloDefs::JetEtRoIWordType ) {
            // AOD component
            JetET_ROI roi( roIWord, roIWord & 0xF );

            // fired Jet ET thresholds
            for( unsigned int i = 0; i < LVL1::TrigT1CaloDefs::numOfJetEtSumThresholds; ++i ) {
               if( ( roIWord >> i ) & 0x1 ) {
                  std::string thrName = "NameNotFound";
                  if (jeNames.find(i) != jeNames.end()) thrName = jeNames[i];
                  roi.addThreshold( thrName );
                  ATH_MSG_DEBUG( "JetEt Thr : " << thrName );
               }
            }

            lvl1ROI->addJetETROI(roi);
         }
         // EnergySum ROI
         else if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 ) {
            uint32_t roiWord0 = roIWord;
            ATH_MSG_DEBUG( "ET RoIWord 0 : " << MSG::hex << std::setw( 8 )
                           << roiWord0 << MSG::dec );
            ++itJET;
            uint32_t roiWord1 = itJET->roIWord();
            ATH_MSG_DEBUG( "ET RoIWord 1 : " << MSG::hex << std::setw( 8 )
                           << roiWord1 << MSG::dec );
            ++itJET;
            uint32_t roiWord2 = itJET->roIWord();
            ATH_MSG_DEBUG( "ET RoIWord 2 : " << MSG::hex << std::setw( 8 )
                           << roiWord2 << MSG::dec );

            // RecRoI
            LVL1::RecEnergyRoI recRoI( roiWord0, roiWord1, roiWord2, &energyThresholds );

            // Overflow bits  
            unsigned int overflows = 0;  
            if (conv.energyOverflow(roiWord0)) overflows |= 0x1;  
            if (conv.energyOverflow(roiWord1)) overflows |= 0x2;  
            if (conv.energyOverflow(roiWord2)) overflows |= 0x4;

            // AOD component
            EnergySum_ROI roi( roiWord0, roiWord1, roiWord2,
                               recRoI.energyX() * caloTrigScale,
                               recRoI.energyY() * caloTrigScale,
                               recRoI.energyT() * caloTrigScale,
                               ( roiWord1 >> 16 ) & 0xFF, ( roiWord2 >> 16 ) & 0xFF,
                               ( roiWord0 >> 16 ) & 0xFF, overflows );

            // fired summed ET thresholds
            for (unsigned int xeMapping : recRoI.etMissThresholdsPassed())
            {
               std::string thrName = "NameNotFound";
               if (xeNames.find(xeMapping - 1) != xeNames.end())
               {
                  thrName = xeNames[xeMapping - 1];
               }
               roi.addThreshold( thrName );
               ATH_MSG_DEBUG( "ETmiss threshold : " << thrName );
            }

            // fired missing ET thresholds
            for (unsigned int teMapping : recRoI.sumEtThresholdsPassed())
            {
               std::string thrName = "NameNotFound";
               if (teNames.find(teMapping - 1) != teNames.end())
               {
                  thrName = teNames[teMapping - 1];
               }
               roi.addThreshold( thrName );
               ATH_MSG_DEBUG( "SumET threshold : " << thrName );
            }

            // fired missing ET  significance thresholds
            for (unsigned int xsMapping : recRoI.mEtSigThresholdsPassed())
            {
               std::string thrName = "NameNotFound";
               if (xsNames.find(xsMapping - 1) != xsNames.end())
               {
                  thrName = xsNames[xsMapping - 1];
               }
               roi.addThreshold( thrName );
               ATH_MSG_DEBUG( "METSig threshold : " << thrName );
            }

            lvl1ROI->addEnergySumROI(roi);
         }
      }
   }

   for( std::map< int, LVL1::JetInput* >::iterator i = jetInputs.begin();
        i != jetInputs.end(); ++i ) {
      delete i->second;
   }

   return;
}

void RoIBResultToAOD::addMuonRoI( const ROIB::RoIBResult* result, LVL1_ROI* lvl1ROI ) {

   ATH_MSG_DEBUG( "in buildMuonRoI()" );

   // empty input
   if( result == 0 ) return;

   /** Get Muon Thresholds from configSvc. Also fill a map of threshold names while
       we are here - will be useful later */

   std::vector< TrigConf::TriggerThreshold* > thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
   std::vector< TrigConf::TriggerThreshold* > muonThresholds;
   std::map< int, std::string > thresholdNames;
   for( std::vector< TrigConf::TriggerThreshold* >::const_iterator it = thresholds.begin();
        it != thresholds.end(); ++it ) {
      if( ( *it )->type() == L1DataDef::muonType() ) {
         muonThresholds.push_back( *it );
         thresholdNames.insert(std::map<int, std::string>::value_type((*it)->thresholdNumber(),(*it)->name()));
      }
   }

   // get Muon ROI
   const std::vector< ROIB::MuCTPIRoI >& muonRoIV = result->muCTPIResult().roIVec();

   // reconstruct ROI
   ATH_MSG_DEBUG( "Muon ROI" );
   std::vector< ROIB::MuCTPIRoI >::const_iterator itMU  = muonRoIV.begin();
   std::vector< ROIB::MuCTPIRoI >::const_iterator itMUe = muonRoIV.end();
   for ( ; itMU != itMUe; ++itMU ) {

      uint32_t roIWord = itMU->roIWord();

      ATH_MSG_DEBUG( MSG::hex << std::setw( 8 ) << roIWord );

      if( ( m_recRPCRoiSvc == false ) || ( m_recTGCRoiSvc == false ) ) {
         continue;
      }

      // RecRoI
      LVL1::RecMuonRoI recRoI( roIWord, m_recRPCRoiSvc.operator->(),
                               m_recTGCRoiSvc.operator->(), &muonThresholds );

      double thrValue = recRoI.getThresholdValue() * CLHEP::GeV;
      int index = recRoI.getThresholdNumber() - 1;
      std::string thrName = "NameNotFound";
      if( thresholdNames.find(index) != thresholdNames.end() ) {
         thrName = thresholdNames[ index ];
      }

      // AOD component
      Muon_ROI roi( roIWord, recRoI.eta(), recRoI.phi(), thrName, thrValue );

      lvl1ROI->addMuonROI( roi );
      ATH_MSG_DEBUG( "Muon Thr : " << thrName << ", value = " << thrValue );

   } // for ( ; itMU != itMUe; ++itMU )

   return;
}
