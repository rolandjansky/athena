/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCSCSegmentInfo.h"

#include <algorithm>
#include <cmath>

namespace Muon {

    bool MCSCSegmentInfo::contains(const MuonPatternCombination* pat) const {
        std::vector<const MuonPatternCombination*>::const_iterator result = find(patPointers.begin(), patPointers.end(), pat);
        if (result == patPointers.end()) return false;
        return true;
    }

    std::ostream& MCSCSegmentInfo::print(std::ostream& os) const {
        double p = fabs(1. / invcurvature) / 10.;
        os << " SegInfo:  nCsc " << nCsc << " nHots " << nHots << " missed Hits " << nMissedHits << " nMult1 " << nMult1 << " nMult2 "
           << nMult2 << " chi2 " << chi2 << " phiPatHits " << phiPatHits << " pattern phi " << patPhi << " theta " << patTheta
           << " pattern momentum " << patMomentum << " station code " << stationCode << " name " << station << " momentum MeV " << p;
        return os;
    }
}  // namespace Muon

std::ostream& operator<<(std::ostream& os, const Muon::MCSCSegmentInfo& info) {
    info.print(os);
    return os;
}
