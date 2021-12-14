/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CURVEDSEGMENTFINDER_H
#define DCMATH_CURVEDSEGMENTFINDER_H

#include <vector>

#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/IsSubsetSegment.h"
#include "TrkDriftCircleMath/ResolvedCollection.h"
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/SortDcsByY.h"
//#include "TrkDriftCircleMath/MdtChamberGeometry.h"

namespace TrkDriftCircleMath {

    class CurvedSegmentFinder {
    public:
        CurvedSegmentFinder(int debugLevel) : m_debugLevel(debugLevel) {}

        ~CurvedSegmentFinder() = default;

        void setMaxCurvatureParameters(double maxDeltaAlpha, double maxDeltab) {
            m_maxDeltaAlpha = maxDeltaAlpha;
            m_maxDeltab = maxDeltab;
        }
        void curvedSegments(const ChamberGeometry& mdtGeo, SegVec& Segs) const;

    private:
        int m_debugLevel{0};
        double m_maxDeltaAlpha{0.1};
        double m_maxDeltab{3.};

    };  // end CurvedSegmentFinder class

}  // namespace TrkDriftCircleMath
#endif  // DCMATH_CURVEDSEGMENTFINDER_H
