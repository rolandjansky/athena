/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeaderCnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/RODHeaderCollection.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"

// Local include(s):
#include "RODHeaderCnvAlg.h"

namespace xAODMaker {

   RODHeaderCnvAlg::RODHeaderCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::RODHeaderCnvTool/RODHeaderCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "RODHeaders" );
      declareProperty( "xAODKey", m_xaodKey = "RODHeaders" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode RODHeaderCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode RODHeaderCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const RODHeaderCollection* esd = nullptr;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::RODHeaderAuxContainer* aux = new xAOD::RODHeaderAuxContainer();
      xAOD::RODHeaderContainer* xaod = new xAOD::RODHeaderContainer();
      xaod->setStore( aux );
      
      // Fill the xAOD container:
      CHECK( m_cnvTool->convert( esd, xaod ) );     
      
      // Record the xAOD containers:
      CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      CHECK( evtStore()->record( xaod, m_xaodKey ) );

      // Return gracefully:
      return StatusCode::SUCCESS;     
   }

} // namespace xAODMaker
