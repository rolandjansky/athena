/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     OrHelperTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "OrHelperTool.h"
#include "./ITrigJetHypoHelperVisitor.h"
#include "./nodeIDPrinter.h"

#include "GaudiKernel/StatusCode.h"
#include <sstream>

OrHelperTool::OrHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}


bool OrHelperTool::pass(HypoJetVector& jets) {
  ATH_MSG_DEBUG("OrHelperTool::pass... " << jets.size() << " jets");
  ++m_nCalls;
  
  m_pass = m_lhs->pass(jets);
  if (m_pass){
    ATH_MSG_DEBUG("LHS passed");
    return m_pass;
  } else {
    m_pass = m_rhs->pass(jets);
    ATH_MSG_DEBUG("RHS " <<std::boolalpha << m_pass);
  }    

  return m_pass;
}

std::string OrHelperTool::toString() const{
  std::stringstream ss;
  ss << nodeIDPrinter(name(), m_nodeID, m_parentNodeID, m_pass)
     << " calls since last reset: " << m_nCalls << '\n';

  return ss.str();

}

void OrHelperTool::accept(ITrigJetHypoHelperVisitor& v){
  v.visit(this);
  m_lhs->accept(v);
  m_rhs->accept(v);
}

void OrHelperTool::resetHistory() {m_nCalls = 0;}

std::string OrHelperTool::toStringAndResetHistory() {
  auto result = toString();
  resetHistory();
  return result;
}
