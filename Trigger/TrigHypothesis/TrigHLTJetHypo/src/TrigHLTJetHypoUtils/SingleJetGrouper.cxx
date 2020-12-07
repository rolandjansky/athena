/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"


SingleJetGrouper::SingleJetGrouper(){}

SingleJetGrouper::SingleJetGrouper(const HypoJetVector& v): m_jets(v){
}

SingleJetGrouper::SingleJetGrouper(const HypoJetCIter& b,
				   const HypoJetCIter& e):
  m_jets(b, e){
}

std::vector<HypoJetGroupVector> SingleJetGrouper::group(HypoJetIter& begin,
							HypoJetIter& end
							) const {
  HypoJetGroupVector hjgv;
  for(; begin != end; ++begin){
    HypoJetVector v;
    v.push_back(*begin);
    hjgv.push_back(v);
  }
   
  return std::vector<HypoJetGroupVector>{hjgv};
}

std::optional<HypoJetGroupVector>
SingleJetGrouper::next() {
  if (m_index == m_size){
    return std::optional<HypoJetGroupVector>();
  }
  
  HypoJetGroupVector result;
  result.push_back(HypoJetVector{m_jets[m_index++]});
  return std::make_optional<HypoJetGroupVector>(result);
}

std::string SingleJetGrouper::getName() const {
  return "SingleJetGrouper";
}

std::string SingleJetGrouper::toString() const {
  return "SingleJetGrouper - repack input jets into vectors of length 1";
}


