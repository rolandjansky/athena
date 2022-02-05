/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraUtils/MuonCalibSimpleGeometry.h"

#include <iostream>

namespace {
    constexpr double z_match_tol = 0.00001;
}
namespace MuonCalib {

    bool MuonCalibDisk::inBounds(const Amg::Vector3D& pos, double tolerance) const {
        // check whether at same z
        if (std::abs(pos.z() - zPos) > z_match_tol) return false;

        // check radius distance
        if (pos.perp() - radius > tolerance) return false;

        return true;
    }

    bool MuonCalibCylinder::inBounds(const Amg::Vector3D& pos, double tolerance) const {
        // check radius distance
        if (std::abs(pos.perp() - radius) > z_match_tol) return false;

        // check whether at same z
        if (std::abs(pos.z()) - lenght > tolerance) return false;

        return true;
    }

}  // namespace MuonCalib
