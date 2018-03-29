/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrt.h
//
#ifndef _VrtSecInclusive_VrtSecInclusive_H
#define _VrtSecInclusive_VrtSecInclusive_H


#include "VrtSecInclusive/Constants.h"

#include "AthenaBaseComps/AthAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
//
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

// for truth
#include "GeneratorObjects/McEventCollection.h"

#include "TrkToolInterfaces/ITruthToTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrInterfaces/IVertexMapper.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

// xAOD Classes
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

// Normal STL and physical vectors
#include <vector>
#include <deque>
#include <functional>


/** Forward declarations **/

class TH1;

namespace Trk {
  class ITruthToTrack;
  class ITrackToVertex;
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
  class IVertexMapper;
  struct MappedVertex;
}

namespace VKalVrtAthena {

  namespace GeoModel { enum GeoModel { Run1=1, Run2=2 }; }
  
  class IntersectionPos;
  class IntersectionPos_barrel;
  class IntersectionPos_endcap;
  
  class NtupleVars;
}


namespace VKalVrtAthena {
  
  class VrtSecInclusive : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    VrtSecInclusive(const std::string& name, ISvcLocator* pSvcLocator);
    
    /** Default Destructor */
    ~VrtSecInclusive();

    virtual StatusCode beginRun()   override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize()   override;
    virtual StatusCode execute()    override;
   
    virtual StatusCode initEvent();
    
  private:
    
    /////////////////////////////////////////////////////////
    //
    //  Member Variables
    //
    
    struct JobProperties {
      // JO: GeoModel
      int    geoModel;
   
      std::string          TrackLocation;
      std::string          PrimVrtLocation;
      std::string          truthParticleContainerName;
      std::string          mcEventContainerName;
      
      std::string all2trksVerticesContainerName;
      std::string secondaryVerticesContainerName;
      
      // common feature flags
      bool   doTruth;
      bool   FillHist;
      bool   FillNtuple;
      bool   FillIntermediateVertices;
      bool   doIntersectionPos;
      bool   doMapToLocal;
      bool   extrapPV; //extrapolate reco and prim truth trks to PV (for testing only)
      
      // track selection conditions
      unsigned int SelTrkMaxCutoff;
      bool  SAloneTRT;
      
      /* impact parameters */
      bool   do_PVvetoCut;
      bool   do_d0Cut;
      bool   do_z0Cut;
      bool   do_d0errCut;
      bool   do_z0errCut;
      bool   do_d0signifCut;
      bool   do_z0signifCut;
      
      bool   ImpactWrtBL;
      double d0TrkPVDstMinCut;
      double d0TrkPVDstMaxCut;
      double d0TrkPVSignifCut;
      double z0TrkPVDstMinCut;
      double z0TrkPVDstMaxCut;
      double z0TrkPVSignifCut;
      double d0TrkErrorCut;
      double z0TrkErrorCut;
      
      /* pT anc chi2 */
      double TrkChi2Cut;
      double TrkPtCut;
      
      /* hit requirements */
      bool doTRTPixCut; // Kazuki
      int  CutSctHits;
      int  CutPixelHits;
      int  CutSiHits;
      int  CutBLayHits;
      int  CutSharedHits;
      int  CutTRTHits; // Kazuki
      
      // Vertex reconstruction
      bool   doPVcompatibilityCut;
      bool   removeFakeVrt;
      bool   removeFakeVrtLate;
      bool   doReassembleVertices;
      bool   doMergeByShuffling;
      bool   doSuggestedRefitOnMerging;
      bool   doMagnetMerging;
      bool   doWildMerging;
      bool   doMergeFinalVerticesDistance; // Kazuki
      bool   doAssociateNonSelectedTracks;
      bool   doFinalImproveChi2;
      double pvCompatibilityCut;
      double SelVrtChi2Cut;
      double VertexMergeFinalDistCut; // Kazuki
      double VertexMergeFinalDistScaling;
      double VertexMergeCut;
      double TrackDetachCut;
      
      
      double associateMinDistanceToPV;
      double associateMaxD0;
      double associateMaxZ0;
      double associatePtCut;
      double associateChi2Cut;
      
