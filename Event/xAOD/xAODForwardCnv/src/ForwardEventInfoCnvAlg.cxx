/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old mbts (tile) info!
#include "TileEvent/MBTSCollisionTime.h"

//New egamma
#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODForward/ForwardEventInfoAuxContainer.h"

// Local include(s):
#include "ForwardEventInfoCnvAlg.h"


namespace xAODMaker {

  ForwardEventInfoCnvAlg::ForwardEventInfoCnvAlg( const std::string& name,
				  ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),m_cnvTool( "xAODMaker::ForwardEventInfoCnvTool/ForwardEventInfoCnvTool", this ) {

    declareProperty( "AODContainerName", m_aodContainerName = "MBTSCollisionTime" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "MBTSForwardEventInfo" );

    declareProperty("CnvTool", m_cnvTool, "The converter tool for MBTSForwardEventInfo");
  }

  StatusCode ForwardEventInfoCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
    ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );

    CHECK(m_cnvTool.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode ForwardEventInfoCnvAlg::execute() {
    
    // Retrieve the AOD particles:
    const MBTSCollisionTime* aod = 0;
    CHECK( evtStore()->retrieve(aod, m_aodContainerName) ); 
    if (!aod) { 
      ATH_MSG_WARNING("No ForwardEventInfoCollection with key " << m_aodContainerName << " found. Do nothing."); 
      return StatusCode::SUCCESS; 
    } 
    ATH_MSG_INFO( "Retrieved particles with key: " << m_aodContainerName );
    
    /////////////////////////////////////////////////////////////////////////////////////////
    
    // Create the xAOD container and its auxiliary store:
    xAOD::ForwardEventInfoContainer* xaod = new xAOD::ForwardEventInfoContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::ForwardEventInfoAuxContainer* aux = new xAOD::ForwardEventInfoAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_INFO( "Recorded ForwardEventInfos with key: " << m_xaodContainerName );

    CHECK( m_cnvTool->convert(aod, xaod) );
    // Return gracefully - like a elephant on roller skates :
    return StatusCode::SUCCESS;
  }
  
} // namespace xAODMaker

