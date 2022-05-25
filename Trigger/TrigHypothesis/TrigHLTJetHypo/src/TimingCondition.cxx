/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./TimingCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

TimingCondition::TimingCondition(double t_min,
				 double t_max):
  m_min(t_min), m_max(t_max) {
}
  

bool TimingCondition::isSatisfied(const HypoJetVector& ips, const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  if(ips.size() != 1){
    std::stringstream ss;
    ss << "TimingCondition::isSatisfied must see exactly 1 particle, but received "
       << ips.size()
       << '\n';
    throw std::runtime_error(ss.str());
  }

  auto jet = ips[0];

  float timing {0.};
  if(!(jet->getAttribute("Timing",timing))){
    throw std::runtime_error("ERROR: TimingCondition cannot retrieve jet moment 'Timing'");
  }
  bool pass = timing >= m_min and timing < m_max;

  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "TimingCondition: (" << address 
        << ") timing[" << m_min << ", " << m_max << "]" 
        << " pass: " <<std::boolalpha << pass <<  '\n';


    auto j_addr = static_cast<const void*>(jet.get());
    std::stringstream ss1;

    ss1 << "  jet : "  << j_addr << ")  timing "  << timing << '\n';
    collector -> collect(ss0.str(), ss1.str());
  }
  return pass;

}

std::string TimingCondition::toString() const {

  std::stringstream ss;

  const void* address = static_cast<const void*>(this);
  ss << "TimingCondition: (" << address << ") Capacity: " << s_capacity
     << " timingMin "<<  m_min 
     << " timingMax " << m_max 
     <<'\n';
  
  return ss.str();
}
