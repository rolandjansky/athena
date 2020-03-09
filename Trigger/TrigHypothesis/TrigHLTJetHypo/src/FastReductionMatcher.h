/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FASTREDUCTIONMATCHER_H
#define TRIGHLTJETHYPO_FASTREDUCTIONMATCHER_H


#include "./IGroupsMatcherMT.h"
#include "./ConditionsDefsMT.h"
#include "./Tree.h"

using TreeVec = std::vector<std::size_t>;
class ITrigJetHypoInfoCollector;

class FastReductionMatcher: public IGroupsMatcherMT {
 public:

  FastReductionMatcher(ConditionsMT,
			    const Tree&,
			    const std::vector<std::vector<int>>&);


  /** determine whether a set of jets satisfies all hypo conditions.
   the jets are packed into jet grpups of length 1. groups_b and
   groups_e are iterators into the data structure containing these groups.
   Jet collector is used to transport the jets that matched the hypo
   conditions. Collector collects information for testing and debugging
   the final bool argument is used to allow further debugging
   within the method.
  */
  
  virtual std::optional<bool>
    match(const HypoJetGroupCIter& groups_b,
	  const HypoJetGroupCIter& groups_e,
	  xAODJetCollector&,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
	  bool
	  ) const override;
 
  std::string toString() const override;

 private:

  ConditionsMT m_conditions;

  /** tree structure for Conditions objects.
   The conditions tree gives relations among conditions (eg parent-child
   and siblings-of)
  */
  
  Tree m_tree;

  /** a vector of shared nodes. All shared nodes are leaf node that
  see the jet collection.
  */
  
  std::vector<std::vector<int>> m_sharedNodes;

};
#endif
