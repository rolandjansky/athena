/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SORTDCSBYY_H
#define DCMATH_SORTDCSBYY_H

#include <functional>
#include <iostream>

#include "CxxUtils/fpcompare.h"
#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/DriftCircle.h"

namespace TrkDriftCircleMath {

    struct SortDcsByY {
        bool operator()(const DriftCircle& dc1, const DriftCircle& dc2) const {
            if (dc1.id() == dc2.id()) return false;
            if (CxxUtils::fpcompare::less(dc1.y(), dc2.y())) return true;
            if (CxxUtils::fpcompare::less(dc2.y(), dc1.y())) return false;
            return CxxUtils::fpcompare::less(dc1.x(), dc2.x());
        }
    };

    struct SameTube {
        bool operator()(const DriftCircle& dc1, const DriftCircle& dc2) const {
            if (dc1.id() == dc2.id()) return true;
            return false;
        }
    };

    struct NeighbourTube {
        bool operator()(const DriftCircle& dc1, const DriftCircle& dc2) const {
            // check whether this is not the same tube
            SameTube sameTube;
            if (sameTube(dc1, dc2)) return false;

            // check whether in the same layer
            if (std::abs(dc1.y() - dc2.y()) > 1.) return false;

            // check whether direct neighbours
            if (std::abs(dc1.x() - dc2.x()) > 31.) return false;

            return true;
        }
    };
}  // namespace TrkDriftCircleMath
#endif
