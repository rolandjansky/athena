/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTrackCountsCnvAlg.cxx 603353 2014-06-24 12:36:04Z azemla $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCountsAuxContainer.h"

// Local include(s):
#include "TrigTrackCountsCnvAlg.h"

namespace xAODMaker {

   TrigTrackCountsCnvAlg::TrigTrackCountsCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigTrackCountsCnvTool/TrigTrackCountsCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigTrackCounts" );
      declareProperty( "xAODKey", m_xaodKey = "xAODTrigTrackCounts" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigTrackCountsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigTrackCountsCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigTrackCountsCollection* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigTrackCountsAuxContainer* aux = new xAOD::TrigTrackCountsAuxContainer();
      xAOD::TrigTrackCountsContainer* xaod = new xAOD::TrigTrackCountsContainer();
      
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
