/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ORDEREDCOLLECTIONSMATCHER_H
#define TRIGHLTJETHYPO_ORDEREDCOLLECTIONSMATCHER_H


// ********************************************************************
//
// NAME:     OrderedCollectionsMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <utility>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/Matcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

class OrderedCollectionsMatcher: virtual public IMatcher, private Matcher{
  /* Match by ordering all jets in the single eta region
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  ConditionsSorter sorterPred;
  OrderedCollectionsMatcher();
  OrderedCollectionsMatcher(const Conditions& cs, const std::string& name);
  ~OrderedCollectionsMatcher(){}
  void match(const HypoJetIter&, const HypoJetIter&) override;
  bool pass() const override;
  const HypoJetVector& passedJets() const noexcept override;
  const HypoJetVector& failedJets() const noexcept override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;

private:
  Conditions m_conditions;
  bool m_pass;

  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
};
#endif
