/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./CompoundConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

CompoundConditionMT::CompoundConditionMT(std::vector<ConditionMT>& elements){

  for(auto& el : elements){m_elements.push_back(std::move(el));}
  unsigned int capacity{0};

  // mis-configuration of elemental Conditions with differing capacities
  // not caught...
  if(!m_elements.empty()){
    capacity = (m_elements.front())->capacity();
  }
  m_capacity = capacity;
}


bool CompoundConditionMT::isSatisfied(const HypoJetVector& ips,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  bool pass{true};
  for(const auto& el : m_elements){
    pass = pass and (el->isSatisfied(ips, collector));
    if(!pass){break;}
  }
  
  
  if(collector){
    const void* address = static_cast<const void*>(this);
    
    std::stringstream ss0;
    ss0 << "CompoundCondition: (" << address << ") " 
        << " pass: "  << std::boolalpha << pass << '\n';

    for(const auto& ip : ips){
      auto j_addr = static_cast<const void*>(ip.get());
      std::stringstream ss1;
      ss1 <<  "     jet : ("<< j_addr << ")\n";
    
      collector->collect(ss0.str(), ss1.str());
    }
  }
  return pass;
}


std::string CompoundConditionMT::toString() const {
  std::stringstream ss;
  ss << "CompoundConditionMT (" << this << ") Capacity: " << m_capacity << '\n';
  for(const auto& el :m_elements){
    ss << el->toString() + '\n';
  }
  return ss.str();
}
