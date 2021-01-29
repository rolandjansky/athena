/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./FastReductionMatcher.h"
#include "./FastReducer.h"
#include "./ITrigJetHypoInfoCollector.h"

#include <map>
#include <algorithm>
#include <sstream>

FastReductionMatcher::FastReductionMatcher(ConditionPtrs& conditions,
					   ConditionFilters& filters,
					   const Tree& tree):
  m_conditions(std::move(conditions)),
  m_conditionFilters(std::move(filters)),
  m_tree(tree){

  int minNjets{0};
  for (const auto& il : m_tree.leaves()){
    const auto& condition = m_conditions[il];

    if (!condition->isFromChainPart()) {
      throw std::runtime_error("Tree leaf condition  but not from ChainPart");
    }
    minNjets += condition->capacity() * condition->multiplicity();

  }

  m_minNjets = std::max(1, minNjets);
  
  if (filters.size() != conditions.size()) {
    throw std::runtime_error("Conditions and ConditionFilters sequence sizes differ");
  }

}
	 

std::optional<bool>
FastReductionMatcher::match(const HypoJetCIter& jets_b,
			    const HypoJetCIter& jets_e,
			    xAODJetCollector& jetCollector,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
			    bool) const {
  /*
    Decide if the incomming jet groups satisfiy all the conditions of the 
    condition tree.

    The leaf nodes are tested first. Jet groups satisfyng the leaves 
    stored in a table. Once all leaves have been processed, the satisfying
    groups are combined, and tested against the aprent nodes.

    The procedure is iterated until the entire tree is checked, or
    an unsatisfied condition is found. If no unsatisfied condition,
    there is a match.
   */

  auto njets = jets_e - jets_b;

  if (njets < 0) {
    throw std::runtime_error("Negative number of jets"); 
  }
  if (njets < m_minNjets) {
    if (collector) {
      collector->collect("FastReductionMatcher",
			"have " + std::to_string(njets) +
			" jets need " + std::to_string(m_minNjets) +
			 "  pass: false");
    }
    return false;
  }

  FastReducer reducer(jets_b,
                      jets_e,
                      m_conditions,
		      m_conditionFilters,
                      m_tree,
                      jetCollector,
                      collector);

  return std::make_optional<bool>(reducer.pass());
}


std::string FastReductionMatcher::toString() const {
  std::stringstream ss;
  ss << "FastReductionMatcher:\n"
     << "  treeVector: " << m_tree << '\n'
     << "  min required jets " << m_minNjets << "\n\n"
     << "FastReductionMatcher Conditions ["
     << m_conditions.size() << "]: \n\n";

  std::size_t count{0u};
  for(const auto& c : m_conditions){
    auto sc = std::to_string(count++);
    sc.insert(sc.begin(), 3-sc.length(), ' ');
    ss << sc <<": "<< c->toString() + '\n';
  }

  ss << "FastReductionMatcher ConditionFilters ["
     << m_conditionFilters.size() << "]: \n";


  count = 0;
  for(const auto& c : m_conditionFilters){
    auto sc = std::to_string(count++);
    sc.insert(sc.begin(), 3-sc.length(), ' ');
    ss << sc <<": "<< c->toString() + '\n';
  }
  

  return ss.str();
}
