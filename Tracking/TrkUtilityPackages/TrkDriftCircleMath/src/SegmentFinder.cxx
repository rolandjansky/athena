/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/SegmentFinder.h"

#include <algorithm>
#include <iostream>
#include <iterator>

#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/DCSLHitSelector.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/ResidualWithSegment.h"
#include "TrkDriftCircleMath/SharedHitsSegments.h"
#include "TrkDriftCircleMath/SortSegBySizeAndChi2.h"
#include "TrkDriftCircleMath/TangentToCircles.h"

namespace TrkDriftCircleMath {

    SegmentFinder::SegmentFinder()
      : m_fitter (std::make_shared<DCSLFitter>())
    {
        // update the cached directions
        updateDirections();
    }

    SegmentFinder::SegmentFinder(double roadWidth, double deltaCut, bool fullScan) :
        m_deltaCut{deltaCut}, m_roadWidth{roadWidth},
        m_fitter (std::make_shared<DCSLFitter>()),
        m_fullScan{fullScan}
    {
        // update the cached directions
        updateDirections();
    }

    SegmentFinder::~SegmentFinder() {
        if (m_debugLevel > 0) {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::cout << " drop summary " << std::endl;
            for (unsigned int i = 0; i < m_dropDepthMax + 1; ++i) { std::cout << " Accepted " << m_dropDepthAcceptCounts[i] << std::endl; }
            for (unsigned int i = 0; i < m_dropDepthMax; ++i) { std::cout << " Rejected " << m_dropDepthRejectCounts[i] << std::endl; }
        }
    }

    void SegmentFinder::setMaxDropDepth(int max) {
        m_dropDepthMax = max;
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dropDepthAcceptCounts.clear();
        m_dropDepthAcceptCounts.resize(max + 1, 0);
        m_dropDepthRejectCounts.clear();
        m_dropDepthRejectCounts.resize(max + 1, 0);
    }

    void SegmentFinder::setRecoverMDT(bool doRecover) { m_recoverMdtHits = doRecover; }

    void SegmentFinder::setSortSegmentsUsingAllHits(bool doAllHitsSort) { m_doAllHitSort = doAllHitsSort; }

    void SegmentFinder::setResidualCutT0(double resCut) { m_resCutT0 = resCut; }

    void SegmentFinder::setDeltaCutT0(double deltaCut) { m_deltaCutT0 = deltaCut; }

    void SegmentFinder::setUseSegmentQuality(bool useQ) { m_useSegmentQuality = useQ; }

    void SegmentFinder::setUseChamberPhi(bool useChamberPhi) { m_useChamberPhi = useChamberPhi; }

    void SegmentFinder::setCurvedSegmentFinder(bool doCurvedSegmentFinder) { m_doCurvedSegmentFinder = doCurvedSegmentFinder; }

    void SegmentFinder::setRemoveSingleOutliers(bool removeSingleOutliers) { m_removeSingleOutliers = removeSingleOutliers; }

    void SegmentFinder::setTGCPullCut(double cut) { m_tgcPullCut = cut; }

    void SegmentFinder::setRPCPullCut(double cut) { m_rpcPullCut = cut; }

    void SegmentFinder::setDropHits(bool doDrop) { m_doDrop = doDrop; }

    void SegmentFinder::setSeedCleaning(bool doCleanup) { m_seedCleaning = doCleanup; }

    void SegmentFinder::setChi2DropCut(double chi2) { m_chi2Cut = chi2; }

    void SegmentFinder::setDeltaCut(double cut) { m_deltaCut = cut; }

    void SegmentFinder::setSingleMultiLayerScan(bool doScan) { m_singleMultiLayerScan = doScan; }

    void SegmentFinder::setRatioEmptyTubesCut(double ratioEmptyTubesCut) { m_ratioEmptyTubesCut = ratioEmptyTubesCut; }

    void SegmentFinder::setPhiRoad(double phiRoad, double phiChamber, double sinPhiCut, bool useRoadPhi, bool useChamberPhi) {
        // set road
        m_phiRoad = phiRoad;

        // set phi chamber
        m_phiChamber = phiChamber;

        // set road cut
        m_phiDifCut = sinPhiCut;

        m_useRoadPhi = useRoadPhi;
        m_useChamberPhi = useChamberPhi;

        // update the cached directions
        updateDirections();
    }

    void SegmentFinder::updateDirections() {
        m_roadDir = LocVec2D(std::cos(m_phiRoad), std::sin(m_phiRoad));
        m_chamberDir = LocVec2D(std::cos(m_phiChamber), std::sin(m_phiChamber));

        // set road cut
        m_phiDifCut = std::cos(m_phiDifCut);

        Line roadLine(LocVec2D(0., 0.), m_roadDir);
        if (roadLine.phi() != m_phiRoad) {
            if (m_debugLevel >= 1) std::cout << " bad phi for road: road " << m_phiRoad << " dir phi " << roadLine.phi() << std::endl;
        }
        Line chamberLine(LocVec2D(0., 0.), m_chamberDir);
        if (chamberLine.phi() != m_phiChamber) {
            if (m_debugLevel >= 1)
                std::cout << " bad phi for chamber: chamber " << m_phiChamber << " dir phi " << chamberLine.phi() << std::endl;
        }
    }

