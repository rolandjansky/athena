/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTAURECOTOOL_H
#define MUON_MUONSTAURECOTOOL_H

#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonCombinedToolInterfaces/IMuonLayerSegmentMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"
#include "MuonRecToolInterfaces/IMuonPRDSelectionTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonHitTimingTool.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MdtCalibSvc/MdtCalibrationDbTool.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h" 
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonRecHelperTools/TimePointBetaFitter.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonClusterization/RpcHitClustering.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "TrkTrack/Track.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "MuonInsideOutRecoTool.h"

#include <string>
#include <vector>
#include <iostream>

namespace Muon {
  class RpcClusterOnTrack;
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
      Muon::TimePointBetaFitter::HitVec    hits;
      Muon::TimePointBetaFitter::FitResult betaFitResult;

      // information filled by refineCandidates: segments found using the beta of the final fit
      std::vector< Muon::MuonLayerRecoData > allLayers;
      
      // information filled by the final tracking: MuonCandidate and the track
      std::unique_ptr<const Muon::MuonCandidate> muonCandidate;
      std::unique_ptr<Trk::Track>                combinedTrack;
      MuGirlNS::StauHits                         stauHits;
      Muon::TimePointBetaFitter::FitResult          finalBetaFitResult;

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
    virtual ~MuonStauRecoTool()=default;
    virtual StatusCode initialize() override;

