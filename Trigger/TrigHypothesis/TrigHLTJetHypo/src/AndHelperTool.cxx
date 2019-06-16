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
#include "./ITrigJetHypoInfoCollector.h"
#include "./nodeIDPrinter.h"
#include "./JetTrigTimer.h"
#include <sstream>

AndHelperTool::AndHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}


bool
AndHelperTool::pass(HypoJetVector& jets,
		    xAODJetCollector& jetCollector,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  ATH_MSG_DEBUG("AndHelperTool::pass... " << jets.size() << " jets");

  JetTrigTimer timer;
  if(collector){
    timer.start();
  }
  bool pass = m_lhs->pass(jets, jetCollector, collector);
  if (pass){
    ATH_MSG_DEBUG("LHS passed");
    pass = m_rhs->pass(jets, jetCollector, collector);
    ATH_MSG_DEBUG("RHS " <<std::boolalpha << pass);
  } else {
    ATH_MSG_DEBUG("LHS failed");
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

std::string AndHelperTool::toString() const{
  return nodeIDPrinter(name(), m_nodeID, m_parentNodeID);                     
}

StatusCode AndHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return m_lhs->getDescription(c) & m_rhs->getDescription(c);
}
