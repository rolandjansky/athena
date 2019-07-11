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

  /*
   * groups_b ... groups_e are iterators that point at HypoJetVectors.
   * This matcher takes each HypoJetVector in turn, and presents it
   * to each of the child objects of type ITrigJetHypoToolHelperMT.
   * The matcher reports pass = true as soon as ot finds a HypoJetVector
   * that satisfies all the children
   */
  
  if(m_helpers.empty()){
    if(collector){
      collector->collect("CombinationsGroupsToHelpersMatcherMT",
			 "Zero conditions");
    }
    return std::make_optional<bool>(false);
  }
  
  if(collector){
    collector->collect("CombinationsGroupsToHelpersMatcherMT",
		       "number of groups: " +
		       std::to_string(groups_e - groups_b));
  }

  for(auto iter = groups_b; iter != groups_e; ++iter){
    if (groupPasses(*iter,
		    jetCollector,
		    collector)){
      jetCollector.addJets((*iter).begin(), (*iter).end());
      return std::make_optional<bool>(true);  
    }
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
CombinationsGroupsToHelpersMatcherMT::groupPasses(const HypoJetVector& group,
						  xAODJetCollector& jetCollector,
						  const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  // test whether this jet vector satisfies the helper chilldren.
  // the pass method of the children requires a non-constant hypo jet
  // vector, so make a copy.

  HypoJetVector jv(group.begin(), group.end());

  for(const auto& h : m_helpers){
    if(!(h->pass(jv, jetCollector, collector))){return false;}
  }
  return true;
} 
  

