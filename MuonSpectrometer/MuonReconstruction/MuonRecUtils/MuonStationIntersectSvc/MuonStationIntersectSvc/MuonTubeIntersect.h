/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONTUBEINTERSECT_H
#define MUON_MUONTUBEINTERSECT_H

#include "Identifier/Identifier.h"

namespace Muon {

    struct MuonTubeIntersect {
        MuonTubeIntersect(const Identifier& id, double rint, double xint) : tubeId(id), rIntersect(rint), xIntersect(xint) {}
        Identifier tubeId;
        double rIntersect;  // distance to wall in precision plane
        double xIntersect;  // distance to tube end
    };

}  // namespace Muon

#endif
