/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SelectEvent.cxx
///////////////////////////////////////////////////////////////////
// Author: Daniel Scheirich <daniel.scheirich@cern.ch>
// Based on the Integrated Simulation Framework
//
// Basic Jpsi->mu mu derivation example

#include "DerivationFrameworkBPhys/SelectEvent.h"

#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include "TVector3.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

namespace DerivationFramework {

  /** THE event selection tool
   */
  SelectEvent::SelectEvent(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expressionParser(0)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);

    // Declare user-defined properties
    declareProperty("SelectionExpression", m_selectionExpression = "");
    
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode SelectEvent::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
    
    ATH_MSG_INFO("Going to parse the expression: \"" << m_selectionExpression << "\""); 
    
    // parse the expression:
    m_expressionParser = new BSelectionParser(m_selectionExpression);
    
    return StatusCode::SUCCESS;
    
  }
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  StatusCode SelectEvent::finalize()
  {
    // cleanup
    if(m_expressionParser) delete m_expressionParser;
    
    // everything all right
    return StatusCode::SUCCESS;
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  bool SelectEvent::eventPassesFilter() const
  {
    // perform the selection. 
    // All the magic happens inside the SelectionExpr class
    return m_expressionParser->eval(evtStore());
//    return true;
  }  
  
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  
}
