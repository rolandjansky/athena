/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Position.h"

namespace MuonGM {

    Position::Position() {
        icut = 0;
        subtype = 0;
        phitype = 0;
        zindex = 0;
        phiindex = 0;
        phi = 0.;
        radius = 0.;
        z = 0.;
        shift = 0.;
        inclination = 0.;
        alpha = 0.;
        beta = 0.;
        gamma = 0.;
        isMirrored = false;
        isBarrelLike = false; // only needed if Pb positioners used...

        //
        isAssigned = false;
        //
    }

    std::ostream &operator<<(std::ostream &os, const Position &p) {
        os << " Position eta/phi index " << p.zindex << "/" << p.phiindex << " phi/z/R/s_shift " << p.phi << " " << p.z << " " << p.radius << " " << p.shift << " alpha/beta/gamma "
           << p.alpha << " " << p.beta << " " << p.gamma;

        return os;
    }
} // namespace MuonGM
