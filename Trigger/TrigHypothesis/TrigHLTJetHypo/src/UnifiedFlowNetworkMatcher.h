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

#include "./FlowNetworkBase.h"
#include "./ConditionsDefsMT.h"

class UnifiedFlowNetworkMatcher:
virtual public FlowNetworkBase {

  /* An initialiser for FlowNetwork base */

public:
  UnifiedFlowNetworkMatcher(ConditionsMT&& cs,
			    const std::vector<std::size_t>& treeVec);
  
  ~UnifiedFlowNetworkMatcher(){}

 private:
  
  //tree stucture:  parent nodes according to position.
  std::vector<std::size_t> m_treeVec; 
};

#endif
