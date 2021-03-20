/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerCnvAlg.cxx 576052 2013-12-18 09:13:50Z morrisj $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigT1CaloEvent/CPMTowerCollection.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

// Local include(s):
#include "CPMTowerCnvAlg.h"

namespace xAODMaker {

   CPMTowerCnvAlg::CPMTowerCnvAlg( const std::string& name,
                                   ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::CPMTowerCnvTool/CPMTowerCnvTool", this ) {

      declareProperty( "ESDKey" , m_esdKey  = "CPMTowers" );
      declareProperty( "xAODKey", m_xaodKey = "CPMTowers" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode CPMTowerCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( " ESD Key: " << m_esdKey );
      ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );

      // Retrieve the converter tool:
      CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode CPMTowerCnvAlg::execute() {
      
      // Retrieve the ESD container:
      const CPMTowerCollection* esd = nullptr;
      CHECK( evtStore()->retrieve( esd, m_esdKey ) );
      
      // Create the xAOD container and its auxiliary store:
      xAOD::CPMTowerAuxContainer* aux = new xAOD::CPMTowerAuxContainer();
      xAOD::CPMTowerContainer* xaod = new xAOD::CPMTowerContainer();
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
