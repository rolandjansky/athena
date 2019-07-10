/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CombinationsGroupsToHelpersMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./CombinationsGroupsToHelpersMatcherMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"

#include <cassert>
#include <algorithm>

CombinationsGroupsToHelpersMatcherMT::CombinationsGroupsToHelpersMatcherMT(const HelperToolArray& helpers):
  m_helpers(helpers){
}

std::optional<bool>
CombinationsGroupsToHelpersMatcherMT::match(const HypoJetGroupCIter& groups_b,
					    const HypoJetGroupCIter& groups_e,
					    xAODJetCollector& jetCollector,
					    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
					    bool) const {

  if(collector){
    collector->collect("CombinationsGroupsToHelpersMatcher", toString());
  }
  
  if(m_helpers.empty()){
    if(collector){
      collector->collect("CombinationsGroupsToHelpersMatcherMT", "Zero conditions");
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


std::string CombinationsGroupsToHelpersMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "CombinationsGroupsToHelpersMatcherMT. "
     << "Passes each hypojet vector to all helpers in turn."
     << "\n No of helpers: "
     << m_helpers.size() << "\n\n";

  unsigned short ih{0};
  for(auto h : m_helpers){
    ss <<"Helper " << ih <<":\n"<< h->toString() << '\n';
    ++ih;
  }
  
  return ss.str();
}


bool
CombinationsGroupsToHelpersMatcherMT::groupsPass(const HypoJetGroupCIter& groups_b,
						 const HypoJetGroupCIter& groups_e,
						 xAODJetCollector& jetCollector,
						 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  auto n_jetgroups = static_cast<std::size_t>(groups_e - groups_b);

  assert(n_jetgroups ==1);
  // test whether this set of job groups pass the conditions
  // note - this method may be called more than once with different
  // collections of job groups.


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
  

