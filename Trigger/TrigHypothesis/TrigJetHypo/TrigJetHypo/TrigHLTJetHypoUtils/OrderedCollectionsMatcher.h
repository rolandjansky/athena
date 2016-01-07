/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ORDEREDCOLLECTIONSMATCHER_H
#define TRIGJETHYPO_ORDEREDCOLLECTIONSMATCHER_H


// ********************************************************************
//
// NAME:     OrderedCollectionsMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <utility>

#include "xAODJet/Jet.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/Matcher.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

class OrderedCollectionsMatcher: virtual public IMatcher, private Matcher{
  /* Match by ordering all jets in the single eta region
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  ConditionsSorter sorterPred;
  OrderedCollectionsMatcher(const Conditions& cs, const std::string& name);
  ~OrderedCollectionsMatcher(){}
  void match(JetIter b, JetIter e) override;
  bool pass() const override;
  std::pair<JetCIter, JetCIter> passed_iters() const override;
  std::pair<JetCIter, JetCIter> failed_iters() const override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;

private:
  Conditions m_conditions;
  bool m_pass;

  JetVector m_passed_jets;
  JetVector m_failed_jets;
  
  std::string m_name;
};
#endif
