/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MULTIMATCHER_H
#define TRIGHLTJETHYPO_MULTIMATCHER_H

#include <utility>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/Matcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

class MultiJetMatcher: virtual public IMatcher, private Matcher{
  /* Matcher selects pairs of jets according to a Condition object.
     The Condition object applies a 
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  ConditionsSorter sorterPred;
  MultiJetMatcher();
  MultiJetMatcher(const Conditions& cs,
		  std::size_t njets,
		  const std::string& name);
  ~MultiJetMatcher(){}
  void match(const HypoJetIter&, const HypoJetIter&) override;
  bool pass() const override;
  const HypoJetVector& passedJets() const noexcept override;
  const HypoJetVector& failedJets() const noexcept override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;

private:
  Conditions m_conditions;
  std::size_t m_njets;
  bool m_pass;

  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
};
#endif
