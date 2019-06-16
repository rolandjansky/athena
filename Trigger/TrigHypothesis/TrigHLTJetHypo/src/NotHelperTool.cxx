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
#include "./ITrigJetHypoInfoCollector.h"
#include "./nodeIDPrinter.h"
#include "./JetTrigTimer.h"

#include <sstream>

NotHelperTool::NotHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}

bool
NotHelperTool::pass(HypoJetVector& jets,
		    xAODJetCollector& jetCollector,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  ATH_MSG_DEBUG("NotHelperTool::pass... " << jets.size() << " jets");

  JetTrigTimer timer;
  if(collector){
    timer.start();
  }
  
  auto pass =  !m_hypoTool->pass(jets, jetCollector, collector);
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

std::string NotHelperTool::toString() const{
  return nodeIDPrinter(name(), m_nodeID, m_parentNodeID);                     
}




StatusCode NotHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return m_hypoTool->getDescription(c);
}
