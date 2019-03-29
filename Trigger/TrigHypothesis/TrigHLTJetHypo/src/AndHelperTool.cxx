/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AndHelperTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "AndHelperTool.h"

#include "GaudiKernel/StatusCode.h"
#include "./ITrigJetHypoHelperVisitor.h"
#include "./nodeIDPrinter.h"
#include <sstream>

AndHelperTool::AndHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent),
    m_timer(std::make_unique<JetTrigTimer>()){
}


bool AndHelperTool::pass(HypoJetVector& jets) {
  ATH_MSG_DEBUG("AndHelperTool::pass... " << jets.size() << " jets");

  m_timer->start();
  m_pass = m_lhs->pass(jets);
  if (m_pass){
    ATH_MSG_DEBUG("LHS passed");
    m_pass = m_rhs->pass(jets);
    ATH_MSG_DEBUG("RHS " <<std::boolalpha << m_pass);
  } else {
    ATH_MSG_DEBUG("LHS failed");
  }    

  m_timer->stop();
  return m_pass;
}



std::string AndHelperTool::toString() const{

  std::stringstream ss;
  
  ss <<nodeIDPrinter(name(), m_nodeID,
                     m_parentNodeID, m_pass, m_timer->readAndReset());
  return ss.str();
                     
}


void AndHelperTool::accept(ITrigJetHypoHelperVisitor& v) {
  v.visit(this);
  m_lhs->accept(v);
  m_rhs->accept(v);
}

std::string AndHelperTool::toStringAndResetHistory() {
  auto result = toString();
  return result;
}

