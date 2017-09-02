
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////// -*- C++ -*- /////////////////////////////
// CutTool.cxx
// Implementation file for class CutTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////

// EventUtils includes
#include "CutTool.h"

// STL includes
#include <vector>
#include <string>

// FrameWork includes
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
CutTool::CutTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent ) :
  ::AthAlgTool  ( type, name, parent ),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_parser(0),
  m_cut("")
  //m_nEventsProcessed(0)
{
  declareInterface< DerivationFramework::ISkimmingTool >(this);

  declareProperty("Cut", m_cut="", "The cut expression" );
  declareProperty("TrigDecisionTool",m_trigDecisionTool,"If you do not use trigger decisions and want to ensure the TrigDecisionTool is not loaded, set this to a blank string");
}

// Destructor
///////////////
CutTool::~CutTool()
{}



// Athena algtool's Hooks
////////////////////////////
StatusCode CutTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

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

  return StatusCode::SUCCESS;
}



StatusCode CutTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  if (m_parser) {
    delete m_parser;
    m_parser = 0;
  }

  return StatusCode::SUCCESS;
}



// Implement the method from the ISkimmingTool interface
bool CutTool::eventPassesFilter() const
{
  ATH_MSG_DEBUG ( "==> eventPassesFilter() " << name()  );
  ATH_MSG_VERBOSE ( "Dumping event store: " << evtStore()->dump() );

  return m_parser->evaluateAsBool();
  return true;
}
