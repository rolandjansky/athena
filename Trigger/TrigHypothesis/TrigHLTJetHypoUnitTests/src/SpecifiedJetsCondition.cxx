/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./SpecifiedJetsCondition.h"
#include "TrigHLTJetHypo/../src/ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

SpecifiedJetsCondition::SpecifiedJetsCondition(const HypoJetGroupVector& accJets) : m_acceptedJetGroups(accJets){
  m_capacity = accJets[0].size();
}

bool 
SpecifiedJetsCondition::groupIsSatisfied(const HypoJetVector& ips,
					 const HypoJetVector& group,
					 const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {

  for(const auto& j : ips){
    if(std::find(group.begin(), group.end(), j) == group.end()){
      return false;
    }
  }

  return true;
}

bool 
SpecifiedJetsCondition::isSatisfied(const HypoJetVector& ips,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  for(const auto& jg : m_acceptedJetGroups){
    if(groupIsSatisfied(ips, jg, collector)){
      return true;
    }
  }
  return false;
}



std::string SpecifiedJetsCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "SpecifiedJetsCondition (" << this << ") accepted jet groupds: ";
  std::size_t count{0};
  for(const auto& jg : m_acceptedJetGroups){
    ss << "group " << count << '\n';
    ++count;
    for(const auto& j : jg){
      auto j_addr = static_cast<const void*>(j.get());
      ss << j_addr << " ";
    }
  }
  ss << '\n';
  return ss.str();
}
