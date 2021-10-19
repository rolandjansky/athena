/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NTUPStringSkimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// Based on the Integrated Simulation Framework
//
// Use of ExpressionParsing to analyse a more complex string

#include "DerivationFrameworkTools/NTUPStringSkimmingTool.h"

#include <vector>
#include <string>

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"

namespace DerivationFramework {

  NTUPStringSkimmingTool::NTUPStringSkimmingTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expression("true"),
    m_parser(nullptr)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("expression", m_expression);
  }

  StatusCode NTUPStringSkimmingTool::initialize()
  {
    m_parser = new ExpressionParsing::ExpressionParser(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    m_parser->loadExpression(m_expression);
    return StatusCode::SUCCESS;
  }

  StatusCode NTUPStringSkimmingTool::finalize()
  {
    if (m_parser) {
      delete m_parser;
      m_parser = nullptr;
    }
    return StatusCode::SUCCESS;
  }

  bool NTUPStringSkimmingTool::eventPassesFilter() const
  {
    return m_parser->evaluateAsBool();
    return true;
  }  

}
