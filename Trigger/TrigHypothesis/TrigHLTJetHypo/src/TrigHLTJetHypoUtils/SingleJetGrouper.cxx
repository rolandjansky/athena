/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"

HypoJetGroupVector SingleJetGrouper::group(HypoJetIter& begin,
                                           HypoJetIter& end
                                           ) const {
  HypoJetGroupVector result;
  for(; begin != end; ++begin){
    HypoJetVector v;
    v.push_back(*begin);
    result.push_back(v);
  }
   
  return result;
}

std::string SingleJetGrouper::getName() const {
  return "SingleJetGrouper";
}

std::string SingleJetGrouper::toString() const {
  return "SingleJetGrouper - repack input jets into vectors of length 1";
}


