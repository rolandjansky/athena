/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old mbts (tile) info!
#include "TileEvent/TileCellContainer.h"

//New egamma
#include "xAODForward/MBTSModuleContainer.h"
#include "xAODForward/MBTSModuleAuxContainer.h"

// Local include(s):
#include "MBTSModuleCnvAlg.h"


namespace xAODMaker {

  MBTSModuleCnvAlg::MBTSModuleCnvAlg( const std::string& name,
				  ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),m_cnvTool( "xAODMaker::MBTSModuleCnvTool/MBTSModuleCnvTool", this ) {

    declareProperty( "AODContainerName", m_aodContainerName = "MBTSContainer" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "MBTSModules" );

    declareProperty("CnvTool", m_cnvTool, "The converter tool for MBTSModules");
  }

  StatusCode MBTSModuleCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );

    CHECK(m_cnvTool.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode MBTSModuleCnvAlg::execute() {
    
    // Retrieve the AOD particles:
    const TileCellContainer* aod = 0;
    CHECK( evtStore()->retrieve(aod, m_aodContainerName) ); 
    if (!aod) { 
      ATH_MSG_WARNING("No MBTSModuleCollection with key " << m_aodContainerName << " found. Do nothing."); 
      return StatusCode::SUCCESS; 
    } 
    ATH_MSG_INFO( "Retrieved particles with key: " << m_aodContainerName );
    
    /////////////////////////////////////////////////////////////////////////////////////////
    
    // Create the xAOD container and its auxiliary store:
    xAOD::MBTSModuleContainer* xaod = new xAOD::MBTSModuleContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::MBTSModuleAuxContainer* aux = new xAOD::MBTSModuleAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_INFO( "Recorded MBTSModules with key: " << m_xaodContainerName );

    CHECK( m_cnvTool->convert(aod, xaod) );
    // Return gracefully - like a elephant on roller skates :
    return StatusCode::SUCCESS;
  }
  
} // namespace xAODMaker

