/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SHAREDHITSSEGMENTS_H
#define DCMATH_SHAREDHITSSEGMENTS_H

#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/SortDcsByY.h"

namespace TrkDriftCircleMath {
    /** counts the number of hits shared by the two segments */

    struct SharedHitsSegments {
        SharedHitsSegments(bool onlyOnTrack = false) : m_mode(onlyOnTrack), m_shared{}, m_onlySeg1{}, m_onlySeg2{} { /*nop **/
        }

        unsigned int operator()(const Segment& seg1, const Segment& seg2) {
            DCOnTrackCit sit1 = seg1.dcs().begin();
            DCOnTrackCit sit1_end = seg1.dcs().end();
            DCOnTrackCit sit2 = seg2.dcs().begin();
            DCOnTrackCit sit2_end = seg2.dcs().end();

            SortDcsByY compDC;

            m_shared = 0;
            m_onlySeg1 = 0;
            m_onlySeg2 = 0;

            while (sit1 != sit1_end && sit2 != sit2_end) {
                // skip all close hits
                if (sit1->state() == DCOnTrack::CloseDC) {
                    ++sit1;
                    continue;
                }
                if (sit2->state() == DCOnTrack::CloseDC) {
                    ++sit2;
                    continue;
                }

                if (m_mode) {
                    // only use hits on track
                    if (sit1->state() != DCOnTrack::OnTrack) {
                        ++sit1;
                        continue;
                    }
                    if (sit2->state() != DCOnTrack::OnTrack) {
                        ++sit2;
                        continue;
                    }
                }

                // dc1 < dc2
                if (compDC(*sit1, *sit2)) {
                    ++sit1;
                    ++m_onlySeg1;                 
                    // dc1 >= dc2
                } else {
                    // dc2 < dc1
                    if (compDC(*sit2, *sit1)) {
                        ++sit2;
                        ++m_onlySeg2;
                        // dc1 == dc2
                    } else {
                        ++m_shared;
                        ++sit1;
                        ++sit2;
                    }
                }
            }

            // add remaining hits to counters
            for (; sit1 != sit1_end; ++sit1) {
                if (m_mode && sit1->state() != DCOnTrack::OnTrack) continue;

                if (sit1->state() == DCOnTrack::CloseDC) continue;

                ++m_onlySeg1;
            }
            for (; sit2 != sit2_end; ++sit2) {
                if (m_mode && sit2->state() != DCOnTrack::OnTrack) continue;

                if (sit2->state() == DCOnTrack::CloseDC) continue;
                ++m_onlySeg2;
            }

            return m_shared;
        }

        unsigned int sharedHits() const { return m_shared; }
        unsigned int onlyInSeg1() const { return m_onlySeg1; }
        unsigned int onlyInSeg2() const { return m_onlySeg2; }

    private:
        bool m_mode;
        unsigned int m_shared;
        unsigned int m_onlySeg1;
        unsigned int m_onlySeg2;
    };

}  // namespace TrkDriftCircleMath

#endif
