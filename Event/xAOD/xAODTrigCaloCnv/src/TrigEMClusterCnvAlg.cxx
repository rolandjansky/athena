/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigEMClusterCnvAlg.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterAuxContainer.h"

// Local include(s):
#include "TrigEMClusterCnvAlg.h"

namespace xAODMaker {

   TrigEMClusterCnvAlg::TrigEMClusterCnvAlg( const std::string& name,
					     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigEMClusterCnvTool/TrigEMClusterCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "HLT_TrigT2CaloEgamma" );
      declareProperty( "xAODKey", m_xaodKey = "HLT_TrigT2CaloEgamma" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigEMClusterCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigEMClusterCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigEMClusterContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigEMClusterAuxContainer* aux = new xAOD::TrigEMClusterAuxContainer();
      xAOD::TrigEMClusterContainer* xaod = new xAOD::TrigEMClusterContainer();
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
