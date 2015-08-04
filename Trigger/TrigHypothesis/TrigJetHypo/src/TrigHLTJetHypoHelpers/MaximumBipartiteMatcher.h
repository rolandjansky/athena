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
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // IMatcher...
#include "xAODJet/Jet.h"

using JetSet = std::set<const xAOD::Jet*>;

class MaximumBipartiteMatcher: public IMatcher {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteMatcher(const Conditions& cs);
  void match(JetIter b, JetIter e) override;
  bool pass() const  override;
  std::pair<JetCIter, JetCIter> passed_iters() const override;
  std::pair<JetCIter, JetCIter> failed_iters() const override;

private:
  Conditions m_conditions;
  bool m_pass;
  JetVector m_passed_jets;
  JetVector m_failed_jets;
};

#endif
