/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SINGLECONDITIONMATCHER_H
#define TRIGHLTJETHYPO_SINGLECONDITIONMATCHER_H


// ********************************************************************
//
// NAME:     SingleConditionMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/GroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <utility>  // std::pair
#include <set>


class SingleConditionMatcher:
virtual public IGroupsMatcher,  private GroupsMatcher {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  SingleConditionMatcher(const ConditionBridge&);
  ~SingleConditionMatcher(){}
  void match(const HypoJetGroupCIter&, const HypoJetGroupCIter&) override;
  bool pass() const  override;
  HypoJetVector passedJets() const noexcept override;
  HypoJetVector failedJets() const noexcept override;
  std::string toString() const noexcept override;
  Conditions getConditions() const noexcept override;
private:
  ConditionBridge m_condition;
  bool m_pass;

  // m_passed_jets is filled only if the event passes.
  // it can be used to determine which jets contribute.
  // it may be possible that different sets of jets could 
  // allow a pass.
  void addToPassedJets(HypoJetGroupCIter);
  void addToFailedJets(HypoJetGroupCIter, HypoJetGroupCIter);

  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
};

#endif
