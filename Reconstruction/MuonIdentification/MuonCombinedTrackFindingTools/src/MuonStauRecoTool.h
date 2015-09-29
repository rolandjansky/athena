/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTAURECOTOOL_H
#define MUON_MUONSTAURECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h" 
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuGirlStau/TimePointBetaFit.h"

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Muon {

  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class IMuonSystemExtensionTool;
  class IMuonSegmentMaker;
  class IMuonLayerSegmentMatchingTool;
  class IMuonLayerAmbiguitySolverTool;
  class IMuonCandidateTrackBuilderTool;
  class IMuonRecoValidationTool;
  class ITrackAmbiguityProcessorTool;
  class IMuonHitTimingTool;
  class IMuonPRDSelectionTool;
  class RpcClusterOnTrack;
}

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace Trk {
  class ITrackAmbiguityProcessorTool;
}

namespace MuonCombined { 
  
  /**
     ID seeded Stau reconstruction

     Steps:
     - Extrapolate ID track to muon system
     - associate Hough maxima in road around ID track
     - 
   */
  class MuonStauRecoTool : virtual public MuonCombined::IMuonCombinedInDetExtensionTool,  public AthAlgTool {
  public:
    
    typedef std::vector<const MuonHough::MuonLayerHough::Maximum*>    MaximumVec;
    typedef std::vector<const MuonHough::MuonPhiLayerHough::Maximum*> PhiMaximumVec;
    
    struct BetaSeed {
      BetaSeed(float beta_, float error_) : beta(beta_), error(error_) {}
      float beta;
      float error;
    };

    struct RpcTimeMeasurement {
      std::vector< std::shared_ptr<const Muon::RpcClusterOnTrack> > rpcClusters;
      float time;
      float error;
    };

    struct MaximumData {
      MaximumData( const Muon::MuonSystemExtension::Intersection& intersection_,
                   const MuonHough::MuonLayerHough::Maximum* maximum_ ) : intersection(intersection_), maximum(maximum_) {}
      Muon::MuonSystemExtension::Intersection intersection;
      const MuonHough::MuonLayerHough::Maximum* maximum;
      std::vector< std::shared_ptr<const Muon::MuonSegment> > t0fittedSegments;
      std::vector< RpcTimeMeasurement >                       rpcTimeMeasurements;
      std::vector<BetaSeed>                                   betaSeeds;
    };

    struct LayerData {
      LayerData(const Muon::MuonSystemExtension::Intersection& intersection_) : intersection(intersection_) {}
      Muon::MuonSystemExtension::Intersection intersection;
      std::vector<MaximumData>                maximumDataVec;
    };

    struct AssociatedData  {
      std::vector< LayerData > layerData;
      PhiMaximumVec            phiMaxima;
    };

    struct Candidate {
      Candidate( const MuonHough::MuonPhiLayerHough::Maximum* phiMaximum_, std::vector< const MaximumData* >&& maxima_ ) : phiMaximum(phiMaximum_), maxima(maxima_) {}  
      const MuonHough::MuonPhiLayerHough::Maximum* phiMaximum;
      std::vector< const MaximumData* > maxima;

      float betaSeed;
      std::vector< std::shared_ptr<const Muon::MuonSegment> > segments;
    };
    typedef std::vector<Candidate> CandidateVec;
    
    /** Default AlgTool functions */
    MuonStauRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonStauRecoTool();
    StatusCode initialize();
    StatusCode finalize();

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */   
    void extend( const InDetCandidateCollection& inDetCandidates ) const;

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate ) const;

    /** associate Hough maxima to intersection */
    void associateHoughMaxima( LayerData& layerData ) const;

    /** extract RPC hit timing */
    void extractRpcTimingFromMaximum( const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData ) const;

    /** find t0 segments for a given maximum */
    void findT0Segments(  const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData, 
                          std::vector< std::shared_ptr<const Muon::MuonSegment> >& t0fittedSegments ) const;

    /** associate Hough maxima and associate time measurements */
    void extractTimeMeasurements( const Muon::MuonSystemExtension& muonSystemExtension, AssociatedData& associatedData ) const;

    /** create candidates */
    void createCandidates( const AssociatedData& associatedData, CandidateVec& candidates ) const;

    /** calculate the beta seeds for a give MaximumData */
    void getBetaSeeds( MaximumData& maximumData ) const;

    /** extract hits for the beta fit */
    void extractTimeHits( const MaximumData& maximumData, Muon::TimePointBetaFit::HitVec& hits, const BetaSeed* seed=0 ) const;

    /** refine candidates */
    void refineCandidates( CandidateVec& candidates ) const;


    /** tool handles */
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper; 
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer; 
    ToolHandle<Muon::MuonEDMHelperTool>              m_edmHelper; 
    ToolHandle<Muon::IMuonSystemExtensionTool>       m_muonSystemExtentionTool;
    ToolHandle<Muon::IMuonSegmentMaker>              m_segmentMaker;
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<Muon::IMuonLayerAmbiguitySolverTool>  m_ambiguityResolver;
    ToolHandle<Muon::IMuonCandidateTrackBuilderTool> m_candidateTrackBuilder;
    ToolHandle<Muon::IMuonRecoValidationTool>        m_recoValidationTool;
    ToolHandle<Rec::ICombinedMuonTrackBuilder>       m_trackFitter;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>    m_trackAmbibuityResolver;
    ToolHandle<Muon::IMuonHitTimingTool>             m_hitTimingTool;
    ToolHandle<Muon::MuonLayerHoughTool>             m_layerHoughTool;
    ToolHandle<Muon::IMuonPRDSelectionTool>          m_muonPRDSelectionTool;
    Muon::MuonSectorMapping                          m_muonSectorMapping;
    
  };
}



#endif
 


