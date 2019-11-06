/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_UNIFIEDFKLOWNETWORKMATCHER_H
#define TRIGHLTJETHYPO_UNIFIEDFKLOWNETWORKMATCHER_H

// ********************************************************************
//
// NAME:     UnifiedFlowNetworkMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "./FlowNetworkMatcherBase.h"
#include "./ConditionsDefsMT.h"

class UnifiedFlowNetworkMatcher:
public virtual IGroupsMatcherMT, private FlowNetworkMatcherBase {

  /* An initialiser for FlowNetwork base */

public:
  UnifiedFlowNetworkMatcher(ConditionsMT&& cs,
			    const std::vector<std::size_t>& treeVec,
			    const std::vector<std::vector<int>>&);
  
  ~UnifiedFlowNetworkMatcher(){}
  virtual std::string toString() const override;

};

#endif