    SegVec SegmentFinder::findSegments(const DCVec& dcsIn) const {
        CLVec clusters;
        return findSegments(dcsIn, clusters);
    }

    void SegmentFinder::printSettings() const {
        std::cout << " SegmentFinder::setting: " << std::endl
                  << "        Road width " << m_roadWidth << " Pull cuts: MDT " << m_deltaCut << "  RPC " << m_rpcPullCut << "  TGC "
                  << m_tgcPullCut << std::endl
                  << "        Phi seed   " << m_phiRoad;

        if (m_useChamberPhi) std::cout << " chamberCut";
        if (m_useRoadPhi) std::cout << " roadCut";

        if (m_useChamberPhi || m_useRoadPhi) std::cout << " phi cut " << m_phiDifCut << std::endl;

        std::cout << "        Empty tube ratio cut " << m_ratioEmptyTubesCut << " chi2 drop cut " << m_chi2Cut << std::endl;
        std::cout << "        Options: ";
        if (m_fullScan) std::cout << " -- full scan ";
        if (m_singleMultiLayerScan) std::cout << " -- single multilayer scan ";
        if (m_seedCleaning) std::cout << " -- seed cleaning ";
        if (m_doDrop) std::cout << " -- drop hits ";
        if (m_doAllHitSort) std::cout << " -- sort with all hits ";
        if (m_doCurvedSegmentFinder) std::cout << " -- curved segment finder ";
        if (m_mdtGeometry) std::cout << " -- using geometry ";
        std::cout << std::endl;
    }

