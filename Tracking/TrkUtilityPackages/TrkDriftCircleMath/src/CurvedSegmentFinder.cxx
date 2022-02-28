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
            for (SegIt ml2 = ml2segs.begin(); ml2 != ml2segs.end(); ++ml2) {
                // angle between the 2 segments
                double deltaAlpha = ml1->line().phi() - ml2->line().phi();
                // distance of closest approach between the 2 segments at the middle of chamber
                double mid1 = (chamberMidPtY - ml1->line().position().y()) / tan(ml1->line().phi()) + ml1->line().position().x();
                double mid2 = (chamberMidPtY - ml2->line().position().y()) / tan(ml2->line().phi()) + ml2->line().position().x();
                double y01 = ml1->line().position().y() - ml1->line().position().x() * tan(ml1->line().phi());
                double y02 = ml2->line().position().y() - ml2->line().position().x() * tan(ml2->line().phi());
                double deltab = (mid2 * tan(ml1->line().phi()) - chamberMidPtY + y01) / (sqrt(1 + sq(tan(ml1->line().phi()))));
                double deltab2 = (mid1 * tan(ml2->line().phi()) - chamberMidPtY + y02) / (sqrt(1 + sq(tan(ml2->line().phi()))));
                if (fabs(deltab2) < fabs(deltab)) deltab = deltab2;
                if (fabs(deltaAlpha) < m_maxDeltaAlpha && fabs(deltab) < m_maxDeltab) {
                    nCurved++;
                    // foundCurvedSeg = true;
                    if (m_debugLevel >= 10)
                        std::cout << "CurvedSegment combination found with parameters (DeltaAlpha,Deltab) = (" << deltaAlpha << ", "
                                  << deltab << ")" << std::endl;
                    // build the curved segment
                    DCOnTrackVec dcs = ml1->dcs();
                    DCOnTrackVec dcs2 = ml2->dcs();
                    DCOnTrackIt ml2dcsIt = dcs2.begin();
                    for (; ml2dcsIt != dcs2.end(); ++ml2dcsIt) dcs.push_back((*ml2dcsIt));
                    double segChi2 = ml1->chi2() + ml2->chi2();
                    double segNDoF = ml1->ndof() + ml2->ndof();
                    Segment seg(ml1->line(), dcs, segChi2, segNDoF, ml1->dtheta(), ml1->dy0());
                    // find the x coordinate of at the middle of the chamber for ml2 segment
                    float xb = ml2->line().position().x() - (ml2->line().position().y() - chamberMidPtY) / tan(ml2->line().phi());
                    // set the curvature parameters
                    seg.setCurvatureParameters(deltaAlpha, xb);
                    // hit information
                    seg.deltas(ml1->deltas() + ml2->deltas());
                    seg.hitsOutOfTime(ml1->hitsOutOfTime() + ml2->hitsOutOfTime());
                    seg.hitsOnTrack(ml1->hitsOnTrack() + ml2->hitsOnTrack());
                    seg.hitsPerMl(ml1->hitsMl1(), ml2->hitsMl2());
                    seg.closeHits(ml1->closeHits() + ml2->closeHits());
                    // associated clusters
                    CLVec clusters;
                    double Ml1y = ml1->line().position().y();
                    double Ml2y = ml2->line().position().y();
                    if (Ml1y < Ml2y) {
                        for (CLVec::const_iterator it = ml1->clusters().begin(); it != ml1->clusters().end(); ++it) {
                            if (it->y() < Ml1y) clusters.push_back((*it));
                        }
                        for (CLVec::const_iterator it = ml2->clusters().begin(); it != ml2->clusters().end(); ++it) {
                            if (it->y() > Ml2y) clusters.push_back((*it));
                        }
                    } else {
                        for (CLVec::const_iterator it = ml1->clusters().begin(); it != ml1->clusters().end(); ++it) {
                            if (it->y() > Ml1y) clusters.push_back((*it));
                        }
                        for (CLVec::const_iterator it = ml2->clusters().begin(); it != ml2->clusters().end(); ++it) {
                            if (it->y() < Ml2y) clusters.push_back((*it));
                        }
                    }

                    // store the new segment
                    segs.push_back(seg);
                    // remove the ML2 segment from the list
                    // ml2segs.erase( ml2 );
                    // break;
                }
            }  // end loop on ml2
        }      // end loop on ml1
        if (m_debugLevel >= 5) std::cout << "Finished CurvedSegments Finding, and found " << nCurved << " CurvedSegments" << std::endl;
   }

}  // end namespace TrkDriftCircleMath
