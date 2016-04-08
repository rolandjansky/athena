/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SELECTEDJETSMATCHER_H
#define TRIGHLTJETHYPO_SELECTEDJETSMATCHER_H


// ********************************************************************
//
// NAME:     SelectedJetsMatcher.h
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

class SelectedJetsMatcher: virtual public IMatcher, private Matcher{
  /* Match by ordering all jets in the single eta region
     in Et, pair of with the Conditions objects also ordered
     in Et. Event pass if the Et of each jet >= to the threshold
     of the paired Condition.
   */
public:
  SelectedJetsMatcher(const Conditions& cs, 
		      const std::vector<unsigned int>& indices,
		      const std::string& name);
  ~SelectedJetsMatcher(){}
  void match(const HypoJetIter&, const HypoJetIter&) override;
  bool pass() const override;
  const HypoJetVector& passedJets() const noexcept override;
  const HypoJetVector& failedJets() const noexcept override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;
private:
  Conditions m_conditions;
  std::vector<unsigned int> m_indices;
  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
  bool m_pass;
};
#endif
