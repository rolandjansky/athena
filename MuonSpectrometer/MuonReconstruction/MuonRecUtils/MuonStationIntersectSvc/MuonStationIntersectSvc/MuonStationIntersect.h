/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTATIONINTERSECT_H
#define MUON_MUONSTATIONINTERSECT_H

#include "MuonStationIntersectSvc/MuonTubeIntersect.h"

namespace Muon {

    class MuonStationIntersect {
    public:
        typedef std::vector<MuonTubeIntersect> TubeIntersects;

    public:
        MuonStationIntersect() {}
        MuonStationIntersect(const TubeIntersects& intersects) : m_intersects(intersects) {}

        const TubeIntersects& tubeIntersects() const { return m_intersects; }

        void setTubeIntersects(TubeIntersects intersects) { m_intersects.swap(intersects); }

    private:
        TubeIntersects m_intersects;
    };

}  // namespace Muon
#endif
