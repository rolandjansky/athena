/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITriggerJetGroomerToolMT_H
#define ITriggerJetGroomerToolMT_H

#include "AsgTools/IAsgTool.h"
// #include "AsgTools/AsgToolMacros.h"
#include "xAODJet/JetContainer.h"

/// ITriggerJetGroomerTool is a dual-use tool interface for a tool
/// that grooms a jet collection in the trigger environment
class PseudoJetContainer;

class ITriggerJetGroomerToolMT: virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(ITriggerJetGroomerToolMT)

 public:
  virtual ~ITriggerJetGroomerToolMT() {};
  virtual xAOD::JetContainer* groom(const xAOD::JetContainer* in,
                                    const PseudoJetContainer&) const = 0;
};

#endif
