/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./MomentCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

MomentCondition::MomentCondition(double mMin,
				     double mMax,
				     const std::string& moment) :
m_min(mMin), m_max(mMax), m_moment(moment) {
}


bool MomentCondition::isSatisfied(const pHypoJet& ip,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  float momentValue;

  if (!(ip -> getAttribute(m_moment, momentValue))){

    if(collector){
      auto j_addr = static_cast<const void*>(ip.get());

      std::stringstream ss0;
      ss0 << "MomentCondition: " 
	  << " unable to retrieve " << m_moment << '\n';
      std::stringstream ss1;
      ss1 <<  "     jet : ("<< j_addr << ")";
      collector->collect(ss0.str(), ss1.str());
    }
    return false;
  }

  bool pass = m_min <= momentValue and m_max > momentValue;

  
  if(collector){
    const void* address = static_cast<const void*>(this);

    std::stringstream ss0;
    ss0 << "MomentCondition: ("<< address 
        << ") min cut value " << m_min
        << " max cut value " << m_max
        << " moment " << m_moment
        << " pass: "  << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void*>(ip.get());
    std::stringstream ss1;
    ss1 <<  "     jet : ("<< j_addr << ") "
      "moment " << m_moment << " value: " << momentValue<< '\n';
    
    collector->collect(ss0.str(), ss1.str());

  }
  return pass;
}

bool 
MomentCondition::isSatisfied(const HypoJetVector& ips,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string MomentCondition::toString() const {
  std::stringstream ss;
  ss << "MomentCondition (" << this << ") "
     << " moment: " << m_moment 
     << " min " << m_min
     << " max " << m_max
     <<'\n';

  return ss.str();
}
