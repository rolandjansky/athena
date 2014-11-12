/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRNNOutputCnvAlg.cxx  $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigCaloEvent/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"

// Local include(s):
#include "TrigRNNOutputCnvAlg.h"

namespace xAODMaker {

   TrigRNNOutputCnvAlg::TrigRNNOutputCnvAlg( const std::string& name,
						 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigRNNOutputCnvTool/TrigRNNOutputCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigRingerNeuralFex" );
      declareProperty( "xAODKey", m_xaodKey = "TrigRingerNeuralFex" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigRNNOutputCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigRNNOutputCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigRNNOutputContainer* aod = 0;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigRNNOutputAuxContainer* aux = new xAOD::TrigRNNOutputAuxContainer();
      xAOD::TrigRNNOutputContainer* xaod = new xAOD::TrigRNNOutputContainer();
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
