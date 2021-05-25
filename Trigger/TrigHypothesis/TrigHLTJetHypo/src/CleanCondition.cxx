/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./CleanCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

CleanCondition::CleanCondition(std::string cName) : m_cName(cName)
{

}

bool CleanCondition::isSatisfied(const pHypoJet& ip,
				const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  bool momentValue;

  if (!(ip->getAttribute(m_cName, momentValue)))
  {

    if (collector)
    {
      auto j_addr = static_cast<const void *>(ip.get());

      std::stringstream ss0;
      ss0 << "CleanCondition: "
          << " unable to retrieve " << m_cName << '\n';
      std::stringstream ss1;
      ss1 << "     jet : (" << j_addr << ")";
      collector->collect(ss0.str(), ss1.str());
    }
    return false;
  }

  bool pass = momentValue;

  if (collector)
  {
    const void *address = static_cast<const void *>(this);

    std::stringstream ss0;
    ss0 << "CleanCondition: (" << address
        << ")"
        << " moment " << m_cName
        << " pass: " << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void *>(ip.get());
    std::stringstream ss1;
    ss1 << "     jet : (" << j_addr << ") "
                                       "moment "
        << m_cName << " value: " << momentValue << '\n';

    collector->collect(ss0.str(), ss1.str());
  }

  return pass;
}


bool 
CleanCondition::isSatisfied(const HypoJetVector& ips,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string CleanCondition::toString() const {
  std::stringstream ss;
  ss << "CleanCondition (" << this << ") "
     << " Cleaning dec: " 
     << m_cName
     <<'\n';

  return ss.str();
}
