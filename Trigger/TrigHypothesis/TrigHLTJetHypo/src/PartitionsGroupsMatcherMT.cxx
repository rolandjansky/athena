/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PartitionsGroupsMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./PartitionsGroupsMatcherMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"

// #include <cmath>
// #include <sstream>
#include <algorithm>
// #include <map>

PartitionsGroupsMatcherMT::PartitionsGroupsMatcherMT(ConditionsMT&& cs):
  m_conditions(std::move(cs)), m_nConditions(m_conditions.size()){
  std::size_t minNjets{0};
  for(const auto& c : m_conditions){minNjets += c->capacity();}
  m_minNjets = minNjets;
}

std::optional<bool>
PartitionsGroupsMatcherMT::match(const HypoJetGroupCIter& groups_b,
				 const HypoJetGroupCIter& groups_e,
				 xAODJetCollector& jetCollector,
				 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				 bool) const {
  


  // minimal input correctness checks. More detailed checking
  // responsibility of the caller.

  auto iter_diff = groups_e - groups_b;
  if (iter_diff < 0){return std::optional<bool>();}
  auto n_jetvecs = static_cast<std::size_t>(iter_diff);
  auto n_conditions = m_conditions.size();
  
  if(n_conditions != n_jetvecs){
    if(collector){
      std::string msg = "Number of HypoJetVectors " +
	std::to_string(n_jetvecs) + " !=  number of children " +
	std::to_string(n_conditions);
      
      collector->collect("PartitionsGroupsMatcherMT", msg);
    }
    return std::optional<bool>();
  }

  for(std::size_t i = 0;  i != n_conditions; ++i){
    // Pass each job group to its corresposonding child, check if pass.
    // Return false any child does not pass.
    if (!m_conditions[i]->isSatisfied(*(groups_b +i), collector)){
      return std::make_optional<bool>(false);
    }

    // passed....
    if(collector){
      collector->collect("PartitionsGroupsMatcherMT", "passed");
    }
    for(auto iter = groups_b; iter != groups_e; ++iter){
      jetCollector.addJets((*iter).begin(), (*iter).end());
    }
  }
  
  return std::make_optional<bool>(true);  
} 


std::string PartitionsGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "PartitionsMatcherMT. No of conditions: "
     << m_conditions.size() << '\n';
  for(const auto& c : m_conditions){ ss <<"  "<< c->toString() << '\n';}
  return ss.str();
}


