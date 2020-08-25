///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EventDecisionAlg.cxx
// Implementation file for class EventDecisionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "EventDecisionAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "AthenaKernel/IDecisionSvc.h"


// Constructors
////////////////
EventDecisionAlg::EventDecisionAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_decSvc("DecisionSvc/DecisionSvc", name),
  m_streamNames()
{
  declareProperty("DecisionService",   m_decSvc,      "The handle to the IDecisionSvc" );
  declareProperty("OutputStreamNames", m_streamNames, "The names of all output streams to check");
}


// Destructor
///////////////
EventDecisionAlg::~EventDecisionAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode EventDecisionAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_decSvc );
  ATH_MSG_DEBUG ( " using = " << m_streamNames );

  // Retrieve the services
  ATH_CHECK( m_decSvc.retrieve() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );
  return StatusCode::SUCCESS;
}



StatusCode EventDecisionAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( m_decSvc.release() );

  return StatusCode::SUCCESS;
}



StatusCode EventDecisionAlg::execute()
{
  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() );
  ATH_MSG_VERBOSE ( "Dumping event store: " << evtStore()->dump() );

  // Make the pass/fail decision
  // Ensure that the even passes if no stream name is given
  bool eventPasses = true;
  for ( const std::string& streamName : m_streamNames.value() ){
    // Ensure that the even passes if no stream name is given
    eventPasses = false;
    eventPasses = m_decSvc->isEventAccepted(streamName);
    if (eventPasses){
      ATH_MSG_DEBUG("Got an event accept from stream name " << streamName);
      break;
    }
  }
  this->setFilterPassed( eventPasses );
  ATH_MSG_DEBUG("Event passes/fails: " << eventPasses );

  return StatusCode::SUCCESS;
}
