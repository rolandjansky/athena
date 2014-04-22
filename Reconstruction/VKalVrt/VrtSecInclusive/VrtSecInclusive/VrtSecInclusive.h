/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VKalVrt.h
//
#ifndef _VrtSecInclusive_VrtSecInclusive_H
#define _VrtSecInclusive_VrtSecInclusive_H
// Normal STL and physical vectors
#include <vector>
#include <deque>
// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
// Other stuff
#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"
#include "TNtuple.h"
#include "TROOT.h"
//
//#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include  "Particle/TrackParticle.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkParameters/TrackParameters.h"

// for AANT - use AthAlgorithm instead
//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// for truth
#include "GeneratorObjects/McEventCollection.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
//-----
//#include "TrkEventUtils/InverseTruthMap.h"
#include "TrkEventUtils/TruthCollectionInverter.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"

  using namespace std;

namespace Trk {
  class ITruthToTrack;
  class ITrackToVertex;
  class ITrackToVertexIPEstimator;
  class IExtrapolator;
}

namespace VKalVrtAthena {

  
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
    double m_cnv_xyz;  /* Conversion constants*/
    double m_cnv_mom;  /* Conversion constants*/
    //    StoreGateSvc*            m_sgSvc;
    const VxContainer* m_vertexTES; // primary vertex container
    int                      m_SummaryToolExist;
    double m_pi; // mass of charged pion
    double m_e; // mass of electron
    double m_PI; // value of pi
    double m_avRad_bp;
    double m_avRad_pix1;
    double m_avRad_pix2;
    double m_avRad_pix3;
    double m_avRad_sct1;
    double m_avRad_sct2;

//
//  Properties
//
    std::string          m_TrackLocation;
    std::string          m_PrimVrtLocation;
    std::string          m_SecVrtLocation;
    std::vector<double>  m_BeamPosition;

    double m_a0TrkPVDstMinCut;
    double m_a0TrkPVDstMaxCut;
    double m_zTrkPVDstMinCut;
    double m_zTrkPVDstMaxCut;
    double m_a0TrkPVSignifCut;
    double m_zTrkPVSignifCut;


    double               m_TrkChi2Cut;
    double               m_SelVrtChi2Cut;
    double               m_TrkPtCut;
                                    // value is set in constructor
    int 		 m_CutSctHits;
    int 		 m_CutPixelHits;
    int 		 m_CutSiHits;
    int			 m_CutBLayHits;
    int 		 m_CutSharedHits;
    bool                 m_SAloneTRT;
    double		 m_A0TrkErrorCut;
    double		 m_ZTrkErrorCut;
    double               m_VertexMergeCut;
    double               m_TrackDetachCut;
    int                  m_FillHist;
    bool                 m_doTruth;
    bool                 m_removeFakeVrt;
    std::string          m_truthParticleContainerName;
    std::string          m_mcEventContainerName;
    double               m_mcTrkResolution;
    double               m_TruthTrkLen;
    double               m_LoPtCut; // also check for truth vertices with truth tracks passing LoPtCut
    bool                 m_extrapPV; //extrapolate reco and prim truth trks to PV (for testing only)

//
//    Trk::ITrackSummaryTool*   m_sumSvc;
//    IVKalVrtFitSvc*          m_fitSvc;
//      ToolHandle < Trk::ITrackSummaryTool >  m_sumSvc;
      ToolHandle < Trk::ITrkVKalVrtFitter >  m_fitSvc; /** VKalVrtFitter tool */
      ToolHandle <Trk::ITruthToTrack> m_truthToTrack; // tool to create trkParam from genPart
      /** get a handle on the Track to Vertex tool */
      ToolHandle< Reco::ITrackToVertex > m_trackToVertexTool;
      //
      ToolHandle<Trk::ITrackToVertexIPEstimator> m_trackToVertexIPEstimatorTool;
      //
      ToolHandle<Trk::IExtrapolator> m_extrapolator;
      //
      double w_1;
      TH1D* m_hb_massPiPi;
      TH1D* m_hb_2Ddist;
      TH1D* m_hb_massEE;
      TH1D* m_hb_nvrt2;
      TH1D* m_hb_ratio;
      TH1D* m_trkSelCuts;
      //
      // define ntuple variables here
      //
      // stuff for new ntuple
      // The standard AANT, CollectionTree, is bare bones
      TTree* m_tree_Vert; 
      // I am adding these variables by hand to the tree.
      // In the past, they were added to CollectionTree by default
      unsigned int    m_runNumber;
      unsigned int    m_eventNumber;
      unsigned int    m_eventTime;
      unsigned int    m_lumiBlock;
      unsigned int    m_bCID;
      unsigned int    m_lVL1ID;
      double  m_eventWeight;
      //


