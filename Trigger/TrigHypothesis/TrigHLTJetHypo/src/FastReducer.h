/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_FASTREDUCER_H
#define TRIGHLTJETHYPO_FASTREDUCER_H


#include "./ConditionsDefsMT.h"
#include "./Tree.h"
#include "./JetGroupProduct.h"
#include "./JetGroupIndAllocator.h"
#include "./xAODJetCollector.h"
#include <string>

using TreeVec = std::vector<std::size_t>;
using IDJetGroup = std::map<std::size_t, HypoJetVector>;
typedef std::unique_ptr<ITrigJetHypoInfoCollector> Collector;

/** define a map from a jet group index to a set of indices
 that can be used to find a jet
*/

using JetGroupInd2ElemInds = std::map<int, std::vector<std::size_t>>;

class ITrigJetHypoInfoCollector;

class FastReducer {
 public:

  FastReducer(const HypoJetGroupCIter& groups_b,
              const HypoJetGroupCIter& groups_e,
              const ConditionsMT& conditionObjects,
              const Tree& conditionsTree,
              const std::vector<std::vector<int>>& sharedConditions,
              xAODJetCollector& jetCollector,
              const Collector& collector);


  /** determine whether a set of jets satisfies all hypo conditions.
   if pass, return a vector of jets that passes the Conditions.
  */
  
  bool pass() const;
 
  std::string toString() const;

 private:

  const ConditionsMT& m_conditions;

  /** tree structure for Conditions objects.
   The conditions tree gives relations among conditions (eg parent-child
   and siblings-of)
  */
  
  Tree m_tree;

  /** A vector of shared Condition indices. All shared Conditions are leaf
   Conditions  that see the jet icoming jets.
  */
  
  std::vector<std::vector<int>> m_sharedConditions;

  // map Condition index onto a list of indices of satisfying job groups.
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

  JetGroupIndAllocator m_jgIndAllocator;
  
  /** set up the data structures for propagation. Propagation is the
   act of combining jet groups satisfying children
   in preparration for testing against parent conditions.
  */
  
  bool findInitialJetGroups(const std::vector<int>& leaves,
			    const HypoJetGroupCIter& groups_b,
			    const HypoJetGroupCIter& groups_e,
			    const Collector& collector);
  
  
  bool propagateJetGroups(const Collector& collector);
  
  
  bool propagate_(std::size_t child,
		  const std::vector<std::size_t>& siblings,
		  const Collector&);
  
  std::string dataStructuresToStr() const;

  void dumpDataStructures(const Collector&) const;
  
  void recordJetGroup(std::size_t ind,
		      const HypoJetVector& jg,
		      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const;

  void collectLeafJets(xAODJetCollector& jetCollector,
		       const Collector& collector) const;


};
#endif