    /**IMuonCombinedInDetExtensionTool interface: extend ID candidate */   
    virtual void extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks,
			 Trk::SegmentCollection* segments) const override;

    virtual void extendWithPRDs(const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
				TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments ) const override;

  private:
    /** handle a single candidate */
    void handleCandidate( const InDetCandidate& inDetCandidate, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, Trk::SegmentCollection* segments ) const;

    /** associate Hough maxima to intersection */
    void associateHoughMaxima( LayerData& layerData ) const;

    /** extract RPC hit timing */
    void extractRpcTimingFromMaximum( const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData ) const;
    
    /** create Rpc hit timing for a set of clusters */
    void createRpcTimeMeasurementsFromClusters( const Muon::MuonSystemExtension::Intersection& intersection,
                                                const std::vector< Muon::RpcClusterObj>& clusterObjects, 
                                                RpcTimeMeasurementVec& rpcTimeMeasurements ) const;

    /** find segments for a given maximum */
    void findSegments(  const Muon::MuonSystemExtension::Intersection& intersection, MaximumData& maximumData, 
                        std::vector< std::shared_ptr<const Muon::MuonSegment> >& t0fittedSegments,
                        const ToolHandle<Muon::IMuonPRDSelectionTool>& muonPRDSelectionTool,
                        const ToolHandle<Muon::IMuonSegmentMaker>& segmentMaker  ) const;

    /** match extension to Hough maxima, extract time measurements, create candidates, run segment finding */
    bool processMuonSystemExtension(  const xAOD::TrackParticle& indetTrackParticle, const Muon::MuonSystemExtension& muonSystemExtension, CandidateVec& candidates );
    
    /** associate Hough maxima and associate time measurements */
    bool extractTimeMeasurements( const Muon::MuonSystemExtension& muonSystemExtension, AssociatedData& associatedData ) const;

    /** create candidates from the beta seeds */
    bool createCandidates( const AssociatedData& associatedData, CandidateVec& candidates ) const;

    /** extend a CandidateVec with the next LayerData */
    void extendCandidates( CandidateVec& candidates,std::set<const MaximumData*>& usedMaximumData,
                           LayerDataVec::const_iterator it, LayerDataVec::const_iterator it_end ) const;

    /** calculate the beta seeds for a give MaximumData */
    void getBetaSeeds( MaximumData& maximumData ) const;

    /** extract hits for the beta fit, returns true if hits were added */
    bool extractTimeHits( const MaximumData& maximumData, Muon::TimePointBetaFitter::HitVec& hits, const BetaSeed* seed=0 ) const;

    /** refine candidates: find segments for the given beta */
    bool refineCandidates( CandidateVec& candidates ) const;

    /** combine reconstruction */
    bool combineCandidates( const xAOD::TrackParticle& indetTrackParticle, CandidateVec& candidates ) const;

    /** resolve ambiguities between the candidates */
    bool resolveAmbiguities( CandidateVec& candidates ) const;
    
    /** create final tag object and add it to the inDetCandidate */
    void addTag( const InDetCandidate& inDetCandidate, Candidate& candidate, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, Trk::SegmentCollection* segments ) const;
    
    /** extract time measurements from the track associated with the candidate */
    void extractTimeMeasurementsFromTrack( Candidate& candidate ) const;

    /** extract truth from the indetTrackParticle */
    TruthInfo* getTruth(  const xAOD::TrackParticle& indetTrackParticle ) const;

    /** if truth tracking is enabled, return whether the pdg is selected */
    bool selectTruth( const TruthInfo* truthInfo ) const {
      if( !m_useTruthMatching ) return true;
      if( truthInfo && m_selectedPdgs.count(truthInfo->pdgId) ) return true;
      return false;
    }
    
    /** helper function to add Candidate to ntuple */
    void addCandidatesToNtuple( const xAOD::TrackParticle& indetTrackParticle, const CandidateVec& candidates, int stage ) const;

    /** */
    void mdtTimeCalibration( const Identifier& id, float& time, float& error ) const;
    void rpcTimeCalibration( const Identifier& id, float& time, float& error ) const;
    void segmentTimeCalibration( const Identifier& id, float& time, float& error ) const;

    float calculateTof(const float beta, const float dist) const;
    float calculateBeta(const float time, const float dist) const;

    /** storegate */
    SG::ReadHandleKey<Muon::MuonLayerHoughTool::HoughDataPerSectorVec> m_houghDataPerSectorVecKey {this, 
        "Key_MuonLayerHoughToolHoughDataPerSectorVec", "HoughDataPerSectorVec", "HoughDataPerSectorVec key"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc","Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper","Muon::MuonEDMHelperSvc/MuonEDMHelperSvc","Handle to the service providing the IMuonEDMHelperSvc interface"};

    ToolHandle<Muon::MuonEDMPrinterTool>            m_printer{this,"MuonEDMPrinterTool","Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"};
    ToolHandle<Muon::IMuonSegmentMaker>             m_segmentMaker{this,"MuonSegmentMaker","Muon::DCMathSegmentMaker/DCMathSegmentMaker"};
    ToolHandle<Muon::IMuonSegmentMaker>             m_segmentMakerT0Fit{this,"MuonSegmentMakerT0Fit","Muon::DCMathSegmentMaker/DCMathT0FitSegmentMaker"};
    ToolHandle<Muon::IMuonLayerSegmentMatchingTool> m_segmentMatchingTool{this,"MuonLayerSegmentMatchingTool","Muon::MuonLayerSegmentMatchingTool/MuonLayerSegmentMatchingTool"};
    ToolHandle<Muon::IMuonRecoValidationTool>       m_recoValidationTool{this,"MuonRecoValidationTool",""};
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>   m_trackAmbibuityResolver{this,"TrackAmbiguityProcessor","Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"};
    ToolHandle<Muon::IMuonHitTimingTool>            m_hitTimingTool{this,"MuonHitTimingTool","Muon::MuonHitTimingTool/MuonHitTimingTool"};
    ToolHandle<Muon::IMuonPRDSelectionTool>         m_muonPRDSelectionTool{this,"MuonPRDSelectionTool","Muon::MuonPRDSelectionTool/MuonPRDSelectionTool"};
    ToolHandle<Muon::IMuonPRDSelectionTool>         m_muonPRDSelectionToolStau{this,"MuonPRDSelectionToolStau","Muon::MuonPRDSelectionTool/MuonPRDSelectionToolStau"};
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtCreator{this,"MdtDriftCircleOnTrackCreator","Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"};
    ToolHandle<Muon::IMdtDriftCircleOnTrackCreator> m_mdtCreatorStau{this,"MdtDriftCircleOnTrackCreatorStau","Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreatorStau"};
    ToolHandle<MuonCombined::MuonInsideOutRecoTool> m_insideOutRecoTool{this,"MuonInsideOutRecoTool","MuonCombined::MuonInsideOutRecoTool/MuonInsideOutRecoTool"};
    ToolHandle<Trk::IUpdator>                       m_updator{this,"Updator","Trk::KalmanUpdator/KalmanUpdator"};
    ToolHandle<MdtCalibrationDbTool>                m_calibrationDbTool{this,"MdtCalibrationDbTool","MdtCalibrationDbTool"};

    Muon::MuonSectorMapping m_muonSectorMapping;

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

    Gaudi::Property<bool> m_doSummary{this,"DoSummary",false,"enable summary output"};
    Gaudi::Property<bool> m_useTruthMatching{this,"UseTruthMatching",false,"enable usage of truth info for reconstruction"};
    Gaudi::Property<bool> m_doTruth{this,"DoTruth",false,"enable truth matching"};
    Gaudi::Property<bool> m_segmentMDTT{this,"UseSegmentMDTT",true};
    Gaudi::Property<bool> m_ignoreSiAssocated{this,"IgnoreSiAssociatedCandidates",true};
    Gaudi::Property<std::vector<int>> m_pdgsToBeConsidered{this,"ConsideredPDGs",{},"PDG IDs considered in truth matching"};
    Gaudi::Property<double> m_ptThreshold{this,"PtThreshold",10000};
    Gaudi::Property<double> m_houghAssociationPullCut{this,"HoughAssociationPullCut",5};
    Gaudi::Property<double> m_mdttBetaAssociationCut{this,"MDTTAssocationCut",0.4};
    Gaudi::Property<double> m_rpcBetaAssociationCut{this,"RPCAssocationCut",0.2};
    Gaudi::Property<double> m_segmentBetaAssociationCut{this,"SegmentAssocationCut",0.2};

    std::set<int> m_selectedPdgs; // set storing particle PDG's considered for matching
    
    const double m_inverseSpeedOfLight = 1 / Gaudi::Units::c_light; // need 1/299.792458 inside calculateTof()/calculateBeta()
  };

  inline float MuonStauRecoTool::calculateTof(const float beta, const float dist) const {
    return dist*m_inverseSpeedOfLight/beta;
  }
  inline float MuonStauRecoTool::calculateBeta(const float time, const float dist) const {
    return dist*m_inverseSpeedOfLight/time;
  }
}



#endif
 


