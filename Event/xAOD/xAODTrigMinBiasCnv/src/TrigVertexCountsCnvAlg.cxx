/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigVertexCountsCnvAlg.cxx 603353 2014-06-24 12:36:04Z azemla $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigInDetEvent/TrigVertexCountsCollection.h"
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCountsAuxContainer.h"

// Local include(s):
#include "TrigVertexCountsCnvAlg.h"

namespace xAODMaker {

   TrigVertexCountsCnvAlg::TrigVertexCountsCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigVertexCountsCnvTool/TrigVertexCountsCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigVertexCounts" );
      declareProperty( "xAODKey", m_xaodKey = "xAODTrigVertexCounts" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigVertexCountsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigVertexCountsCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigVertexCountsCollection* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigVertexCountsAuxContainer* aux = new xAOD::TrigVertexCountsAuxContainer();
      xAOD::TrigVertexCountsContainer* xaod = new xAOD::TrigVertexCountsContainer();
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
