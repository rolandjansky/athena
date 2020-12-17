/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./JVTConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

JVTConditionMT::JVTConditionMT(double workingPoint):
  m_workingPoint(workingPoint) {
}
  

bool JVTConditionMT::isSatisfied(const HypoJetVector& ips, const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  if(ips.size() != 1){
    std::stringstream ss;
    ss << "DijetMT::isSatisfied must see exactly 1 particle, but received "
       << ips.size()
       << '\n';
    throw std::runtime_error(ss.str());
  }

  auto jet = ips[0];

  // The conditions for each jet are: JVT>JVTwp or |eta|>2.5 or pT>60
  auto pt      = jet->pt() * 0.001; // MeV -> GeV
  float detEta = 0;
  if(!(jet->getAttribute("DetectorEta",detEta))){
    throw std::runtime_error("JVT condition cannot retrieve variable 'DetectorEta', 'DetectorEta' does not exist");
  }
  auto absdetEta = std::abs(detEta);
  bool jvtApplicable  = (absdetEta<m_maxEta and pt<m_maxPt) ? true : false;
  bool pass = false;
  float jvt = -1.;
  if(!jvtApplicable){ // jvt not applicable
    pass = true;
  } else { // jvt applicable
    if(!(jet->getAttribute("Jvt",jvt))){
      throw std::runtime_error("JVT condition cannot retrieve variable 'Jvt', 'Jvt' does not exist");
    }

    pass = (jvt>m_workingPoint) ? true : false;
  }

  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "JVTConditionMT: (" << address 
        << ") jvt " << jvt
        << " pt "   << pt
        << " absdetEta "  << absdetEta
        << " pass: " <<std::boolalpha << pass <<  " jet group: \n";

    std::stringstream ss1;

    for(const auto& ip : ips){
      address = static_cast<const void*>(ip.get());
      ss1 << "    "  << address << " " << ip->eta() << " pt " << ip->pt() << '\n';
    }
    ss1 << '\n';
    collector -> collect(ss0.str(), ss1.str());
  }
  return pass;

}

std::string JVTConditionMT::toString() const {

  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "JVTConditionMT: (" << address << ") Capacity: " << s_capacity
     << " workingPoint: " << m_workingPoint << '\n';
  
  return ss.str();
}