      int m_allTrks;     // num of tracks in event
      int m_selTrks;     // num passing cuts in SelGoodTrkParticle
      int m_sizeIncomp;  // Size of 2-track combinations that DON'T make a 2-track vertex 
      int m_numPV;      // num of PV
      std::vector<int>* m_numDOFPV;  // num of DOF at each primary vertex 
      int m_numTrksPV; // # tracks attached to vertex labelled as Primary Vertex
      double m_PVX;
      double m_PVY;     // X,Y, Z of 1st primary vertex
      double m_PVZ;
      std::vector<double>* m_PVZpile; //Z of all vertices in event
      int m_numAll2trkVert;
      int m_numAfFak2trkVert;
      std::vector<double>* m_All2trkMass; // this is when we are filling Incomp
      std::vector<double>* m_All2trkPt;  // for all vertices before the chi2 cut
      std::vector<int>* m_All2trkChg;
      std::vector<int>* m_All2trkBLHSum; // sum of BLayer hits on the two tracks
      std::vector<double>* m_All2trkVertX;
      std::vector<double>* m_All2trkVertY;
      std::vector<double>* m_All2trkVertZ;
      std::vector<double>* m_All2trkVertChi2;
      //
      std::vector<double>* m_AfFak2trkMass; // this is when we are filling Incomp
      std::vector<double>* m_AfFak2trkPt;  // for all vertices after chi2 cut and fake removal
      std::vector<int>* m_AfFak2trkChg;
      std::vector<double>* m_AfFak2trkVertX;
      std::vector<double>* m_AfFak2trkVertY;
      std::vector<double>* m_AfFak2trkVertZ;
      std::vector<double>* m_AfFak2trkVpos;
      std::vector<double>* m_AfFak2trkVertChi2;

      // look at properties of reconstructed tracks
      std::vector<double>* m_RecoTrk_pT;
      std::vector<double>* m_RecoTrk_phi;
      std::vector<double>* m_RecoTrk_eta; // all these params are relative to (0,0,0)
      std::vector<double>* m_RecoTrk_chi2;
      std::vector<double>* m_RecoTrk_2dIP; 
      std::vector<double>* m_RecoTrk_zIP;
      std::vector<double>* m_RecoTrk_A0Error; // error on 2DIP
      std::vector<double>* m_RecoTrk_ZError; // error in Z-IP
      std::vector<double>* m_RecoTrk_2dIPPV; // relative to PV
      std::vector<double>* m_RecoTrk_zIPPV;

      std::vector<int>* m_RecoTrk_numBLay;
      std::vector<int>* m_RecoTrk_numPix;
      std::vector<int>* m_RecoTrk_numSCT;
      std::vector<int>* m_RecoTrk_numTRT;
      std::vector<int>* m_RecoTrk_PixBar1;
      std::vector<int>* m_RecoTrk_PixBar2;
      /*
      std::vector<int>* m_RecoTrk_SctBar0;
      std::vector<int>* m_RecoTrk_SctBar1;
      std::vector<int>* m_RecoTrk_SctBar2;
      std::vector<int>* m_RecoTrk_SctBar3;
      */
      std::vector<int>* m_RecoTrk_barcode; // barcode of matching GenParticle for all reco trks
      std::vector<double>* m_RecoTrk_matchPr; // match Prob
      std::vector<int>* m_RecoTrk_PixShare;
      std::vector<int>* m_RecoTrk_SctShare;
      std::vector<int>* m_SelTrk_barcode; // barcode of matching GenParticle for Selected track
      std::vector<int>* m_RecoTrk_TrkAuth; // 1=inside-out w/TRT, 2 = w/o TRT
                                           // 3=BackTracking, 4= TRT standalone
      std::vector<int>* m_RecoTrk_TrkLowPt; // low pt tracking

