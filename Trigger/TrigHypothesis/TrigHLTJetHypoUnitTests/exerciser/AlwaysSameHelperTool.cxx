/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AlwaysSameHelperTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigHLTJetHypoUnitTests/AlwaysSameHelperTool.h"

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/../src/nodeIDPrinter.h"
#include "TrigHLTJetHypo/../src/JetTrigTimer.h"
#include <sstream>

AlwaysSameHelperTool::AlwaysSameHelperTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) :
  base_class(type, name, parent){
}


bool
AlwaysSameHelperTool::pass(HypoJetVector&,
		    xAODJetCollector&,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  ATH_MSG_DEBUG("AlwaysSameHelperTool::pass... " << m_pass);
  return m_pass;
}

std::string AlwaysSameHelperTool::toString() const{
  std::stringstream ss;
  ss << name() << " alwways returns ";
  
  if(m_pass){
    ss << " true";
  } else {
    ss << " false";
  }
  ss << '\n';
  return ss.str();                     
}

StatusCode AlwaysSameHelperTool::getDescription(ITrigJetHypoInfoCollector& c) const {
  c.collect(name(), toString());
  return StatusCode::SUCCESS;
}

std::size_t AlwaysSameHelperTool::requiresNJets() const {
  return 0;
}

