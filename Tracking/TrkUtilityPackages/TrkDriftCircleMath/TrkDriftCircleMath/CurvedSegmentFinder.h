/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CURVEDSEGMENTFINDER_H
#define DCMATH_CURVEDSEGMENTFINDER_H

#include <vector>
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/ResolvedCollection.h"
#include "TrkDriftCircleMath/SortDcsByY.h"
#include "TrkDriftCircleMath/IsSubsetSegment.h"
#include "TrkDriftCircleMath/ChamberGeometry.h"
//#include "TrkDriftCircleMath/MdtChamberGeometry.h"

namespace TrkDriftCircleMath {

  class CurvedSegmentFinder {    
  public:
    CurvedSegmentFinder(int debugLevel)
      : m_debugLevel(debugLevel),m_maxDeltaAlpha(0.1),m_maxDeltab(3) {}

    ~CurvedSegmentFinder() {}
    
    void setMaxCurvatureParameters(double maxDeltaAlpha, double maxDeltab) {
      m_maxDeltaAlpha = maxDeltaAlpha;
      m_maxDeltab = maxDeltab;
    }
    void curvedSegments(const ChamberGeometry& mdtGeo, SegVec& Segs);
    

  private:
    int m_debugLevel;
    double m_maxDeltaAlpha;
    double m_maxDeltab;

  };//end CurvedSegmentFinder class



}//end namespace
#endif //DCMATH_CURVEDSEGMENTFINDER_H
