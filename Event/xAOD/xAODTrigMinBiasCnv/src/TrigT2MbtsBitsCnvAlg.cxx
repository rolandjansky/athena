/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2MbtsBitsCnvAlg.cxx 608072 2014-07-23 00:27:40Z azemla $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsContainer.h"
#include "xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h"

// Local include(s):
#include "TrigT2MbtsBitsCnvAlg.h"

namespace xAODMaker {

   TrigT2MbtsBitsCnvAlg::TrigT2MbtsBitsCnvAlg( const std::string& name,
                                 ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::TrigT2MbtsBitsCnvTool/TrigT2MbtsBitsCnvTool", this ) {

      declareProperty( "AODKey", m_aodKey = "TrigT2MbtsBits" );
      declareProperty( "xAODKey", m_xaodKey = "xAODTrigT2MbtsBits" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode TrigT2MbtsBitsCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigT2MbtsBitsCnvAlg::execute() {

      // Retrieve the AOD container:
      const TrigT2MbtsBitsContainer* aod = nullptr;
      CHECK( evtStore()->retrieve( aod, m_aodKey ) );

      // Create the xAOD container and its auxiliary store:
      xAOD::TrigT2MbtsBitsAuxContainer* aux = new xAOD::TrigT2MbtsBitsAuxContainer();
      xAOD::TrigT2MbtsBitsContainer* xaod = new xAOD::TrigT2MbtsBitsContainer();
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
