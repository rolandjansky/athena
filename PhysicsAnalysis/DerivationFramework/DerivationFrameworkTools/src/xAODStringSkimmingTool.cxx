/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"

namespace DerivationFramework {

  xAODStringSkimmingTool::xAODStringSkimmingTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expression("true"),
    m_parser(0),
    m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("expression", m_expression);
  }

  StatusCode xAODStringSkimmingTool::initialize()
  {
    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
    proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));

    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
    m_parser->loadExpression(m_expression);
    return StatusCode::SUCCESS;
  }

  StatusCode xAODStringSkimmingTool::finalize()
  {
    if (m_parser) {
      delete m_parser;
      m_parser = 0;
    }
    return StatusCode::SUCCESS;
  }

  bool xAODStringSkimmingTool::eventPassesFilter() const
  {
    return m_parser->evaluateAsBool();
    return true;
  }  

}
