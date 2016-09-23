/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAXIMUMBIPARTITEMATCHER_H
#define TRIGHLTJETHYPO_MAXIMUMBIPARTITEMATCHER_H


// ********************************************************************
//
// NAME:     MaximumBipartiteMatcher.h
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
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <utility>  // std::pair
#include <set>


class MaximumBipartiteMatcher: virtual public IMatcher, private Matcher {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteMatcher(const Conditions& cs, const std::string& name);
  ~MaximumBipartiteMatcher(){}
  void match(const HypoJetIter&, const HypoJetIter&) override;
  bool pass() const  override;
  const HypoJetVector& passedJets() const noexcept override;
  const HypoJetVector& failedJets() const noexcept override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;
private:
  Conditions m_conditions;
  bool m_pass;
  FlowNetwork m_G;

  // m_passed_jets is filled only if the event passes.
  // it can be used to determine which jets contribute.
  // it may be possible that different sets of jets could 
  // allow a pass.
  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
};

#endif
