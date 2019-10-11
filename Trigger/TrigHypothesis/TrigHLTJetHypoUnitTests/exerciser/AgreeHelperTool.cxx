/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AgreeHelperTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigHLTJetHypoUnitTests/AgreeHelperTool.h"

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/../src/nodeIDPrinter.h"
#include "TrigHLTJetHypo/../src/JetTrigTimer.h"
#include <sstream>

AgreeHelperTool::AgreeHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}


bool
AgreeHelperTool::pass(HypoJetVector& jets,
		    xAODJetCollector& jetCollector,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  ATH_MSG_DEBUG("AgreeHelperTool::pass... " << jets.size() << " jets");

  JetTrigTimer timer;
  if(collector){
    timer.start();
  }
  bool lpass = m_lhs->pass(jets, jetCollector, collector);
  bool rpass = m_rhs->pass(jets, jetCollector, collector);
  // bool lpass = true;
  // bool rpass = true;
  bool pass = (lpass and rpass) or (not(lpass or rpass));

  if (collector){
    timer.stop();
    collector->collect(name(), nodeIDPrinter(name(),
                                             m_nodeID,
                                             m_parentNodeID,
                                             pass,
                                             timer.readAndReset()));
  }
  if(not pass){
    auto lstr =  lpass ? " true " : " false ";
    auto rstr =  rpass ? " true " : " false ";
    ATH_MSG_ERROR("Left - right pass disagree."
		  << " lhs pass: " << lstr 
		  << " rhs pass: " << rstr);

  }

  if(collector){
    std::string msg = pass ? "pass" : "fail";
    collector->collect(name(), msg);
  }
  
  return pass;
}

std::string AgreeHelperTool::toString() const{
  return nodeIDPrinter(name(), m_nodeID, m_parentNodeID);                     
}

StatusCode AgreeHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return m_lhs->getDescription(c) & m_rhs->getDescription(c);
}

std::size_t AgreeHelperTool::requiresNJets() const {
  return m_lhs->requiresNJets() + m_rhs->requiresNJets();
}

