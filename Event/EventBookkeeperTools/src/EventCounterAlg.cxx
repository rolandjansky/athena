///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventCounterAlg.cxx
// Implementation file for class EventCounterAlg
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////

// EventBookkeeperTools includes
#include "EventCounterAlg.h"

// STL includes

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"



///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
EventCounterAlg::EventCounterAlg( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  //
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
EventCounterAlg::~EventCounterAlg()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode EventCounterAlg::initialize()
{
  ATH_MSG_VERBOSE ("Initializing " << name() << "...");

  cutFlowSvc()->registerTopFilter( name(),
                                   "Number of processed events before any cut",
                                   xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
                                   "AllStreams");
  return StatusCode::SUCCESS;
}

StatusCode EventCounterAlg::finalize()
{
  ATH_MSG_VERBOSE ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode EventCounterAlg::execute()
{
  ATH_MSG_VERBOSE ("Executing " << name() << "...");

  setFilterPassed(true);
  return StatusCode::SUCCESS;
}
