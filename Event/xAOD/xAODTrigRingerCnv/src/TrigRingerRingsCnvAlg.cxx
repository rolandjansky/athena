/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRingsCnvAlg.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"

// Local include(s):
#include "TrigRingerRingsCnvAlg.h"

namespace xAODMaker {

   TrigRingerRingsCnvAlg::TrigRingerRingsCnvAlg( const std::string& name,
						 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigRingerRingsCnvTool/TrigRingerRingsCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigT2CaloEgamma" );
      declareProperty( "xAODKey", m_xaodKey = "TrigT2CaloEgamma" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigRingerRingsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigRingerRingsCnvAlg::execute() {

      // Retrieve the AOD container:
      const RingerRingsContainer* aod = 0;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigRingerRingsAuxContainer* aux = new xAOD::TrigRingerRingsAuxContainer();
      xAOD::TrigRingerRingsContainer* xaod = new xAOD::TrigRingerRingsContainer();
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
