/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ITRIGJETHYPOTOOL_H
#define TRIGHLTJETHYPO_ITRIGJETHYPOTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"
#include "DecisionHandling/TrigCompositeUtils.h"

class ITrigJetHypoTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITrigJetHypoTool)
    
public:

  virtual ~ITrigJetHypoTool(){};
  virtual StatusCode decide(const xAOD::JetContainer*, bool&) const = 0;
  virtual TrigCompositeUtils::DecisionID decisionId() const = 0; 
};
#endif
