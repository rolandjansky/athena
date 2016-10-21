/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGen.h"

#include <sstream>

CombinationsGrouper::CombinationsGrouper(unsigned int groupSize):
  m_groupSize(groupSize){
}


HypoJetGroupVector CombinationsGrouper::group(HypoJetIter& begin,
                                              HypoJetIter& end) const {
  HypoJetGroupVector result;
  
  // create a combinations generator. Used to select the jets
  // to be tested by the condition objects
  CombinationsGen combgen(end-begin, m_groupSize);
  
  while(true){
    auto combs = combgen.next();
    if (combs.second == false) {break;}
    
    HypoJetVector v;
    for(auto i : combs.first){ v.push_back(*(begin + i));}
    result.push_back(v);
  }

  return result;
}

std::string CombinationsGrouper::getName() const {
  return "CombinationsGrouper";
}

std::string CombinationsGrouper::toString() const {
  std::stringstream ss;
  ss << "CombinationsGrouper - create all combinations of ";
  ss << "jets of length " ;
  ss << m_groupSize << '\n';
  return ss.str();
}


