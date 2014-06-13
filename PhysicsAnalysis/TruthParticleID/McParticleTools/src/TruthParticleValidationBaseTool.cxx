/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleValidationBaseTool.cxx 
// Implementation file for class TruthParticleValidationBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes

// CLHEP/HepMC includes
#include "GeneratorObjects/McEventCollection.h"

// McParticleTools includes
#include "TruthParticleValidationBaseTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
TruthParticleValidationBaseTool::TruthParticleValidationBaseTool( const std::string& type, 
								  const std::string& name, 
								  const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_nAnalysedEvents ( 0 )
{
  //
  // Property declaration
  // 

  declareInterface<ITruthParticleValidationTool>(this);
}

/// Destructor
///////////////
TruthParticleValidationBaseTool::~TruthParticleValidationBaseTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode TruthParticleValidationBaseTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // Give the concrete (derived) tool a chance to initialize itself
  if ( initializeTool().isFailure() ) {
    ATH_MSG_ERROR("Could not initialize concrete tool !");
    return StatusCode::FAILURE;
  }

  // initializing the event counter
  m_nAnalysedEvents = 0;
  return StatusCode::SUCCESS;
}

StatusCode TruthParticleValidationBaseTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  ATH_MSG_INFO("Analysed [" << m_nAnalysedEvents << "] events.");

  // Give the concrete (derived) tool a chance to finalize itself
  if ( finalizeTool().isFailure() ) {
    ATH_MSG_ERROR("Could not finalize concrete tool !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TruthParticleValidationBaseTool::execute()
{
  ++m_nAnalysedEvents;
  return executeTool();
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

