/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_SINGLEETAREGIONMATCHER_H
#define TRIGJETHYPO_SINGLEETAREGIONMATCHER_H


// ********************************************************************
//
// NAME:     SingleEtaRegionMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <utility>

#include "xAODJet/Jet.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

class SingleEtaRegionMatcher: public IMatcher{
  /* Match by ordering all jets in the single eta region
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  ConditionsSorter sorterPred;
  SingleEtaRegionMatcher(const Conditions& cs);
  void match(JetIter b, JetIter e) override;
  bool pass() const override;
  std::pair<JetCIter, JetCIter> passed_iters() const override;
  std::pair<JetCIter, JetCIter> failed_iters() const override;

private:
  Conditions m_conditions;
  bool m_pass;

  JetVector m_passed_jets;
  JetVector m_failed_jets;
};
#endif
