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
#include "MuGirlStau/IStauBetaTofTool.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonClusterization/RpcHitClustering.h"
#include "MuGirlStau/MuonBetaCalculationUtils.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "TrkTrack/Track.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"

#include <vector>
#include <iostream>

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
  class IMdtDriftCircleOnTrackCreator;
  class IMuonRecoValidationTool;
  class ITrackAmbiguityProcessorTool;
  class IMuonHitTimingTool;
  class IMuonPRDSelectionTool;
  class RpcClusterOnTrack;
}
class MdtCalibrationDbSvc;

namespace Rec {
  class ICombinedMuonTrackBuilder;
}

namespace Trk {
  class ITrackAmbiguityProcessorTool;
  class IUpdator;
}

namespace MuonCombined { 
  class MuonInsideOutRecoTool;

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
    typedef std::vector< RpcTimeMeasurement > RpcTimeMeasurementVec;

    struct MaximumData {
      MaximumData( const Muon::MuonSystemExtension::Intersection& intersection_,
                   const MuonHough::MuonLayerHough::Maximum* maximum_,
                   std::vector< std::shared_ptr<const Muon::MuonClusterOnTrack> > phiClusterOnTracks_ ) : 
        intersection(intersection_), maximum(maximum_), phiClusterOnTracks(phiClusterOnTracks_) {}
      Muon::MuonSystemExtension::Intersection intersection;
      const MuonHough::MuonLayerHough::Maximum* maximum;
      std::vector< std::shared_ptr<const Muon::MuonClusterOnTrack> > phiClusterOnTracks;

      std::vector< std::shared_ptr<const Muon::MuonSegment> > t0fittedSegments;
      RpcTimeMeasurementVec                                   rpcTimeMeasurements;
      std::vector<BetaSeed>                                   betaSeeds;
    };
    typedef std::vector< std::shared_ptr<MaximumData> > MaximumDataVec;

    struct LayerData {
      LayerData(const Muon::MuonSystemExtension::Intersection& intersection_) : intersection(intersection_) {}
      Muon::MuonSystemExtension::Intersection     intersection;
      MaximumDataVec maximumDataVec;
    };
    typedef std::vector< LayerData > LayerDataVec;

    struct AssociatedData  {
      LayerDataVec  layerData;
      PhiMaximumVec phiMaxima;
    };

    struct Candidate {
      Candidate( const BetaSeed& betaSeed_ ) : betaSeed(betaSeed_) {}  
      
      // inital parameters
      BetaSeed                       betaSeed;
      
      // information filled by createCandidates: associated layers with MaximumData, time measurements and final beta fit result
      LayerDataVec                      layerDataVec;
      Muon::TimePointBetaFit::HitVec    hits;
      Muon::TimePointBetaFit::FitResult betaFitResult;

      // information filled by refineCandidates: segments found using the beta of the final fit
      std::vector< Muon::MuonLayerRecoData > allLayers;
      
      // information filled by the final tracking: MuonCandidate and the track
      std::unique_ptr<const Muon::MuonCandidate> muonCandidate;
      std::unique_ptr<const Trk::Track>          combinedTrack;
      MuGirlNS::StauHits                         stauHits;
      Muon::TimePointBetaFit::FitResult          finalBetaFitResult;

    };
    typedef std::vector< std::shared_ptr<Candidate> > CandidateVec;
    
    struct TruthInfo {
      TruthInfo(int pdgId_, float mass_, float beta_) : pdgId(pdgId_),mass(mass_),beta(beta_) {}
      int pdgId;
      float mass;
      float beta;
      std::string toString() const {
        std::ostringstream sout;
        sout << " pdgId " << pdgId << " mass " << mass << " beta " << beta;
        return sout.str();
      }
    };

