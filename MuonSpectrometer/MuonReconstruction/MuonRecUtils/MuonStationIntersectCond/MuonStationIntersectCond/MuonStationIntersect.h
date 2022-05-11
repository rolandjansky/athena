/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTATIONINTERSECT_H
#define MUON_MUONSTATIONINTERSECT_H

#include "MuonStationIntersectCond/MuonTubeIntersect.h"

namespace Muon {

    class MuonStationIntersect {
    public:
        using TubeIntersects = std::vector<MuonTubeIntersect>;

    public:
        MuonStationIntersect() = default;
        MuonStationIntersect(TubeIntersects&& intersects) : m_intersects{std::move(intersects)} {}

        const TubeIntersects& tubeIntersects() const { return m_intersects; }

        void setTubeIntersects(TubeIntersects intersects) { m_intersects.swap(intersects); }

    private:
        TubeIntersects m_intersects;
    };

}  // namespace Muon
#endif
