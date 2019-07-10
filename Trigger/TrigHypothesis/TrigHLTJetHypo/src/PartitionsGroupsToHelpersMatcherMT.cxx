/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PartitionsGroupsToHelpersMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./PartitionsGroupsToHelpersMatcherMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"

#include <cassert>
#include <algorithm>

PartitionsGroupsToHelpersMatcherMT::PartitionsGroupsToHelpersMatcherMT(const HelperToolArray& helpers):
  m_helpers(helpers){
}

std::optional<bool>
PartitionsGroupsToHelpersMatcherMT::match(const HypoJetGroupCIter& groups_b,
					    const HypoJetGroupCIter& groups_e,
					    xAODJetCollector& jetCollector,
					    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
					    bool) const {

  if(collector){
    collector->collect("PartitionsGroupsToHelpersMatcher", toString());
  }
  
  if(m_helpers.empty()){
    if(collector){
      collector->collect("PartitionsGroupsToHelpersMatcherMT", "Zero conditions");
    }
    return std::make_optional<bool>(false);
  }
  
 
  if (groupsPass(groups_b,
		 groups_e,
		 jetCollector,
		 collector)){
    std::for_each(groups_b, groups_e, [&jetCollector](auto& jv){
	jetCollector.addJets(jv.begin(), jv.end());});
    return std::make_optional<bool>(true);  
  }
  
  return std::make_optional<bool>(false);  
} 


std::string PartitionsGroupsToHelpersMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "PartitionsGroupsToHelpersMatcherMT. No of helpers: "
     << m_helpers.size() << '\n';
  for(auto h : m_helpers){ ss <<"  "<< h->toString() << '\n';}
  return ss.str();
}


bool
PartitionsGroupsToHelpersMatcherMT::groupsPass(const HypoJetGroupCIter& groups_b,
						 const HypoJetGroupCIter& groups_e,
						 xAODJetCollector& jetCollector,
						 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  auto n_jetgroups = static_cast<std::size_t>(groups_e - groups_b);

  // test whether this set of job groups pass the conditions
  // note - this method may be called more than once with different
  // collections of job groups.


  assert(n_jetgroups == m_helpers.size());
  for(long unsigned int i = 0; i != n_jetgroups; ++i){
    // step through conditions and job groups simulataneously, and
    // enquire whethwer each conidtion is satisfied by its corresponding
    // job group.
    const auto& helper = *(m_helpers.begin() + i);
    auto group = *(groups_b + i);

    if(!(helper->pass(group, jetCollector, collector))){return false;}
  }
  return true;
} 
  

