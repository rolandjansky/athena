/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETCnvAlg.cxx 592539 2014-04-11 10:17:29Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"

// Local include(s):
#include "TrigMissingETCnvAlg.h"

namespace xAODMaker {

   TrigMissingETCnvAlg::TrigMissingETCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigMissingETCnvTool/TrigMissingETCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "HLT_TrigEFMissingET" );
      declareProperty( "xAODKey", m_xaodKey = "HLT_TrigEFMissingET" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigMissingETCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigMissingETCnvAlg::execute() {

      // Check if the input is available:
      if( ! evtStore()->contains< TrigMissingETContainer >( m_aodKey ) ) {
         ATH_MSG_WARNING( "No TrigMissingETContainer with key \""
                          << m_aodKey << "\" available" );
         return StatusCode::SUCCESS;
      }

      // Retrieve the AOD container:
      const TrigMissingETContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigMissingETAuxContainer* aux = new xAOD::TrigMissingETAuxContainer();
      xAOD::TrigMissingETContainer* xaod = new xAOD::TrigMissingETContainer();
      xaod->setStore( aux );

      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( aod, xaod ) );

      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