    /** Default AlgTool functions */
    MuonStauRecoTool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MuonStauRecoTool();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */   
    virtual void extend( const InDetCandidateCollection& inDetCandidates ) override;

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate );

    /** associate Hough maxima to intersection */
    void associateHoughMaxima( LayerData& layerData );

    /** extract RPC hit timing */
    void extractRpcTimingFromMaximum( const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData );
    
    /** create Rpc hit timing for a set of clusters */
    void createRpcTimeMeasurementsFromClusters( const Muon::MuonSystemExtension::Intersection& intersection,
                                                const std::vector< Muon::RpcClusterObj>& clusterObjects, 
                                                RpcTimeMeasurementVec& rpcTimeMeasurements );

    /** find segments for a given maximum */
    void findSegments(  const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData, 
                        std::vector< std::shared_ptr<const Muon::MuonSegment> >& t0fittedSegments,
                        const ToolHandle<Muon::IMuonPRDSelectionTool>& muonPRDSelectionTool,
                        const ToolHandle<Muon::IMuonSegmentMaker>& segmentMaker  ) const;

    /** match extension to Hough maxima, extract time measurements, create candidates, run segment finding */
    bool processMuonSystemExtension(  const xAOD::TrackParticle& indetTrackParticle, const Muon::MuonSystemExtension& muonSystemExtension, CandidateVec& candidates );
    
    /** associate Hough maxima and associate time measurements */
    bool extractTimeMeasurements( const Muon::MuonSystemExtension& muonSystemExtension, AssociatedData& associatedData );

    /** create candidates from the beta seeds */
    bool createCandidates( const AssociatedData& associatedData, CandidateVec& candidates ) const;

    /** extend a CandidateVec with the next LayerData */
    void extendCandidates( CandidateVec& candidates,std::set<const MaximumData*>& usedMaximumData,
                           LayerDataVec::const_iterator it, LayerDataVec::const_iterator it_end ) const;

    /** calculate the beta seeds for a give MaximumData */
    void getBetaSeeds( MaximumData& maximumData ) const;

    /** extract hits for the beta fit, returns true if hits were added */
    bool extractTimeHits( const MaximumData& maximumData, Muon::TimePointBetaFit::HitVec& hits, const BetaSeed* seed=0 ) const;

    /** refine candidates: find segments for the given beta */
    bool refineCandidates( CandidateVec& candidates );

    /** combine reconstruction */
    bool combineCandidates( const xAOD::TrackParticle& indetTrackParticle, CandidateVec& candidates );

    /** resolve ambiguities between the candidates */
    bool resolveAmbiguities( CandidateVec& candidates );
    
    /** create final tag object and add it to the inDetCandidate */
    void addTag( const InDetCandidate& inDetCandidate, Candidate& candidate ) const;
    
    /** extract time measurements from the track associated with the candidate */
    void extractTimeMeasurementsFromTrack( Candidate& candidate  );

    /** extract truth from the indetTrackParticle */
    TruthInfo* getTruth(  const xAOD::TrackParticle& indetTrackParticle ) const;

    /** if truth tracking is enabled, return whether the pdg is selected */
    bool selectTruth( const TruthInfo* truthInfo ) const {
      if( !m_useTruthMatching ) return true;
      if( truthInfo && m_selectedPdgs.count(truthInfo->pdgId) ) return true;
      return false;
    }
    
    /** helper function to add Candidate to ntuple */
    void addCandidatesToNtuple( const xAOD::TrackParticle& indetTrackParticle, const CandidateVec& candidates, int stage );

    /** */
    void mdtTimeCalibration( const Identifier& id, float& time, float& error ) const;
    void rpcTimeCalibration( const Identifier& id, float& time, float& error ) const;
    void segmentTimeCalibration( const Identifier& id, float& time, float& error ) const;


    /** tool handles */
    ToolHandle<Muon::MuonIdHelperTool>               m_idHelper; 
    ToolHandle<Muon::MuonEDMPrinterTool>             m_printer; 
    ToolHandle<Muon::MuonEDMHelperTool>              m_edmHelper; 
    ToolHandle<Muon::IMuonSystemExtensionTool>       m_muonSystemExtentionTool;
    ToolHandle<Muon::IMuonSegmentMaker>              m_segmentMaker;
    ToolHandle<Muon::IMuonSegmentMaker>              m_segmentMakerT0Fit;
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool>  m_segmentMatchingTool;
    ToolHandle<Muon::IMuonRecoValidationTool>        m_recoValidationTool;
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>    m_trackAmbibuityResolver;
    ToolHandle<Muon::IMuonHitTimingTool>             m_hitTimingTool;
    ToolHandle<Muon::MuonLayerHoughTool>             m_layerHoughTool;
    ToolHandle<Muon::IMuonPRDSelectionTool>          m_muonPRDSelectionTool;
    ToolHandle<Muon::IMuonPRDSelectionTool>          m_muonPRDSelectionToolStau;
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtCreator;
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>  m_mdtCreatorStau;
    ToolHandle<MuGirlNS::IStauBetaTofTool>           m_stauTofTool;
    ToolHandle<MuonCombined::MuonInsideOutRecoTool>  m_insideOutRecoTool;
    ToolHandle<Trk::IUpdator>                        m_updator;
    ServiceHandle<MdtCalibrationDbSvc>               m_mdtCalibrationDbSvc;
    Muon::MuonSectorMapping                          m_muonSectorMapping;

    struct TruthMatchingCounters {
      TruthMatchingCounters() : ntruth(0) {}
      unsigned int ntruth;
      std::map<int,int> nrecoPerStage;

      void fillTruth() {
        ++ntruth;
      }
      void fillStage(int stage) {
        ++nrecoPerStage[stage];
      }
      std::string summary() const {
        std::ostringstream sout;
        float scale = ntruth != 0 ? 1./ntruth : 1.;
        sout << " Truth " << ntruth;
        for( auto& stage : nrecoPerStage ){ 
          sout << std::endl << " stage " << stage.first << " reco " << stage.second << " eff " << stage.second*scale;
        }
        return sout.str();
      }
    };

    // map to store truth counters for a given pdgID (uses abs(pdg))
    mutable std::map<int,TruthMatchingCounters> m_truthMatchingCounters;
    
    TruthMatchingCounters* getTruthMatchingCounters( const TruthInfo* truthInfo ) const {
      if( !truthInfo ) return nullptr;
      auto pos = m_truthMatchingCounters.find(std::abs(truthInfo->pdgId));
      if( pos == m_truthMatchingCounters.end() ) return nullptr;
      return &pos->second;
    }

    bool m_doSummary; // enable summary output
    bool m_useTruthMatching; // enable usage of truth info for reconstruction
    bool m_doTruth; // enable truth matching
    IntegerArrayProperty m_pdgsToBeConsidered; // pdg's considered in truth matching
    std::set<int> m_selectedPdgs; // set storing particle PDG's considered for matching
    
    bool m_segmentMDTT;
    double m_ptThreshold;
    double m_houghAssociationPullCut;
    double m_mdttBetaAssociationCut;    
    double m_rpcBetaAssociationCut;    
    double m_segmentBetaAssociationCut;    
    bool m_ignoreSiAssocated;
  };
}



#endif
 


