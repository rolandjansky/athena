/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"

HypoJetGroupVector AllJetsGrouper::group(HypoJetIter& begin,
                                           HypoJetIter& end
                                           ) const {
  return HypoJetGroupVector{HypoJetVector(begin, end)};
}

std::string AllJetsGrouper::getName() const {
  return "AllJetsGrouper";
}

std::string AllJetsGrouper::toString() const {
  return "AllJetsGrouper - repack all input jets into vector of vectors, sole element is vector of all jets";
}


