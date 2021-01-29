/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FASTREDUCTIONMATCHER_H
#define TRIGHLTJETHYPO_FASTREDUCTIONMATCHER_H


#include "./IJetsMatcherMT.h"
#include "./CapacityCheckedConditionsDefs.h"
#include "./ConditionFilter.h"
#include "./Tree.h"

using TreeVec = std::vector<std::size_t>;
class ITrigJetHypoInfoCollector;

using  ConditionFilters = std::vector<std::unique_ptr<ConditionFilter>>;

class FastReductionMatcher: public IJetsMatcherMT {
 public:

  FastReductionMatcher(ConditionPtrs&,
		       ConditionFilters&,
		       const Tree&);


  /** determine whether a set of jets satisfies all hypo conditions.
   the jets are packed into jet grpups of length 1. groups_b and
   groups_e are iterators into the data structure containing these groups.
   Jet collector is used to transport the jets that matched the hypo
   conditions. Collector collects information for testing and debugging
   the final bool argument is used to allow further debugging
   within the method.
  */
  
  virtual std::optional<bool>
    match(const HypoJetCIter& jets_b,
	  const HypoJetCIter& jets_e,
	  xAODJetCollector&,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
	  bool
	  ) const override;
 
  std::string toString() const override;

 private:

  ConditionPtrs m_conditions;
  std::vector<std::unique_ptr<ConditionFilter>> m_conditionFilters;
  /** tree structure for Conditions objects.
   The conditions tree gives relations among conditions (eg parent-child
   and siblings-of)
  */
  
  Tree m_tree;

  // minimum number of jets required - determined by summing
  // leaf Condition capacities
  long int m_minNjets{0};


};
#endif
