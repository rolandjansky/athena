/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigSpacePointCountsCnvAlg.cxx 575028 2013-12-11 14:30:06Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h"

// Local include(s):
#include "TrigSpacePointCountsCnvAlg.h"

namespace xAODMaker {

   TrigSpacePointCountsCnvAlg::TrigSpacePointCountsCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigSpacePointCountsCnvTool/TrigSpacePointCountsCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigSpacePointCounts" );
      declareProperty( "xAODKey", m_xaodKey = "xAODTrigSpacePointCounts" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigSpacePointCountsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigSpacePointCountsCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigSpacePointCountsCollection* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigSpacePointCountsAuxContainer* aux = new xAOD::TrigSpacePointCountsAuxContainer();
      xAOD::TrigSpacePointCountsContainer* xaod = new xAOD::TrigSpacePointCountsContainer();
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
