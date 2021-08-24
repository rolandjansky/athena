/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "MuonSegmentMakerUtils/MuonSegmentKey.h"

Muon::CompareMuonSegmentKeys::OverlapResult Muon::CompareMuonSegmentKeys::operator()(const Muon::MuonSegmentKey& sk1,
                                                                                     const Muon::MuonSegmentKey& sk2,
                                                                                     bool ignoreTriggerHits) {
    std::vector<std::pair<unsigned int, int> > intersection;
    std::set_intersection(sk1.keys().begin(), sk1.keys().end(), sk2.keys().begin(), sk2.keys().end(), std::back_inserter(intersection));
    intersectionSize = intersection.size();
    segment1Size = sk1.keys().size();
    segment2Size = sk2.keys().size();

    segment1SizeTrigger = sk1.keysTrigger().size();
    segment2SizeTrigger = sk2.keysTrigger().size();
    std::vector<std::pair<unsigned int, int> > intersectionTrig;
    std::set_intersection(sk1.keysTrigger().begin(), sk1.keysTrigger().end(), sk2.keysTrigger().begin(), sk2.keysTrigger().end(),
                          std::back_inserter(intersectionTrig));
    intersectionSizeTrigger = intersectionTrig.size();

    if (intersectionSize == 0) {
        if (segment1Size == 0 && segment2Size == 0) {
            if (segment1SizeTrigger == intersectionSizeTrigger) {
                // overlap between second segment and the intersection also includes all hits
                // the two segments share all hits
                if (segment2SizeTrigger == intersectionSizeTrigger) {
                    return Identical;
                } else if (segment1SizeTrigger > segment2SizeTrigger) {
                    return SuperSet;
                } else {
                    return SubSet;
                }
            } else if (segment2SizeTrigger == intersectionSizeTrigger) {
                if (segment1SizeTrigger > intersectionSizeTrigger) return SuperSet;
            } else {
                return PartialOverlap;
            }
        }
        return NoOverlap;
    } else {
        // overlap between first segment and the intersection includes all hits
        if (segment1Size == intersectionSize) {
            // overlap between second segment and the intersection also includes all hits
            // the two segments share all hits
            if (segment2Size == intersectionSize) {
                if (ignoreTriggerHits) return Identical;

                if (segment1SizeTrigger == intersectionSizeTrigger) {
                    // overlap between second segment and the intersection also includes all hits
                    // the two segments share all hits
                    if (segment2SizeTrigger == intersectionSizeTrigger) {
                        return Identical;
                    } else if (segment1SizeTrigger > segment2SizeTrigger) {
                        return SuperSet;
                    } else {
                        return SubSet;
                    }
                } else if (segment2SizeTrigger == intersectionSizeTrigger) {
                    if (segment1SizeTrigger > intersectionSizeTrigger) return SuperSet;
                } else {
                    return PartialOverlap;
                }
            } else if (segment2Size > intersectionSize) {
                return SubSet;
            }
        } else if (segment2Size == intersectionSize) {
            if (segment1Size > intersectionSize) {
                // the first segment has more hits and is better
                return SuperSet;
            }
        } else {
            return PartialOverlap;
        }
    }
    return Muon::CompareMuonSegmentKeys::Unknown;
}

std::string Muon::CompareMuonSegmentKeys::print(const Muon::CompareMuonSegmentKeys::OverlapResult& result) const {
    std::ostringstream sout;

    if (result == Identical)
        sout << "Identical";
    else if (result == SubSet)
        sout << "SubSet";
    else if (result == SuperSet)
        sout << "SuperSet";
    else if (result == PartialOverlap)
        sout << "PartialOverlap";
    else if (result == NoOverlap)
        sout << "NoOverlap";
    else if (result == Unknown)
        sout << "Unknown";
    else
        sout << "UnknownType";
    sout << " intersects (" << intersectionSize << "," << intersectionSizeTrigger << ") seg1 (" << segment1Size << ","
         << segment1SizeTrigger << ") "
         << " seg2 (" << segment2Size << "," << segment2SizeTrigger << ") ";
    return sout.str();
}
