/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrt.h
//
#ifndef _VrtSecFuzzy_VrtSecFuzzy_H
#define _VrtSecFuzzy_VrtSecFuzzy_H


#include "VrtSecFuzzy/Constants.h"

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

#include "TFile.h"



/** Forward declarations **/

class TH1;

namespace Trk {
  class ITruthToTrack;
  class ITrackToVertex;
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
}

namespace VKalVrtAthena {

  namespace GeoModel { enum GeoModel { Run1=1, Run2=2 }; }
  
}

namespace MVAUtils { class BDT; }


namespace VKalVrtAthena {
  
  class VrtSecFuzzy : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    VrtSecFuzzy(const std::string& name, ISvcLocator* pSvcLocator);
    
    /** Default Destructor */
    ~VrtSecFuzzy();

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
   
      // Container names
      std::string   TrackLocation;
      std::string   PrimVrtLocation;
      std::string   truthParticleContainerName;
      std::string   mcEventContainerName;
      
      std::string   all2trksVerticesContainerName;
      std::string   secondaryVerticesContainerName;
      std::string   twoTrksVerticesInDVContainerName;
      
      // common feature flags
      bool   doTruth;
      bool   FillIntermediateVertices;
      
      // track selection conditions
      
      // impact parameters
      bool   do_d0Cut;
      bool   do_z0Cut;
      
      double d0TrkPVDstMinCut;
      double d0TrkPVDstMaxCut;
      double z0TrkPVDstMinCut;
      double z0TrkPVDstMaxCut;
      
      // pt, trk unc.
      bool   do_trkChi2Cut;
      double TrkChi2Cut;
      double TrkPtCut;     
 
      // others
      bool   do_PVvetoCut;
      unsigned int SelTrkMaxCutoff; // if # tracks > the cutoff, this vertexing won't be executed
     
      // track extrpolator; 1==VKalGetImpact, 2==m_trackToVertexTool
      int trkExtrapolator;
      
      // (2-trk) vertex selections
      bool   doPVcompatibilityCut;
      bool   removeFakeVrt;
      double pvCompatibilityCut;
      double SelVrtChi2Cut;
      
      // BDT cut
      std::vector<std::string>  BDTFilesName;
      std::vector<float>        BDTMins_1stTry;
      std::vector<float>        BDTMins_2ndTry;
      std::vector<float>        BDTMins_primary;

