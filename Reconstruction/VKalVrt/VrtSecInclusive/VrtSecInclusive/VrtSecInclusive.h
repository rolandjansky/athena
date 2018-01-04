/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrt.h
//
#ifndef _VrtSecInclusive_VrtSecInclusive_H
#define _VrtSecInclusive_VrtSecInclusive_H


#include "AthenaBaseComps/AthAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "StoreGate/StoreGateSvc.h"
//
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
//#include "TrkTrack/TrackInfo.h"
//#include "TrkParameters/TrackParameters.h"


// for truth
#include "GeneratorObjects/McEventCollection.h"

//#include "ParticleTruth/TrackParticleTruthCollection.h"
//-----
//#include "TrkEventUtils/InverseTruthMap.h"
//#include "TrkEventUtils/TruthCollectionInverter.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrInterfaces/IVertexMapper.h"

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

// Normal STL and physical vectors
#include <vector>
#include <deque>


class TH1D;

//using namespace std;

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
    
  class VrtSecInclusive : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    VrtSecInclusive(const std::string& name, ISvcLocator* pSvcLocator);
    
    /** Default Destructor */
    ~VrtSecInclusive();

    virtual StatusCode beginRun();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();
    virtual StatusCode initEvent();
   
  private:
    
    /////////////////////////////////////////////////////////
    //
    //  Member Variables
    //
    
    // Physics Constants
    const double m_pi; // mass of charged pion
    const double m_e; // mass of electron
    const double m_proton; // mass of proton
    const double m_PI; // value of pi
    
    // Unit Conversion Constants
    const double m_cnv_xyz;  /* Conversion constants*/
    const double m_cnv_mom;  /* Conversion constants*/
    
    // Detector Geometry Constants
    const double m_avRad_bp;
    const double m_avRad_pix0;
    const double m_avRad_pix1;
    const double m_avRad_pix2;
    const double m_avRad_pix3;
    const double m_avRad_sct0;
    const double m_avRad_sct1;
    
    const double m_ec_pix_rmin;
    const double m_ec_pix_rmax;
    const double m_ec_pix0;
    const double m_ec_pix1;
    const double m_ec_pix2;
    
    const double m_ec_sct_rmin;
    const double m_ec_sct_rmax;
    const double m_ec_sct0_rmin;
    const double m_ec_sct0;
    const double m_ec_sct1;
    const double m_ec_sct2;
    const double m_ec_sct3;
    const double m_ec_sct4;
    
    // xAOD Accessors
    const xAOD::VertexContainer*        m_vertexTES; // primary vertex container
    
    /////////////////////////////////////////////////////////
    //
    //  Athena JobOption Properties
    //
    std::string          m_TrackLocation;
    std::string          m_PrimVrtLocation;
    std::string          m_SecVrtLocation;
    std::string          m_truthParticleContainerName;
    std::string          m_mcEventContainerName;
    std::vector<double>  m_BeamPosition;
    
    // JO: GeoModel
    int    m_geoModel;
    
    // JO: Analysis Cut Variables
    bool   m_ImpactWrtBL;
    double m_a0TrkPVDstMinCut;
    double m_a0TrkPVDstMaxCut;
    double m_zTrkPVDstMinCut;
    double m_zTrkPVDstMaxCut;
    double m_a0TrkPVSignifCut;
    double m_zTrkPVSignifCut;
    
    double               m_TrkChi2Cut;
    double               m_SelVrtChi2Cut;
    double               m_TrkPtCut;
    int 		 m_CutSctHits;
    int 		 m_CutPixelHits;
    int 		 m_CutSiHits;
    int			 m_CutBLayHits;
    int 		 m_CutSharedHits;
    int 		 m_CutTRTHits; // Kazuki
    double 		 m_VertexMergeFinalDistCut; // Kazuki
    double		 m_A0TrkErrorCut;
    double		 m_ZTrkErrorCut;
    double               m_VertexMergeCut;
    double               m_TrackDetachCut;
    bool                 m_FillHist;
    bool                 m_FillNtuple;
    double               m_LoPtCut; // also check for truth vertices with truth tracks passing LoPtCut
    unsigned int         m_SelTrkMaxCutoff;
    bool                 m_doIntersectionPos;
    bool                 m_doMapToLocal;
    
    // Internal statuses of the algorithm
    bool                 m_SAloneTRT;
    bool                 m_doTRTPixCut; // Kazuki
    bool                 m_mergeFinalVerticesDistance; // Kazuki
    bool                 m_doTruth;
    bool                 m_removeFakeVrt;
    double               m_mcTrkResolution;
    double               m_TruthTrkLen;
    bool                 m_extrapPV; //extrapolate reco and prim truth trks to PV (for testing only)
    const xAOD::Vertex*  m_thePV;
    
    
    ToolHandle < Trk::ITrkVKalVrtFitter >  m_fitSvc;       // VKalVrtFitter tool
    ToolHandle <Trk::ITruthToTrack>        m_truthToTrack; // tool to create trkParam from genPart
    
    /** get a handle on the Track to Vertex tool */
    ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
    ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimatorTool;
    ToolHandle<Trk::IExtrapolator> m_extrapolator;
    ToolHandle<Trk::IVertexMapper> m_vertexMapper;
    
    // Histograms for stats
    TH1D* m_hb_massPiPi;
    TH1D* m_hb_2Ddist;
    TH1D* m_hb_massEE;
    TH1D* m_hb_nvrt2;
    TH1D* m_hb_ratio;
    TH1D* m_trkSelCuts;
    
    //////////////////////////////////////////////////////////////////////////////////////
    //
    // define ntuple variables here
    //
    
    // The standard AANT, CollectionTree, is bare bones
    TTree      *m_tree_Vert; 
    NtupleVars *m_ntupleVars;
    
    
    // to get truthParticle from Reco. particle
    const xAOD::TrackParticleContainer* m_importedTrkColl;        
    const xAOD::TruthParticleContainer* m_importedTrkTruthColl;        
    const xAOD::TruthEventContainer* m_importedFullTruthColl;
    const xAOD::TrackParticle* m_TrkColl;
    
    
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
    
    //    StatusCode m_scRes;
    //
    struct WrkVrt { 
      bool Good;
      std::deque<long int> SelTrk;
      Amg::Vector3D        vertex;
      TLorentzVector       vertexMom;
      long int             Charge;
      std::vector<double>  vertexCov;
      std::vector<double>  Chi2PerTrk;
      std::vector< std::vector<double> > TrkAtVrt; 
      double               Chi2;
      int                  nCloseVrt;
      double               dCloseVrt;
    };
    
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Vertexing Algorithm Tool Member Functions
    //
    
    StatusCode  SelGoodTrkParticle( const xAOD::TrackParticleContainer* );

    StatusCode extractIncompatibleTracks( std::vector<int>& );
    StatusCode reconstruct2TrackVertices( std::vector<int>&, std::vector<WrkVrt>* );
    StatusCode reconstructNTrackVertices( std::vector<WrkVrt>* );
    StatusCode mergeFinalVertices( std::vector<WrkVrt>* ); // Kazuki
    StatusCode refitAndSelectGoodQualityVertices( std::vector<WrkVrt>* );
    
    void printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name);

    StatusCode DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int , 
				 const xAOD::TrackParticleContainer* );
    
    void TrackClassification(std::vector< WrkVrt >* , 
                             std::vector< std::deque<long int> >* );
    
    double MaxOfShared(std::vector<WrkVrt> *, 
                       std::vector< std::deque<long int> >*,
	               long int & ,long int & );

    void RemoveTrackFromVertex(std::vector<WrkVrt> *, 
                               std::vector< std::deque<long int> > *,
			       long int & ,long int & );
 
    int   nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2) const;
    
    double minVrtVrtDist( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
    
    double minVrtVrtDistNext( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
    
    void MergeVertices( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
    
    double VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
		      const Amg::Vector3D & Vrt2, const std::vector<double>  & VrtErr2);

    double improveVertexChi2( std::vector<WrkVrt> *WrkVrtSet, int V, 
			      const xAOD::TrackParticleContainer* AllTrackList);
    
    template<class Track> void getIntersection(Track *trk, std::vector<IntersectionPos*>& layers, const Trk::Perigee* per);
    template<class Track> void setIntersection(Track *trk, IntersectionPos *bec, const Trk::Perigee* per);
    
    StatusCode CutTrk(double PInvVert,double ThetaVert,double A0Vert, double Zvert, double Chi2, 
		      long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits, long int TRTHits);

    const Trk::Perigee* GetPerigee( const xAOD::TrackParticle* i_ntrk);
    
    StatusCode RefitVertex( WrkVrt& WrkVrt, const xAOD::TrackParticleContainer* );
    
    void  FillCovMatrix(int iTrk, std::vector<double> & Matrix, AmgSymMatrix(5)& );
    
    
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
   
    void fillTrackSummary( track_summary& summary, const xAOD::TrackParticle *trk );
    bool passedFakeReject( const Amg::Vector3D& FitVertex, const xAOD::TrackParticle *itrk, const xAOD::TrackParticle *jtrk );
    
   
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Truth Information Algorithms Member Functions
    //
    // 
    
    // new version of truth routine - MCEventCollection (GEN_AOD or TruthEvent (ESD)
    StatusCode getNewTruthInfo();
    
    //
    //HepMC::GenParticle *getTrkGenParticle(const xAOD::TrackParticle*, double& matchProb);
    const xAOD::TruthParticle *getTrkGenParticle(const xAOD::TrackParticle* /*, double& matchProb*/) const;
    
    
    StatusCode categorizeVertexTruthTopology( xAOD::Vertex *vertex );
      
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // 
    // xAOD Accessors Member Functions
    //
    void setMcEventCollection( const xAOD::TruthEventContainer*); 
    void setTrackParticleTruthCollection( const xAOD::TruthParticleContainer*);        
    void setTrackParticleContainer( const xAOD::TrackParticleContainer*);

  };
  
} // end of namespace bracket


// This header file contains the definition of member templates
#include "details/Utilities.h"


#endif /* _VrtSecInclusive_VrtSecInclusive_H */
