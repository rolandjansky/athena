/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGen.h"

#include <sstream>

CombinationsGrouper::CombinationsGrouper(){}

CombinationsGrouper::CombinationsGrouper(unsigned int groupSize):
  m_groupSize{groupSize}{}


CombinationsGrouper::CombinationsGrouper(unsigned int groupSize,
					 const HypoJetVector& jets):
  m_groupSize(groupSize), m_jets{jets}{
}


CombinationsGrouper::CombinationsGrouper(unsigned int groupSize,
					 const HypoJetCIter& b,
					 const HypoJetCIter& e):
  m_groupSize(groupSize), m_jets{b, e}{
}


std::vector<HypoJetGroupVector>
CombinationsGrouper::group(HypoJetIter& begin, HypoJetIter& end) const {
  HypoJetGroupVector hjgv;
  
  // create a combinations generator. Used to select the jets
  // to be tested by the condition objects
  CombinationsGen combgen(end-begin, m_groupSize);
  
  while(true){
    auto combs = combgen.next();
    if (combs.second == false) {break;}
    
    HypoJetVector v;
    for(auto i : combs.first){ v.push_back(*(begin + i));}
    hjgv.push_back(v);
  }

  return std::vector<HypoJetGroupVector>{hjgv};
}

std::optional<HypoJetVector>
CombinationsGrouper::next() {
  HypoJetGroupVector hjgv;
  
  // create a combinations generator. Used to select the jets
  // to be tested by the condition objects
  CombinationsGen combgen(m_jets.size(), m_groupSize);
  
  auto combs = combgen.next();
  if (combs.second == false){
    std::optional<HypoJetVector>();
  }
  
  HypoJetVector v;
  for(auto i : combs.first){ v.push_back(*(m_jets.begin() + i));}
  
  return std::make_optional<HypoJetVector>(v);
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


