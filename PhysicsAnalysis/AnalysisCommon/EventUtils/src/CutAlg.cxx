///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CutAlg.cxx
// Implementation file for class CutAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "CutAlg.h"

// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "ExpressionEvaluation/StackElement.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
CutAlg::CutAlg( const std::string& name,
                ISvcLocator* pSvcLocator ) :
  ExpressionParserUserWithTrigSupport<::AthFilterAlgorithm>( name, pSvcLocator ),
  m_cut(""),
  m_nEventsProcessed(0)
{
  declareProperty("Cut",              m_cut="",           "The cut expression" );
}



// Destructor
///////////////
CutAlg::~CutAlg()
{}



// Athena Algorithm's Hooks
////////////////////////////
StatusCode CutAlg::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // Print out the used configuration
  ATH_MSG_DEBUG ( " using = " << m_trigDecisionTool );
  ATH_MSG_DEBUG ( " using = " << m_cut );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;

  if (!m_cut.value().empty()) {
     ATH_CHECK( initializeParser(m_cut.value()) );
  }

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode CutAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  ATH_CHECK( finalizeParser() );
  return StatusCode::SUCCESS;
}



StatusCode CutAlg::execute()
{
  // Increase the event counter
  ++m_nEventsProcessed;

  // Simple status message at the beginning of each event execute,
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );
  ATH_MSG_VERBOSE ( "Dumping event store: " << evtStore()->dump() );

  // Make the pass/fail decision
  bool eventPasses = true;
  if (m_parser) eventPasses = m_parser->evaluateAsBool();
  this->setFilterPassed( eventPasses );
  ATH_MSG_DEBUG("Event passes/fails: " << eventPasses );

  return StatusCode::SUCCESS;
}
