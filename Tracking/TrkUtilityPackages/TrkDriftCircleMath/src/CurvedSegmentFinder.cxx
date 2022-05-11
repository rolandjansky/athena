/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/CurvedSegmentFinder.h"
#define sq(x) ((x) * (x))

namespace TrkDriftCircleMath {

    void CurvedSegmentFinder::curvedSegments(const ChamberGeometry& mdtGeo, SegVec& segs) const {
        int nCurved = 0;
        // collect all the ML1 and ML2 only segments
        SegVec ml1segs, ml2segs;
        for (SegIt it = segs.begin(); it != segs.end(); ++it) {
            int isBarrel = (*it).dcs()[0].id().isBarrel();
            if (!isBarrel) continue;

            if ((*it).hitsMl2() == 0) {
                ml1segs.push_back((*it));
            } else if ((*it).hitsMl1() == 0) {
                ml2segs.push_back((*it));
            }
        }
        // check that both ML have segments
        if (ml1segs.empty() || ml2segs.empty()) return;
        // Chamber information needed to calculate Delta b
        const LocVec2D& ml1LocVec2D = mdtGeo.tubePosition(0, mdtGeo.nlay(), 0);
        const LocVec2D& ml2LocVec2D = mdtGeo.tubePosition(1, 1, 0);
        double chamberMidPtY = (ml1LocVec2D.y() + ml2LocVec2D.y()) / 2.0;
        // loop over the ML segments and find matches
        if (m_debugLevel >= 10)
            std::cout << "CurvedSegmentsFinder begining match with " << ml1segs.size() << " ML1 segments and " << ml2segs.size()
                      << " ML2 segments" << std::endl;
        for (SegIt ml1 = ml1segs.begin(); ml1 != ml1segs.end(); ++ml1) {
            // bool foundCurvedSeg = false;
            const double tanML1 = std::tan(ml1->line().phi());
            const double mid1 = (chamberMidPtY - ml1->line().position().y()) / tanML1 + ml1->line().position().x();
            const double y01 = ml1->line().position().y() - ml1->line().position().x() * tanML1;
            for (SegIt ml2 = ml2segs.begin(); ml2 != ml2segs.end(); ++ml2) {
                // angle between the 2 segments
                const double deltaAlpha = ml1->line().phi() - ml2->line().phi();
                // distance of closest approach between the 2 segments at the middle of chamber
                                const double tanML2 = std::tan(ml2->line().phi());
                
                const double mid2 = (chamberMidPtY - ml2->line().position().y()) / tanML2 + ml2->line().position().x();
                const double y02 = ml2->line().position().y() - ml2->line().position().x() * tanML2;
                double deltab =  (mid2 * tanML1 - chamberMidPtY + y01) / std::hypot(1, tanML1);
                const double deltab2 = (mid1 * tanML2 - chamberMidPtY + y02) / std::hypot(1, tanML2);
                if (std::abs(deltab2) < std::abs(deltab)) deltab = deltab2;
                if (std::abs(deltaAlpha) >= m_maxDeltaAlpha  ||  std::abs(deltab) >= m_maxDeltab) continue;
                ++nCurved;
                
                // foundCurvedSeg = true;
                if (m_debugLevel >= 10)
                    std::cout << "CurvedSegment combination found with parameters (DeltaAlpha,Deltab) = (" << deltaAlpha << ", "
                                << deltab << ")" << std::endl;
                // build the curved segment
                DCOnTrackVec dcs = ml1->dcs();
                dcs.insert(dcs.end(), ml2->dcs().begin(), ml2->dcs().end());
                double segChi2 = ml1->chi2() + ml2->chi2();
                double segNDoF = ml1->ndof() + ml2->ndof();
                Segment seg(ml1->line(), dcs, segChi2, segNDoF, ml1->dtheta(), ml1->dy0());
                // find the x coordinate of at the middle of the chamber for ml2 segment
                const double xb = ml2->line().position().x() - (ml2->line().position().y() - chamberMidPtY) / tan(ml2->line().phi());
                // set the curvature parameters
                seg.setCurvatureParameters(deltaAlpha, xb);
                // hit information
                seg.deltas(ml1->deltas() + ml2->deltas());
                seg.hitsOutOfTime(ml1->hitsOutOfTime() + ml2->hitsOutOfTime());
                seg.hitsOnTrack(ml1->hitsOnTrack() + ml2->hitsOnTrack());
                seg.hitsPerMl(ml1->hitsMl1(), ml2->hitsMl2());
                seg.closeHits(ml1->closeHits() + ml2->closeHits());
                
                // store the new segment
                segs.push_back(seg);
                // remove the ML2 segment from the list
                // ml2segs.erase( ml2 );
                // break;
            }
        }  // end loop on ml2
        if (m_debugLevel >= 5) std::cout << "Finished CurvedSegments Finding, and found " << nCurved << " CurvedSegments" << std::endl;
   }

}  // end namespace TrkDriftCircleMath
