/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"

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
SingleJetGrouper::next(HypoJetIter& begin,
		       HypoJetIter& end
		       ) const {
  HypoJetGroupVector hjgv;
  for(; begin != end; ++begin){
    HypoJetVector v;
    v.push_back(*begin);
    hjgv.push_back(v);
  }
  
  return std::make_optional<HypoJetGroupVector>(hjgv);
}

std::string SingleJetGrouper::getName() const {
  return "SingleJetGrouper";
}

std::string SingleJetGrouper::toString() const {
  return "SingleJetGrouper - repack input jets into vectors of length 1";
}


