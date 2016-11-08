/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHER_H
#define TRIGHLTJETHYPO_MAXIMUMBIPARTITEGROUPSMATCHER_H


// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcher.h
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
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include <utility>  // std::pair
#include <set>


class MaximumBipartiteGroupsMatcher:
virtual public IGroupsMatcher,  private GroupsMatcher {

  /* Used to find jets pass multithreshold,
     possibly overlapping eta regions
     where any particular jet can be assigned to at most
     one threshold. Uses the Ford Fulkerson Alg.
     See Algorithms, Sedgewick and Wayne 4th edition */

public:
  MaximumBipartiteGroupsMatcher(const Conditions& cs);
  ~MaximumBipartiteGroupsMatcher(){}
  void match(const HypoJetGroupCIter&, const HypoJetGroupCIter&) override;
  bool pass() const  override;
  HypoJetVector passedJets() const noexcept override;
  HypoJetVector failedJets() const noexcept override;
  std::string toString() const noexcept override;
  Conditions getConditions() const noexcept override;
private:
  Conditions m_conditions;
  bool m_pass;
  FlowNetwork m_G;

  // m_passed_jets is filled only if the event passes.
  // it can be used to determine which jets contribute.
  // it may be possible that different sets of jets could 
  // allow a pass.
  void addToFailedJets(const HypoJetVector&);
  void addToFailedJets(HypoJetGroupCIter, HypoJetGroupCIter);

  HypoJetVector m_passed_jets;
  HypoJetVector m_failed_jets;
};

#endif
