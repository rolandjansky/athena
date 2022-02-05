/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPattern/MuonPatternChamberIntersect.h"

namespace Muon {

    MuonPatternChamberIntersect::MuonPatternChamberIntersect(const Trk::TrackSurfaceIntersection& intersect,
                                                             const std::vector<const Trk::PrepRawData*>& rios) :
        m_intersect(intersect), m_rios(rios) {
        std::sort(m_rios.begin(), m_rios.end(), SortMuonPrepData());
    }

    MuonPatternChamberIntersect::MuonPatternChamberIntersect(const Amg::Vector3D& pos, const Amg::Vector3D& dir,
                                                             const std::vector<const Trk::PrepRawData*>& rios) :
        m_intersect(pos, dir, 0.), m_rios(rios) {
        std::sort(m_rios.begin(), m_rios.end(), SortMuonPrepData());
    }

}  // namespace Muon
