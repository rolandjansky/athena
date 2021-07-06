/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// xAODStringSkimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// Based on the Integrated Simulation Framework
//
// Use of ExpressionParsing to analyse a more complex string

#include "DerivationFrameworkTools/xAODStringSkimmingTool.h"

#include <vector>
#include <string>


namespace DerivationFramework {

  xAODStringSkimmingTool::xAODStringSkimmingTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    ExpressionParserUserWithTrigSupport<AthAlgTool>(t,n,p),
    m_expression("true")
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("expression", m_expression);
  }

  StatusCode xAODStringSkimmingTool::initialize()
  {
    ATH_CHECK( initializeParser(m_expression) );
    return StatusCode::SUCCESS;
  }

  StatusCode xAODStringSkimmingTool::finalize()
  {
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
  }

  bool xAODStringSkimmingTool::eventPassesFilter() const
  {
    return m_parser->evaluateAsBool();
  }
}
