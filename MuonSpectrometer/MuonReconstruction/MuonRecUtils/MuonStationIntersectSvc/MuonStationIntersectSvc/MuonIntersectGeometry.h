/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONINTERSECTGEOMETRY_H
#define MUONINTERSECTGEOMETRY_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonStationIntersectSvc/MuonStationIntersect.h"

namespace Muon {

    class MuonIntersectGeometry {
    public:
        virtual ~MuonIntersectGeometry() = default;
        virtual const MuonStationIntersect intersection(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const = 0;
    };

}  // namespace Muon

#endif
