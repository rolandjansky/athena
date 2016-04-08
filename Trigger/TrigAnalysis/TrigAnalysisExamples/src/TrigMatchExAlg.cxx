/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMatchExAlg.cxx 723145 2016-02-10 16:06:08Z ssnyder $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// EDM includes

// L1 includes
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"

// L2 includes
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"

// EF + offline includes
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "muonEvent/MuonContainer.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/JetCollection.h"

// matching tool include
#include "TrigObjectMatching/TrigMatchTool.h"

// Local include(s):
#include "TrigMatchExAlg.h"
#include "ElectronClusterMetric.h"

TrigMatchExAlg::TrigMatchExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
     m_matchTool( "TrigMatchTool/TrigMatchTool" ) {

   // job option configurable properties

   // The matching tool
   declareProperty("TrigMatchTool", m_matchTool);

   // AOD Keys
   declareProperty("MuonContainerKey", m_muonContainerKey = "StacoMuonCollection");
   declareProperty("ElectronContainerKey", m_electronContainerKey = "ElectronAODCollection");
   declareProperty("TauJetContainerKey", m_tauContainerKey = "TauRecContainer");
   declareProperty("PhotonContainerKey", m_photonContainerKey = "PhotonAODCollection");
   declareProperty("JetCollectionKey", m_jetContainerKey = "Cone4H1TowerJets");

   // User flags
   declareProperty("MatchMuons", m_matchMuons = true);
   declareProperty("MatchElectrons", m_matchElectrons = true);
   declareProperty("MatchPhotons", m_matchPhotons = true);
   declareProperty("MatchJets", m_matchJets = true);
   declareProperty("MatchTaus", m_matchTaus = true);
   declareProperty("MatchClusterElectrons", m_matchClusterElectrons = true);

   declareProperty("MatchL1", m_matchL1 = true);
   declareProperty("MatchL2", m_matchL2 = true);
   declareProperty("MatchEF", m_matchEF = true);
}

