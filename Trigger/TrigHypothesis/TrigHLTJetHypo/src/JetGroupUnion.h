/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPUNION_H
#define TRIGHLTJETHYPO_JETGROUPUNION_H

#include "./IJetGroupProduct.h"
#include "./DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>

using CondInd2JetGroupsInds = std::map<int, std::vector<std::size_t>>;

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;


typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

class JetGroupUnion: public IJetGroupProduct{
  /*
   * Form the union of the sets of jets passing sibling Conditions.
   */
 public:
  JetGroupUnion(const std::vector<std::size_t>& siblings,
		const CondInd2JetGroupsInds& satisfiedBy,
		const JetGroupInd2ElemInds& jg2elemjgs,
		const Collector&);
  
  virtual std::vector<std::size_t> next(const Collector&) override;
  
 private:
  std::vector<std::size_t>  m_jetIndices;
  bool m_done{false};
};

#endif