      // seed clustering condition
      std::vector<float> SeedDist;

    };
    
    struct JobProperties m_jp;
    
    // xAOD Accessors
    const xAOD::VertexContainer*  m_primaryVertices;
    xAOD::Vertex*                 m_thePV;
    std::unique_ptr<std::vector<const xAOD::TrackParticle*> > m_selectedTracks;
    
    std::vector<double>  m_BeamPosition;
    
    /////////////////////////////////////////////////////////
    //
    //  Athena JobOption Properties
    //
    
    ToolHandle <Trk::ITrkVKalVrtFitter>        m_fitSvc;       // VKalVrtFitter tool
    ToolHandle <Trk::ITruthToTrack>            m_truthToTrack; // tool to create trkParam from genPart
    
    // get a handle on the Track to Vertex tool 
    ToolHandle< Reco::ITrackToVertex >         m_trackToVertexTool;
    ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimatorTool;
    ToolHandle<Trk::IExtrapolator>             m_extrapolator;
    
    // Condition service 
    ServiceHandle <IInDetConditionsSvc> m_pixelCondSummarySvc;
    ServiceHandle <IInDetConditionsSvc> m_sctCondSummarySvc;
    
    const AtlasDetectorID* m_atlasId;
    const PixelID* m_pixelId;
    const SCT_ID*  m_sctId;
    
    std::string m_checkPatternStrategy;
    using PatternStrategyFunc = bool (VrtSecFuzzy::*) ( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    std::map<std::string, PatternStrategyFunc> m_patternStrategyFuncs;
    
    // AuxElement decorators
    std::unique_ptr< SG::AuxElement::Decorator< char > > m_decor_isSelected;
    std::unique_ptr< SG::AuxElement::Decorator< char > > m_decor_is_svtrk_final;
    std::map< unsigned, SG::AuxElement::Decorator<float> > m_trkDecors;
    
    using IPDecoratorType = SG::AuxElement::Decorator< std::vector< std::vector<float> > >;
    std::unique_ptr< IPDecoratorType > m_decor_d0_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_z0_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_pt_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_eta_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_phi_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_d0err_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_z0err_wrtSVs;
    
    using VertexELType = SG::AuxElement::Decorator< std::vector<ElementLink< xAOD::VertexContainer > > >;
    std::unique_ptr< VertexELType > m_decor_svLink;
    
    // BDT
    std::unique_ptr<TFile> m_tfile;
    std::vector<MVAUtils::BDT*> m_bdt; 

    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Private member functions
    //
    
    // for event info to new ntuple (used to go by default in CollectionTree)
    void declareProperties();
    
    StatusCode processPrimaryVertices();
    
    //
    struct WrkVrt { 
      bool isGood;                                    //! flaged true for good vertex candidates
      std::deque<long int> selectedTrackIndices;      //! list if indices in TrackParticleContainer for selectedBaseTracks
      Amg::Vector3D        vertex;                    //! VKalVrt fit vertex position
      TLorentzVector       vertexMom;                 //! VKalVrt fit vertex 4-momentum
      std::vector<double>  vertexCov;                 //! VKalVrt fit covariance
      double               Chi2;                      //! VKalVrt fit chi2 result
      std::vector<double>  Chi2PerTrk;                //! list of VKalVrt fit chi2 for each track
      long int             Charge;                    //! total charge of the vertex
      std::vector< std::vector<double> > TrkAtVrt;    //! list of track parameters wrt the reconstructed vertex
      unsigned ntrks;                                 //! # tracks
      inline double ndof() const { return 2.0*( selectedTrackIndices.size() ) - 3.0; }
      inline double fitQuality() const { return Chi2 / ndof(); }
      
      std::vector<float>   BDTScores;                 //! BDT score of two track pair 
      float                maxBDTScore;               //! max one in several BDT scores
      std::deque<TLorentzVector> trackLorentzVectors; //! lorentz vector of tracks in a vertex
    };
    
    struct sortSeed_wBDT { 
      bool operator()(const WrkVrt& a, const WrkVrt& b) const noexcept {
        return a.maxBDTScore > b.maxBDTScore; 
      }
    };
    
    using Detector = int;
    using Bec      = int;
    using Layer    = int;
    using Flag     = int;
    using ExtrapolatedPoint   = std::tuple<const TVector3, Detector, Bec, Layer, Flag>;
    using ExtrapolatedPattern = std::vector< ExtrapolatedPoint >;
    using PatternBank         = std::map<const xAOD::TrackParticle*, std::pair< std::unique_ptr<ExtrapolatedPattern>, std::unique_ptr<ExtrapolatedPattern> > >;
    
    PatternBank m_extrapolatedPatternBank;
    
    
    ////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Vertexing Algorithm Member Functions
    ///
    
    // select tracks which is used to form seeds
    void storeSelectedTracks( const xAOD::TrackParticle* );
    StatusCode selectInDetTracks();
    
    using TrackSelectionAlg = StatusCode (VrtSecFuzzy::*)();
    std::vector<TrackSelectionAlg> m_trackSelectionAlgs;
    
    // track selection 
    using CutFunc = bool (VrtSecFuzzy::*) ( const xAOD::TrackParticle* ) const;
    std::vector<CutFunc> m_trackSelectionFuncs;
    
    // track-by-track selection strategies 
    bool selectTrack_notPVassociated ( const xAOD::TrackParticle* ) const;
    bool selectTrack_pTCut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_chi2Cut         ( const xAOD::TrackParticle* ) const;
    bool selectTrack_d0Cut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_z0Cut           ( const xAOD::TrackParticle* ) const;
    bool selectTrack_hitAndPt        ( const xAOD::TrackParticle* ) const;
    bool selectTrack_nSiHitsCut      ( const xAOD::TrackParticle* ) const;
    
    // vertexing procedure
    StatusCode form2trackVertices( std::vector<WrkVrt>* );
    StatusCode merge2trackVertices( std::vector<WrkVrt>* );
 
    using vertexingAlg = StatusCode (VrtSecFuzzy::*)( std::vector<WrkVrt>* );
    std::vector< std::pair<std::string, vertexingAlg> > m_vertexingAlgorithms;
    unsigned m_vertexingAlgorithmStep;
 
     // selection of track pairs based on the BDT score
    void BDTSelection(const xAOD::TrackParticle* itrk, const xAOD::TrackParticle* jtrk, std::vector<float>& wgtSelect);
    
    // get secondary vertex impact parameters 
    bool getSVImpactParameters(const xAOD::TrackParticle* trk, Amg::Vector3D vertex, std::vector<double>& impactParameters, std::vector<double>& impactParErrors); 
    enum TrkParameter    { k_d0=0, k_z0=1, k_theta=2, k_phi=3, k_qOverP=4 ,k_nTP=5 };
    enum TrkParameterUnc { k_d0d0=0, k_z0z0=1, k_nTPU=2 };
 
    // get the category of the first hit 
    float getFirstHitCategory(const xAOD::TrackParticle* trk);
   
    // apply all 2-trk vertex selections 
    int vertexSelection(WrkVrt& wrkvrt, const xAOD::TrackParticle* itrk, const xAOD::TrackParticle* jtrk);
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Supporting utility functions
    
    using AlgForVerticesPair = double (VrtSecFuzzy::*)( const WrkVrt&, const WrkVrt& ) const;
   
    ExtrapolatedPattern* extrapolatedPattern( const xAOD::TrackParticle*, enum Trk::PropDirection );
   
    bool patternCheck    ( const uint32_t& pattern, const Amg::Vector3D& vertex );
    bool patternCheckRun1( const uint32_t& pattern, const Amg::Vector3D& vertex );
    bool patternCheckRun2( const uint32_t& pattern, const Amg::Vector3D& vertex );
    
    bool patternCheckOuterOnly    ( const uint32_t& pattern, const Amg::Vector3D& vertex );
    bool patternCheckRun1OuterOnly( const uint32_t& pattern, const Amg::Vector3D& vertex );
    bool patternCheckRun2OuterOnly( const uint32_t& pattern, const Amg::Vector3D& vertex );
    
    // A classical method with hard-coded geometry 
    bool checkTrackHitPatternToVertex( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    // A classical method with hard-coded geometry 
    bool checkTrackHitPatternToVertexOuterOnly( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    // New method with track extrapolation 
    bool checkTrackHitPatternToVertexByExtrapolation( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    // New method with track extrapolation 
    bool checkTrackHitPatternToVertexByExtrapolationAssist( const xAOD::TrackParticle *trk, const Amg::Vector3D& vertex );
    
    // Flag false if the consistituent tracks are not consistent with the vertex position 
    bool passedFakeReject( const Amg::Vector3D& FitVertex, const xAOD::TrackParticle *itrk, const xAOD::TrackParticle *jtrk );
 
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Truth Information Algorithms Member Functions
    //
    // 
    
    const xAOD::TruthParticle *getTrkGenParticle(const xAOD::TrackParticle*) const;
    
    StatusCode categorizeVertexTruthTopology( xAOD::Vertex *vertex );
    
    std::vector<const xAOD::TruthVertex*> m_tracingTruthVertices;
    
  };
  
} // end of namespace bracket

#endif /* _VrtSecFuzzy_VrtSecFuzzy_H */
