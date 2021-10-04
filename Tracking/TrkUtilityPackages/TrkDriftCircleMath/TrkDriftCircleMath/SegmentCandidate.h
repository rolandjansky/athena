/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __DCMATH__SEGMENT_CANDIDATE__H__
#define __DCMATH__SEGMENT_CANDIDATE__H__

#include <ostream>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/Segment.h"

namespace TrkDriftCircleMath {

    class Segment;

    class SegmentCandidate {
    public:
        SegmentCandidate(const Line& l, const DCVec& dcs, const CLVec& clusters) : m_line(l), m_dcs(dcs), m_clusters(clusters) {}

        SegmentCandidate(const Segment& segment);

        virtual ~SegmentCandidate() {}

        const Line& line() const { return m_line; }
        const DCVec& dcs() const { return m_dcs; }
        const CLVec& clusters() const { return m_clusters; }

        void dcs(const DCVec& dcs) { m_dcs = dcs; }
        void clusters(const CLVec& cls) { m_clusters = cls; }

    private:
        Line m_line;
        DCVec m_dcs;
        CLVec m_clusters;
    };

    class SegCandVec : public std::vector<SegmentCandidate> {
    public:
        SegCandVec(){};
        SegCandVec(const SegVec& segVec);
    };

    typedef SegCandVec::iterator SegCandIt;
    typedef SegCandVec::const_iterator SegCandCit;

}  // namespace TrkDriftCircleMath

std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::SegmentCandidate& dc);

MsgStream& operator<<(MsgStream& os, const TrkDriftCircleMath::SegmentCandidate& dc);

#endif  //__DCMATH__SEGMENT_CANDIDATE__H__
