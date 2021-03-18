///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
#include "ExpressionEvaluation/StackElement.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
CutTool::CutTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent ) :
  ExpressionParserUserWithTrigSupport<::AthAlgTool>  ( type, name, parent ),
  m_cut("")
{
  declareInterface< DerivationFramework::ISkimmingTool >(this);

  declareProperty("Cut", m_cut="", "The cut expression" );
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

  if (!m_cut.value().empty()) {
     ATH_CHECK( initializeParser(m_cut.value()) );
  }

  return StatusCode::SUCCESS;
}



StatusCode CutTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");

  ATH_CHECK( finalizeParser() );
  return StatusCode::SUCCESS;
}



// Implement the method from the ISkimmingTool interface
bool CutTool::eventPassesFilter() const
{
  ATH_MSG_DEBUG ( "==> eventPassesFilter() " << name()  );
  ATH_MSG_VERBOSE ( "Dumping event store: " << evtStore()->dump() );

  return (m_parser ? m_parser->evaluateAsBool() : true);
}
