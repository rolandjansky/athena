/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H
#define TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHERMT_H

// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcherMT.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "./FlowNetworkMatcherBase.h"
#include "./ConditionsDefsMT.h"

class MaximumBipartiteGroupsMatcherMT:
public virtual IGroupsMatcherMT, private FlowNetworkMatcherBase {

  /* An initialiser for FlowNetwork base */

public:
  MaximumBipartiteGroupsMatcherMT(ConditionsMT&& cs);
  ~MaximumBipartiteGroupsMatcherMT(){}
  virtual std::string toString() const override;
};

#endif
