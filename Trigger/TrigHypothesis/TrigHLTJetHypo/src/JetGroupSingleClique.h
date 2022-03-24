/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPSINGLECLIQUE_H
#define TRIGHLTJETHYPO_JETGROUPSINGLECLIQUE_H

#include "./IJetGroupProduct.h"
#include "./DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>

using CondInd2JetGroupsInds = std::map<int, std::vector<std::size_t>>;
using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;

typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

class JetGroupSingleClique: public IJetGroupProduct{
  /*
   * Returns the elementary jet groups for jet groups satisfying a condition.
   * Intended to be used with a single clique set of Conditions
   * with an AcceptAll parent. Here, all siblings have the same passing jets
   * and next() need return only the set of jets satisfying 
   * one of the siblings. 
   * The simple scenario gives rise to this special case.
   */
public:
  JetGroupSingleClique(const std::vector<std::size_t>& satisfyingJetGroups,
		       const JetGroupInd2ElemInds& jg2elemjgs
		       );
  
  virtual
  std::vector<std::size_t> next(const Collector&) override;
  
private:
  std::vector<std::size_t>  m_jetIndices;
  bool m_done{false};

};

#endif
