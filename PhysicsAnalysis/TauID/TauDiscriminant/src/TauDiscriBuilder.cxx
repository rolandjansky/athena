/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDiscriBuilder.cxx
// package:     PhysicsAnalysis/TauID/TauDiscriminant
// authors:     M. Wolter, A. Kaczmarska, Noel Dawe
// date:        13 March 2008
//-----------------------------------------------------------------------------

#include "TauDiscriminant/TauDiscriBuilder.h"
#include "TauDiscriminant/TauDiscriToolBase.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJet.h"
#include "xAODTau/TauDefs.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"

#include "TauDiscriminant/TauDiscriminantProcessor.h"
#include "CxxUtils/make_unique.h"
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauDiscriBuilder::TauDiscriBuilder( const std::string &name,
		ISvcLocator * pSvcLocator ) :
    		AthAlgorithm( name, pSvcLocator ),
    		m_tauInputContainerName( "TauContainer" ),
    		m_tools( this ) //make tools private
{
	declareProperty( "container", m_tauInputContainerName );
	declareProperty( "tools", m_tools, "List of TauDiscriToolBase tools" );
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauDiscriBuilder::~TauDiscriBuilder()
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::initialize()
{
  m_tauIDTool = CxxUtils::make_unique<TauDiscriminantProcessor>("TauIDTool");

  //TODO: set via property interface, for some reason that doesn't work at the moment
  //if( m_tauIDTool->setProperty("tools", m_tools).isFailure() ){
  //  ATH_MSG_ERROR("Couldn't set tools property of TauDiscriminantProcessor");
  //  return StatusCode::FAILURE;
  //}
  m_tauIDTool->setToolArray(m_tools);
  m_tauIDTool->msg().setLevel( this->msg().level() );

  if( m_tauIDTool->initialize().isFailure() ){
    ATH_MSG_ERROR("Unsuccessful initialization of TauDiscriminantProcessor.");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::finalize()
{
	return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauDiscriBuilder::execute()
{
	xAOD::TauJetContainer  *tau_container;
	const xAOD::TauJetContainer  *const_tau_container;
	StatusCode sc = evtStore()->retrieve(const_tau_container, m_tauInputContainerName);
	tau_container = const_cast<xAOD::TauJetContainer *>(const_tau_container);

	if (sc.isFailure() || ! tau_container)
	{
		ATH_MSG_WARNING("No input tau container found!");
		return StatusCode::SUCCESS;
	}
	ATH_MSG_VERBOSE("Processing input tau Container Name = " << m_tauInputContainerName);

	if( m_tauIDTool->eventInitialize().isFailure() ){
	  ATH_MSG_ERROR("Error in event initialization of TauDiscriminantProcessor");
	  return StatusCode::FAILURE;
	}
	
	for(auto tau: *tau_container){
	  if(! (m_tauIDTool->applyDiscriminant(*tau) == CP::CorrectionCode::Ok)){
	    ATH_MSG_FATAL("TauDiscriminant failed to execute all tools");
	    return StatusCode::FAILURE;
	  }
	}

	return StatusCode::SUCCESS;
}