      //std::vector<int> *m_RecoTrk_numTRTholes;
      //std::vector<int>* m_RecoTrk_numTRTout;


      std::vector<double>* m_RecoTrk_Int_bpZ; // Z position of track intersection with bp 
      std::vector<double>* m_RecoTrk_Int_pix1Z; // Z position of track intersection with pix1
      std::vector<double>* m_RecoTrk_Int_pix2Z; // Z position of track intersection with pix2
      std::vector<double>* m_RecoTrk_Int_pix3Z; // Z position of track intersection with pix3
      std::vector<double>* m_RecoTrk_Int_sct1Z; // Z position of track intersection with sct1
      std::vector<double>* m_RecoTrk_Int_sct2Z; // Z position of track intersection with sct2
      //
      /*
      std::vector<int>* m_RecoTrk_BLayOut;
      std::vector<double>* m_RecoTrk_BLayX;
      std::vector<double>* m_RecoTrk_BLayY;
      std::vector<double>* m_RecoTrk_BLayZ;
      */
      // look at properties of tracks selected for secondary vertices

      std::vector<double>* m_SVTrk_pT;
      std::vector<double>* m_SVTrk_p;
      std::vector<double>* m_SVTrk_phi;
      std::vector<double>* m_SVTrk_eta;
      std::vector<double>* m_SVTrk_2dIP;
      std::vector<double>* m_SVTrk_zIP;
      std::vector<double>* m_SVTrk_delp;
      std::vector<double>* m_SVTrk_del2dIP;
      std::vector<double>* m_SVTrk_delzIP;
      std::vector<int>* m_SVTrk_numBLay;
      std::vector<int>* m_SVTrk_numPix;
      std::vector<int>* m_SVTrk_numSCT;
      std::vector<int>* m_SVTrk_numTRT;
      std::vector<int>* m_SVTrk_barcode; // barcode of matching GenParticle
      std::vector<double>* m_SVTrk_matchPr; // match Prob
      std::vector<int>* m_SVTrk_TrkAuth; // 1=inside-out w/TRT, 2 = w/o TRT
                                           // 3=BackTracking, 4= TRT standalone
      std::vector<int>* m_SVTrk_TrkLowPt; // low pt tracking
      /*
      */
      //
      int m_InitialSVsize;  // number of SV after initial solutions are found
      int m_RearrangedSVsize; // number of SV after rearranging of solutions
      int m_numSecVrt; // number of final sec. vertex
      std::vector<double>* m_SecVtxX;
      std::vector<double>* m_SecVtxY;  // X,Y,Z of secondary vertices 
      std::vector<double>* m_SecVtxZ;
      std::vector<int>* m_numTrksSecVtx; // num tracks associated with sec. vertex 
      std::vector<double>* m_massSecVtx; // mass of sec. vertex assuming pions
      std::vector<double>* m_masseSecVtx; // mass of sec. vertex assuming tracks are electrons
      std::vector<double>* m_chi2SecVtx; // chi2 of sec. vertex
      std::vector<double>* m_pTSecVtx; // pT of sec. vertex
      std::vector<double>* m_pZSecVtx; // pZ of sec. vertex
      std::vector<int>*    m_chgSecVtx; // charge of secondary vertex
      std::vector<int>*    m_SumBLayHits; // sum of BLayer hits on trks in vrt.
      std::vector<int>*    m_AllTrksBLayHits; // =1 if all trks in vrt have BLay hits
      std::vector<double>* m_SecVtxMinOpAng; //min OpAng of tracks at SV
      std::vector<double>* m_SecVtxTrkPt;
      std::vector<double>* m_SecVtxTrkPhi;
      std::vector<double>* m_SecVtxTrkEta;   // store information about all tracks in all vertices
      std::vector<int>* m_SecVtxTrkBLay;    // tracks are in order, extract later
      std::vector<int>* m_SecVtxTrkPixExclBLay; 
      std::vector<int>* m_SecVtxTrkSCT;
      //
      // truth info
      int m_numChg; // no. of charged stable particles
      int m_numpT;  // + pT>500
      int m_numEta; // + abs(eta) < 2.5
      int m_numPix; // + starting radius of track is within pixels
      int m_numpt200; // number of tracks passing all these cuts but with pT=200-500
      //
      double m_truthSV1X; 
      double m_truthSV1Y; // X/Y/Z of the SV caused by the inc. trk (for single pi MC)
      double m_truthSV1Z; // NO cuts on whether we have outgoing trks to find a vertex
      //

