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
  virtual std::string toString() const override;
 private:
 
  virtual void
    reportPassingJets(const std::map<int, pHypoJet>& nodeToJet,
		      const std::unique_ptr<FlowNetwork>& G,
		      const std::unique_ptr<ITrigJetHypoInfoCollector>&,
		      xAODJetCollector&
		      ) const override;
};

#endif