    void SegmentFinder::handleHits(const DCVec& dcsIn, const CLVec& clsIn,
                                   ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const {
        DCVec dcs = dcsIn;

        if (m_debugLevel >= 1) {
            std::cout << "In handleHits: dcs " << dcsIn.size() << " clusters " << clsIn.size() << std::endl;
            printSettings();
        }
        // sort input drift circles
        std::stable_sort(dcs.begin(), dcs.end(), SortDcsByY());

        SegVec preSelSegs;
        if (!m_fullScan) {
            // find seeds in multilayer
            std::pair<DCVec, DCVec> dcsPerMl = splitInMulitlayers(dcs);

            DCVec seedsMl1;
            DCVec seedsMl2;

            if (m_seedCleaning) {
                // remove triples or larger ranges per layer
                selectSeeds(dcsPerMl.first, 3).swap(seedsMl1);
                selectSeeds(dcsPerMl.second, 3).swap(seedsMl2);
            } else {
                seedsMl1 = dcsPerMl.first;
                seedsMl2 = dcsPerMl.second;
            }

            // search for segments crossing both multilayers
            twoMultiLayerScan(seedsMl1, seedsMl2, dcs, clsIn, segments);

            if (m_debugLevel >= 1) {
                std::cout << " two layer scan " << std::endl;
                for (SegCit it = segments.data().begin(); it != segments.data().end(); ++it) { std::cout << "  " << *it << std::endl; }
            }

            // reset segments so it contains only the cleaned segments
            cleanSegments(segments.data()).swap(preSelSegs);
            segments.set(preSelSegs);

            if (m_debugLevel >= 1) {
                std::cout << " after cleaning " << std::endl;
                for (SegCit it = segments.data().begin(); it != segments.data().end(); ++it) { std::cout << "  " << *it << std::endl; }
            }

            unsigned int usedHits(0);
            for (SegCit it = segments.data().begin(); it != segments.data().end(); ++it) { usedHits += it->hitsOnTrack(); }

            // if remaining dcs not associated with selected track smaller then 2 no additional segments can be formed
            // thus no further search in single multi layers is needed
            if ((usedHits >= dcs.size() - 2 && !m_doCurvedSegmentFinder) || !m_singleMultiLayerScan) {
            } else {
                // full combinatorics in single ml
                fullScan(seedsMl1, dcsPerMl.first, clsIn, segments);

                if (m_debugLevel >= 1) {
                    std::cout << " segments after scan multilayer 1 " << std::endl;
                    for (SegCit it = segments.data().begin(); it != segments.data().end(); ++it) { std::cout << "  " << *it << std::endl; }
                }

                fullScan(seedsMl2, dcsPerMl.second, clsIn, segments);

                if (m_debugLevel >= 1) {
                    std::cout << " segments after scan multilayer 2 " << std::endl;
                    for (SegCit it = segments.data().begin(); it != segments.data().end(); ++it) { std::cout << "  " << *it << std::endl; }
                }
            }

        } else {
            // remove triples or larger ranges per layer
            DCVec seeds = selectSeeds(dcs, 3);
            fullScan(seeds, dcs, clsIn, segments);
        }
    }

    SegVec SegmentFinder::findSegments(const DCVec& dcsIn, const CLVec& clsIn) const {
        // prepare segment collection
        ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>> segments;

        // find segments
        handleHits(dcsIn, clsIn, segments);

        // Curved Segment finder
        if (m_mdtGeometry && m_doCurvedSegmentFinder) {
            CurvedSegmentFinder CurvedSegFinder(m_debugLevel);
            CurvedSegFinder.curvedSegments(*m_mdtGeometry, segments.data());
        } else if (m_debugLevel >= 5 && m_doCurvedSegmentFinder)
            std::cout << "CurvedSegmentFinder was passed a NULL pointer to the ChamberGeometry" << std::endl;

        SegVec selectedSegments;

        // final cleaning of segments
        cleanSegments(segments.data()).swap(selectedSegments);

        // redo cluster association with final tracks
        associateClusters(selectedSegments, clsIn);

        if (m_debugLevel >= 1) {
            std::cout << " final segments " << std::endl;
            for (SegIt it = selectedSegments.begin(); it != selectedSegments.end(); ++it) { std::cout << *it << std::endl; }
        }

        return selectedSegments;
    }

    std::pair<DCVec, DCVec> SegmentFinder::splitInMulitlayers(const DCVec& dcs) {
        DCVec ml1, ml2;
        ml1.reserve(dcs.size());
        ml2.reserve(dcs.size());
        for (const DriftCircle& circ : dcs) {
            if (circ.id().ml() == 0)
                ml1.emplace_back(circ);
            else
                ml2.emplace_back(circ);
        }
        return std::make_pair(std::move(ml1), std::move(ml2));
    }

    DCVec SegmentFinder::selectSeeds(const DCVec& dcs, int maxSerie) {
        // this algorithm assumes a sorted collection!!
        if (dcs.size() <= 2) return dcs;

        DCVec seeds;

        NeighbourTube isNeighbour;

        DCCit it_start_serie;
        DCCit it = dcs.begin();
        DCCit it_end = dcs.end() - 1;

        while (it <= it_end) {
            // mark first entry as start serie
            it_start_serie = it;

            // loop over next entries until an entry is no neighbour or we reached the end of the data set
            while (it != it_end && isNeighbour(*it, *(it + 1))) { ++it; }

            // if less then 2 neighbours found, add hits to seeds
            if (std::distance(it_start_serie, it + 1) < maxSerie) {
                // add new seeds
                for (; it_start_serie != it + 1; ++it_start_serie) {
                    // only fill hits that are "in time"
                    if (it_start_serie->state() == DriftCircle::InTime) { seeds.push_back(*it_start_serie); }
                }
            }
            // all other combinations are not taken into account

            // if we reached the end of the data set quit loop
            if (it == dcs.end()) break;
            ++it;
        }

        return seeds;
    }

    SegVec SegmentFinder::refitSegments(const SegVec& segs) const {
        // no segments found return empty segment vector
        if (segs.empty()) return segs;

        MatchDCWithLine matchWithLine;

        SegVec segments;
        segments.reserve(segs.size());

        if (m_debugLevel >= 3) { std::cout << " refitting segments " << std::endl; }

        /// Copy the segment due to dropHits in line 324
        for (const Segment& inSegment : segs) {
            // keep all curved segments
            if (inSegment.hasCurvatureParameters()) {
                segments.emplace_back(inSegment);
                continue;
            }
            /// Make a copy of the segment
            Segment resultSeg{inSegment};

            // drop hits is switched on
            bool goodSeg{true}, hasDroppedHit{false};
            unsigned int dropDepth = 0;
            if (m_doDrop) goodSeg = dropHits(resultSeg, hasDroppedHit, dropDepth);

            if (goodSeg) {
                if (!hasDroppedHit && !m_removeSingleOutliers) {
                    // if the segment was accepted without dropping we still have to refit it

                    // find all hits within cut-off
                    matchWithLine.set(inSegment.line(), m_deltaCut, MatchDCWithLine::Pull, tubeRadius());
                    bool usePrecise = resultSeg.hasT0Shift();
                    const DCOnTrackVec& hitsOnLine = matchWithLine.match(inSegment.dcs(), nullptr, m_recoverMdtHits, usePrecise);

                    if (matchWithLine.matchDifference() > 0) {
                        if (m_debugLevel >= 4) {
                            std::cout << " -- " << inSegment;
                            std::cout << "   -- new DCS " << hitsOnLine.size() << std::endl;
                            for (const DCOnTrack& cit : hitsOnLine) { std::cout << "    " << cit << std::endl; }
                        }

                        // reject segment if less then 3 hits remain
                        if (matchWithLine.hitsOnTrack() <= 2) { continue; }

                        // refit segment
                        Segment result(Line(0., 0., 0.), DCOnTrackVec());
                        if (inSegment.hasT0Shift()) {
                            if (!m_fitter->fit(result, inSegment.line(), hitsOnLine, m_hitSelector.selectHitsOnTrack(hitsOnLine),
                                               inSegment.t0Shift())) {
                                if (m_debugLevel >= 4) std::cout << " failed fit " << std::endl;
                                continue;
                            }
                        } else {
                            if (!m_fitter->fit(result, inSegment.line(), hitsOnLine, m_hitSelector.selectHitsOnTrack(hitsOnLine))) {
                                if (m_debugLevel >= 4) std::cout << " failed fit " << std::endl;
                                continue;
                            }
                        }
                        if (m_debugLevel >= 4) {
                            std::cout << " after fit " << std::endl;
                            std::cout << result << std::endl;
                        }
                        resultSeg = result;

                        // update match parameters
                        updateMatch(resultSeg, matchWithLine);

                        // keep close hits from initial road as information is not available anymore and shouldn't change to much
                        resultSeg.showerHits(inSegment.showerHits());  // copy showerHits of initial road

                        // keep clusters
                        resultSeg.clusters(inSegment.clusters());  // copy clusters of initial road

                        // update crossed tubes
                        crossedTubes(resultSeg);

                        // check hit ratio
                        if (!goodHitRatio(resultSeg)) continue;

                        // once more do dropping
                        if (m_doDrop) goodSeg = dropHits(resultSeg, hasDroppedHit, dropDepth);

                        if (!goodSeg) {
                            if (m_debugLevel >= 2) { std::cout << " segment lossed after refit and dropping " << resultSeg << std::endl; }
                            continue;
                        }
                    }
                }

                // update segment parameters
                segments.emplace_back(std::move(resultSeg));
                if (m_debugLevel >= 2) {
                    std::cout << " new segment added " << segments.size() << std::endl;
                    std::cout << segments.back() << std::endl;
                }
            } else {
                if (m_debugLevel >= 2) std::cout << " segment lost during hit dropping " << std::endl;
            }
        }

        return segments;
    }

    bool SegmentFinder::dropHits(Segment& segment, bool& hasDroppedHit, unsigned int& dropDepth) const {
        if (dropDepth > m_dropDepthMax) {
            if (m_debugLevel >= 5) {
                std::cout << " dropDepth too large keeping segment " << segment << std::endl;
                std::cout << " dropDepth " << dropDepth << std::endl;
                for (const DCOnTrack& hit : segment.dcs()) {
                    if (segment.hasT0Shift()) {
                        std::cout << " t0 fit residual " << hit.residual() << std::endl;
                    } else {
                        std::cout << " NO t0 fit residual " << hit.residual() << std::endl;
                    }
                }
            }
            if (m_debugLevel > 0) {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_dropDepthAcceptCounts[dropDepth];
            }
            return true;
        }
        ++dropDepth;

        if (m_debugLevel >= 5) std::cout << " dropDepth " << dropDepth << " " << segment << std::endl;

        // discard all segments with less then 3 hits
        if (segment.ndof() <= 0) {
            --dropDepth;
            if (m_debugLevel > 0) {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_dropDepthRejectCounts[dropDepth];
            }
            return false;
        }

        // if chi2/ndof of segment smaller than cut-off:
        //  do dropping for t0 reffited segments  if  residual > 1 mm or 5 sigma precise errors
        //              for NOT refitted if residual > 3 mm
        if (segment.chi2() / segment.ndof() < m_chi2Cut) {
            bool hasBadHit = false;
            if (m_removeSingleOutliers) {
                for (const DCOnTrack& hit : segment.dcs()) {
                    if (segment.hasT0Shift()) {
                        double res = std::abs(hit.residual());
                        if (hit.state() == DCOnTrack::OnTrack && (res > m_resCutT0 || res / hit.drPrecise() > m_deltaCutT0)) {
                            hasBadHit = true;
                            break;
                        }
                    } else {
                        if (hit.state() == DCOnTrack::OnTrack && (std::abs(hit.residual()) > m_deltaCut * hit.dr())) {
                            hasBadHit = true;
                            break;
                        }
                    }
                }
            }

            if (!hasBadHit) {
                if (m_debugLevel >= 5) std::cout << " keeping candidate  " << segment << std::endl;
                --dropDepth;
                if (m_debugLevel > 0) {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    ++m_dropDepthAcceptCounts[dropDepth];
                }
                return true;
            }
        }

        if (m_debugLevel >= 5) std::cout << " dropHits " << segment << std::endl;

        // if segment has 3 hots and fails cut discard segment
        if (segment.hitsOnTrack() <= 3) {
            --dropDepth;
            if (m_debugLevel > 0) {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_dropDepthRejectCounts[dropDepth];
            }
            return false;
        }

        // create selection hits for segment
        HitSelection selection = m_hitSelector.selectHitsOnTrack(segment.dcs());

        // vector to hold results refit
        SegVec segs;
        segs.reserve(segment.hitsOnTrack());

        double bestChi2(1e9);
        int indexBest(-1);

        MatchDCWithLine matchWithLine;

        // loop over selection and refit without hit
        for (unsigned int i = 0; i < selection.size(); ++i) {
            // if this is a hot
            if (selection[i] == 0) {
                // unselect hit
                selection[i] = 1;

                // refit segment
                Segment result(Line(0., 0., 0.), DCOnTrackVec());
                if (!m_fitter->fit(result, segment.line(), segment.dcs(), selection)) {
                    if (m_debugLevel >= 5) std::cout << " failed fit (dropHits) " << std::endl;
                } else {
                    if (m_debugLevel >= 5) std::cout << " dropping hit " << i << "  " << segment.dcs()[i] << std::endl;

                    // add segment to vector
                    segs.push_back(std::move(result));
                    Segment& newSegment = segs.back();

                    // find all hits within cut-off
                    matchWithLine.set(newSegment.line(), 0.7 * m_deltaCut, MatchDCWithLine::Pull, tubeRadius());
                    bool usePrecise = newSegment.hasT0Shift();
                    DCOnTrackVec hitsOnLine = matchWithLine.match(newSegment.dcs(), &selection, m_recoverMdtHits, usePrecise);

                    // if less then three hits left drop segment
                    if (matchWithLine.hitsOnTrack() <= 2) {
                        // reset flag
                        selection[i] = 0;
                        segs.pop_back();  // remove segment from vector
                        continue;
                    }

                    // reset DCOnTracks
                    newSegment.dcs(hitsOnLine);

                    // check if number of hits used in fit equal to number of hits within 5 sigma of fit result
                    if (matchWithLine.matchDifference() > 0) {
                        if (m_debugLevel >= 5) {
                            std::cout << " Hits on track content changed after match, redoing fit " << matchWithLine.hitsOnTrack()
                                      << std::endl;
                            std::cout << " fit result " << newSegment << std::endl;
                        }
                        // redo refit using new prediction
                        if (!m_fitter->fit(result, newSegment.line(), newSegment.dcs(),
                                           m_hitSelector.selectHitsOnTrack(newSegment.dcs()))) {
                            if (m_debugLevel >= 5) std::cout << " failed fit (dropHits2) " << std::endl;
                            // reset flag
                            selection[i] = 0;
                            segs.pop_back();  // remove segment from vector
                            continue;
                        } else {
                            matchWithLine.set(result.line(), 0.7 * m_deltaCut, MatchDCWithLine::Pull, tubeRadius());
                            usePrecise = result.hasT0Shift();
                            hitsOnLine = matchWithLine.match(result.dcs(), &selection, m_recoverMdtHits, usePrecise);
                            newSegment = result;
                            // reset DCOnTracks
                            newSegment.dcs(hitsOnLine);
                            if (m_debugLevel >= 5) std::cout << " redid refit " << newSegment << std::endl;
                        }
                    }

                    // update match parameters
                    updateMatch(segs.back(), matchWithLine);

                    // keep close hits from initial road as information is not available anymore and shouldn't change to much
                    segs.back().showerHits(segment.showerHits());  // copy showerHits of initial road

                    // keep clusters
                    segs.back().clusters(segment.clusters());  // copy clusters of initial road
                    segs.back().clusterLayers(segment.clusterLayers());

                    // update crossed tubes
                    crossedTubes(segs.back());

                    // remove segments with bad onTrack/emptyTube ratio
                    if (!goodHitRatio(segs.back())) {
                        // reset flag
                        selection[i] = 0;
                        continue;
                    }

                    // flag best candidate
                    double chi2 = segs.back().chi2();
                    if (m_useSegmentQuality) chi2 += 5.1 * (segs.back().hitsOutOfTime() + segs.back().emptyTubes().size());
                    if (segs.empty() || bestChi2 > chi2) {
                        bestChi2 = chi2;
                        indexBest = segs.size() - 1;
                    }

                    if (m_debugLevel >= 5) std::cout << " fit result " << segs.back() << std::endl;
                }

                // reset flag
                selection[i] = 0;
            }
        }

        // if we get here we performed hit dropping
        hasDroppedHit = true;

        if (indexBest == -1) {
            if (m_debugLevel >= 5) std::cout << " not good candidate found " << std::endl;
            --dropDepth;
            if (m_debugLevel > 0) {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_dropDepthRejectCounts[dropDepth];
            }
            return false;
        }

        segment = segs[indexBest];
        if (m_debugLevel >= 5) std::cout << " best candidate " << segment << std::endl;

        bool drop = dropHits(segment, hasDroppedHit, dropDepth);
        --dropDepth;
        return drop;
    }

    SegVec SegmentFinder::cleanSegments(const SegVec& segs) const {
        SegVec segments = refitSegments(segs);

        // no segments found return empty segment vector
        // if only one segment found no further processing needed
        if (segments.size() <= 1) return segments;

        // vector to hold clean segments
        SegVec selectedSegments;

        // sort segments
        if (m_doAllHitSort)
            std::stable_sort(segments.begin(), segments.end(), SortSegByNumberOfHitsAndChi2());
        else
            std::stable_sort(segments.begin(), segments.end(), SortSegByNumberOfMDTHitsAndChi2());

        if (m_debugLevel >= 3) {
            std::cout << " sorting segments " << std::endl;
            for (SegIt it = segments.begin(); it != segments.end(); ++it) { std::cout << *it << std::endl; }
        }
        // first segment is automatically accepted
        selectedSegments.push_back(segments.front());

        // function object to count number of shared hits between segments
        SharedHitsSegments sharedHits(false);

        // discard all segments that share hits with the best segment(s)
        SegIt it = segments.begin() + 1;
        SegIt it_end = segments.end();
        for (; it != it_end; ++it) {
            unsigned int shareHits(0);
            bool addSeg(true);
            for (SegIt sit = selectedSegments.begin(); sit != selectedSegments.end(); ++sit) {
                // count number of shared hits
                shareHits = sharedHits(*it, *sit);
                if (shareHits != 0) {
                    unsigned int nmdtHitsIt = it->hitsOnTrack();
                    unsigned int nmdtHitsSit = sit->hitsOnTrack();

                    // deal with curved segments
                    if (it->hasCurvatureParameters()) {
                        if (sit->hasCurvatureParameters()) continue;
                        if (shareHits == it->hitsOnTrack()) {
                            if ((sit->chi2() < it->chi2()) || (std::abs(it->deltaAlpha()) < 0.01)) {
                                addSeg = false;
                                break;
                            }
                        } else if (sit->hasCurvatureParameters()) {
                            if (std::abs(it->line().phi() - sit->line().phi()) < 0.05 &&
                                std::abs(it->deltaAlpha() - sit->deltaAlpha()) < 0.01) {
                                ResidualWithLine resWithLine(it->line());
                                if (std::abs(resWithLine.residual(sit->line().position())) < 0.1) {
                                    addSeg = false;
                                    break;
                                }
                            }
                        } else {
                            // if the curved segment has unique hits, keep both
                            sit->ambigue(2);
                            continue;
                        }
                    }

                    // reject segment if it containes less hits on track
                    if (nmdtHitsSit > nmdtHitsIt) {
                        addSeg = false;
                        break;
                    }

                    // reject segment if slected has clusters and current doesn't
                    if (!sit->clusters().empty() && it->clusters().empty()) {
                        addSeg = false;
                        break;
                    }
                    if (std::abs(it->chi2() - sit->chi2()) > sit->hitsOnTrack()) {
                        addSeg = false;
                        break;
                    }

                    // if number of hits the same reject if difference in angle smaller 0.05 and distance between lines small 0.1 mm
                    if (std::abs(it->line().phi() - sit->line().phi()) < 0.05) {
                        ResidualWithLine resWithLine(it->line());
                        if (std::abs(resWithLine.residual(sit->line().position())) < 0.1) {
                            addSeg = false;
                            break;
                        }
                    }

                    // mark segment as ambiguous
                    sit->ambigue(2);
                }
            }
            if (addSeg) {
                selectedSegments.push_back(*it);
                // set number of ambiguities
                if (shareHits != 0) selectedSegments.back().ambigue(2);
            }
        }

        return selectedSegments;
    }

    void SegmentFinder::handleSeedPair(const DriftCircle& seed1, const DriftCircle& seed2, const DCVec& dcs, const CLVec& cls,
                                       MatchDCWithLine& matchWithLine,
                                       ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const {
        // create tangent lines
        TangentToCircles::LineVec lines = TrkDriftCircleMath::TangentToCircles::tangentLines(seed1, seed2);

        // loop over tangent lines match dcs with line
        for (TangentToCircles::LineVec::const_iterator lit = lines.begin(); lit != lines.end(); ++lit) {
            // only accept segments with reasonable angle
            if (!directionCheck(lit->direction())) {
                if (m_debugLevel >= 19) {
                    std::cout << " failed direction cut " << lit->direction() * m_roadDir << " line: " << lit->phi() << " road "
                              << atan2(m_roadDir.y(), m_roadDir.x()) << " chamber " << atan2(m_chamberDir.y(), m_chamberDir.x())
                              << std::endl;
                }
                continue;
            }
            matchWithLine.set(*lit, m_roadWidth, MatchDCWithLine::Road, tubeRadius());
            const DCOnTrackVec& hitsOnLine = matchWithLine.match(dcs);

            if (matchWithLine.hitsOnTrack() <= 2) {
                if (m_debugLevel >= 19) { std::cout << " too few hits associated " << matchWithLine.hitsOnTrack() << std::endl; }
                continue;
            }
            if (matchWithLine.hitsOutOfTime() + matchWithLine.deltas() >= matchWithLine.hitsOnTrack()) {
                if (m_debugLevel >= 19) {
                    std::cout << " too many outliers: hoo " << matchWithLine.hitsOutOfTime() << " delta " << matchWithLine.deltas()
                              << " hot " << matchWithLine.hitsOnTrack() << std::endl;
                }
                continue;
            }

            Segment result(Line(0., 0., 0.), DCOnTrackVec());
            if (!m_fitter->fit(result, *lit, hitsOnLine, m_hitSelector.selectHitsOnTrack(hitsOnLine))) {
                if (m_debugLevel >= 3) std::cout << " failed fit " << std::endl;
                continue;
            }

            // also apply direction cuts on output
            if (!directionCheck(result.line().direction())) {
                if (m_debugLevel >= 19) {
                    std::cout << " failed direction cut (2) " << result.line().direction() * m_roadDir << " line: " << result.line().phi()
                              << " road " << atan2(m_roadDir.y(), m_roadDir.x()) << " chamber " << atan2(m_chamberDir.y(), m_chamberDir.x())
                              << std::endl;
                }
                continue;
            }
            Segment seg = result;

            // update match parameters
            updateMatch(seg, matchWithLine);

            // add shower hits
            seg.showerHits(matchWithLine.showerHits());

            // associate clusters with segment

            // hack for avoiding bad alloc problem (bug 45261), should be properly fixed!
            if (cls.size() < 500) { associateClusters(seg, cls); }
            // calculate crossed tubes
            crossedTubes(seg);

            // remove segments with bad onTrack/emptyTube ratio
            if (!goodHitRatio(seg)) {
                if (m_debugLevel >= 3) std::cout << " candidate dropped due to hit ratio " << seg << std::endl;
                continue;
            }

            if (m_debugLevel >= 3) { std::cout << " new segment candidate " << seg << std::endl; }

            // add segment to collection
            segments.insert(seg);
        }
    }

    void SegmentFinder::crossedTubes(Segment& seg) const {
        if (m_mdtGeometry) {
            // calculated number of passed tubes

            const DCVec ct = m_mdtGeometry->tubesPassedByLine(seg.line());
            const MatchResult result = m_matchCrossed(seg.dcs(), ct);
            unsigned int tubesMl1 {0}, tubesMl2 {0};
            DCOnTrackCit doit = result.first.begin();
            DCOnTrackCit doit_end = result.first.end();
            for (; doit != doit_end; ++doit) {
                if (doit->state() == DCOnTrack::CloseDC) continue;

                if (doit->id().ml() == 0)
                    ++tubesMl1;
                else
                    ++tubesMl2;
            }

            DCCit dcit = ct.begin();
            DCCit dcit_end = ct.end();
            for (; dcit != dcit_end; ++dcit) {
                if (dcit->id().ml() == 0)
                    ++tubesMl1;
                else
                    ++tubesMl2;
            }

            if (tubesMl1 + tubesMl2 != ct.size() + result.first.size()) {
                if (m_debugLevel >= 1)
                    std::cout << " ERROR in empty tube calculation: ml1 " << tubesMl1 << " ml2 " << tubesMl2 << " tot "
                              << ct.size() + result.first.size() << std::endl;
            }

            seg.crossedTubes(tubesMl1, tubesMl2);

            seg.emptyTubes(result.second);
            if (m_debugLevel >= 1) {
                if (seg.crossedTubes() != seg.hitsOnTrack() + seg.deltas() + seg.hitsOutOfTime() + seg.emptyTubes().size()) {
                    std::cout << "  ---- mismatch!!! " << std::endl;
                    DCCit eit = result.second.begin();
                    DCCit eit_end = result.second.end();
                    for (; eit != eit_end; ++eit) { std::cout << "  match result  " << *eit << std::endl; }
                    std::cout << seg << std::endl;
                }
            }
        }
    }

    void SegmentFinder::updateMatch(Segment& seg, MatchDCWithLine& matchWithLine) {
        seg.deltas(matchWithLine.deltas());
        seg.hitsOutOfTime(matchWithLine.hitsOutOfTime());
        seg.hitsOnTrack(matchWithLine.hitsOnTrack());
        seg.hitsPerMl(matchWithLine.hitsMl1(), matchWithLine.hitsMl2());
        seg.closeHits(matchWithLine.closeHits());
    }

    void SegmentFinder::twoMultiLayerScan(const DCVec& seeds_ml1, const DCVec& seeds_ml2, const DCVec& dcs, const CLVec& cls,
                                          ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const {
        MatchDCWithLine matchWithLine;

        // hack for bug #45261, should be properly fixed!
        if (seeds_ml1.size() * seeds_ml2.size() > 2500) return;

        // combine a dc from the first set with a dc from the second set
        for (DCCit it1 = seeds_ml1.begin(); it1 != seeds_ml1.end(); ++it1) {
            for (DCVec::const_reverse_iterator it2 = seeds_ml2.rbegin(); it2 != seeds_ml2.rend(); ++it2) {
                // find segments using the two seeds
                handleSeedPair(*it1, *it2, dcs, cls, matchWithLine, segments);
            }
        }
    }

    void SegmentFinder::fullScan(const DCVec& seeds, const DCVec& dcs, const CLVec& cls,
                                 ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const {
        MatchDCWithLine matchWithLine;

        // use all combinations of segments as seed

        // hack for bug #45261, should be properly fixed!
        if (seeds.size() > 50) return;

        for (DCCit it = seeds.begin(); it != seeds.end(); ++it) {
            for (DCVec::const_reverse_iterator rit = seeds.rbegin(); rit != seeds.rend(); ++rit) {
                // break of inner loop when *it == *rit
                if (std::distance(seeds.begin(), it) >= std::distance(rit, seeds.rend() - 1)) break;

                // hits in the same ml are not combined
                if (std::abs(it->position().y() - rit->position().y()) < 1.) break;

                // find segments using the two seeds
                handleSeedPair(*it, *rit, dcs, cls, matchWithLine, segments);
            }
        }
    }

    DCVec SegmentFinder::removeDCOnSegments(const DCVec& dcs, const SegVec& segs) {
        if (segs.empty()) return dcs;

        DCVec newdcs;

        SameTube sameTube;

        DCCit dit = dcs.begin();
        DCCit dit_end = dcs.end();

        for (; dit != dit_end; ++dit) {
            bool found(false);
            for (SegCit sit = segs.begin(); sit != segs.end(); ++sit) {
                DCOnTrackCit pos = std::lower_bound(sit->dcs().begin(), sit->dcs().end(), *dit, SortDcsByY());
                if (pos != sit->dcs().end() && pos->state() != DCOnTrack::CloseDC && sameTube(*pos, *dit)) {
                    found = true;
                    break;
                }
            }
            if (!found) { newdcs.push_back(*dit); }
        }

        return newdcs;
    }

    void SegmentFinder::associateClusters(SegVec& segs, const CLVec& cls) const {
        // loop over all segments pass to associateClusters
        SegIt sit = segs.begin();
        SegIt sit_end = segs.end();
        for (; sit != sit_end; ++sit) { associateClusters(*sit, cls); }
    }
    void SegmentFinder::associateClusters(Segment& seg, const CLVec& cls) const {
        // calculate residuals with line
        ResidualWithSegment resSeg(seg);

        // store associated clusters
        CLVec assCls;

        bool hasClustersBefore = false;
        bool hasClustersAfter = false;

        CLCit cit = cls.begin();
        CLCit cit_end = cls.end();
        for (; cit != cit_end; ++cit) {
            // always assign phi hits
            if (cit->id().measuresPhi() == 1) {
                if (m_debugLevel >= 1)
                    std::cout << " phi hit, not associated: id " << cit->id().id() << "  pos " << cit->position() << std::endl;
                continue;
            }

            // calculate residual
            double res = resSeg.residual(*cit);
            double error = std::sqrt(cit->err() * cit->err() + resSeg.trackError2(*cit));
            double pull = res / error;

            if (m_debugLevel >= 4)
                std::cout << " handling cluster " << cit->id() << " res " << res << " pull " << pull << " hit error " << cit->err()
                          << " track error " << sqrt(resSeg.trackError2(*cit)) << " index " << cit->index() << " pos " << cit->position();

            double pullCut = cit->id().isTgc() ? m_tgcPullCut : m_rpcPullCut;

            // spatial associate
            if (std::abs(pull) < pullCut) {
                assCls.push_back(*cit);
                if (cit->position().y() < 0.) hasClustersBefore = true;
                if (cit->position().y() > 0.) hasClustersAfter = true;
                if (m_debugLevel >= 4) std::cout << " associated" << std::endl;
            } else {
                if (m_debugLevel >= 4) std::cout << " dropped" << std::endl;
            }
        }

        if (assCls.empty()) return;

        seg.clusters(assCls);
        unsigned int ncl = 0;
        if (hasClustersBefore) ++ncl;
        if (hasClustersAfter) ++ncl;
        seg.clusterLayers(ncl);
    }

    bool SegmentFinder::goodHitRatio(Segment& seg) const {
        if (!m_mdtGeometry) return true;
        bool good = (double)(seg.crossedTubes() - seg.hitsOnTrack()) < m_ratioEmptyTubesCut * seg.hitsOnTrack();
        if (!good) {
            if (seg.hitsMl1() == 0) {
                good = (double)(seg.crossedTubesMl2() - seg.hitsMl2()) < m_ratioEmptyTubesCut * seg.hitsMl2();
            } else if (seg.hitsMl2() == 0) {
                good = (double)(seg.crossedTubesMl1() - seg.hitsMl1()) < m_ratioEmptyTubesCut * seg.hitsMl1();
            }
        }
        return good;
    }

}  // namespace TrkDriftCircleMath