StatusCode TrigMatchExAlg::initialize() {

   REPORT_MESSAGE( MSG::DEBUG ) << "TrigMatchExAlg::Intialize()";

   // load the matching tool
   if( ! m_matchTool.empty() ) {
      CHECK( m_matchTool.retrieve() );
      ATH_MSG_INFO( "Successfully retrived the TrigMatchTool!" );
   } else {
      REPORT_ERROR( StatusCode::FAILURE ) << "Could not retrive the TrigMatchTool as it "
                                          << "was not specified!";
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode TrigMatchExAlg::execute() {

   REPORT_MESSAGE( MSG::DEBUG ) << "In TrigMatchExAlg::Execute()";

   /*************************************************/
   /*         Trigger Matching Example              */
   /*************************************************/

   // The following code illustrates how to use the trigger matching
   // tools for each of the major reconstruction types.  A large
   // portion of the code is the same for each type, but is repeated
   // so that you can go straight to the object you are interested in

   // First, are five simple examples using the default deltaR
   // based matching:
   // Electron
   // Muon
   // Photon
   // Tau
   // Jet

   // Folowing these is a more detailed example that shows how to use
   // a custom metric to match on something other than the deltaR
   // of the object.  

   if( m_matchElectrons ) {

      /***********************************************/
      /*               Electron Example              */
      /***********************************************/
      // In this example, we grab all reconstructed electrons and match
      // them to L1, L2, and EF trigger objects and print their
      // properties.

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Electrons...." );
      ATH_MSG_INFO( "" );

      // load the electron container from store gate
      const ElectronContainer *electronContainer = 0;
      CHECK( evtStore()->retrieve( electronContainer, m_electronContainerKey ) );

      // loop through reconstructed electrons, and print out trigger information
      for( ElectronContainer::const_iterator iter = electronContainer->begin();
           iter != electronContainer->end(); ++iter ) {

         // print the info for the electron
         ATH_MSG_INFO( "Have reconstructed electron with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", "
                       << (*iter)->pt() );

         // do level 1 matching
         if( m_matchL1 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigElectronL1 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in L1 items, so lets skip the others
               if( chainIter->find( "L1_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigElectronL1 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL1 >( *iter,
                                                                                  *chainIter,
                                                                                  0.1,
                                                                                  true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L1 match in item " << *chainIter << " with eta, phi, pt: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L1 trigger" );
            }

         }

         // do level 2 matching
         if( m_matchL2 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigElectronL2 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in l2 chains, so only print those
               if( chainIter->find( "L2_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigElectronL2 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL2 >( *iter,
                                                                                  *chainIter,
                                                                                  0.1,
                                                                                  true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L2 trigger" );
            }

         }

         // do the EF matching
         if( m_matchEF ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigElectronEF >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               const TrigMatch::TrigElectronEF *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronEF >( *iter,
                                                                                  *chainIter,
                                                                                  0.1,
                                                                                  true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "EF match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to EF trigger" );
            }

         }

      }
   }

   if( m_matchMuons ) {

      /***********************************************/
      /*               Muon Example              */
      /***********************************************/
      // In this example, we grab all reconstructed muons and match
      // them to L1, L2, and EF trigger objects and print their
      // properties.

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Muons...." );
      ATH_MSG_INFO( "" );

      // load the muon container from store gate
      const Analysis::MuonContainer *muonContainer = 0;
      CHECK( evtStore()->retrieve( muonContainer, m_muonContainerKey ) );

      // loop through reconstructed muons, and print out trigger information
      for( Analysis::MuonContainer::const_iterator iter = muonContainer->begin();
           iter != muonContainer->end(); ++iter ) {

         // print the info for the muon
         ATH_MSG_INFO( "Have reconstructed muon with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", " << (*iter)->pt() );

         // do level 1 matching
         if( m_matchL1 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigMuonL1 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in L1 items, so lets skip the others
               if( chainIter->find( "L1_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigMuonL1 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigMuonL1 >( *iter,
                                                                              *chainIter,
                                                                              0.15,
                                                                              true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L1 match in item " << *chainIter << " with eta, phi, pt: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L1 trigger" );
            }

         }

         // do level 2 matching
         if( m_matchL2 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigMuonL2 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in l2 chains, so only print those
               if( chainIter->find( "L2_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigMuonL2 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigMuonL2 >( *iter,
                                                                              *chainIter,
                                                                              0.1,
                                                                              true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L2 trigger" );
            }

         }

         // do the EF matching
         if( m_matchEF ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMuonEFInfo >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector<std::string>::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               const TrigMuonEFInfo *match = 
                  m_matchTool->matchToTriggerObject< TrigMuonEFInfo >( *iter,
                                                                       *chainIter,
                                                                       0.1,
                                                                       true );

               // if the match is good, print it out!
               if( match ) {
                  // get the track from the TrigMuonEFInfo object
                  const TrigMuonEFTrack *track = 0;
                  if( match->hasLegacyTrack() )
                     track = match->legacySpectrometerTrack();
                  else if( match->TrackContainer() && match->TrackContainer()->size() > 0 ) {
                     const TrigMuonEFInfoTrack *infoTrack = match->TrackContainer()->at( 0 );
                     if( infoTrack->hasSpectrometerTrack() )
                        track = infoTrack->SpectrometerTrack();
                     else if( infoTrack->hasExtrapolatedTrack() )
                        track = infoTrack->ExtrapolatedTrack();
                     else if( infoTrack->hasCombinedTrack() )
                        track = infoTrack->CombinedTrack();
                  }
                  if( track ) {
                     hasMatch = true;
                     ATH_MSG_INFO( "EF match in chain " << *chainIter << " with eta, phi: "
                                   << track->eta() << ", " << track->phi() << ", "
                                   << track->pt() );
                  }
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to EF trigger" );
            }

         }
      }
   }

   if( m_matchPhotons ) {

      /***********************************************/
      /*               Photon Example              */
      /***********************************************/
      // In this example, we grab all reconstructed photons and match
      // them to L1, L2, and EF trigger objects and print their
      // properties.

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Photons...." );
      ATH_MSG_INFO( "" );

      // load the photon container from store gate
      const PhotonContainer *photonContainer = 0;
      CHECK( evtStore()->retrieve( photonContainer, m_photonContainerKey ) );

      // loop through reconstructed photons, and print out trigger information
      for( PhotonContainer::const_iterator iter = photonContainer->begin();
           iter != photonContainer->end(); ++iter ) {

         // print the info for the photon
         ATH_MSG_INFO( "Have reconstructed photon with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", " << (*iter)->pt() );

         // do level 1 matching
         if( m_matchL1 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigPhotonL1 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in L1 items, so lets skip the others
               if( chainIter->find( "L1_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigPhotonL1 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonL1 >( *iter,
                                                                                *chainIter,
                                                                                0.1,
                                                                                true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L1 match in item " << *chainIter << " with eta, phi, pt: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L1 trigger" );
            }

         }

         // do level 2 matching
         if( m_matchL2 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigPhotonL2 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in l2 chains, so only print those
               if( chainIter->find( "L2_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigPhotonL2 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonL2 >( *iter,
                                                                                *chainIter,
                                                                                0.1,
                                                                                true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L2 trigger" );
            }

         }

         // do the EF matching
         if( m_matchEF ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigPhotonEF >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               const TrigMatch::TrigPhotonEF *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonEF >( *iter,
                                                                                *chainIter,
                                                                                0.1,
                                                                                true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "EF match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to EF trigger" );
            }
         }
      }
   }

   if( m_matchTaus ) {

      /***********************************************/
      /*               Tau Example              */
      /***********************************************/
      // In this example, we grab all reconstructed taus and match
      // them to L1, L2, and EF trigger objects and print their
      // properties.

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Taus...." );
      ATH_MSG_INFO( "" );

      // load the tau container from store gate
      const Analysis::TauJetContainer *tauContainer = 0;
      CHECK( evtStore()->retrieve( tauContainer, m_tauContainerKey ) );

      // loop through reconstructed taus, and print out trigger information
      for( Analysis::TauJetContainer::const_iterator iter = tauContainer->begin();
           iter != tauContainer->end(); ++iter ) {

         // print the info for the tau
         ATH_MSG_INFO( "Have reconstructed tau with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", " << (*iter)->pt() );

         // do level 1 matching
         if( m_matchL1 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigTauL1 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in L1 items, so lets skip the others
               if( chainIter->find( "L1_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigTauL1 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigTauL1 >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L1 match in item " << *chainIter << " with eta, phi, pt: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L1 trigger" );
            }

         }

         // do level 2 matching
         if( m_matchL2 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigTauL2 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in l2 chains, so only print those
               if( chainIter->find( "L2_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigTauL2 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigTauL2 >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L2 trigger" );
            }

         }

         // do the EF matching
         if( m_matchEF ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigTauEF >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               const TrigMatch::TrigTauEF *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigTauEF >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "EF match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to EF trigger" );
            }

         }
      }
   }

   if( m_matchJets ) {

      /***********************************************/
      /*               Jet Example              */
      /***********************************************/
      // In this example, we grab all reconstructed jets and match
      // them to L1, L2, and EF trigger objects and print their
      // properties.

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Jets...." );
      ATH_MSG_INFO( "" );

      // load the jet container from store gate
      const JetCollection *jetContainer = 0;
      CHECK( evtStore()->retrieve( jetContainer, m_jetContainerKey ) );

      // loop through reconstructed jets, and print out trigger information
      for( JetCollection::const_iterator iter = jetContainer->begin();
           iter != jetContainer->end(); ++iter ) {

         // print the info for the jet
         ATH_MSG_INFO( "Have reconstructed jet with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", " << (*iter)->pt() );

         // do level 1 matching
         if( m_matchL1 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigJetL1 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in L1 items, so lets skip the others
               if( chainIter->find( "L1_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigJetL1 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigJetL1 >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L1 match in item " << *chainIter << " with eta, phi, pt: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L1 trigger" );
            }

         }

         // do level 2 matching
         if( m_matchL2 ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigJetL2 >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               // we're only interested in l2 chains, so only print those
               if( chainIter->find( "L2_" ) == std::string::npos )
                  continue;

               const TrigMatch::TrigJetL2 *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigJetL2 >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to L2 trigger" );
            }

         }

         // do the EF matching
         if( m_matchEF ) {

            // first, figure out the chains where it matches something.
            // Note that we can specify the level using the typedefs
            // TrigObjectMatching/TrigMatchToolCore.h.  Alternatively,
            // we could specify whatever trigger object we want to match
            // with
            std::vector< std::string > matchingChains =
               m_matchTool->chainsPassedByObject< TrigMatch::TrigJetEF >( *iter );

            // now, figure out what the match was and print its info
            bool hasMatch = false;
            for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                 chainIter != matchingChains.end(); ++chainIter ) {

               const TrigMatch::TrigJetEF *match = 
                  m_matchTool->matchToTriggerObject< TrigMatch::TrigJetEF >( *iter,
                                                                             *chainIter,
                                                                             0.1,
                                                                             true );

               // if the match is good, print it out!
               if( match ) {
                  hasMatch = true;
                  ATH_MSG_INFO( "EF match in chain " << *chainIter << " with eta, phi: "
                                << match->eta() << ", " << match->phi() << ", "
                                << match->pt() );
               }
            }
            // print something if we didn't find any matches
            if( ! hasMatch ) {
               ATH_MSG_INFO( "Does not match to EF trigger" );
            }
         }
      }
   }

   if( m_matchClusterElectrons ) {

      /***********************************************/
      /*          Advanced Electron Example          */
      /***********************************************/
      // In this example, we match reconstructed electrons
      // with L2 electrons of type TrigElectron using
      // a custom metric that does quality cuts on the 
      // trigger electron

      ATH_MSG_INFO( "" );
      ATH_MSG_INFO( "Matching Electrons with cluster metric...." );
      ATH_MSG_INFO( "" );

      // load the electron container from store gate
      const ElectronContainer *electronContainer = 0;
      CHECK( evtStore()->retrieve( electronContainer, m_electronContainerKey ) );

      const ElectronClusterMetric metric;

      // loop through reconstructed electrons, and print out trigger information
      for( ElectronContainer::const_iterator iter = electronContainer->begin();
           iter != electronContainer->end(); ++iter ) {

         // print the info for the electron
         ATH_MSG_INFO( "Have reconstructed electron with eta, phi, pt: " 
                       << (*iter)->eta() << ", " << (*iter)->phi() << ", " << (*iter)->pt() );

         // print the matching chains - note that we pass the custom metric and
         // set a cut off for matching
         std::vector< std::string > matchingChains =
            m_matchTool->chainsPassedByObject< TrigElectron >( *iter, 0.1, &metric );

         bool hasMatch = false;
         for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
              chainIter != matchingChains.end(); ++chainIter ) {

            // skip the non-L2 chains
            if( chainIter->find( "L2_" ) == std::string::npos )
               continue;

            // Now we ask for the best match in the chain.  Note that we pass
            // true for "onlyPassedFeatures" as we know that this electron
            // matches to a trigger electron that caused the chain to fire
            const TrigElectron *match = 
               m_matchTool->matchToTriggerObject< TrigElectron >( *iter,
                                                                  *chainIter,
                                                                  0.1,
                                                                  true,
                                                                  &metric );

            // Note that if we would like the distance to the best match, we can simply call
            // float dist = m_matchTool->distance(*iter, match);
            if( match ) {
               hasMatch = true;
               ATH_MSG_INFO( "L2 match in chain " << *chainIter << " with eta, phi, et: "
                             << match->eta() << ", " << match->phi() << ", "
                             << match->et() );
            }
         }
         if( ! hasMatch ) {
            ATH_MSG_INFO( "Does not match to trigger" );
         }
      }
   }

   return StatusCode::SUCCESS;
}
