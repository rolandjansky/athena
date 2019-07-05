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

PartitionsGroupsMatcherMT::PartitionsGroupsMatcherMT(const ConditionsMT& cs):
  m_conditions(cs), m_nConditions(cs.size()){
  std::size_t minNjets{0};
  for(const auto& c : m_conditions){minNjets += c.capacity();}
  m_minNjets = minNjets;
}

std::optional<bool>
PartitionsGroupsMatcherMT::match(const HypoJetGroupCIter& groups_b,
				 const HypoJetGroupCIter& groups_e,
				 xAODJetCollector& jetCollector,
				 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
				 bool) const {

  if(collector){
    collector->collect("PartitionsGroupsMatcher", toString());
  }
  
  if(!m_nConditions){
    if(collector){
      collector->collect("PartitionsGroupsMatcherMT", "Zero conditions");
    }
    return std::make_optional<bool>(false);
  }
  
  // Determine whether the number of jets is sufficient to satisfy
  // the conditions

  auto njets =
    static_cast<unsigned int> (std::accumulate(groups_b,
					       groups_e,
					       0,
					       [](int sum, auto v){
						 return sum + v.size();}));
  
  if(njets < m_minNjets){
    if(collector){
      collector->collect("PartitionsGroupsMatcherMT",
			 "Too few jets: " +
			 std::to_string(njets) + " required: " +
			 std::to_string(m_minNjets));
    }
    return std::make_optional<bool>(false);
  }


  if (groupsPass(groups_b,
		 groups_e,
		 collector)){
    std::for_each(groups_b, groups_e, [&jetCollector](auto& jv){
	jetCollector.addJets(jv.begin(), jv.end());});
    return std::make_optional<bool>(true);  
  }
  
  return std::make_optional<bool>(false);  
} 


std::string PartitionsGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "PartitionsMatcherMT. No of conditions: "
     << m_conditions.size() << '\n';
  for(auto c : m_conditions){ ss <<"  "<< c.toString() << '\n';}
  return ss.str();
}


ConditionsMT PartitionsGroupsMatcherMT::getConditions() const noexcept {
  return m_conditions;
}

bool
PartitionsGroupsMatcherMT::groupsPass(const HypoJetGroupCIter& groups_b,
				      const HypoJetGroupCIter& groups_e,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  auto n_jetgroups = static_cast<std::size_t>(groups_e - groups_b);

  // test whether this set of job groups pass the conditions
  // note - this method may be called more than once with different
  // collections of job groups.


  for(long unsigned int i = 0; i != n_jetgroups; ++i){
    // step through conditions and job groups simulataneously, and
    // enquire whethwer each conidtion is satisfied by its corresponding
    // job group.
    const auto& condition = *(m_conditions.begin() + i);
    const auto& group = *(groups_b + i);

    if(!condition.isSatisfied(group, collector)){return false;}
  }
  return true;
} 
  

