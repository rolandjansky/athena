/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJetReaderAlg.cxx 632174 2014-11-29 15:23:36Z krasznaa $

// EDM include(s):
#include "xAODTau/TauJetContainer.h"

// Local include(s):
#include "TauJetReaderAlg.h"

namespace xAODMaker {

   TauJetReaderAlg::TauJetReaderAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : ::AthAlgorithm( name, svcLoc ) {

      declareProperty( "SGKey", m_sgKey = "TauRecContainer",
                       "StoreGate key of the container to test" );
   }

   StatusCode TauJetReaderAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "SGKey = " << m_sgKey );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TauJetReaderAlg::execute() {

      // Retrieve the tau container:
      const xAOD::TauJetContainer* taus = nullptr;
      ATH_CHECK( evtStore()->retrieve( taus, m_sgKey ) );

      // Print some info about them:
      ATH_MSG_INFO( "Number of tau jets: " << taus->size() );

      // Loop over them, printing some info about each of them:
      for( const xAOD::TauJet* tau : *taus ) {

         // Print its 4-momentum:
         ATH_MSG_DEBUG( "  pt = " << tau->pt() << ", eta = " << tau->eta()
                        << ", phi = " << tau->phi() << ", m = " << tau->m() );

      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