      double reassembleMaxImpactParameterD0;
      double reassembleMaxImpactParameterZ0;
      double mergeByShufflingMaxSignificance;
      double mergeByShufflingAllowance;
      
      double improveChi2ProbThreshold;
      
      // vertexing using muons (test implementation)
      bool doSelectTracksFromMuons;
      bool doSelectTracksFromElectrons;
      
      // Additional dressing option
      bool doAugmentDVimpactParametersToMuons;     // potentially useful for DV + muon search
      bool doAugmentDVimpactParametersToElectrons; // potentially useful for analyses involving electrons
      
      // MC truth
      double               mcTrkResolution;
      double               TruthTrkLen;
      
    };
    
    struct JobProperties m_jp;
    
    // xAOD Accessors
    const xAOD::VertexContainer*  m_primaryVertices;
    xAOD::Vertex*                 m_thePV;
    std::unique_ptr<std::vector<const xAOD::TrackParticle*> > m_selectedTracks;
    std::unique_ptr<std::vector<const xAOD::TrackParticle*> > m_associatedTracks;
    
    std::vector<double>  m_BeamPosition;
    
    /////////////////////////////////////////////////////////
    //
    //  Athena JobOption Properties
    //
    
    ToolHandle <Trk::ITrkVKalVrtFitter>        m_fitSvc;       // VKalVrtFitter tool
    ToolHandle <Trk::ITruthToTrack>            m_truthToTrack; // tool to create trkParam from genPart
    
    /** get a handle on the Track to Vertex tool */
    ToolHandle< Reco::ITrackToVertex >         m_trackToVertexTool;
    ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimatorTool;
    ToolHandle<Trk::IExtrapolator>             m_extrapolator;
    ToolHandle<Trk::IVertexMapper>             m_vertexMapper;
    
    /** Condition service **/
    ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;
    ServiceHandle <IInDetConditionsSvc> m_sctCondSummarySvc;
    
    const AtlasDetectorID* m_atlasId;
    const PixelID* m_pixelId;
    const SCT_ID*  m_sctId;
    
