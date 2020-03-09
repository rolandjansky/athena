/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SEGMENTFINDER_H
#define DCMATH_SEGMENTFINDER_H

#include "CxxUtils/checker_macros.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/LocDir.h"
#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/Cluster.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"
#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/IsSubsetSegment.h"
#include "TrkDriftCircleMath/ResolvedCollection.h"
#include "TrkDriftCircleMath/SortDcsByY.h"
#include "TrkDriftCircleMath/ChamberGeometry.h"
#include "TrkDriftCircleMath/TangentToCircles.h"
#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/DCSLHitSelector.h"
#include "TrkDriftCircleMath/MatchDCWithLine.h"
#include "TrkDriftCircleMath/MatchCrossedTubes.h"
#include "TrkDriftCircleMath/CurvedSegmentFinder.h"

#include <cmath>
#include <mutex>
#include <vector>

namespace TrkDriftCircleMath {

  class SegmentFinder{
  public:
    SegmentFinder();
    /// copy c'tor deleted to prevent ownership/leak issues 
    SegmentFinder(const SegmentFinder &) = delete;
    /// assignment deleted to prevent ownership/leak issues 
    SegmentFinder & operator=(const SegmentFinder &) = delete;
    //
    SegmentFinder(double roadWidth, double deltaCut, bool fullScan);
    ~SegmentFinder();
    void handleHits( const DCVec& dcs, const CLVec& clusters,
                     ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments ) const;

    SegVec findSegments( const DCVec& dcs ) const;
    SegVec findSegments( const DCVec& dcs, const CLVec& clusters ) const;
    SegVec cleanSegments( const SegVec& segments ) const;
    SegVec refitSegments( const SegVec& segs ) const;
    bool   dropHits( Segment& segment, bool& hasDroppedHit, unsigned int& dropDepth ) const;

    void  setTGCPullCut(double cut);
    void  setRPCPullCut(double cut);
    void  setRecoverMDT(bool doRecover);
    void  setDropHits(bool doDrop);
    void  setSeedCleaning(bool doCleanup);
    void  setSingleMultiLayerScan( bool doScan );
    void  setChi2DropCut(double chi2); 
    void  setDeltaCut(double cut); 
    void  setRatioEmptyTubesCut(double ratioEmptyTubesCut);
    void  setMdtGeometry( const ChamberGeometry* mdtGeo ) { m_mdtGeometry = mdtGeo; }
    void  setPhiRoad(double phiRoad, double phiChamber, double sinPhiCut = 0.2, bool useRoadPhi = true, bool useChamberPhi = true );
    void  setSortSegmentsUsingAllHits( bool doAllHitsSort );
    void  setUseChamberPhi( bool useChamberPhi );
    void  setRemoveSingleOutliers( bool removeSingleOutliers );
    void  setCurvedSegmentFinder( bool doCurvedSegmentFinder );
    void  setFitter( const DCSLFitter* fitter ) {
      if( m_ownsFitter ) delete m_fitter;
      m_ownsFitter = false;
      m_fitter = fitter;
    }
    void setMaxDropDepth( int max );
    void setResidualCutT0( double resCut );
    void setDeltaCutT0( double deltaCut );
    void setUseSegmentQuality( bool useQ );

    void  debug(bool debug) { m_debugLevel = debug ? 1 : 0; }
    void  debugLevel(int debugLevel) { m_debugLevel = debugLevel; }

  protected:
    //SegVec  fullMatch( const Segment& seg ) const;

    DCVec   selectSeeds( const DCVec& dcs, int maxSerie  ) const;
    std::pair<DCVec,DCVec> splitInMulitlayers( const DCVec& dcs ) const;
    DCVec   removeDCOnSegments( const DCVec& dcs, const SegVec& segs ) const;

    unsigned int emptyCrossedTubes( const Line& line, const DCVec& closeEmptyTubes ) const;
    DCVec        emptyNeighbours( const DCVec& dcs ) const;

    void handleSeedPair( const DriftCircle& seed1, const DriftCircle& seeds2, const DCVec& dcs, const CLVec& cls, MatchDCWithLine& matchWithLine,
                         ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments ) const;
    void fullScan( const DCVec& seeds, const DCVec& dcs, const CLVec& cls,
                   ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments ) const;
    void twoMultiLayerScan( const DCVec& seeds_ml1, const DCVec& seeds_ml2, const DCVec& dcs, const CLVec& cls,
                            ResolvedCollection<Segment, IsSubsetSegment<SortDcsByY>>& segments ) const;

    bool directionCheck( const LocDir& locDir ) const;

    void    associateClusters( SegVec& segs, const CLVec& cls ) const;
    void    associateClusters( Segment& seg, const CLVec& cls ) const;


    void crossedTubes( Segment& seg ) const;
    void updateMatch( Segment& seg, MatchDCWithLine& matchWithLine ) const;

    bool goodHitRatio( Segment& seg ) const;

    /** update the cached values for the phi road and chamber road */
    void updateDirections();

    /** print settings */
    void printSettings() const;

    double tubeRadius() const {
      return m_mdtGeometry ? m_mdtGeometry->tubeRadius() : 14.6;
    }

    double m_deltaCut;
    double m_roadWidth;
    double m_phiRoad;
    LocDir m_roadDir;
    double m_phiChamber;
    LocDir m_chamberDir;

    double m_phiDifCut;
    double m_ratioEmptyTubesCut;
    double m_chi2Cut;
    double m_tgcPullCut;
    double m_rpcPullCut;
    double m_resCutT0;
    double m_deltaCutT0;
    bool   m_useSegmentQuality;
    bool   m_recoverMdtHits;

    TangentToCircles m_tanCreator;
    bool m_ownsFitter;
    const DCSLFitter* m_fitter;
    DCSLHitSelector m_hitSelector;
    MatchCrossedTubes m_matchCrossed;

    bool m_removeSingleOutliers;
    bool m_fullScan;
    bool m_singleMultiLayerScan;
    bool m_seedCleaning;
    bool m_doDrop;
    unsigned int  m_dropDepthMax;
    bool m_doAllHitSort;
    bool m_doCurvedSegmentFinder;
    bool m_useChamberPhi;
    bool m_useRoadPhi;
    const ChamberGeometry* m_mdtGeometry;
    int m_debugLevel;

    mutable std::vector<int> m_dropDepthAcceptCounts ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable std::vector<int> m_dropDepthRejectCounts ATLAS_THREAD_SAFE; // Guarded by m_mutex
    mutable std::mutex m_mutex;

  };

  inline bool SegmentFinder::directionCheck( const LocDir& lineDir ) const {
    
    // check whether the 
    if( m_useRoadPhi ){
      double prodRoad = lineDir*m_roadDir;
      if( prodRoad < 0. ) prodRoad *= -1.;
      if( prodRoad > m_phiDifCut ) return true;
    }
    if( m_useChamberPhi ) {
      double prodChamber = lineDir*m_chamberDir;
      if( prodChamber < 0. ) prodChamber *= -1.;
      if( prodChamber > m_phiDifCut ) return true;
    }
    return false;
  }

}

#endif
