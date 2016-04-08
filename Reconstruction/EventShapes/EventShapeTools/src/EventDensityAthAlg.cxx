///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventDensityAthAlg.cxx 
// Implementation file for class EventDensityAthAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// EventShapeTools includes
#include "EventDensityAthAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
EventDensityAthAlg::EventDensityAthAlg( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_densityTool(this)
{
  //
  // Property declaration
  // 
  declareProperty( "EventDensityTool", m_densityTool );

}

// Destructor
///////////////
EventDensityAthAlg::~EventDensityAthAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode EventDensityAthAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_densityTool.retrieve()  );
  return StatusCode::SUCCESS;
}

StatusCode EventDensityAthAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode EventDensityAthAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  StatusCode sc = m_densityTool->fillEventShape();
  if(! sc.isSuccess() ) {
    ATH_MSG_ERROR(" Error while computing density with tool "<< m_densityTool->name() );
    return StatusCode::RECOVERABLE;
  }
  return StatusCode::SUCCESS;
}



