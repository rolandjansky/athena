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

SpecifiedJetsCondition::SpecifiedJetsCondition(const HypoJetVector& accJets,
					       std::size_t conditionID):
m_acceptedJets(accJets), m_conditionID(conditionID){
  m_capacity = accJets.size();
}


bool 
SpecifiedJetsCondition::isSatisfied(const HypoJetVector& ips,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  std::cerr<<toString();
  std::cerr<<"SpecifiedJetsCondition::isSatisfied seeking ";
  for(const auto& j : ips){
    auto j_addr = static_cast<const void*>(j);
    std::cerr<< j_addr << " ";
      
    if(std::find(m_acceptedJets.begin(),
		 m_acceptedJets.end(), j) == m_acceptedJets.end()){
      std::cerr << "not found\n";
      return false;
    }
  }
  std::cerr << "found\n";
  return true;
}



std::string SpecifiedJetsCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "SpecifiedJetsCondition (" << this << ") accepted jets: ";
  for(const auto& j : m_acceptedJets){
    auto j_addr = static_cast<const void*>(j);
    ss << j_addr << " ";
  }
  ss << '\n';
  return ss.str();
}