    std::string m_checkPatternStrategy;
    using PatternStrategyFunc = bool (VrtSecInclusive::*) ( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    std::map<std::string, PatternStrategyFunc> m_patternStrategyFuncs;
    
    
    //////////////////////////////////////////////////////////////////////////////////////
    //
    // define ntuple variables here
    //
    
    // The standard AANT, CollectionTree, is bare bones
    TTree      *m_tree_Vert; 
    std::unique_ptr<NtupleVars> m_ntupleVars;
    
    // Histograms for stats
    std::map<std::string, TH1*> m_hists;
    
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Private member functions
    //
    
    // for event info to new ntuple (used to go by default in CollectionTree)
    void declareProperties();
    
    StatusCode addEventInfo(); 
    StatusCode setupNtupleVariables();
    StatusCode setupNtuple();
    StatusCode clearNtupleVariables();
    StatusCode deleteNtupleVariables();
    StatusCode processPrimaryVertices();
    StatusCode fillAANT_SelectedBaseTracks();
    StatusCode fillAANT_SecondaryVertices( xAOD::VertexContainer* );
    
    //
    struct WrkVrt { 
      bool isGood;                                    //! flaged true for good vertex candidates
      std::deque<long int> selectedTrackIndices;      //! list if indices in TrackParticleContainer for selectedBaseTracks
      std::deque<long int> associatedTrackIndices;    //! list if indices in TrackParticleContainer for associatedTracks
      Amg::Vector3D        vertex;                    //! VKalVrt fit vertex position
      TLorentzVector       vertexMom;                 //! VKalVrt fit vertex 4-momentum
      std::vector<double>  vertexCov;                 //! VKalVrt fit covariance
      double               Chi2;                      //! VKalVrt fit chi2 result
      std::vector<double>  Chi2PerTrk;                //! list of VKalVrt fit chi2 for each track
      long int             Charge;                    //! total charge of the vertex
      std::vector< std::vector<double> > TrkAtVrt;    //! list of track parameters wrt the reconstructed vertex
      unsigned long        closestWrkVrtIndex;        //! stores the index of the closest WrkVrt in std::vector<WrkVrt>
      double               closestWrkVrtValue;        //! stores the value of some observable to the closest WrkVrt ( observable = e.g. significance )
      
      inline double ndof() const { return 2.0*( selectedTrackIndices.size() + associatedTrackIndices.size() ) - 3.0; }
      inline unsigned nTracksTotal() const { return selectedTrackIndices.size() + associatedTrackIndices.size(); }
    };
    
    
    using Detector = int;
    using Bec      = int;
    using Layer    = int;
    using Flag     = int;
    using ExtrapolatedPoint   = std::tuple<const TVector3, Detector, Bec, Layer, Flag>;
    using ExtrapolatedPattern = std::vector< ExtrapolatedPoint >;
    
    std::vector< std::pair<int, int> > m_incomp;
    
    
    ////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Vertexing Algorithm Member Functions
    ///
    
    /** select tracks which become seeds for vertex finding */
    StatusCode selectTracks();
    StatusCode selectTracksFromMuons();
    StatusCode selectTracksFromElectrons();
    
    using TrackSelectionAlg = StatusCode (VrtSecInclusive::*)();
    std::vector<TrackSelectionAlg> m_trackSelectionAlgs;
    
    /** track-by-track selection strategies */
    bool selectTrack_notPVassociated ( const xAOD::TrackParticle* ) const;
    bool selectTrack_pTCut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_chi2Cut         ( const xAOD::TrackParticle* ) const;
    bool selectTrack_hitPattern      ( const xAOD::TrackParticle* ) const;
    bool selectTrack_d0Cut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_z0Cut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_d0errCut        ( const xAOD::TrackParticle* ) const;
    bool selectTrack_z0errCut        ( const xAOD::TrackParticle* ) const;
    bool selectTrack_d0signifCut     ( const xAOD::TrackParticle* ) const;
    bool selectTrack_z0signifCut     ( const xAOD::TrackParticle* ) const;
    
    /** related to the graph method and verte finding */
    StatusCode extractIncompatibleTrackPairs( std::vector<WrkVrt>* );
    StatusCode findNtrackVertices(std::vector<WrkVrt>* );
    StatusCode rearrangeTracks( std::vector<WrkVrt>* );
    
    /** attempt to merge vertices when all tracks of a vertex A is close to vertex B in terms of impact parameter */
    StatusCode reassembleVertices( std::vector<WrkVrt>* );
    
    /** attempt to merge splitted vertices when they are significantly distant
        due to the long-tail behavior of the vertex reconstruction resolution */
    StatusCode mergeByShuffling( std::vector<WrkVrt>* );
    
    /** attempt to merge vertices by lookng at the distance between two vertices */
    StatusCode mergeFinalVertices( std::vector<WrkVrt>* ); // Kazuki
    
    /** in addition to selected tracks, associate as much tracks as possible */
    StatusCode associateNonSelectedTracks( std::vector<WrkVrt>* );
    
    /** finalization of the vertex and store to xAOD::VertexContainer */
    StatusCode refitAndSelectGoodQualityVertices( std::vector<WrkVrt>* );
    
    using vertexingAlg = StatusCode (VrtSecInclusive::*)( std::vector<WrkVrt>* );
    std::vector< std::pair<std::string, vertexingAlg> > m_vertexingAlgorithms;
    unsigned m_vertexingAlgorithmStep;
    
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Supporting utility functions
    
    /** print the contents of reconstructed vertices */
    void printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name);

    /** refit the vertex. */
    StatusCode refitVertex( WrkVrt& );
    
    /** refit the vertex with suggestion */
    StatusCode refitVertexWithSuggestion( WrkVrt&, const Amg::Vector3D& );
    
    /** attempt to improve the vertex chi2 by removing the most-outlier track one by one until 
        the vertex chi2 satisfies a certain condition. */
    double improveVertexChi2( WrkVrt& );
    
    void removeTrackFromVertex(std::vector<WrkVrt>*, 
                               std::vector< std::deque<long int> > *,
			       const long int & ,const long int & );
 
    StatusCode disassembleVertex(std::vector<WrkVrt> *, const unsigned& vertexIndex );
    
    void trackClassification(std::vector< WrkVrt >* , std::map< long int, std::vector<long int> >& );
    
