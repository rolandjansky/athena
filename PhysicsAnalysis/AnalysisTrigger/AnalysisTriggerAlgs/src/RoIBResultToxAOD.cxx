/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//

// STL include(s):
#include <algorithm>
#include <cmath>

// Local include(s):
#include "RoIBResultToxAOD.h"

// Gaudi/Athena include(s):
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// LVL1 trigger include(s):
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1CaloEvent/JetInput.h"

// Trigger configuration interface includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// xAOD include(s):
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODTrigger/JetEtRoIAuxInfo.h"
#include "xAODTrigger/JetRoIAuxContainer.h"

namespace {

   /// Giga-electronvolts
   static const double GeV = 1000.0;

   /// Multiplier when we want to divide by GeV
   static const double InvGeV = 1 / GeV;

} // private namespace

/// Helper macro for initialising SG keys that are allowed to be missing
#define INIT_SG_KEY(VAR)                                    \
   do {                                                     \
      ATH_CHECK( VAR.initialize( ! VAR.key().empty() ) );   \
   } while( false )

RoIBResultToxAOD::RoIBResultToxAOD( const std::string& name,
                                    ISvcLocator* svcLoc )
   : AthAlgorithm( name, svcLoc ) {

}

StatusCode RoIBResultToxAOD::initialize() {

   // Print system info.
   if( m_doCalo == false ) {
      ATH_MSG_WARNING( "Inputs from LVL1 Calo systems switched off" );
   }
   if( m_doMuon == false ) {
      ATH_MSG_WARNING( "Inputs from LVL1 Muon systems switched off" );
   }

   // Connect to the LVL1ConfigSvc for the trigger configuration.
   ATH_CHECK( m_configSvc.retrieve() );
   ATH_MSG_DEBUG( "Connected to " << m_configSvc.typeAndName() );

   if( m_doMuon ) {
      // Get the RPC RecRoI service
      ATH_CHECK( m_recRPCRoiSvc.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recRPCRoiSvc.typeAndName() );

      // Get the TGC RecRoI service
      ATH_CHECK( m_recTGCRoiSvc.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recTGCRoiSvc.typeAndName() );
   }

   if( m_doCalo ) {
      // Get tools
      ATH_CHECK( m_emTauTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_emTauTool.typeAndName() );

      ATH_CHECK( m_jetTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_jetTool.typeAndName() );
   }

   // Initialise the keys.
   ATH_CHECK( m_roibResultKey.initialize() );
   INIT_SG_KEY( m_cpmTowerKey );
   INIT_SG_KEY( m_jetElementKey );
   if( m_doMuon ) {
      ATH_CHECK( m_muonRoIKey.initialize() );
   }
   if( m_doCalo ) {
      ATH_CHECK( m_emtauRoIKey.initialize() );
      ATH_CHECK( m_energysumRoIKey.initialize() );
      ATH_CHECK( m_jetetRoIKey.initialize() );
      ATH_CHECK( m_jetRoIKey.initialize() );
   }

   // Return gracefully.
   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToxAOD::execute() {

   // Tell the user what's happening.
   ATH_MSG_DEBUG( "in execute()" );

   // Access the input object.
   auto roibResult = SG::makeHandle( m_roibResultKey, getContext() );

   // Create the muon RoIs:
   if( m_doMuon ) {
      ATH_CHECK( createMuonRoI( *roibResult, getContext() ) );
   }

   // Create the calo RoIs:
   if( m_doCalo ) {
      ATH_CHECK( createEmTauRoI( *roibResult, getContext() ) );
      ATH_CHECK( createJetEnergyRoI( *roibResult, getContext() ) );
   }

   // Return gracefully.
   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToxAOD::createEmTauRoI( const ROIB::RoIBResult& result,
                                             const EventContext& ctx ) {

   // Tell the user what's happening.
   ATH_MSG_DEBUG( "building EmTauRoI" );

   /// Create the xAOD container.
   auto emtau_xaod = std::make_unique< xAOD::EmTauRoIContainer >();
   auto emtau_aux = std::make_unique< xAOD::EmTauRoIAuxContainer >();
   emtau_xaod->setStore( emtau_aux.get() );

   /// Digit scale for calorimeter trigger
   const float caloTrigScale =
      InvGeV * m_configSvc->thresholdConfig()->caloInfo().globalEmScale();
   ATH_MSG_DEBUG( "caloTrigScale = " << caloTrigScale );

   /** Get EmTau Thresholds from configSvc. Also fill a map of threshold names while
       we are here - will be useful later */

   const std::vector< TrigConf::TriggerThreshold* >& thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
   std::vector< TrigConf::TriggerThreshold* > caloThresholds;
   std::map< int, std::string > emThresholdNames;
   std::map< int, std::string > tauThresholdNames;
   for( TrigConf::TriggerThreshold* tt : thresholds ) {
      if( tt->type() == TrigConf::L1DataDef::emType() ) {
         caloThresholds.push_back( tt );
         emThresholdNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::tauType() ) {
         caloThresholds.push_back( tt );
         tauThresholdNames[ tt->thresholdNumber() ] = tt->name();
      }
   }

   // Tool to reconstruct EM/tau cluster & isolation sums
   //   - need to form tower map for RoI reconstruction
   xAOD::CPMTowerMap_t cpmtowers;
   if( m_emTauTool.isEnabled() && ( ! m_cpmTowerKey.key().empty() ) ) {
      auto cpmTower = SG::makeHandle( m_cpmTowerKey, ctx );
      if (cpmTower.isValid()) {
         m_emTauTool->mapTowers( cpmTower.cptr(), &cpmtowers );
      } else {
        ATH_MSG_DEBUG( "No CPMTowerCollection found at " << m_cpmTowerKey.key() );
      }
   }

   // reconstruct ROI
   ATH_MSG_DEBUG( "EmTau ROI" );
   for( const ROIB::EMTauResult& emtResult : result.eMTauResult() ) {
      for( const ROIB::EMTauRoI& emtRoI : emtResult.roIVec() ) {

         uint32_t roIWord = emtRoI.roIWord();
         ATH_MSG_DEBUG( "About to create RecEmTauRoI : " << MSG::hex
                        << std::setw( 8 ) << roIWord << MSG::dec );

         // RecRoI
         LVL1::RecEmTauRoI recRoI( roIWord, &caloThresholds );

         // xAOD component
         // ATLAS standard phi convention differs from L1 hardware convention
         double roiPhi = recRoI.phi();
         if( roiPhi > M_PI ) roiPhi -= 2 * M_PI;
         
         xAOD::EmTauRoI* roi = new xAOD::EmTauRoI();
         emtau_xaod->push_back( roi );
         roi->initialize( roIWord, recRoI.eta(), roiPhi );
         roi->setEtScale( caloTrigScale );
         roi->setThrPattern( recRoI.thresholdPattern() );

         // fired thresholds
         std::unique_ptr< std::vector< unsigned int > > thrV(
            recRoI.thresholdsPassed() );
         for( unsigned int thr : *thrV ) {
            const float thrValue = recRoI.triggerThreshold( thr ) * GeV;
            auto thrType = recRoI.thresholdType( thr );
            auto emNameItr = emThresholdNames.find( thr );
            auto tauNameItr = tauThresholdNames.find( thr );
            std::string thrName = "NameNotFound";
            if( ( thrType == LVL1::TrigT1CaloDefs::EMAlg ) &&
                ( emNameItr != emThresholdNames.end() ) ) {
               thrName = emNameItr->second;
            }
            else if( ( thrType == LVL1::TrigT1CaloDefs::TauAlg ) &&
                     ( tauNameItr != tauThresholdNames.end() ) ) {
               thrName = tauNameItr->second;
            }

            roi->addThreshold( thrName, thrValue );

            ATH_MSG_DEBUG( "EmTau Thr : " << thr << ", name = " << thrName
                           << ", value = "   << thrValue );
         }
 
         // Cluster ET values, reconstructed from TriggerTowers
         if( m_emTauTool.isEnabled() ) {
            m_emTauTool->formSums( roIWord, &cpmtowers );
            roi->setCore( m_emTauTool->Core() * caloTrigScale );
            roi->setEmClus( m_emTauTool->EMClus() * caloTrigScale );
            roi->setTauClus( m_emTauTool->TauClus() * caloTrigScale );
            roi->setEmIsol( m_emTauTool->EMIsol() * caloTrigScale );
            roi->setHadIsol( m_emTauTool->HadIsol() * caloTrigScale );
            roi->setHadCore( m_emTauTool->HadCore() * caloTrigScale );
         }
      }
   }

   // Record the results.
   auto emtauRoI = SG::makeHandle( m_emtauRoIKey, ctx );
   ATH_CHECK( emtauRoI.record( std::move( emtau_xaod ),
                               std::move( emtau_aux ) ) );

   // Return gracefully.
   return StatusCode::SUCCESS;
}

StatusCode
RoIBResultToxAOD::createJetEnergyRoI( const ROIB::RoIBResult& result,
                                      const EventContext& ctx ) {

   ATH_MSG_DEBUG( "building JetEnergyRoI" );
   
   // Containers for xAOD
   auto jet_xaod = std::make_unique< xAOD::JetRoIContainer >();
   auto jet_aux = std::make_unique< xAOD::JetRoIAuxContainer >();
   jet_xaod->setStore( jet_aux.get() );

   auto esum_xaod = std::make_unique< xAOD::EnergySumRoI >();
   auto esum_aux = std::make_unique< xAOD::EnergySumRoIAuxInfo >();
   esum_xaod->setStore( esum_aux.get() );

   auto jetet_xaod = std::make_unique< xAOD::JetEtRoI >();
   auto jetet_aux = std::make_unique< xAOD::JetEtRoIAuxInfo >();
   jetet_xaod->setStore( jetet_aux.get() );

   // Digit scale for calorimeter trigger
   const float caloTrigScale =
      InvGeV * m_configSvc->thresholdConfig()->caloInfo().globalJetScale();
   ATH_MSG_DEBUG( "caloTrigScale = " << caloTrigScale );

   /** Get Jet/Energy Thresholds from configSvc. Also fill maps of threshold names while
       we are here - will be useful later.

       Unfortunately there are 3 types of jet threshold and 3 types of ET trigger threshold,
       so this bit doesn't look very elegant */

   const std::vector< TrigConf::TriggerThreshold* >& thresholds =
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

   for( TrigConf::TriggerThreshold* tt : thresholds ) {
      if( tt->type() == TrigConf::L1DataDef::jetType() ) {
         jetThresholds.push_back( tt );
         jetNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::jfType() ) {
         jetThresholds.push_back( tt );
         jfNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::jbType() ) {
         jetThresholds.push_back( tt );
         jbNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::xeType() ) {
         energyThresholds.push_back( tt );
         xeNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::teType() ) {
         energyThresholds.push_back( tt );
         teNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::xsType() ) {
         energyThresholds.push_back( tt );
         xsNames[ tt->thresholdNumber() ] = tt->name();
      }
      else if( tt->type() == TrigConf::L1DataDef::jeType() ) {
         jetEnergyThresholds.push_back( tt );
         jeNames[ tt->thresholdNumber() ] = tt->name();
      }
   }

   // Tool to reconstruct Jet cluster ET sums
   //   - form input map ready for analysis
   std::map< int, LVL1::JetInput* > jetInputs;
   if( m_jetTool.isEnabled() && ( ! m_jetElementKey.key().empty() ) ) {
      auto jetElement = SG::makeHandle( m_jetElementKey, ctx );
      if (jetElement.isValid()) {
         m_jetTool->mapJetInputs( jetElement.cptr(), &jetInputs );
      } else {
         ATH_MSG_DEBUG( "No JetElementContainer found at " << m_jetElementKey.key() );
      }
   }
   std::vector< std::unique_ptr< LVL1::JetInput > > jetInputsHolder;
   for( auto pair : jetInputs ) {
      jetInputsHolder.emplace_back( pair.second );
   }

   // reconstruct ROI
   for( const ROIB::JetEnergyResult& jeteResult : result.jetEnergyResult() ) {
      auto itJET = jeteResult.roIVec().begin();
      auto endJET = jeteResult.roIVec().end();
      for( ; itJET != endJET; ++itJET ) {

         const uint32_t roIWord = itJET->roIWord();

         ATH_MSG_DEBUG( "Jet RoI, RoIWord = " << MSG::hex << std::setw( 8 )
                        << roIWord << MSG::dec );

         // RoI type
         LVL1::JEPRoIDecoder conv;
         const int roiType = conv.roiType( roIWord );

         // Jet ROI
         if( roiType == LVL1::TrigT1CaloDefs::JetRoIWordType ) {
            // RecRoI
            LVL1::RecJetRoI recRoI( roIWord, &jetThresholds );

            // xAOD component
            // Convert to ATLAS phi convention
            double roiPhi = recRoI.phi();
            if( roiPhi > M_PI ) roiPhi -= 2 * M_PI;
            
            xAOD::JetRoI* roi = new xAOD::JetRoI();
            jet_xaod->push_back( roi );
            roi->initialize( roIWord, recRoI.eta(), roiPhi );
            roi->setEtScale( caloTrigScale );
            roi->setThrPattern( recRoI.thresholdPattern() );
            
            // fired Jet thresholds
            std::unique_ptr< std::vector< unsigned int > > thrV(
               recRoI.thresholdsPassed() );
            for( unsigned int thr : *thrV ) {

               const double thrValue = recRoI.triggerThreshold( thr ) * GeV;
               auto jetNameItr = jetNames.find( thr );
               auto jfNameItr = jfNames.find( thr );
               auto jbNameItr = jbNames.find( thr );
               std::string thrName = "NameNotFound";
               if( ! recRoI.isForwardJet() ) {
                  if( jetNameItr != jetNames.end() ) {
                     thrName = jetNameItr->second;
                  }
               }
               else if( recRoI.eta() > 0 ) {
                  if( jfNameItr != jfNames.end() ) {
                     thrName = jfNameItr->second;
                  }
               }
               else {
                  if( jbNameItr != jbNames.end() ) {
                     thrName = jbNameItr->second;
                  }
               }

               roi->addThreshold( thrName, thrValue );

               ATH_MSG_DEBUG( "Jet Thr : " << thrName
                              << ", value = " << thrValue );
            }

            // Jet Cluster ET sums
            if( m_jetTool.isEnabled() ) {
               m_jetTool->formSums( roIWord, &jetInputs );
               roi->setEt4x4( m_jetTool->ET4x4() * caloTrigScale );
               roi->setEt6x6( m_jetTool->ET6x6() * caloTrigScale );
               roi->setEt8x8( m_jetTool->ET8x8() * caloTrigScale );
            }

         }
         // Jet ET ROI
         else if( roiType == LVL1::TrigT1CaloDefs::JetEtRoIWordType ) {

            // xAOD component
            jetet_xaod->setRoIWord( roIWord );

            // fired Jet ET thresholds
            for( unsigned int i = 0;
                 i < LVL1::TrigT1CaloDefs::numOfJetEtSumThresholds; ++i ) {
               if( ( roIWord >> i ) & 0x1 ) {
                  std::string thrName = "NameNotFound";
                  if (jeNames.find(i) != jeNames.end()) thrName = jeNames[i];
                  jetet_xaod->addThreshold( thrName );
                  ATH_MSG_DEBUG( "JetEt Thr : " << thrName );
               }
            }

         }
         // EnergySum ROI
         else if ( roiType == LVL1::TrigT1CaloDefs::EnergyRoIWordType0 ) {
            
            // Extract information and fill EnergySumRoI
            const uint32_t roiWord0 = roIWord;
            ATH_MSG_DEBUG( "ET RoIWord 0 : " << MSG::hex << std::setw( 8 )
                           << roiWord0 << MSG::dec );
            ++itJET;
            const uint32_t roiWord1 = itJET->roIWord();
            ATH_MSG_DEBUG( "ET RoIWord 1 : " << MSG::hex << std::setw( 8 )
                           << roiWord1 << MSG::dec );
            ++itJET;
            const uint32_t roiWord2 = itJET->roIWord();
            ATH_MSG_DEBUG( "ET RoIWord 2 : " << MSG::hex << std::setw( 8 )
                           << roiWord2 << MSG::dec );

            // RecRoI
            LVL1::RecEnergyRoI recRoI( roiWord0, roiWord1, roiWord2,
                                       &energyThresholds );

            // Overflow bits  
            unsigned int overflows = 0;  
            if (conv.energyOverflow(roiWord0)) overflows |= 0x1;  
            if (conv.energyOverflow(roiWord1)) overflows |= 0x2;  
            if (conv.energyOverflow(roiWord2)) overflows |= 0x4;

            // xAOD component
            esum_xaod->initialize( roiWord0, roiWord1, roiWord2,
                                   recRoI.energyX() * caloTrigScale,
                                   recRoI.energyY() * caloTrigScale,
                                   recRoI.energyT() * caloTrigScale );

            // fired summed ET thresholds
            std::unique_ptr< std::vector< unsigned int > > thrEtMiss(
               recRoI.etMissThresholdsPassed() );
            for( unsigned int thr : *thrEtMiss ) {
               auto xeNameItr = xeNames.find( thr - 1 );
               const std::string thrName = ( xeNameItr != xeNames.end() ?
                                             xeNameItr->second :
                                             "NameNotFound" );
               esum_xaod->addThreshold( thrName );
               ATH_MSG_DEBUG( "ETmiss threshold : " << thrName );
            }

            // fired missing ET thresholds
            std::unique_ptr< std::vector< unsigned int > > thrSumEt(
               recRoI.sumEtThresholdsPassed() );
            for( unsigned int thr : *thrSumEt ) {
               auto teNameItr = teNames.find( thr - 1 );
               const std::string thrName = ( teNameItr != teNames.end() ?
                                             teNameItr->second :
                                             "NameNotFound" );
               esum_xaod->addThreshold( thrName );
               ATH_MSG_DEBUG( "SumET threshold : " << thrName );
            }

            // fired missing ET  significance thresholds
            std::unique_ptr< std::vector< unsigned int > > thrMETSig(
               recRoI.mEtSigThresholdsPassed() );
            for( unsigned int thr : *thrMETSig ) {
               auto xsNameItr = xsNames.find( thr - 1 );
               const std::string thrName = ( xsNameItr != xsNames.end() ?
                                             xsNameItr->second :
                                             "NameNotFound" );
               esum_xaod->addThreshold( thrName );
               ATH_MSG_DEBUG( "METSig threshold : " << thrName );
            }
         }
      }
   }

   /// Record the results
   auto jetRoI = SG::makeHandle( m_jetRoIKey, ctx );
   ATH_CHECK( jetRoI.record( std::move( jet_xaod ), std::move( jet_aux ) ) );

   auto jetetRoI = SG::makeHandle( m_jetetRoIKey, ctx );
   ATH_CHECK( jetetRoI.record( std::move( jetet_xaod ),
                               std::move( jetet_aux ) ) );

   auto energysumRoI = SG::makeHandle( m_energysumRoIKey, ctx );
   ATH_CHECK( energysumRoI.record( std::move( esum_xaod ),
                                   std::move( esum_aux ) ) );

   // Return gracefully.   
   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToxAOD::createMuonRoI( const ROIB::RoIBResult& result,
                                            const EventContext& ctx ) const {

   ATH_MSG_DEBUG( "in buildMuonRoI()" );

   // Create the xAOD container.
   auto mu_xaod = std::make_unique< xAOD::MuonRoIContainer >();
   auto mu_aux = std::make_unique< xAOD::MuonRoIAuxContainer >();
   mu_xaod->setStore( mu_aux.get() );

   /** Get Muon Thresholds from configSvc. Also fill a map of threshold names while
       we are here - will be useful later */

   const std::vector< TrigConf::TriggerThreshold* >& thresholds =
      m_configSvc->ctpConfig()->menu().thresholdVector();
   std::vector< TrigConf::TriggerThreshold* > muonThresholds;
   std::map< int, std::string > thresholdNames;
   for( TrigConf::TriggerThreshold* tt : thresholds ) {
      if( tt->type() == TrigConf::L1DataDef::muonType() ) {
         muonThresholds.push_back( tt );
         thresholdNames[ tt->thresholdNumber() ] = tt->name();
      }
   }

   // get Muon ROI
   const std::vector< ROIB::MuCTPIRoI >& muonRoIV =
      result.muCTPIResult().roIVec();

   // reconstruct ROI
   ATH_MSG_DEBUG( "Muon ROI" );
   for( const ROIB::MuCTPIRoI& roi : muonRoIV ) {

      uint32_t roIWord = roi.roIWord();

      ATH_MSG_DEBUG( MSG::hex << std::setw( 8 ) << roIWord );

      // RecRoI
      const LVL1::RecMuonRoI recRoI( roIWord, m_recRPCRoiSvc.get(),
                                     m_recTGCRoiSvc.get(), &muonThresholds );

      const double thrValue = recRoI.getThresholdValue() * GeV;
      const int index = recRoI.getThresholdNumber() - 1;
      auto thrNameItr = thresholdNames.find( index );
      const std::string thrName = ( thrNameItr != thresholdNames.end() ?
                                    thrNameItr->second :
                                    "NameNotFound" );

      xAOD::MuonRoI* xaod_roi = new xAOD::MuonRoI();
      mu_xaod->push_back( xaod_roi );
      xaod_roi->initialize( roIWord, recRoI.eta(), recRoI.phi(), thrName,
                            thrValue );

      ATH_MSG_DEBUG( "Muon Thr : " << thrName << ", value = " << thrValue );
   }

   // Record the muon RoIs.
   auto muonRoI = SG::makeHandle( m_muonRoIKey, ctx );
   ATH_CHECK( muonRoI.record( std::move( mu_xaod ), std::move( mu_aux ) ) );

   // Return gracefully.
   return StatusCode::SUCCESS;
}
