/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/Segment.h"

#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/SegmentCandidate.h"

namespace TrkDriftCircleMath {

    std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::Segment& seg) {
        os << seg.line() << " chi2 " << seg.chi2() << " ndof " << seg.ndof() << " t0 shift " << seg.t0Shift() << std::endl
           << "   hot " << seg.hitsOnTrack() << " deltas " << seg.deltas() << " hoo " << seg.hitsOutOfTime() << " cl " << seg.closeHits()
           << " ml1 " << seg.hitsMl1() << " ml2 " << seg.hitsMl2() << " cls " << seg.clusterLayers() << " empty " << seg.emptyTubes().size()
           << " cross ml1 " << seg.crossedTubesMl1() << " ml2 " << seg.crossedTubesMl2() << std::endl;

        //   TrkDriftCircleMath::ResidualWithLine resLine( seg.line() );

        TrkDriftCircleMath::DCOnTrackCit it = seg.dcs().begin();
        TrkDriftCircleMath::DCOnTrackCit it_end = seg.dcs().end();

        for (; it != it_end; ++it) { os << *it << std::endl; }
        return os;
    }

    MsgStream& operator<<(MsgStream& os, const TrkDriftCircleMath::Segment& seg) {
        os << seg.line() << " chi2 " << seg.chi2() << " ndof " << seg.ndof() << " t0 shift " << seg.t0Shift() << "\n   hot "
           << seg.hitsOnTrack() << " deltas " << seg.deltas() << " hoo " << seg.hitsOutOfTime() << " cl " << seg.closeHits() << " ml1 "
           << seg.hitsMl1() << " ml2 " << seg.hitsMl2() << " cls " << seg.clusterLayers() << " empty " << seg.emptyTubes().size()
           << " cross ml1 " << seg.crossedTubesMl1() << " ml2 " << seg.crossedTubesMl2() << endmsg;

        TrkDriftCircleMath::DCOnTrackCit it = seg.dcs().begin();
        TrkDriftCircleMath::DCOnTrackCit it_end = seg.dcs().end();

        for (; it != it_end; ++it) { os << *it << endmsg; }

        return os;
    }

    void Segment::set(double chi2, unsigned int ndof, double dtheta, double dy0) {
        m_chi2 = chi2;
        m_ndof = ndof;
        m_dtheta = dtheta;
        m_dy0 = dy0;
        m_t0Shift = -99999.;
        m_deltaAlpha = -99999.;
        m_deltab = -99999.;
        m_deltas = 0;
        m_hitsOutOfTime = 0;
        m_hitsOnTrack = 0;
        m_closeHits = 0;
        m_showerHits = 0;
        m_crossedTubesMl1 = 0;
        m_crossedTubesMl2 = 0;
        m_hitsMl1 = 0;
        m_hitsMl2 = 0;
        m_ambigue = 0;
        m_clusterLayers = 0;
    }

    void Segment::setAndReset(double chi2, unsigned int ndof, double dtheta, double dy0) {
        m_chi2 = chi2;
        m_ndof = ndof;
        m_dtheta = dtheta;
        m_dy0 = dy0;
        m_t0Shift = -99999.;
        m_deltaAlpha = -99999.;
        m_deltab = -99999.;
        m_deltas = 0;
        m_hitsOutOfTime = 0;
        m_hitsOnTrack = 0;
        m_closeHits = 0;
        m_showerHits = 0;
        m_crossedTubesMl1 = 0;
        m_crossedTubesMl2 = 0;
        m_hitsMl1 = 0;
        m_hitsMl2 = 0;
        m_ambigue = 0;
        m_clusterLayers = 0;
        m_dcs.clear();
        m_clusters.clear();
        m_emptyTubes.clear();
    }

}  // namespace TrkDriftCircleMath
