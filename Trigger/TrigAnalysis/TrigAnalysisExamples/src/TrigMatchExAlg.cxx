/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMatchExAlg.cxx 772674 2016-09-12 09:12:29Z rwhite $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"

// matching tool include
#include "TriggerMatchingTool/IMatchingTool.h"

// Local include(s):
#include "TrigMatchExAlg.h"

TrigMatchExAlg::TrigMatchExAlg( const std::string& name, ISvcLocator *pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
     m_matchTool( "Trig::MatchingTool/MyMatchingTool" ) {

   // job option configurable properties

   // The matching tool
   declareProperty("TrigMatchTool", m_matchTool);

   // AOD Keys
   declareProperty("MuonContainerKey", m_muonContainerKey = "Muons");
   declareProperty("ElectronContainerKey", m_electronContainerKey = "Electrons");
   declareProperty("TauJetContainerKey", m_tauContainerKey = "Taus");
   declareProperty("PhotonContainerKey", m_photonContainerKey = "Photons");
   declareProperty("JetCollectionKey", m_jetContainerKey = "Jets");

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

       std::vector<std::string> matchingChains = {"HLT_e26_lhtight_nod0_ivarloose"};
       // load the electron container from store gate
       const xAOD::ElectronContainer *electronContainer = 0;
       CHECK( evtStore()->retrieve( electronContainer, m_electronContainerKey ) );

       // loop through reconstructed electrons, and print out trigger information
       for( xAOD::ElectronContainer::const_iterator iter = electronContainer->begin();
               iter != electronContainer->end(); ++iter ) {

           // print the info for the electron
           ATH_MSG_INFO( "Have reconstructed electron with eta, phi, pt: " 
                   << (*iter)->eta() << ", " << (*iter)->phi() << ", "
                   << (*iter)->pt() );

           // do the EF matching
           if( m_matchEF ) {

               // first, figure out the chains where it matches something.
               // Note that we can specify the level using the typedefs
               // TrigObjectMatching/TrigMatchToolCore.h.  
               // Updates to TriggerMatchingTool:
               // Use TrigDefs to match to all TEs or only active
               // Use typedefs as in TrigObjectMatching
               // Alternatively,
               // we could specify whatever trigger object we want to match
               // with.
               // 
               // Return best match object
               // Return the metric
               // Return sorted list of matched objects 
               // Use custom metric
               //   For example: const ElectronClusterMetric metric;
               // Return the TriggerElement of the the trigger object
               // Match to Selected objects or All objects (TrigDefs::alsoDeactivateTEs
               
               
               // Following gets all configured chains
               // returns the matching chains to object type 
               //std::vector< std::string > matchingChains =
               //m_matchTool->chainsPassedByObject< TrigMatch::TrigElectronEF >( *iter );

               // now, figure out what the match was and print its info
               for( std::vector< std::string >::const_iterator chainIter = matchingChains.begin();
                       chainIter != matchingChains.end(); ++chainIter ) {
                   
                   // TrigObjectMatching returns best match and set whether match: true or false
                   //const TrigMatch::TrigElectronL1 *match =
                   //    m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL1 >( *iter,*chainIter,0.1,true );
                   // also can return metric:                                                                                     
                   // const TrigMatch::TrigElectronL1 *match =
                   //    m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL1 >( *iter,*chainIter,0.1,true,&metric );
                   // also equivalent to returning the distance:   
                   // float dist = m_matchTool->distance(*iter, match);                                                                                     
                   if(m_matchTool->match( {*iter},*chainIter,0.1))
                       ATH_MSG_INFO( "EF match in chain " << *chainIter); 
                   // print something if we didn't find any matches
                   else
                       ATH_MSG_INFO( "Does not match to HLT trigger" );

               }
           }
       }
   }
   return StatusCode::SUCCESS;
}