    double findWorstChi2ofMaximallySharedTrack(std::vector<WrkVrt>*, std::map< long int, std::vector<long int> >&, long int & ,long int & );
    
    /** returns the number of tracks commonly present in both vertices */
    size_t nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, const std::pair<unsigned, unsigned>& pairIndex ) const;
    
    /** calculate the significance (Mahalanobis distance) between two reconstructed vertices */
    double significanceBetweenVertices( const WrkVrt&, const WrkVrt& ) const;
    
    /** calculate the physical distance */
    double distanceBetweenVertices( const WrkVrt&, const WrkVrt& ) const;
    
    using AlgForVerticesPair = double (VrtSecInclusive::*)( const WrkVrt&, const WrkVrt& ) const;
    
    /** returns the pair of vertices that give minimum in terms of some observable (e.g. distance, significance) */
    double findMinVerticesPair( std::vector<WrkVrt>*, std::pair<unsigned, unsigned>&, AlgForVerticesPair );
    
    /** returns the next pair of vertices that give next-to-minimum distance significance */
    double findMinVerticesNextPair( std::vector<WrkVrt>*, std::pair<unsigned, unsigned>& );
    
    /** the 2nd vertex is merged into the 1st vertex. A destructive operation. */
    StatusCode mergeVertices( WrkVrt& destination, WrkVrt& source );
    
    enum mergeStep { RECONSTRUCT_NTRK, REASSEMBLE, SHUFFLE1, SHUFFLE2, SHUFFLE3, FINAL };
    
    typedef struct track_summary_properties {
      uint8_t numIBLHits;
      uint8_t numBLayerHits;
      uint8_t numPixelLayer1_Hits;
      uint8_t numPixelLayer2_Hits;
      uint8_t numPixelDisk0_Hits;
      uint8_t numPixelDisk1_Hits;
      uint8_t numPixelDisk2_Hits;
      uint8_t numPixelHits;
      uint8_t numSctBarrelLayer0_Hits;
      uint8_t numSctBarrelLayer1_Hits;
      uint8_t numSctBarrelLayer2_Hits;
      uint8_t numSctBarrelLayer3_Hits;
      uint8_t numSctEC0_Hits;
      uint8_t numSctEC1_Hits;
      uint8_t numSctEC2_Hits;
      uint8_t numSctEC3_Hits;
      uint8_t numSctEC4_Hits;
      uint8_t numSctHits;
      uint8_t numTrtHits;
    } track_summary;
   
    /** cretrieve the track hit information */
    void fillTrackSummary( track_summary& summary, const xAOD::TrackParticle *trk );
    
    ExtrapolatedPattern* extrapolatedPattern( const xAOD::TrackParticle* );
   
    /** A classical method with hard-coded geometry */
    bool checkTrackHitPatternToVertex( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    /** New method with track extrapolation */
    bool checkTrackHitPatternToVertexByExtrapolation( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    /** Flag false if the consistituent tracks are not consistent with the vertex position */
    bool passedFakeReject( const Amg::Vector3D& FitVertex, const xAOD::TrackParticle *itrk, const xAOD::TrackParticle *jtrk );
    
    /** Remove inconsistent tracks from vertices */
    void removeInconsistentTracks( WrkVrt& );
   
    template<class Track> void getIntersection(Track *trk, std::vector<IntersectionPos*>& layers, const Trk::Perigee* per);
    template<class Track> void setIntersection(Track *trk, IntersectionPos *bec, const Trk::Perigee* per);
    
    /** monitor the intermediate status of vertexing */
    StatusCode monitorVertexingAlgorithmStep( std::vector<WrkVrt>*, const std::string name, bool final = false );
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Truth Information Algorithms Member Functions
    //
    // 
    
    const xAOD::TruthParticle *getTrkGenParticle(const xAOD::TrackParticle*) const;
    
    StatusCode categorizeVertexTruthTopology( xAOD::Vertex *vertex );
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Additional augmentation
    //
    // 
    
    template<class LeptonFlavor>
    StatusCode augmentDVimpactParametersToLeptons( const std::string& containerName );
    
  };
  
} // end of namespace bracket


// This header file contains the definition of member templates
#include "details/Utilities.h"


#endif /* _VrtSecInclusive_VrtSecInclusive_H */
