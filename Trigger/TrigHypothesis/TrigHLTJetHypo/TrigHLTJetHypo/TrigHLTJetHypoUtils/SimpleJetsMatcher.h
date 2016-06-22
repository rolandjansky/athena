/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SIMPLEJETSMATCHER_H
#define TRIGHLTJETHYPO_SIMPLEJETSMATCHER_H


// ********************************************************************
//
// NAME:     SimpleJetsMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/Matcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <utility>
#include <sstream>

class SimpleJetsMatcher: virtual public IMatcher, private Matcher{
  /* Match by ordering all jets in the single eta region
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  SimpleJetsMatcher(const Conditions& cs, 
		      const std::string& name);
  ~SimpleJetsMatcher(){}
  void match(const HypoJetIter&, const HypoJetIter&) override;
  bool pass() const override;
  const HypoJetVector& passedJets() const noexcept override;
  const HypoJetVector& failedJets() const noexcept override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;
private:
  Conditions m_conditions;
  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
  bool m_pass;
};
#endif
