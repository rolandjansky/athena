/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old egamma Includes:
#include "egammaEvent/ElectronContainer.h"

//New egamma
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"

// Local include(s):
#include "ElectronCnvAlg.h"


namespace xAODMaker {

  ElectronCnvAlg::ElectronCnvAlg( const std::string& name,
				  ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ) {

    declareProperty( "AODContainerName", m_aodContainerName = "ElectronAODCollection" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "ElectronCollection" );
    declareProperty( "xAODContainerFrwdName", m_xaodFrwdContainerName = "FwdElectrons" );

    declareProperty("CnvTool", m_cnvTool, "The converter tool for Electrons");
  }

  StatusCode ElectronCnvAlg::initialize() {

    ATH_MSG_DEBUG( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_DEBUG( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_DEBUG( "xAODContainerName = " << m_xaodContainerName );
    ATH_MSG_DEBUG( "xAODContainerFrwdName = " << m_xaodFrwdContainerName );

    CHECK(m_cnvTool.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode ElectronCnvAlg::execute() {
    
    // Retrieve the AOD particles:
    const egammaContainer* aod = evtStore()->tryConstRetrieve<egammaContainer>(m_aodContainerName); 
    if (!aod) { 
      ATH_MSG_WARNING("No egammaContainer with key " << m_aodContainerName << " found. Do nothing."); 
      return StatusCode::SUCCESS; 
    } 
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );
    
    /////////////////////////////////////////////////////////////////////////////////////////
    
    // Create the xAOD container and its auxiliary store:
    xAOD::ElectronContainer* xaod = new xAOD::ElectronContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::ElectronAuxContainer* aux = new xAOD::ElectronAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_xaodContainerName );


    //Create the container for forward electrons
    xAOD::ElectronContainer* xaodFrwd = new xAOD::ElectronContainer();
    CHECK( evtStore()->record( xaodFrwd, m_xaodFrwdContainerName ) );
    xAOD::ElectronAuxContainer* auxFrwd = new xAOD::ElectronAuxContainer();
    CHECK( evtStore()->record( auxFrwd, m_xaodFrwdContainerName + "Aux." ) );
    xaodFrwd->setStore( auxFrwd );
    ATH_MSG_DEBUG( "Recorded Electrons with key: " << m_xaodFrwdContainerName );

    CHECK( m_cnvTool->convert(aod, xaod, xaodFrwd) );
    // Return gracefully - like a elephant on roller skates :
    return StatusCode::SUCCESS;
  }
  
} // namespace xAODMaker

