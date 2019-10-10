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

  /*
   * groups_b ... groups_e are iterators that point at HypoJetVectors.
   * It is required that the number of hypoJetVectors = the number of
   * ITrigJetHypoToolHelperMT children.
   * the pass requirement is that,
   * for  each i in range(0, n_children),  the ith child passes the ith 
   * HypoJetVector.
   */
  
 
  // minimal input correctness checks. More detailed checking
  // responsibility of the caller.
  auto iter_diff = groups_e - groups_b;
  if (iter_diff < 0){return std::optional<bool>();}
  auto n_jetvecs = static_cast<std::size_t>(iter_diff);
  auto n_helpers = m_helpers.size();

  if(n_helpers != n_jetvecs){
    if(collector){
      std::string msg = "Number of HypoJetVectors " +
	std::to_string(n_jetvecs) + " !=  number of children " +
	std::to_string(n_helpers);
      
      collector->collect("PartitionsGroupsToHelpersMatcherMT", msg);
    }
    return std::optional<bool>();
  }


  for(std::size_t i = 0;  i != n_helpers; ++i){
    // Pass each job group to its corresposonding child, check if pass.
    // Return false any child does not pass.

    // the IGroupsMatcher species that the input HypoJetVectors are constant,
    // so make a copy...

    HypoJetVector jv((*(groups_b +i)).begin(), (*(groups_b +i)).end());
    if(collector){
      std::string msg{"jets: group " +std::to_string(i)};
      for(const auto& j :  jv){
	msg += " " + std::to_string(j->et());
      }
      collector->collect("PartitionsGroupsToHelpersMatcherMT", msg);
    }
    
    if (!m_helpers[i]->pass(jv,
			    jetCollector,
			    collector)){
      
      if(collector){
	collector->collect("PartitionsGroupsToHelpersMatcherMT", "fail");
      }
      return std::make_optional<bool>(false);
    }
  }
  
  // passed....
  for(auto iter = groups_b; iter != groups_e; ++iter){
    jetCollector.addJets((*iter).begin(), (*iter).end());
  }
  
  if(collector){
    collector->collect("PartitionsGroupsToHelpersMatcherMT", "pass");
  }
  return std::make_optional<bool>(true);  
}


std::string PartitionsGroupsToHelpersMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "PartitionsGroupsToHelpersMatcherMT. "
     << "Create a 1-1 matching between HypoJetVectors and child helpers\n"
     << "Pass each hypojetvector to its correponding child."
     << "\n No of helpers: "
     << m_helpers.size() << "\n\n";

  unsigned short ih{0};
  for(auto h : m_helpers){
    ss <<"Helper " << ih <<":\n"<< h->toString() << '\n';
    ++ih;
  }
  
  return ss.str();
}

  

