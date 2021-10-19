/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MATCHCROSSEDTUBES_H
#define DCMATH_MATCHCROSSEDTUBES_H

#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/SortDcsByY.h"

namespace TrkDriftCircleMath {
    /** counts the number of hits shared by the two segments */

    typedef std::pair<DCOnTrackVec, DCVec> MatchResult;

    struct MatchCrossedTubes {
        MatchCrossedTubes(bool onlyOnTrack = false) : m_mode(onlyOnTrack) {}

        const MatchResult operator()(const DCOnTrackVec& hits, const DCVec& crossedTubes) const {
            MatchResult result;
            result.first.reserve(50);
            result.second.reserve(50);

            DCOnTrackCit sit1 = hits.begin();
            DCOnTrackCit sit1_end = hits.end();
            DCCit sit2 = crossedTubes.begin();
            DCCit sit2_end = crossedTubes.end();

            SortDcsByY compDC;
            SameTube sameTube;

            result.first.clear();
            result.second.clear();

            while (sit1 != sit1_end && sit2 != sit2_end) {
                if (sit1->state() == DCOnTrack::CloseDC) {
                    ++sit1;
                    continue;
                }

                if (m_mode) {
                    // only use hits on track
                    if (sit1->state() != DCOnTrack::OnTrack) {
                        ++sit1;
                        continue;
                    }
                }

                if (sameTube(*sit1, *sit2)) {
                    ++sit1;
                    ++sit2;
                    continue;
                }

                // dc1 < dc2
                if (compDC(*sit1, *sit2)) {
                    result.first.push_back(*sit1);
                    ++sit1;
                    // dc1 >= dc2
                } else {
                    // dc2 < dc1
                    if (compDC(*sit2, *sit1)) {
                        result.second.push_back(*sit2);
                        ++sit2;
                        // dc1 == dc2
                    } else {
                        ++sit1;
                        ++sit2;
                    }
                }
            }

            // add remaining hits to counters
            for (; sit1 != sit1_end; ++sit1) {
                if (sit1->state() == DCOnTrack::CloseDC) { continue; }
                if (m_mode && sit1->state() != DCOnTrack::OnTrack) continue;
                result.first.push_back(*sit1);
            }
            for (; sit2 != sit2_end; ++sit2) { result.second.push_back(*sit2); }

            return result;
        }

    private:
        bool m_mode;
    };

}  // namespace TrkDriftCircleMath

#endif
