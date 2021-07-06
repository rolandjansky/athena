/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "egammaEvent/PhotonContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"

// Local include(s):
#include "PhotonCnvAlg.h"


namespace xAODMaker {

  PhotonCnvAlg::PhotonCnvAlg( const std::string& name,
			      ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ) {

    declareProperty( "AODContainerName", m_aodContainerName = "PhotonAODCollection" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "PhotonCollection" );
    
    declareProperty("CnvTool", m_cnvTool, "The converter tool for Photons");
    
  }

  StatusCode PhotonCnvAlg::initialize() {

    ATH_MSG_DEBUG( "Initializing" );
    ATH_MSG_DEBUG( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_DEBUG( "xAODContainerName = " << m_xaodContainerName );

    CHECK(m_cnvTool.retrieve());
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode PhotonCnvAlg::execute() {

    // Retrieve the AOD particles:
    const egammaContainer* aod = evtStore()->tryConstRetrieve<egammaContainer>(m_aodContainerName); 
    if (!aod) { 
      ATH_MSG_WARNING("No egammaContainer with key " << m_aodContainerName << " found. Do nothing."); 
      return StatusCode::SUCCESS; 
    }
    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );
    
    // Create the xAOD container and its auxiliary store:
    xAOD::PhotonContainer* xaod = new xAOD::PhotonContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::PhotonAuxContainer* aux = new xAOD::PhotonAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded Photons with key: " << m_xaodContainerName );

    CHECK( m_cnvTool->convert(aod, xaod) );
    // Return gracefully - like a elephant on roller skates :
    return StatusCode::SUCCESS;
  }
  
} // namespace xAODMaker


//  LocalWords:  Gaudi
