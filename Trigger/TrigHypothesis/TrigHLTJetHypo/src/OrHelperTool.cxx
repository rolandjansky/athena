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
#include "./ITrigJetHypoInfoCollector.h"
#include "./nodeIDPrinter.h"
#include "./JetTrigTimer.h"

#include "GaudiKernel/StatusCode.h"
#include <sstream>

OrHelperTool::OrHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}


bool
OrHelperTool::pass(HypoJetVector& jets,
		   xAODJetCollector& jetCollector,
		   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  ATH_MSG_DEBUG("OrHelperTool::pass... " << jets.size() << " jets");

 JetTrigTimer timer;
  if(collector){
       timer.start();
  }
  
  bool pass = m_lhs->pass(jets, jetCollector, collector);
  if (pass){
    ATH_MSG_DEBUG("LHS passed");
    return pass;
  } else {
    pass = m_rhs->pass(jets, jetCollector, collector);
    ATH_MSG_DEBUG("RHS " <<std::boolalpha << pass);
  }    

  if (collector){
    timer.stop();
    collector->collect(name(), nodeIDPrinter(name(),
                                             m_nodeID,
                                             m_parentNodeID,
                                             pass,
                                             timer.readAndReset()));
  }
  
  return pass;
}

std::string OrHelperTool::toString() const{
  return nodeIDPrinter(name(), m_nodeID, m_parentNodeID);
}


StatusCode OrHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return m_lhs->getDescription(c) & m_rhs->getDescription(c);
}

std::size_t OrHelperTool::requiresNJets() const {
  return m_lhs->requiresNJets() + m_rhs->requiresNJets();
}
