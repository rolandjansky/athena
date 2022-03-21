/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTUBEINTERSECT_H
#define MUON_MUONTUBEINTERSECT_H

#include "Identifier/Identifier.h"

namespace Muon {

    struct MuonTubeIntersect {
        MuonTubeIntersect(const Identifier& id, double rint, double xint) : tubeId(id), rIntersect(rint), xIntersect(xint) {}
        Identifier tubeId{-1};
        double rIntersect{0.};  // distance to wall in precision plane
        double xIntersect{0.};  // distance to tube end
    };

}  // namespace Muon

#endif
