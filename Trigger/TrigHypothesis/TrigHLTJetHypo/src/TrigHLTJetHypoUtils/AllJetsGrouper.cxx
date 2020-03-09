
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"

std::vector<HypoJetGroupVector> AllJetsGrouper::group(HypoJetIter& begin,
						      HypoJetIter& end
						      ) const {
  HypoJetGroupVector hjgv{HypoJetVector(begin, end)};
  return std::vector<HypoJetGroupVector>{hjgv};
}

std::optional<HypoJetGroupVector> AllJetsGrouper::next(HypoJetIter& begin,
						       HypoJetIter& end
						       ) const {
  HypoJetGroupVector hjgv{HypoJetVector(begin, end)};
  return std::make_optional<HypoJetGroupVector>(hjgv);
}

std::string AllJetsGrouper::getName() const {
  return "AllJetsGrouper";
}

std::string AllJetsGrouper::toString() const {
  return "AllJetsGrouper - repack all input jets into vector of vectors, sole element is vector of all jets";
}


