/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/SegmentCandidate.h"

#include <iostream>

#include "TrkDriftCircleMath/ResidualWithLine.h"
#include "TrkDriftCircleMath/Segment.h"

std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::SegmentCandidate& seg) {
    os << "line " << seg.line() << " dcs " << seg.dcs().size() << " cls " << seg.clusters().size() << std::endl;

    TrkDriftCircleMath::DCCit it = seg.dcs().begin();
    TrkDriftCircleMath::DCCit it_end = seg.dcs().end();

    for (; it != it_end; ++it) { os << *it << std::endl; }

    return os;
}

MsgStream& operator<<(MsgStream& os, const TrkDriftCircleMath::SegmentCandidate& seg) {
    os << "line " << seg.line() << " dcs " << seg.dcs().size() << " cls " << seg.clusters().size() << endmsg;

    TrkDriftCircleMath::DCCit it = seg.dcs().begin();
    TrkDriftCircleMath::DCCit it_end = seg.dcs().end();

    for (; it != it_end; ++it) { os << *it << endmsg; }

    return os;
}

namespace TrkDriftCircleMath {

    SegmentCandidate::SegmentCandidate(const Segment& seg) : m_line(seg.line()), m_clusters(seg.clusters()) {
        m_dcs.reserve(seg.dcs().size());
        DCOnTrackCit it = seg.dcs().begin();
        DCOnTrackCit itEnd = seg.dcs().end();
        for (; it != itEnd; ++it) { m_dcs.push_back(*it); }
    }

    SegCandVec::SegCandVec(const SegVec& segVec) {
        reserve(segVec.size());

        SegCit segIt = segVec.begin();
        SegCit segItEnd = segVec.end();
        for (; segIt != segItEnd; ++segIt) { push_back(SegmentCandidate(*segIt)); }
    }

}  // namespace TrkDriftCircleMath
