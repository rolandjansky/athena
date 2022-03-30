/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPREDUCER_H
#define TRIGHLTJETHYPO_JETGROUPREDUCER_H

#include "./IJetGroupProduct.h"
#include "./DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>

using CondInd2JetGroupsInds = std::map<int, std::vector<std::size_t>>;

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;


typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

// Reduce  one or more vectors of possibly non-elementary jet group indices to
// a vector of elementary jet group indices
// A possible use of this class is to efficiently combine
// satisfying jet groups from the children of an AcceptAll
// condition. This approach would not be used for simpleTrees
// as specialised reducers for such trees exist which exploit the
// fact the the child jet groups are elementary.


class JetGroupReducer: public IJetGroupProduct{
  /*
   * Form the reducer of the sets of jets passing sibling Conditions.
   */
 public:
  JetGroupReducer(const std::vector<std::size_t>& siblings,
		  const CondInd2JetGroupsInds& satisfiedBy,
		  const JetGroupInd2ElemInds&);
  
  virtual std::vector<std::size_t> next(const Collector&) override;
  virtual bool valid() const override;
  
 private:
  std::vector<std::size_t>  m_jetIndices;
  bool m_done{false};
};

#endif
