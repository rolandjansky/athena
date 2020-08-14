///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/StackElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"


///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
CutAlg::CutAlg( const std::string& name,
                ISvcLocator* pSvcLocator ) :
  ::AthFilterAlgorithm( name, pSvcLocator ),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_parser(0),
  m_cut(""),
  m_nEventsProcessed(0)
{
  declareProperty("Cut",              m_cut="",           "The name of the output container" );
  declareProperty("TrigDecisionTool", m_trigDecisionTool, "If you do not use trigger decisions and want to ensure the TrigDecisionTool is not loaded, set this to a blank string");
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

  // initialize proxy loaders for expression parsing
  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();

  // initialise TDT explicitly, needed for the tool to properly work with trigger decisions in AthAnalysisBase (until fixed)
  if( !m_trigDecisionTool.empty() ) {
    ATH_CHECK( m_trigDecisionTool.retrieve() );
    proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
  }

  proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
  proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));

  // load the expressions
  m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
  m_parser->loadExpression( m_cut.value() );

  ATH_MSG_DEBUG ( "==> done with initialize " << name() << "..." );

  return StatusCode::SUCCESS;
}



StatusCode CutAlg::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  // Release all tools and services
  if (m_parser) {
    delete m_parser;
    m_parser = 0;
  }

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
  eventPasses = m_parser->evaluateAsBool();
  this->setFilterPassed( eventPasses );
  ATH_MSG_DEBUG("Event passes/fails: " << eventPasses );

  return StatusCode::SUCCESS;
}
