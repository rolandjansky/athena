/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     NotHelperTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "NotHelperTool.h"

#include "GaudiKernel/StatusCode.h"
#include "./ITrigJetHypoHelperVisitor.h"
#include "./nodeIDPrinter.h"
#include <sstream>

NotHelperTool::NotHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}

bool NotHelperTool::pass(HypoJetVector& jets) {
  ATH_MSG_DEBUG("NotHelperTool::pass... " << jets.size() << " jets");

  ++m_nCalls;
  
  m_pass =  ! m_hypoTool->pass(jets);
  return m_pass;
}

std::string NotHelperTool::toString() const{
  std::stringstream ss;
  ss << nodeIDPrinter(name(), m_nodeID, m_parentNodeID, m_pass)
     << " calls since last reset: " << m_nCalls << '\n';
  return ss.str();
}


void NotHelperTool::accept(ITrigJetHypoHelperVisitor& v) {
   m_hypoTool->accept(v);
}

void NotHelperTool::resetHistory() {m_nCalls = 0;}


std::string NotHelperTool::toStringAndResetHistory() {
  auto result = toString();
  resetHistory();
  return result;
}
