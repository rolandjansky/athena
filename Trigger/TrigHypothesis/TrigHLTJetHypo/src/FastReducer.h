/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FASTREDUCER_H
#define TRIGHLTJETHYPO_FASTREDUCER_H

#include "./RepeatedConditionsDefs.h"
#include "./Tree.h"
#include "./JetGroupProduct.h"
#include "./xAODJetCollector.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./JetGroupRegister.h"
#include "./ConditionFilter.h"
#include <string>
#include <map>

using TreeVec = std::vector<std::size_t>;
using IDJetGroup = std::map<std::size_t, HypoJetVector>;
typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

/** define a map from a jet group index to a set of indices
 that can be used to find a jet
*/

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;
using  ConditionFilters = std::vector<std::unique_ptr<ConditionFilter>>;


class FastReducer {
 public:

  FastReducer(const HypoJetCIter& jets_b,
              const HypoJetCIter& jets_e,
              const ConditionPtrs& conditionObjects,
	      const ConditionFilters& conditionFilters,
              const Tree& conditionsTree,
              xAODJetCollector& jetCollector,
              const Collector& collector);


  /** determine whether a set of jets satisfies all hypo conditions.
   if pass, return a vector of jets that passes the Conditions.
  */
  
  bool pass() const;
 
  std::string toString() const;

 private:

  // conditions owned by the matcher
  const ConditionPtrs& m_conditions;
  std::vector<std::size_t> m_conditionMult;

  // conditionFilters owned by the matcher
  const ConditionFilters& m_conditionFilters;

  /** tree structure for Conditions objects.
   The conditions tree gives relations among conditions (eg parent-child
   and siblings-of)
  */
  
  Tree m_tree;

  // map Condition index onto a list of indices of satisfying jet groups.
  CondInd2JetGroupsInds m_satisfiedBy;

  /** map Condition index onto a set of indices the condition
  has been tested with - used to prevent retesting and reduce combinations
  */
  
  std::map<std::size_t, std::set<std::size_t>> m_testedBy;

  /** map jet group indices to jet groups */
  std::map<std::size_t, HypoJetVector> m_indJetGroup;

  /** map jet group indices to indices of incoming jet groups */
  JetGroupInd2ElemInds m_jg2elemjgs;


  /** event pass flag */
  bool m_pass{false};

  HypoJetVector m_passingJets;

  JetGroupRegister m_jgRegister;
  /** set up the data structures for propagation. Propagation is the
   act of combining jet groups satisfying children
   in preparration for testing against parent conditions.
  */
  
  bool findInitialJetGroups(const HypoJetCIter& jets_b,
			    const HypoJetCIter& jets_e,
			    const Collector& collector);
  
  
  bool propagateJetGroups(const Collector& collector);
  
  
  bool propagate_(std::size_t child,
		  const std::vector<std::size_t>& siblings,
		  const Collector&);
  
  std::string dataStructuresToStr() const;

  void dumpDataStructures(const Collector&) const;
  
  void recordJetGroup(std::size_t ind,
		      const HypoJetVector& jg,
		      const Collector& collector) const;

  void recordFiltering(std::size_t leaf_ind,
		       std::size_t n_inputjets,
		       int n_filteredjets,
		       const Collector& collector) const;

  void collectLeafJets(xAODJetCollector& jetCollector,
		       const Collector& collector) const;

  bool capacitySatisfied(std::size_t ind,
			 const Collector& collector) const;

};
#endif
