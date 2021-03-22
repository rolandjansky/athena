/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCaloClusterCnvAlg.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigCaloEvent/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigCaloClusterAuxContainer.h"

// Local include(s):
#include "TrigCaloClusterCnvAlg.h"

namespace xAODMaker {

   TrigCaloClusterCnvAlg::TrigCaloClusterCnvAlg( const std::string& name,
						 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigCaloClusterCnvTool/TrigCaloClusterCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "HLT_TrigT2Calo" );
      declareProperty( "xAODKey", m_xaodKey = "HLT_TrigT2Calo" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigCaloClusterCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigCaloClusterCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigCaloClusterContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigCaloClusterAuxContainer* aux = new xAOD::TrigCaloClusterAuxContainer();
      xAOD::TrigCaloClusterContainer* xaod = new xAOD::TrigCaloClusterContainer();
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
