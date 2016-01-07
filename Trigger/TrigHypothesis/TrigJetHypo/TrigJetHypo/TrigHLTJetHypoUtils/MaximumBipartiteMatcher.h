/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_MAXIMUMBIPARTITEMATCHER_H
#define TRIGJETHYPO_MAXIMUMBIPARTITEMATCHER_H


// ********************************************************************
//
// NAME:     MaximumBipartiteMatcher.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <utility>  // std::pair
#include "TrigJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/Matcher.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "xAODJet/Jet.h"

using JetSet = std::set<const xAOD::Jet*>;

class MaximumBipartiteMatcher: virtual public IMatcher, private Matcher {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteMatcher(const Conditions& cs, const std::string& name);
  ~MaximumBipartiteMatcher(){}
  void match(JetIter b, JetIter e) override;
  bool pass() const  override;
  std::pair<JetCIter, JetCIter> passed_iters() const override;
  std::pair<JetCIter, JetCIter> failed_iters() const override;
  std::string toString() const noexcept override;
  const Conditions& getConditions() const noexcept override;
private:
  Conditions m_conditions;
  bool m_pass;
  JetVector m_passed_jets;
  JetVector m_failed_jets;
};

#endif
