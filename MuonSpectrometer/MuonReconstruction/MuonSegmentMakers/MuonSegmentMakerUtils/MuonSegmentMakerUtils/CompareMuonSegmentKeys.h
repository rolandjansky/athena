/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_COMPAREMUONSEGMENTKEYS_H
#define MUON_COMPAREMUONSEGMENTKEYS_H

#include <string>

namespace Muon {

    class MuonSegmentKey;

    struct CompareMuonSegmentKeys {
        /** enum for the overlap result */
        enum OverlapResult {
            Identical = 0,       //<! segments are identical
            SubSet = 1,          //<! the first segment is a subset of the second
            SuperSet = 2,        //<! the first segment is a superset of the second
            PartialOverlap = 3,  //<! some hits of the segment overlap
            NoOverlap = 4,       //<! there is no overlap between the segments
            Unknown = 5          //<! unknown overlap tyep, this should not happen
        };

        /** calculate overlap between two SegmentKey2s, if ignoreTriggerHits == true trigger hits will not be included in the comparison,
         * returns an OverlapResult */
        OverlapResult operator()(const MuonSegmentKey& sk1, const MuonSegmentKey& sk2, bool ignoreTriggerHits = false);

        /** print result to string */
        std::string print(const OverlapResult& result) const;

        unsigned int intersectionSize;
        unsigned int segment1Size;
        unsigned int segment2Size;

        unsigned int segment1SizeTrigger;
        unsigned int segment2SizeTrigger;
        unsigned int intersectionSizeTrigger;
    };

}  // namespace Muon

#endif
