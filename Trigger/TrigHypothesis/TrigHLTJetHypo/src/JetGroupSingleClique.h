/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
   * create a jet group for monoclique simple trees
   */
 public:
  JetGroupSingleClique(const std::vector<std::size_t>& satisfyingJets,
		       std::size_t n_required);
  
  virtual std::vector<std::size_t> next(const Collector&) override;

  virtual bool valid() const override;
  
 private:
  std::vector<std::size_t>  m_jetIndices;
  bool m_done{false};
};

#endif
