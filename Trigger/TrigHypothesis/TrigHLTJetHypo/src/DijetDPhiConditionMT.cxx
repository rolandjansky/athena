/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./DijetDPhiConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

DijetDPhiConditionMT::DijetDPhiConditionMT(double min,
					   double max) :
  m_min(min), m_max(max) {
}
  

bool
DijetDPhiConditionMT::isSatisfied(const HypoJetVector& ips,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  if(ips.size() != 2){
    std::stringstream ss;
    ss << "DijetMT::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto j0 = ips[0];
  auto j1 = ips[1];

  auto rj0 = j0 -> p4();
  auto rj1 = j1 -> p4();

  auto dphi = std::abs(rj0.DeltaPhi(rj1));


  bool pass = dphi >= m_min and dphi < m_max;

  
   if(collector){
     std::stringstream ss0;
     const void* address = static_cast<const void*>(this);
     ss0 << "DijetDPhiConditionMT: (" << address << ") dphi "
	 << dphi  <<  " pass: "  <<std::boolalpha << pass <<  " jet group: \n";
     
     std::stringstream ss1;
     
     for(const auto& ip : ips){
       address = static_cast<const void*>(ip.get());
       ss1 << "    "  << address << " " << ip->eta() << " e " << ip->e() << '\n';
     }
     ss1 << '\n';
     collector -> collect(ss0.str(), ss1.str());
   }
   return pass;
   
}

std::string DijetDPhiConditionMT::toString() const {


  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  
  ss << "DijetDPhiConditionMT: (" << address << ") Capacity: " << s_capacity
     << " dPhi min: " << m_min << " dPhi max: " << m_max <<'\n';
  
  return ss.str();
}
