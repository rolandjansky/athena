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

SpecifiedJetsCondition::SpecifiedJetsCondition(const HypoJetGroupVector& accJets,
					       std::size_t conditionID):
m_acceptedJetGroups(accJets), m_conditionID(conditionID){
  m_capacity = accJets[0].size();
}

bool 
SpecifiedJetsCondition::groupIsSatisfied(const HypoJetVector& ips,
					 const HypoJetVector& group,
					 const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  std::cerr<<"SpecifiedJetsCondition::isSatisfied seeking ";
  for(const auto& j : ips){
    auto j_addr = static_cast<const void*>(j);
    std::cerr<< j_addr << " ";
    if(std::find(group.begin(), group.end(), j) == group.end()){
      return false;
    }
  }
  return true;
}

bool 
SpecifiedJetsCondition::isSatisfied(const HypoJetVector& ips,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  std::cerr<<toString();
  std::cerr<<"SpecifiedJetsCondition::isSatisfied seeking ";
  std::size_t count{0};
  for(const auto& jg : m_acceptedJetGroups){
    if(groupIsSatisfied(ips, jg, collector)){
      std::cerr <<"SpecifiedJetsCondition::isSatisfied job group "
		<< count << " is satisfied ";
      ++count;
      return true;
    }
  }
  std::cerr<<"SpecifiedJetsCondition::isSatisfied no job group satisfied ";
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
      auto j_addr = static_cast<const void*>(j);
      ss << j_addr << " ";
    }
  }
  ss << '\n';
  return ss.str();
}
