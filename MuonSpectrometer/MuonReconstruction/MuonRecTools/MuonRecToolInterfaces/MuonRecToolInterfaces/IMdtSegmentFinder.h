/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMDTSEGMENTFINDER_H
#define MUON_IMDTSEGMENTFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Segment.h"

namespace TrkDriftCircleMath {
    class DCStatistics;
    class ChamberGeometry;
    class Road;
}  // namespace TrkDriftCircleMath

static const InterfaceID IID_IMdtSegmentFinder("Muon::IMdtSegmentFinder", 1, 0);

namespace Muon {

    /*
        @brief The IMdtSegmentFinder is an interface (pure virtual class) for accessing MDT segment finders.
        Each segment finder is an AlgTool that is called by MDT segment maker tools, receives hit and road
        information in station coordinates and returns segment candidates that the segment maker can fit
        the hits to, perform other post processing tasks, and return them upstream.

        The interface consists of a single function, findSegments -

        @code
        virtual SegCandVec* findSegments(DCVec &dcvec, CLVec &clvec, MdtMultiChamberGeometry &geometry, Road &road);
        @endcode
    */

    class IMdtSegmentFinder : virtual public IAlgTool {
    public:
        /** @brief find segments from Hits in a Muon Station
            @param dcvec a vector of drift circles (MDT hits)
        @param clvec a vector of clusters (TGC/RPC hits)
        @param geometry chamber geometry in station coordinates
        @param road poision, direction and width of region of interest, defined on the station surface
            @return a vector of ISegmentCandidates that an ISegmentMakerTool can fit and process into IMuonSegments
        */

        virtual const TrkDriftCircleMath::SegVec findSegments(const TrkDriftCircleMath::DCVec& dcvec,
                                                              const TrkDriftCircleMath::CLVec& clvec, const TrkDriftCircleMath::Road& road,
                                                              const TrkDriftCircleMath::DCStatistics& dcstats,
                                                              const TrkDriftCircleMath::ChamberGeometry* geometry = 0) const = 0;

        /** access to tool interface */
        static const InterfaceID& interfaceID() { return IID_IMdtSegmentFinder; }
    };

}  // namespace Muon

#endif  // __IMDTSEGMENTFINDER_H__
