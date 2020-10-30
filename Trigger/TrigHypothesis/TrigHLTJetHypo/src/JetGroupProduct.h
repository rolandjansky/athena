/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPPRODUCT_H
#define TRIGHLTJETHYPO_JETGROUPPRODUCT_H

#include "./ProductGen.h"
#include "./DebugInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>
#include <optional>

using CondInd2JetGroupsInds = std::map<int, std::vector<std::size_t>>;

typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

class JetGroupProduct{
  /*
   * Iterate through the combinations of jet groups.
   * The job groups are those that satisfied a set up siblings.
   * Their parent is tested against all the combioinations of job groups
   * that satisfy the siblings.
   * 
   * eg for a vector of siblings [s1, s2]
   * satisfiedBy[s1] is a  vector<vector<size_t>> say <[jg1, jg2], [jg3, jg4]>
   * satisfiedBy[s2] is a  vector<vector<size_t>> say <[jg5, jg6]>
   * the products are then [jg1, jg2, jg5, jg6], [jg3, jg4, jg5, jg6]
   * jg1 is an key in a map that has a value of an input job group (typically
   * containing a single jet.
   */
 public:
  JetGroupProduct(const std::vector<std::size_t>& siblings,
		  const CondInd2JetGroupsInds& satisfiedBy);
  std::optional<std::vector<std::size_t>> next(const Collector&);
  
 private:
  const std::vector<std::size_t> m_siblings;
  const CondInd2JetGroupsInds m_satisfiedBy;

  ProductGen m_productGen;
};

#endif
