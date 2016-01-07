/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_SELECTEDJETSMATCHER_H
#define TRIGJETHYPO_SELECTEDJETSMATCHER_H


// ********************************************************************
//
// NAME:     SelectedJetsMatcher.h
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
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"


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
  void match(JetIter b, JetIter e) override;
  bool pass() const override;
  std::pair<JetCIter, JetCIter> passed_iters() const override;
  std::pair<JetCIter, JetCIter> failed_iters() const override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;
private:
  Conditions m_conditions;
  std::vector<unsigned int> m_indices;
  JetVector m_passed_jets;
  JetVector m_failed_jets;
  bool m_pass;
};
#endif
