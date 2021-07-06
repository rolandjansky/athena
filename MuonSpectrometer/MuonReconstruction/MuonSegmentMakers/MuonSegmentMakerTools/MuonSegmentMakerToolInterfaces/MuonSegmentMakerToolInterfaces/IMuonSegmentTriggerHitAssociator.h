/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTTRIGGERHITASSOCIATOR_H
#define MUON_IMUONSEGMENTTRIGGERHITASSOCIATOR_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"

namespace Muon {

    class MuonSegment;
    class MuonClusterOnTrack;

    /**
       @brief tool to merge segment with overlapping hits in the precision plain but different phi hits

    */
    class IMuonSegmentTriggerHitAssociator : virtual public IAlgTool {
    public:
        typedef std::vector<const MuonSegment*> SegVec;
        typedef SegVec::iterator SegIt;
        typedef SegVec::const_iterator SegCit;

    public:
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentTriggerHitAssociator("Muon::IMuonSegmentTriggerHitAssociator", 1, 0);

            return IID_IMuonSegmentTriggerHitAssociator;
        }

        /** associate trigger hits to an existing segment
            - a segment
            - a list of MuonClusterOnTrack objects
            - a flag indicating whether the eta hits should be added

            returns the new segment, owner ship passed to the caller.

        */
        virtual const MuonSegment* associateTriggerHits(const MuonSegment& seg, const std::vector<const MuonClusterOnTrack*>& clus,
                                                        bool includeEtaHits) const = 0;

        virtual ~IMuonSegmentTriggerHitAssociator() = default;
    };
}  // namespace Muon

#endif