      // num of tracks at the SV caused by the incident track (for single pi MC) 
      // I require that tracks coming out of this SV pass eta/Pixel fid cuts	  
      // and have pT>500                                                         
      int m_numTrksSV1a;   
      int m_numTrksSV1b; // here the cut is pT>200
      //
      // eta/phi/pt of incident single pion
      double m_truthEta;
      double m_truthPhi;
      double m_truthPt;
      int m_truthInt; // did this single pion interact?
      // keep track of all stable truth tracks
      //
      /*
      std::vector<double>* m_TruthTrk_pt;
      std::vector<double>* m_TruthTrk_eta;
      std::vector<double>* m_TruthTrk_phi;
      std::vector<double>* m_TruthTrk_R; // originating radius of track
      std::vector<double>* m_TruthTrk_Z; // originating z of track
      std::vector<double>* m_TruthTrk_EndR; // ending radius of track
      std::vector<double>* m_TruthTrk_EndZ; // ending z of track
      std::vector<double>* m_TruthTrk_L; // distance between prod/end vertex
      std::vector<int>* m_TruthTrk_barcode; // barcode 
      std::vector<int>* m_TruthTrk_pid; // pdgid 
      std::vector<double>* m_TruthTrk_matchPr; // match Prob., 0 if no match
      std::vector<double>* m_TruthTrk_2dIP; // 2d IP
      std::vector<double>* m_TruthTrk_ZIP; // Z IP
      */
      //
      int m_nSVHiPt; // no. of SV (incl. initial pion SV) with >=2 trks passing all cuts (pT> m_TrkPtCut)
      int m_nSVGe1HiPt; // no. of SV (incl. initial pion SV) with >=1 trks passing all cuts (pT> m_TrkPtCut)
      int m_nSVLoPt; // (incl. intial SV) with >=2 trks. passing all cuts (pT> m_LoPtCut)
      std::vector<int>*    m_truthAllVtxType;// =1 for 1 HiPt trk, =2 for >=1 HiPt trk, else 0
      std::vector<double>* m_truthAllSVX;
      std::vector<double>* m_truthAllSVY; // X,Y,Z of all SV (incl. initial pion SV)
      std::vector<double>* m_truthAllSVZ; // with >=2 tracks passing all cuts (pT>500)
      std::vector<int>*    m_truthAllVtxHasLifetime; // vertex has incoming part. w/ lifetime
      std::vector<int>*    m_truthAllVtxStrangeBaryon; // pid of strange baryon
      std::vector<int>*    m_truthAllVtxIncomingPID; // PID of part incident on SV
      std::vector<int>*    m_truthAllVtxNumIncident; //number of incoming particles into vertex
      std::vector<double>* m_truthAllVtxPtIncident; // Pt/Pz of incoming particle
      std::vector<double>* m_truthAllVtxPzIncident;
      std::vector<int>* m_truthAllNumTrks; // no. of outgoing trks
      std::vector<int>* m_truthAllNumRecoTrks; // no. of outgoing trks that were reco'd
      std::vector<int>* m_truthAllCharge; // SumChargeoutgoing trks
      std::vector<double>* m_truthAllSumTrksPt; // SumPt of outgoing trks
      std::vector<double>* m_truthAllSumTrksPz; // SumPz of outgoing trks
      std::vector<double>* m_truthAllVtxTrk1Pt;
      std::vector<double>* m_truthAllVtxTrk2Pt; //Pt of the 1st three tracks at the SV
      std::vector<double>* m_truthAllVtxTrk3Pt;
      std::vector<double>* m_truthAllVtxTrk4Pt;
      std::vector<double>* m_truthAllVtxTrk5Pt;
      std::vector<double>* m_truthAllMinOpAng; // minimum opAng between tracks at SV
      std::vector<double>* m_truthAllTrk2dIP; // for all truth trks in all SV
      std::vector<double>* m_truthAllTrkzIP; // for all truth trks in all SV
      std::vector<double>* m_truthAllTrkPt;  
      std::vector<double>* m_truthAllTrkEta; // for all truth trks in all SV
      std::vector<double>* m_truthAllTrkPhi; 
      std::vector<double>* m_truthAllTrkR; 
      std::vector<double>* m_truthAllTrkZ; // for all truth trks in all SV
      std::vector<int>* m_truthAllTrkbc; // for all truth trks in all SV
      // some variables for refit of truth trks
      //
      std::vector<int>* m_truthAllRefitStatus;
      std::vector<int>* m_truthAllNumReTrk;
      std::vector<double>* m_truthAllRefitChi2;
      std::vector<double>* m_truthAllRefitMass;
      std::vector<double>* m_truthAllRefitSVX;
      std::vector<double>* m_truthAllRefitSVY;
      std::vector<double>* m_truthAllRefitSVZ;
      //
      int m_maxTrks; // make it once - used for the last three variables
      bool myfunction (double i,double j) { return (i<j); }


