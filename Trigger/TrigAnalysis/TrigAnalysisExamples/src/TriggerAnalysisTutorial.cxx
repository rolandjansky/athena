/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerAnalysisTutorial.cxx 723125 2016-02-10 15:37:04Z ssnyder $

// System include(s):
#include <iomanip>

// ROOT include(s):
#include "TTree.h"
#include "TH1F.h"

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/Units.h"

// TrigDecisionTool include(s):
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

// Analysis include(s):
#include "AnalysisUtils/AnalysisCombination.h"

// Trigger EDM include(s):
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"

// Offline EDM include(s):
#include "egammaEvent/ElectronContainer.h"

// Local include(s):
#include "TriggerAnalysisTutorial.h"

using Athena::Units::GeV;

TriggerAnalysisTutorial::TriggerAnalysisTutorial( const std::string &name,
                                                  ISvcLocator *pSvcLocator)
   : AthAlgorithm( name, pSvcLocator ),
     m_eventNr( 0 ),
     m_eventsPrinted( 0 ),
     //m_runNr( 0 ),
     //m_lbNr( 0 ),
     m_triggerAccepts(nullptr),
     m_tree(nullptr),
     m_l2Jet_et(0),
     m_l2Jet_eta(0),
     m_l2Jet_phi(0),
     m_l1Jet_et88(0),
     m_l1Jet_eta(0),
     m_l1Jet_phi(0),
     m_l1roi_eta(0),
     m_l1roi_phi(0),
     m_MZ( new std::vector< float >() ),
     m_pE1( new std::vector< float >() ),
     m_pE2( new std::vector< float >() ),
     m_matchDistance( new std::vector< float >() ),
     m_pEOther( new std::vector< float >() ),
     m_matchOther( new std::vector< int >() ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
     m_matchTool( "TrigMatchTool", this ),
     m_histSvc( "THistSvc", name ) {

   declareProperty("StatTriggerChains",        m_triggerChains,
                   "list of triggers for which to print statistics");
   declareProperty("JetTrigger",               m_jetTriggerChain = "",
                   "Trigger chain for jet trigger study");
   declareProperty("ZeeTrigger",               m_zeeTriggerChain = "",
                   "Trigger chain for Z->ee analysis");
   declareProperty("ElectronMatchMaxDistance", m_zeeMaxMatchDistance = 0.1,
                   "Maximum distance for matched electrons");
}

TriggerAnalysisTutorial::~TriggerAnalysisTutorial()
{
}

StatusCode TriggerAnalysisTutorial::initialize() {

   // retrieve the tools and services:
   CHECK( m_trigDec.retrieve() );
   CHECK( m_matchTool.retrieve() );
   CHECK( m_histSvc.retrieve() );

   // build and register tree
   m_tree = new TTree( "trigtr", "TriggerAnalysisTutorial" );

   m_tree->Branch( "Trig_l2Jet_et",   &m_l2Jet_et  , "Trig_l2Jet_et/f"  );
   m_tree->Branch( "Trig_l2Jet_eta",  &m_l2Jet_eta , "Trig_l2Jet_eta/f" );
   m_tree->Branch( "Trig_l2Jet_phi",  &m_l2Jet_phi , "Trig_l2Jet_phi/f" );
   m_tree->Branch( "Trig_l1Jet_et88", &m_l1Jet_et88, "Trig_l1Jet_et88/f" );
   m_tree->Branch( "Trig_l1Jet_eta",  &m_l1Jet_eta , "Trig_l1Jet_eta/f" );
   m_tree->Branch( "Trig_l1Jet_phi",  &m_l1Jet_phi , "Trig_l1Jet_phi/f" );
   m_tree->Branch( "Trig_l1roi_eta",  &m_l1roi_eta , "Trig_l1roi_eta/f" );
   m_tree->Branch( "Trig_l1roi_phi",  &m_l1roi_phi , "Trig_l1roi_phi/f" );
   m_tree->Branch( "Zmass", &m_MZ ); 
   m_tree->Branch( "e1P", &m_pE1 ); 
   m_tree->Branch( "e2P", &m_pE2 ); 
   m_tree->Branch( "deltaR", &m_matchDistance ); 
   m_tree->Branch( "probeP", &m_pEOther ); 
   m_tree->Branch( "probeMatch", &m_matchOther ); 

   std::string fullNtupleName =  "/file/trigtr";
   CHECK( m_histSvc->regTree( fullNtupleName, m_tree ) );

   // build and register histograms
   m_triggerAccepts = new TH1F( "TriggerAccepts", "TriggerAccepts", 3, 0, 3 );
   CHECK( m_histSvc->regHist( "/Trigger/TriggerAccepts", m_triggerAccepts ) );

   // initialize the trigger accept counters
   std::vector< std::string >::const_iterator it;
   for( it = m_triggerChains.begin(); it != m_triggerChains.end(); ++it ) {
      m_triggersPassed[ *it ] = 0;
   }

   // done
   ATH_MSG_INFO( "Initialization successful" );

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::finalize() {

   ATH_MSG_INFO( "STAT Trigger Statistics on " << m_eventNr << " processed events" );

   std::vector< std::string >::const_iterator it;
   for( it = m_triggerChains.begin(); it != m_triggerChains.end(); ++it ) {
      ATH_MSG_INFO( "STAT Passed events for chain " << *it << "  " << m_triggersPassed[ *it ]
                    << " (" << 100. * m_triggersPassed[ *it ] / m_eventNr << "%)" );
   }

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::execute() {

   ++m_eventNr;

   ATH_MSG_INFO( "Event Number " << m_eventNr );

   if( m_jetTriggerChain != "" ) {
      // print out chains in each level and prescales
      if( m_eventNr ==1 ) {
         CHECK( printTriggerChainsAllLevels() );
         CHECK( printPrescales() );
      }

      // print pass/fail for a few chains
      CHECK( printSimplePassInfo() );

      // collect statistics for the chains in m_triggerChains
      CHECK( collectTriggerStatistics() );

      // a simple study of jet trigger reconstruction
      if( jetTriggerStudy( m_jetTriggerChain ).isSuccess() ) {
         m_tree->Fill();
      }
   }

   if( m_zeeTriggerChain != "" ) {
      // Z->ee trigger analysis
      if( zeeTriggerAnalysis( m_zeeTriggerChain ).isSuccess() ) {
         m_tree->Fill();
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::printTriggerChainsAllLevels() {

   // print out list of chains in each level for the first event:
   ATH_MSG_INFO( "L1 Items : " << m_trigDec->getListOfTriggers( "L1_.*" ) );
   ATH_MSG_INFO( "L2 Chains: " << m_trigDec->getListOfTriggers( "L2_.*" ) );
   ATH_MSG_INFO( "EF Chains: " << m_trigDec->getListOfTriggers( "EF_.*" ) );

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::printPrescales() {

   // on the first event we are printing out prescale factors for all
   // EF chains

   std::vector< std::string >::const_iterator it;
   const std::vector< std::string > allEF = m_trigDec->getListOfTriggers( "EF_.*" );
   for( it = allEF.begin(); it != allEF.end(); ++it ) {
      ATH_MSG_INFO( "Prescale info: chain " << std::left << *it << " has prescale "
                    << m_trigDec->getPrescale( *it ) );
   }
   ATH_MSG_INFO( "Stream info: " << m_trigDec->getListOfStreams() );

   for( it = m_triggerChains.begin(); it != m_triggerChains.end(); ++it ) {
      std::vector< std::string > chgrcnt = m_trigDec->getChainGroup( *it )->getListOfTriggers();
      std::vector<std::string>::const_iterator chgrit;
      for( chgrit = chgrcnt.begin(); chgrit != chgrcnt.end(); ++chgrit ) {
         ATH_MSG_INFO( "Chain belonging to " << *it << ": " << *chgrit );
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::printSimplePassInfo() {

   /// simple example of isPassed():
   /// isPassed([chain], [condition]) is called with the default argument condition = Physics
   /// a ChainGroup is defined implicitly by the regular expression given by "EF.*" in the call to isPassed()
   ATH_MSG_INFO( "Pass state L1 = " << m_trigDec->isPassed( "L1_.*" ) );
   ATH_MSG_INFO( "Pass state L2 = " << m_trigDec->isPassed( "L2_.*" ) );
   ATH_MSG_INFO( "Pass state EF = " << m_trigDec->isPassed( "EF_.*" ) );

   ATH_MSG_INFO( "Pass state L2_tau16i_2j23       = " << m_trigDec->isPassed( "L2_tau16i_2j23" ) );
   ATH_MSG_INFO( "Pass state EF_tau16i_2j23       = " << m_trigDec->isPassed( "EF_tau16i_2j23" ) );
   ATH_MSG_INFO( "Pass state EF_mu10              = " << m_trigDec->isPassed( "EF_mu10" ) );
   ATH_MSG_INFO( "Pass state EF_mu20              = " << m_trigDec->isPassed( "EF_mu20" ) );
   ATH_MSG_INFO( "Pass state EF_e15_medium        = " << m_trigDec->isPassed( "EF_e15_medium" ) );
   ATH_MSG_INFO( "Pass state EF_e20_loose         = " << m_trigDec->isPassed( "EF_e20_loose" ) );

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::collectTriggerStatistics() {

   // Now we'd like to collect some trigger statistics for the chains specified 

   std::vector< std::string >::const_iterator it;
   for( it = m_triggerChains.begin(); it != m_triggerChains.end(); ++it ) {
      if( m_trigDec->isPassed( *it ) ) {
         m_triggersPassed[ *it ]++;
         m_triggerAccepts->Fill( it->c_str(), 1 );
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode TriggerAnalysisTutorial::jetTriggerStudy( const std::string& chain ) {

   using Trig::Feature;
   using Trig::FeatureContainer;
   using Analysis::TauJetContainer;

   m_l2Jet_et   = 0;
   m_l2Jet_eta  = 0;
   m_l2Jet_phi  = 0;
   m_l1Jet_et88 = 0;
   m_l1Jet_eta  = 0;
   m_l1Jet_phi  = 0;
   m_l1roi_eta  = 0;
   m_l1roi_phi  = 0;
	     
   if( ! m_trigDec->isPassed( chain ) ) return StatusCode::FAILURE;

   ++m_eventsPrinted;

   if( m_eventsPrinted < 100 ) {
      ATH_MSG_INFO("FLAT Pass state " << chain << " = " << m_trigDec->isPassed( chain ) );
   }

   //bool useAlsoDeactivatedTEs = false;

   // first declare a FeatureContainer; fill it using the features(std::string chain_name) method
   FeatureContainer f = m_trigDec->features( chain,
                                             //useAlsoDeactivatedTEs ? TrigDefs::alsoDeactivateTEs :
                                             TrigDefs::Physics );

   // let's get the jets at level 2
   std::vector< Feature< TrigT2Jet > > l2jets = f.get<TrigT2Jet>();
   if( m_eventsPrinted < 100 ) {
      ATH_MSG_INFO( "FLAT Number of level 2 jets <TrigT2Jet>: " << l2jets.size() );
   }
   std::vector< Feature< TrigT2Jet > >::const_iterator fIt;
   for( fIt = l2jets.begin(); fIt < l2jets.end(); ++fIt ) {

      // get from the feature to the object
      const Feature< TrigT2Jet >& l2jetF = *fIt;
      if( ! l2jetF.empty() ) {
         const TrigT2Jet* t2jet = l2jetF.cptr();
         if( m_eventsPrinted < 100 ) {
            ATH_MSG_INFO( "FLAT    e    : " << t2jet->e() );
            ATH_MSG_INFO( "FLAT    eta  : " << t2jet->eta() );
            ATH_MSG_INFO( "FLAT    phi  : " << t2jet->phi() );
            ATH_MSG_INFO( "FLAT    ehad : " << t2jet->ehad0() );
            ATH_MSG_INFO( "FLAT    eem  : " << t2jet->eem0() );
         }
         m_l2Jet_et   = t2jet->e()/cosh(t2jet->eta());
         m_l2Jet_eta  = t2jet->eta();
         m_l2Jet_phi  = t2jet->phi();
      }
	     
      // we can also access the L1 Jet_ROI using the ancestor method of the TrigDecisionTool
      Feature< Jet_ROI > jRoIF = m_trigDec->ancestor< Jet_ROI >( l2jetF );
      if( m_eventsPrinted < 100 ) {
         ATH_MSG_INFO( "FLAT Found " << ( jRoIF.empty() ? "no " : "" )
                       << "corresponding Jet_ROI" );
      }
      if( ! jRoIF.empty() ) {
         const Jet_ROI* jroi = jRoIF.cptr();
         if( m_eventsPrinted < 100 ) {
            ATH_MSG_INFO( "FLAT Passed thresholds" << jroi->getThresholdNames() );
            ATH_MSG_INFO( "FLAT    ET4x4 : " << jroi->getET4x4() );
            ATH_MSG_INFO( "FLAT    ET6x6 : " << jroi->getET6x6() );
            ATH_MSG_INFO( "FLAT    ET8x8 : " << jroi->getET8x8() );
            ATH_MSG_INFO( "FLAT    eta   : " << jroi->eta() );
            ATH_MSG_INFO( "FLAT    phi   : " << jroi->phi() );
         }
         m_l1Jet_et88 = jroi->getET8x8();
         m_l1Jet_eta  = jroi->eta();
         m_l1Jet_phi  = jroi->phi();
      }

      Feature< TrigRoiDescriptor > initialROI =
         m_trigDec->ancestor< TrigRoiDescriptor >( l2jetF, "initialRoI" );
      if ( initialROI.empty() ) {
         if( m_eventsPrinted < 100 ) {
            ATH_MSG_INFO( "FLAT no matching initial ROIDescriptor" );
         }
      } else {
         const TrigRoiDescriptor* trigroi = initialROI.cptr();
         if( m_eventsPrinted < 100 ) {
            ATH_MSG_INFO( "FLAT matching initial ROIDescriptor " << trigroi );
            ATH_MSG_INFO( "FLAT    eta   : " << trigroi->eta() );
            ATH_MSG_INFO( "FLAT    phi   : " << trigroi->phi() );
         }
         m_l1roi_eta  = trigroi->eta();
         m_l1roi_phi  = trigroi->phi();
      }

   }

   // now we like to look at the Combinations of jets and tau that make up the chain decision
   if( m_eventsPrinted >= 100 ) return StatusCode::SUCCESS;

   ATH_MSG_INFO( "COMB ==> Event Number " << m_eventNr << " <==" );
   ATH_MSG_INFO( "COMB Pass state " << chain << " = " << m_trigDec->isPassed( chain ) );

   const std::vector< Trig::Combination >& tauJetCombinations = f.getCombinations();
   ATH_MSG_INFO( "COMB Number of " /*<< ( useAlsoDeactivatedTEs ? "" : "active " )*/
                 << "TauJetCombinations in " << chain << ": " << tauJetCombinations.size() );
   std::vector< Trig::Combination >::const_iterator cIt;
   for( cIt = tauJetCombinations.begin(); cIt != tauJetCombinations.end(); ++cIt ) {

      const Trig::Combination& comb = *cIt;
	     
      std::vector< Feature< TauJetContainer > > tauC = comb.get<TauJetContainer>();
      std::vector< Feature< JetCollection > >   jetC = comb.get<JetCollection>( "TrigJetRec" ); // needs to be specified to disentangle from JetCollections produced during tau reconstruction (those have label TrigTauRec)

      ATH_MSG_INFO( "COMB Combination was " << ( comb.active() ? "" : "not " ) << "active." );

      if( tauC.size() || jetC.size() ) {

         ATH_MSG_INFO( "COMB Combination has " << tauC.size() << " TauJetContainer Fs and " 
                       << jetC.size() << " JetCollection Fs" );

         int ntaus = -1;
         if( tauC.size() ) {
            const TauJetContainer* tjc = tauC[ 0 ];
            ntaus = tjc->size();
         }
         int njets = -1;
         if( jetC.size() ) {
            const JetCollection* jc = jetC[ 0 ];
            njets = jc->size();
         }

         ATH_MSG_INFO( "COMB In the TauJetContainer are " << ntaus
                       << " taus and in the JetCollection are "
                       << njets << " jets." );
      } else {
         ATH_MSG_INFO( "COMB TauJetContainer or JetCollection missing." );
      }
	
      std::vector< Feature< TrigTau > >   tauFV = comb.get< TrigTau >();
      std::vector< Feature< TrigT2Jet > > jetFV = comb.get< TrigT2Jet >();
	
      ATH_MSG_INFO( "COMB Combination has " << tauFV.size() << " TrigTau Fs and "
                    << jetFV.size() << " TrigT2Jet Fs." );

   }

   return StatusCode::SUCCESS;
}
 
StatusCode TriggerAnalysisTutorial::zeeTriggerAnalysis( const std::string& chain ) {

   m_MZ->clear();
   m_pE1->clear();
   m_pE2->clear();
   m_matchDistance->clear();
   m_pEOther->clear();
   m_matchOther->clear();

   bool chainAccept = m_trigDec->isPassed( chain );
   ATH_MSG_INFO( "ZEE Pass state " << chain << " = " << ( chainAccept ? "PASS" : "FAIL" ) );
   if( ! chainAccept ) return StatusCode::SUCCESS;

   using Trig::Feature;
   using Trig::FeatureContainer;
   using Analysis::Electron;

   // now the offline part 
   const ElectronContainer* elecTES = 0;
   CHECK( evtStore()->retrieve( elecTES, "ElectronAODCollection" ) );
   ATH_MSG_INFO( "ElectronContainer successfully retrieved - size is " << elecTES->size()
                 << " electrons" );

   typedef std::vector< const Electron* > ListOfElectrons;

   ListOfElectrons electrons;

   ElectronContainer::const_iterator el = elecTES->begin();
   ElectronContainer::const_iterator lastel = elecTES->end();
   for( ; el != lastel; ++el ) {
      electrons.push_back( *el );
   }

   AnalysisUtils::Combination< ListOfElectrons > comb( &electrons, 2 );

   ListOfElectrons electronPair;
   while( comb.get( electronPair ) ) {
      const Electron* el1 = electronPair[ 0 ];
      const Electron* el2 = electronPair[ 1 ];
      double zmass = ( el1->hlv() + el2->hlv() ).m() / GeV;

      m_MZ->push_back( zmass );
      if( el1->pt() > el2->pt() ) {
         m_pE1->push_back( el1->pt() );
         m_pE2->push_back( el2->pt() );
      } else {
         m_pE1->push_back( el2->pt() );
         m_pE2->push_back( el1->pt() );
      }

      if( zmass < 80 || zmass > 100 ) continue;

      for( int m = 0; m < 2; ++m ) {

         const Electron* offlineElectron = electronPair[ m ];
         const Electron* otherOfflineElectron = electronPair[ 1 - m ];

         // print the info for the electron
         ATH_MSG_INFO( "ZEE Have reconstructed electron with eta, phi, pt: " 
                       << offlineElectron->eta() << ", " << offlineElectron->phi() << ", "
                       << offlineElectron->pt() );

         //const egamma *triggerElectronMatch = m_matchTool->matchToTriggerObject< egamma >( offlineElectron, chain, true ); // true - only passed trig electrons
         std::vector< const egamma* > allTriggerElectronMatch =
            m_matchTool->matchToTriggerObjects< egamma >( offlineElectron, chain, true); // true - only passed trig electrons

         if( allTriggerElectronMatch.size() ) {
            ATH_MSG_INFO( "ZEE Number of trigger matches " << allTriggerElectronMatch.size() );

            const egamma *triggerElectronMatch = allTriggerElectronMatch[ 0 ];
            if( triggerElectronMatch ) {
               ATH_MSG_INFO( "ZEE EF match in chain " << chain << " with eta, phi, et: "
                             << triggerElectronMatch->eta() << ", "
                             << triggerElectronMatch->phi() << ", "
                             << triggerElectronMatch->et() );
               float distance = m_matchTool->distance( offlineElectron, triggerElectronMatch );
               ATH_MSG_INFO( "ZEE Distance " << distance );
               m_matchDistance->push_back( distance );
            } else {
               ATH_MSG_INFO( "ZEE No match in chain " << chain );
            }
         }

         // using the matching tool to asked for match
         // could also check if distance (above) is smaller than m_zeeMaxMatchDistance
         bool matchWithinDistance =
            m_matchTool->chainPassedByObject< egamma, Analysis::Electron >( offlineElectron,
                                                                            chain,
                                                                            m_zeeMaxMatchDistance );

         // if the offline electron is matched to a trigger electron within m_zeeMaxMatchDistance
         // then
         if( matchWithinDistance ) {
            bool otherMatchWithinDistance =
               m_matchTool->chainPassedByObject< egamma, Analysis::Electron >( otherOfflineElectron,
                                                                               chain,
                                                                               m_zeeMaxMatchDistance );
            m_pEOther->push_back( otherOfflineElectron->pt() );
            m_matchOther->push_back( otherMatchWithinDistance );
         }
      }
   }

   return StatusCode::SUCCESS;
}
