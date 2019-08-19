/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGen.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <sstream>
PartitionsGrouper::PartitionsGrouper(const std::vector<std::size_t>& mults) :
  m_mults(mults){
}


std::vector<HypoJetGroupVector>
PartitionsGrouper::group(HypoJetIter& begin,
			 HypoJetIter& end) const {


  
  std::vector<HypoJetGroupVector> result;

  auto pg = PartitionsGen(end-begin, m_mults);
  while(true){

    auto p = pg.next();
    if(!p.has_value()){break;}

    HypoJetGroupVector hjgv;
    for(const auto& iv : *p){
      // iv is a vector if indices
      HypoJetVector hjv;
      for(const auto i : iv) {
	// i is an index
	hjv.push_back(*(begin + i));
      }
      hjgv.push_back(hjv);
    }

    result.push_back(hjgv);
  }

  return result;

}

std::string PartitionsGrouper::getName() const {
  return "PartitionsGrouper";
}

std::string PartitionsGrouper::toString() const {

  std::stringstream ss;

  ss << "PartitionsGrouper - allocate jets to Conditions. jets/condition: ";
  for(const auto & i : m_mults){ss << i << " ";}
  ss << '\n';
  return ss.str();
}


