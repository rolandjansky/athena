/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./FastReductionMatcher.h"
#include "./FastReducer.h"
#include "./ITrigJetHypoInfoCollector.h"

#include <map>
#include <algorithm>
#include <sstream>

FastReductionMatcher::FastReductionMatcher(ConditionPtrs conditions,
					   const Tree& tree):
  m_conditions(std::move(conditions)),
  m_tree(tree){

  for (const auto& il : m_tree.leaves()){
    auto label = m_conditions[il]->label();
    if (label.rfind("leg", 0) != 0) { // startswith "leg"
      throw std::runtime_error("Leaf condition " + std::to_string(il) +
			       "has no leg label");
    }
  }
}
	 


std::optional<bool>
FastReductionMatcher::match(const HypoJetGroupCIter& groups_b,
			    const HypoJetGroupCIter& groups_e,
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


  FastReducer reducer(groups_b,
                      groups_e,
                      m_conditions,
                      m_tree,
                      jetCollector,
                      collector);

  return std::make_optional<bool>(reducer.pass());
}


std::string FastReductionMatcher::toString() const {
  std::stringstream ss;
  ss << "FastReductionMatcher:\n"
     << "  treeVector: " << m_tree << '\n'
     << "FastReductionMatcher Conditions ["
     << m_conditions.size() << "]: \n";

  std::size_t count{0u};
  for(const auto& c : m_conditions){
    auto sc = std::to_string(count++);
    sc.insert(sc.begin(), 3-sc.length(), ' ');
    ss << sc <<": "<< c->toString() + '\n';
  }

  return ss.str();
}
