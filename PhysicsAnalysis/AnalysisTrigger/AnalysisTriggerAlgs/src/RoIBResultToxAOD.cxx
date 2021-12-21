/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "TrigT1CaloUtils/CPMTobAlgorithm.h"
#include "TrigT1CaloUtils/JEMJetAlgorithm.h"

// Trigger configuration interface includes:
#include "TrigConfData/L1Menu.h"

// xAOD include(s):
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODTrigger/JetEtRoIAuxInfo.h"
#include "xAODTrigger/JetRoIAuxContainer.h"

namespace {

   /// Giga-electronvolts
   static const double GeV = 1000.0;

} // private namespace

RoIBResultToxAOD::RoIBResultToxAOD( const std::string& name,
                                    ISvcLocator* svcLoc )
   : AthReentrantAlgorithm( name, svcLoc )
{}

StatusCode RoIBResultToxAOD::initialize() {

   // Print system info.
   if( m_doCalo == false ) {
      ATH_MSG_INFO( "Inputs from LVL1 Calo systems switched off" );
   }
   if( m_doMuon == false ) {
      ATH_MSG_INFO( "Inputs from LVL1 Muon systems switched off" );
   }

   if( m_doMuon ) {
      // Get the RPC RecRoI tool
      ATH_CHECK( m_recRPCRoiTool.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recRPCRoiTool.typeAndName() );

      // Get the TGC RecRoI tool
      ATH_CHECK( m_recTGCRoiTool.retrieve() );
      ATH_MSG_DEBUG( "Connected to " << m_recTGCRoiTool.typeAndName() );
   } else {
      m_recRPCRoiTool.disable();
      m_recTGCRoiTool.disable();
   }

   if( m_doCalo ) {
      // Get tools
      ATH_CHECK( m_emTauTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_emTauTool.typeAndName() );

      ATH_CHECK( m_jetTool.retrieve() );
      ATH_MSG_DEBUG( "Got " << m_jetTool.typeAndName() );
   } else {
      m_emTauTool.disable();
      m_jetTool.disable();
   }

   // Initialise the keys.
   ATH_CHECK( m_roibResultKey.initialize() );
   ATH_CHECK( m_cpmTowerKey.initialize(SG::AllowEmpty) );
   ATH_CHECK( m_jetElementKey.initialize(SG::AllowEmpty) );
   ATH_CHECK( m_muonRoIKey.initialize(m_doMuon) );
   ATH_CHECK( m_emtauRoIKey.initialize(m_doCalo) );
   ATH_CHECK( m_energysumRoIKey.initialize(m_doCalo) );
   ATH_CHECK( m_jetetRoIKey.initialize(m_doCalo) );
   ATH_CHECK( m_jetRoIKey.initialize(m_doCalo) );

   // Return gracefully.
   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToxAOD::execute(const EventContext& ctx) const {

   // Tell the user what's happening.
   ATH_MSG_DEBUG( "in execute()" );

   // Access the input object.
   auto roibResult = SG::makeHandle( m_roibResultKey, ctx );
   if (!roibResult.isValid()) {
      ATH_MSG_ERROR("Failed to retrieve " << m_roibResultKey.key());
      return StatusCode::FAILURE;
   }

   // Create the muon RoIs:
   if( m_doMuon ) {
      ATH_CHECK( createMuonRoI( *roibResult, ctx ) );
   }

   // Create the calo RoIs:
   if( m_doCalo ) {
      ATH_CHECK( createEmTauRoI( *roibResult, ctx ) );
      ATH_CHECK( createJetEnergyRoI( *roibResult, ctx ) );
   }

   // Return gracefully.
   return StatusCode::SUCCESS;
}

StatusCode RoIBResultToxAOD::createEmTauRoI( const ROIB::RoIBResult& result,
                                             const EventContext& ctx ) const {

   // Tell the user what's happening.
   ATH_MSG_DEBUG( "building EmTauRoI" );

   /// Create the xAOD container.
   auto emtau_xaod = std::make_unique< xAOD::EmTauRoIContainer >();
   auto emtau_aux = std::make_unique< xAOD::EmTauRoIAuxContainer >();
   emtau_xaod->setStore( emtau_aux.get() );

   const TrigConf::L1Menu * l1menu = nullptr;
   std::map< int, std::string > emThresholdNames;
   std::map< int, std::string > tauThresholdNames;

   ATH_CHECK( detStore()->retrieve(l1menu) );
   // Digit scale for calorimeter trigger
   float caloTrigScale = static_cast<float>(l1menu->thrExtraInfo().EM().resolutionMeV());
   for( auto thr : l1menu->thresholds("EM")) {
      emThresholdNames[ thr->mapping() ] = thr->name();
   }
   for( auto thr : l1menu->thresholds("TAU")) {
      tauThresholdNames[ thr->mapping() ] = thr->name();
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
         LVL1::RecEmTauRoI recRoI( roIWord, l1menu );

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
         std::unique_ptr< std::vector< unsigned int > > thrV( recRoI.thresholdsPassed() );
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
            LVL1::CPMTobAlgorithm roiSums = m_emTauTool->formSums( roIWord, &cpmtowers );
            roi->setCore( roiSums.CoreET() * caloTrigScale );
            roi->setEmClus( roiSums.EMClusET() * caloTrigScale );
            roi->setTauClus( roiSums.TauClusET() * caloTrigScale );
            roi->setEmIsol( roiSums.EMIsolET() * caloTrigScale );
            roi->setHadIsol( roiSums.HadIsolET() * caloTrigScale );
            roi->setHadCore( roiSums.HadCoreET() * caloTrigScale );
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
                                      const EventContext& ctx ) const {

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

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) );
   
   // Digit scale for calorimeter trigger
   float caloTrigScale = static_cast<float>(l1menu->thrExtraInfo().JET().resolutionMeV());
   ATH_MSG_DEBUG( "caloTrigScale = " << caloTrigScale );

   // Fill maps of threshold names
   std::map<int, std::string> jetNames;
   std::map<int, std::string> jfNames;
   std::map<int, std::string> jbNames;
   std::map<int, std::string> xeNames;
   std::map<int, std::string> teNames;
   std::map<int, std::string> xsNames;
   std::map<int, std::string> jeNames;
   for( auto thr : l1menu->thresholds("JET")) {
      jetNames[ thr->mapping() ] = thr->name();
   }
   for( auto thr : l1menu->thresholds("XE")) {
      xeNames[ thr->mapping() ] = thr->name();
   }
   for( auto thr : l1menu->thresholds("TE")) {
      teNames[ thr->mapping() ] = thr->name();
   }
   for( auto thr : l1menu->thresholds("XS")) {
      xsNames[ thr->mapping() ] = thr->name();
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
            LVL1::RecJetRoI recRoI( roIWord, l1menu );

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
            for( unsigned int thr : recRoI.thresholdsPassed() ) {

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
               LVL1::JEMJetAlgorithm roiSums = m_jetTool->formSums( roIWord, &jetInputs );
               roi->setEt4x4( roiSums.ET4x4() * caloTrigScale );
               roi->setEt6x6( roiSums.ET6x6() * caloTrigScale );
               roi->setEt8x8( roiSums.ET8x8() * caloTrigScale );
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
            LVL1::RecEnergyRoI recRoI( roiWord0, roiWord1, roiWord2, l1menu );

            // xAOD component
            esum_xaod->initialize( roiWord0, roiWord1, roiWord2,
                                   recRoI.energyX() * caloTrigScale,
                                   recRoI.energyY() * caloTrigScale,
                                   recRoI.energyT() * caloTrigScale );

            // fired summed ET thresholds
            for( unsigned int thr : recRoI.etMissThresholdsPassed() ) {
               auto xeNameItr = xeNames.find( thr - 1 );
               const std::string thrName = ( xeNameItr != xeNames.end() ?
                                             xeNameItr->second :
                                             "NameNotFound" );
               esum_xaod->addThreshold( thrName );
               ATH_MSG_DEBUG( "ETmiss threshold : " << thrName );
            }

            // fired missing ET thresholds
            for( unsigned int thr : recRoI.sumEtThresholdsPassed() ) {
               auto teNameItr = teNames.find( thr - 1 );
               const std::string thrName = ( teNameItr != teNames.end() ?
                                             teNameItr->second :
                                             "NameNotFound" );
               esum_xaod->addThreshold( thrName );
               ATH_MSG_DEBUG( "SumET threshold : " << thrName );
            }

            // fired missing ET  significance thresholds
            for( unsigned int thr : recRoI.mEtSigThresholdsPassed() ) {
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

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) );
   
   // Create the xAOD container.
   auto mu_xaod = std::make_unique< xAOD::MuonRoIContainer >();
   auto mu_aux = std::make_unique< xAOD::MuonRoIAuxContainer >();
   mu_xaod->setStore( mu_aux.get() );


   std::vector< TrigConf::TriggerThreshold* > muonThresholds;
   std::map< int, std::string > thresholdNames;
   for( auto thr : l1menu->thresholds("MU")) {
      thresholdNames[ thr->mapping() ] = thr->name();
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
      LVL1::RecMuonRoI recRoI( roIWord, m_recRPCRoiTool.get(), m_recTGCRoiTool.get(), l1menu );

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
