/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SEGMENTFINDER_H
#define DCMATH_SEGMENTFINDER_H

#include <cmath>
#include <memory>
#include <mutex>
#include <vector>

#include "CxxUtils/checker_macros.h"
#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/CurvedSegmentFinder.h"
#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/DCSLHitSelector.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/IsSubsetSegment.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/LocVec2D.h"
#include "TrkDriftCircleMath/MatchCrossedTubes.h"
#include "TrkDriftCircleMath/MatchDCWithLine.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"
#include "TrkDriftCircleMath/ResolvedCollection.h"
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/SortDcsByY.h"
#include "TrkDriftCircleMath/TangentToCircles.h"

namespace TrkDriftCircleMath {

    class SegmentFinder {
    public:
        SegmentFinder();
        /// copy c'tor deleted to prevent ownership/leak issues
        SegmentFinder(const SegmentFinder&) = delete;
        /// assignment deleted to prevent ownership/leak issues
        SegmentFinder& operator=(const SegmentFinder&) = delete;
        //
        SegmentFinder(double roadWidth, double deltaCut, bool fullScan);
        ~SegmentFinder();
        void handleHits(const DCVec& dcs, const CLVec& clusters, ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const;

        SegVec findSegments(const DCVec& dcs) const;
        SegVec findSegments(const DCVec& dcs, const CLVec& clusters) const;
        SegVec cleanSegments(const SegVec& segments) const;
        SegVec refitSegments(const SegVec& segs) const;
        bool dropHits(Segment& segment, bool& hasDroppedHit, unsigned int& dropDepth) const;

        void setTGCPullCut(double cut);
        void setRPCPullCut(double cut);
        void setRecoverMDT(bool doRecover);
        void setDropHits(bool doDrop);
        void setSeedCleaning(bool doCleanup);
        void setSingleMultiLayerScan(bool doScan);
        void setChi2DropCut(double chi2);
        void setDeltaCut(double cut);
        void setRatioEmptyTubesCut(double ratioEmptyTubesCut);
        void setMdtGeometry(const ChamberGeometry* mdtGeo) { m_mdtGeometry = mdtGeo; }
        void setPhiRoad(double phiRoad, double phiChamber, double sinPhiCut = 0.2, bool useRoadPhi = true, bool useChamberPhi = true);
        void setSortSegmentsUsingAllHits(bool doAllHitsSort);
        void setUseChamberPhi(bool useChamberPhi);
        void setRemoveSingleOutliers(bool removeSingleOutliers);
        void setCurvedSegmentFinder(bool doCurvedSegmentFinder);
        void setFitter(std::shared_ptr<const DCSLFitter> fitter) { m_fitter = fitter; }
        void setMaxDropDepth(int max);
        void setResidualCutT0(double resCut);
        void setDeltaCutT0(double deltaCut);
        void setUseSegmentQuality(bool useQ);

        void debugLevel(int debugLevel) { m_debugLevel = debugLevel; }

    protected:
        static DCVec selectSeeds(const DCVec& dcs, int maxSerie) ;
        static std::pair<DCVec, DCVec> splitInMulitlayers(const DCVec& dcs) ;
        static DCVec removeDCOnSegments(const DCVec& dcs, const SegVec& segs) ;

        unsigned int emptyCrossedTubes(const Line& line, const DCVec& closeEmptyTubes) const;
        DCVec emptyNeighbours(const DCVec& dcs) const;

        void handleSeedPair(const DriftCircle& seed1, const DriftCircle& seeds2, const DCVec& dcs, const CLVec& cls,
                            MatchDCWithLine& matchWithLine, ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const;
        void fullScan(const DCVec& seeds, const DCVec& dcs, const CLVec& cls,
                      ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const;
        void twoMultiLayerScan(const DCVec& seeds_ml1, const DCVec& seeds_ml2, const DCVec& dcs, const CLVec& cls,
                               ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments) const;

        bool directionCheck(const LocVec2D& LocVec2D) const;

        void associateClusters(SegVec& segs, const CLVec& cls) const;
        void associateClusters(Segment& seg, const CLVec& cls) const;

        void crossedTubes(Segment& seg) const;
        static void updateMatch(Segment& seg, MatchDCWithLine& matchWithLine) ;

        bool goodHitRatio(Segment& seg) const;

        /** update the cached values for the phi road and chamber road */
        void updateDirections();

        /** print settings */
        void printSettings() const;

        double tubeRadius() const { return m_mdtGeometry ? m_mdtGeometry->tubeRadius() : 14.6; }

        double m_deltaCut{5.};
        double m_roadWidth{1.5};
        double m_phiRoad{M_PI_2};
        LocVec2D m_roadDir{0., 0.};
        double m_phiChamber{M_PI_2};
        LocVec2D m_chamberDir{0., 0.};

        double m_phiDifCut{0.5};
        double m_ratioEmptyTubesCut{1.};
        double m_chi2Cut{10};
        double m_tgcPullCut{5};
        double m_rpcPullCut{5};
        double m_resCutT0{1.};
        double m_deltaCutT0{5.};
        bool m_useSegmentQuality{false};
        bool m_recoverMdtHits{true};

        TangentToCircles m_tanCreator;
        std::shared_ptr<const DCSLFitter> m_fitter;
        DCSLHitSelector m_hitSelector;
        MatchCrossedTubes m_matchCrossed;

        bool m_removeSingleOutliers{true};
        bool m_fullScan{false};
        bool m_singleMultiLayerScan{true};
        bool m_seedCleaning{false};
        bool m_doDrop{true};
        unsigned int m_dropDepthMax{4};
        bool m_doAllHitSort{false};
        bool m_doCurvedSegmentFinder{false};
        bool m_useChamberPhi{true};
        bool m_useRoadPhi{true};
        const ChamberGeometry* m_mdtGeometry{nullptr};
        int m_debugLevel{0};

        mutable std::vector<int> m_dropDepthAcceptCounts ATLAS_THREAD_SAFE{(int)m_dropDepthMax + 2, 0};  // Guarded by m_mutex
        mutable std::vector<int> m_dropDepthRejectCounts ATLAS_THREAD_SAFE{(int)m_dropDepthMax + 1, 0};  // Guarded by m_mutex
        mutable std::mutex m_mutex;
    };

    inline bool SegmentFinder::directionCheck(const LocVec2D& lineDir) const {
        // check whether the
        if (m_useRoadPhi) {
            double prodRoad = lineDir * m_roadDir;
            if (prodRoad < 0.) prodRoad *= -1.;
            if (prodRoad > m_phiDifCut) return true;
        }
        if (m_useChamberPhi) {
            double prodChamber = lineDir * m_chamberDir;
            if (prodChamber < 0.) prodChamber *= -1.;
            if (prodChamber > m_phiDifCut) return true;
        }
        return false;
    }

}  // namespace TrkDriftCircleMath

#endif