      //    StatusCode m_scRes;
//

    struct WrkVrt{ 
        bool Good;
        deque<long int> SelTrk;
        Amg::Vector3D     vertex;
        TLorentzVector  vertexMom;
	long int Charge;
	vector<double> vertexCov;
	vector<double> Chi2PerTrk;
        vector< vector<double> > TrkAtVrt; 
	double Chi2;
	int nCloseVrt;
	double dCloseVrt;
	};
//
    // for event info to new ntuple (used to go by default in CollectionTree)
    StatusCode addEventInfo(); 
    StatusCode setupNtuple();
    void printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string name);

    void DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int , 
                         vector<const Trk::TrackParticleBase*> );
    void TrackClassification(std::vector< WrkVrt >* , 
                             std::vector< std::deque<long int> >* );
    double MaxOfShared(std::vector<WrkVrt> *, 
                       std::vector< std::deque<long int> >*,
	               long int & ,long int & );

    void RemoveTrackFromVertex(std::vector<WrkVrt> *, 
                               std::vector< std::deque<long int> > *,
			       long int & ,long int & );
 
   double minVrtVrtDist( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
   double minVrtVrtDistNext( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
   void MergeVertices( vector<WrkVrt> *WrkVrtSet, int & V1, int & V2);
   double VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>  & VrtErr1,
                     const Amg::Vector3D & Vrt2, const std::vector<double>  & VrtErr2);


   void  SelGoodTrkParticle( const vector<const Trk::TrackParticleBase*>& InpTrk,
                              const Trk::RecVertex                        & PrimVrt,
                               vector<const Trk::TrackParticleBase*>& SelectedTracks);

   void getIntersection(const Trk::Perigee* per);			       
   StatusCode CutTrk(double PInvVert,double ThetaVert,double A0Vert, double Zvert, double Chi2, 
           long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits);

   const Trk::Perigee* GetPerigee( const Trk::TrackParticleBase* i_ntrk); 
   StatusCode RefitVertex( vector<WrkVrt> *WrkVrtSet, int SelectedVertex,
                           vector<const Trk::TrackParticleBase*> & SelectedBaseTracks);
   void  FillCovMatrix(int iTrk, std::vector<double> & Matrix, AmgSymMatrix(5)& );
   // to get truthParticle from Reco. particle
   const Trk::TrackParticleBaseCollection* m_importedTrkColl;        
   const TrackParticleTruthCollection* m_importedTrkTruthColl;        
   const McEventCollection* m_importedFullTruthColl;
   // 
   // new version of truth routine - MCEventCollection (GEN_AOD or TruthEvent (ESD)
   StatusCode getNewTruthInfo(vector<const Trk::TrackParticleBase*> & SelectedBaseTracks);
   // older version uses SpclMC. it is obsolete now. keep for posterity
   // VJ Mar. 4, 2010
   StatusCode getTruthInfo(vector<const Trk::TrackParticleBase*> & SelectedBaseTracks);

   // to get Reco. track from truth particle
   typedef Trk::InverseTruthMap<TrackParticleTruthCollection> MyMap;
   MyMap m_invMap;

   //
   void setMcEventCollection(const McEventCollection*); 
   HepMC::GenParticle *getTrkGenParticle(const Trk::TrackParticleBase*, double& matchProb);
   void setTrackParticleTruthCollection(const TrackParticleTruthCollection*);        
   void setTrackParticleContainer(const Trk::TrackParticleBaseCollection*);

  };
} // end of namespace bracket

#endif
