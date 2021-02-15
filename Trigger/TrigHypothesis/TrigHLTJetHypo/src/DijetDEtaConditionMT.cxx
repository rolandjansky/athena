/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./DijetDEtaConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

DijetDEtaConditionMT::DijetDEtaConditionMT(double min,
					   double max):
  m_min(min),
  m_max(max) {
}
  

bool
DijetDEtaConditionMT::isSatisfied(const HypoJetVector& ips,
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

  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();
  auto adeta = std::abs(eta0 -eta1);

  bool pass = adeta >= m_min and adeta < m_max;

   if(collector){
     std::stringstream ss0;
     const void* address = static_cast<const void*>(this);
     ss0 << "DijetDEtaConditionMT: (" << address 
	 << ") adeta "  << adeta
	 <<  " pass: " <<std::boolalpha << pass <<  " jet group: \n";

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

std::string DijetDEtaConditionMT::toString() const {


  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "DijetDEtaConditionMT: (" << address << ") Capacity: " << s_capacity
     << " dEta min: " << m_min
     << " dEta max: "  << m_max <<'\n';
  
  return ss.str();
}
