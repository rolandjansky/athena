/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBjetCnvAlg.cxx 619848 2014-10-03 13:59:50Z lidiaz $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigParticle/TrigEFBjet.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"

// Local include(s):
#include "TrigBjetCnvAlg.h"

namespace xAODMaker {

   TrigBjetCnvAlg::TrigBjetCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigBjetCnvTool/TrigBjetCnvTool", this ) {

     //      declareProperty( "AODKey", m_aodKey = "TrigEFBjetContainer" );
      declareProperty( "AODKey", m_aodKey = "HLT_EFBjetFex" );
      declareProperty( "xAODKey", m_xaodKey = "TrigBjet" ); // ? Lidija
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigBjetCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigBjetCnvAlg::execute() {

      // Retrieve the AOD container:
     const TrigEFBjetContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::BTaggingAuxContainer* aux = new xAOD::BTaggingAuxContainer();
      xAOD::BTaggingContainer* xaod = new xAOD::BTaggingContainer();
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
