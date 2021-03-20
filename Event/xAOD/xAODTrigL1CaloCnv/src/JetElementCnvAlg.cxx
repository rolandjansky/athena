/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetElementCnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/JetElementCollection.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"

// Local include(s):
#include "JetElementCnvAlg.h"

namespace xAODMaker {

   JetElementCnvAlg::JetElementCnvAlg( const std::string& name,
                                       ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::JetElementCnvTool/JetElementCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "JetElements" );
      declareProperty( "xAODKey", m_xaodKey = "JetElements" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode JetElementCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JetElementCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const JetElementCollection* esd = nullptr;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::JetElementAuxContainer* aux = new xAOD::JetElementAuxContainer();
      xAOD::JetElementContainer* xaod = new xAOD::JetElementContainer();
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
