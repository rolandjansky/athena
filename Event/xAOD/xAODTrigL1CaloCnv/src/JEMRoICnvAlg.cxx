/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMRoICnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/JEMRoICollection.h"
#include "xAODTrigL1Calo/JEMRoIContainer.h"
#include "xAODTrigL1Calo/JEMRoIAuxContainer.h"

// Local include(s):
#include "JEMRoICnvAlg.h"

namespace xAODMaker {

   JEMRoICnvAlg::JEMRoICnvAlg( const std::string& name,
                               ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::JEMRoICnvTool/JEMRoICnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "JEMRoIs" );
      declareProperty( "xAODKey", m_xaodKey = "JEMRoIs" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode JEMRoICnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode JEMRoICnvAlg::execute() {
      
      // Retrieve the ESD container:
      const JEMRoICollection* esd = nullptr;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::JEMRoIAuxContainer* aux = new xAOD::JEMRoIAuxContainer();
      xAOD::JEMRoIContainer* xaod = new xAOD::JEMRoIContainer();
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
