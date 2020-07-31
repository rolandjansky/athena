/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// InDetVKalVxInJetTool.h - Description
//
/*
   Tool for secondary vertex inside jet reconstruction.
   It returns a pointer to Trk::VxSecVertexInfo object which contains
   vector of pointers to xAOD::Vertex's of found secondary verteces.
   In case of failure pointer to Trk::VxSecVertexInfo is 0.
   

   Package creates a derivative object VxSecVKalVertexInfo which contains also additional variables
   see  Tracking/TrkEvent/VxSecVertex/VxSecVertex/VxSecVKalVertexInfo.h
   
    
   Additional returned values:
      Results    - vector of variables for b-tagging
                   [0] - secondary vertex mass
                   [1] - energy ratio   Esec/Ejet
		   [2] - number of 2-track vertices
		   [3] - pointers to "bad" tracks
		   [4] - pointers to identified V0 tracks

  Package has "single vertex" and "multiple vertices" modes of work.

  More details at

     https://twiki.cern.ch/twiki/bin/save/Atlas/BTagVrtSec

    Author: Vadim Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch

-----------------------------------------------------------------------------*/



#ifndef _VKalVrt_InDetVKalVxInJetTool_H
#define _VKalVrt_InDetVKalVxInJetTool_H
// Normal STL and physical vectors
#include <vector>
// Gaudi includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "boost/graph/adjacency_list.hpp"
//
#include  "Particle/TrackParticle.h"
#include  "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
//
//
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
//#include "InDetMaterialRejTool/InDetMaterialRejTool.h"
#include "InDetVKalVxInJetTool/InDetTrkInJetType.h"

class TH1D;
class TH2D;
class TH1F;
class TProfile;
class TTree;
class IChronoStatSvc;

namespace Trk{
  class TrkVKalVrtFitter;
  class IVertexFitter;
  class IVKalState;
}


typedef std::vector<double> dvect;

 
//------------------------------------------------------------------------
namespace InDet {

  class VKalVxInJetTemp{
    public:
      std::vector<int> m_Incomp;
      std::vector<int> m_Prmtrack;
  };

  struct workVectorArrxAOD{
        std::vector<const xAOD::TrackParticle*> listJetTracks;
        std::vector<const xAOD::TrackParticle*> tmpListTracks;
        std::vector<const xAOD::TrackParticle*> listSecondTracks;
        std::vector<const xAOD::TrackParticle*> TracksForFit;
        std::vector<const xAOD::TrackParticle*> InpTrk;
        std::vector< std::vector<const xAOD::TrackParticle*> >  FoundSecondTracks;
		     std::vector<const xAOD::TrackParticle*>    TrkFromV0; 
  };
  
  class ATLAS_NOT_THREAD_SAFE InDetVKalVxInJetTool : //This tool should not be used in a reentrant algorithm because of the mutable m_NRefPVTrk
    public AthAlgTool, virtual public ISecVertexInJetFinder{


  public:
       /* Constructor */
      InDetVKalVxInJetTool(const std::string& type, const std::string& name, const IInterface* parent);
       /* Destructor */
      virtual ~InDetVKalVxInJetTool();


      StatusCode initialize();
      StatusCode finalize();

      Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & primaryVertex,
                                                const TLorentzVector & jetMomentum,
                                                const std::vector<const xAOD::IParticle*> & inputTracks) const;

//------------------------------------------------------------------------------------------------------------------
// Private data and functions
//

    private:

      double m_w_1{};
      TTree* m_tuple{};
      TH1D* m_hb_massPiPi{};
      TH1D* m_hb_massPiPi1{};
      TH1D* m_hb_massPPi{};
      TH1D* m_hb_massEE{};
      TH1D* m_hb_totmassEE{};
      TH1D* m_hb_totmass2T0{};
      TH1D* m_hb_totmass2T1{};
      TH1D* m_hb_totmass2T2{};
      TH1D* m_hb_nvrt2{};
      TH1D* m_hb_ratio{};
      TH1D* m_hb_totmass{};
      TH1D* m_hb_impact{};
      TH1D* m_hb_impactR{};
      TH2D* m_hb_impactRZ{};
      TH1D* m_hb_trkD0{};
      TH1D* m_hb_ntrkjet{};
      TH1D* m_hb_impactZ{};
      TH1D* m_hb_r2d{};
      TH1D* m_hb_r1dc{};
      TH1D* m_hb_r2dc{};
      TH1D* m_hb_r3dc{};
      TH1D* m_hb_rNdc{};
      TH1D* m_hb_dstToMat{};
      TH1D* m_hb_jmom{};
      TH1D* m_hb_mom{};
      TH1D* m_hb_signif3D{};
      TH1D* m_hb_impV0{};
      TH1D* m_hb_sig3DTot{};
      TH1F* m_hb_goodvrtN{};
      TH1D* m_hb_distVV{};
      TH1D* m_hb_diffPS{};
      TH1D* m_hb_sig3D1tr{};
      TH1D* m_hb_sig3D2tr{};
      TH1D* m_hb_sig3DNtr{};
      TH1D* m_hb_trkPtMax{};
      TH1F* m_hb_blshared{};
      TH1F* m_hb_pxshared{};
      TH1F* m_hb_rawVrtN{};
      TH1F* m_hb_lifetime{};
      TH1F* m_hb_trkPErr{};
      TH1F* m_hb_deltaRSVPV{};
//--
      TProfile * m_pr_NSelTrkMean{};
      TProfile * m_pr_effVrt2tr{};
      TProfile * m_pr_effVrt2trEta{};
      TProfile * m_pr_effVrt{};
      TProfile * m_pr_effVrtEta{};
      long int m_iflag{};

      SimpleProperty<int>    m_Robustness;
      SimpleProperty<double> m_MassForConstraint;
      std::vector<double>    m_MassInputParticles;



      long int m_cutSctHits{};
      long int m_cutPixelHits{};
      long int m_cutSiHits{};
      long int m_cutBLayHits{};
      long int m_cutSharedHits{};
      double m_cutPt{};
      double m_cutZVrt{};
      double m_cutA0{};
      double m_cutChi2{};
      double m_secTrkChi2Cut{};
      double m_coneForTag{};
      double m_sel2VrtChi2Cut{};
      double m_sel2VrtSigCut{};
      double m_trkSigCut{};
      double m_a0TrkErrorCut{};
      double m_zTrkErrorCut{};
      double m_cutHFClass{};
      double m_antiGarbageCut{};
      double m_antiFragmentCut{};
      double m_Vrt2TrMassLimit{};

      bool m_fillHist{};

      bool m_existIBL{};

      long int m_RobustFit{};

      double m_beampipeX{};
      double m_beampipeY{};
      double m_xLayerB{};
      double m_yLayerB{};
      double m_xLayer1{};
      double m_yLayer1{};
      double m_xLayer2{};
      double m_yLayer2{};
      double m_beampipeR{};
      double m_rLayerB{};
      double m_rLayer1{};
      double m_rLayer2{};
      double m_rLayer3{};

      bool     m_useVertexCleaning{};
      bool     m_multiVertex{};
      bool     m_multiWithPrimary{};
      bool     m_getNegativeTail{};
      bool     m_getNegativeTag{};
      bool     m_multiWithOneTrkVrt{};

      double    m_vertexMergeCut{};
      double    m_trackDetachCut{};


      ToolHandle < Trk::IVertexFitter >       m_fitterSvc;
      Trk::TrkVKalVrtFitter*   m_fitSvc{};
      IChronoStatSvc * m_timingProfile{}; 
 
      ToolHandle < IInDetTrkInJetType >       m_trackClassificator;

      double m_massPi {};
      double m_massP {};
      double m_massE{};
      double m_massK0{};
      double m_massLam{};
      double m_massB{};
      mutable int m_NRefPVTrk{};    //Measure of track in jet multiplicity
      std::string m_instanceName;

//-------------------------------------------
//For ntuples (only for development/tuning!)

      int notFromBC(int PDGID) const;
      const xAOD::TruthParticle * getPreviousParent(const xAOD::TruthParticle * child, int & ParentPDG) const;
      int getIdHF(const  Rec::TrackParticle* TP ) const;
      int getIdHF(const xAOD::TrackParticle* TP ) const;
      int getG4Inter( const  Rec::TrackParticle* TP ) const;
      int getG4Inter( const xAOD::TrackParticle* TP ) const;
      int getMCPileup(const  Rec::TrackParticle* TP ) const;
      int getMCPileup(const xAOD::TrackParticle* TP ) const;

      struct DevTuple 
     { 
       static const int maxNTrk=100;
       static const int maxNVrt=100;
       int nTrkInJet;
       float ptjet;
       float etajet;
       float phijet;
       float etatrk[maxNTrk];
       float p_prob[maxNTrk];
       float s_prob[maxNTrk];
       int   idMC[maxNTrk];
       float SigR[maxNTrk];
       float SigZ[maxNTrk];
       float   d0[maxNTrk];
       float   Z0[maxNTrk];
       float pTvsJet[maxNTrk];
       float  prodTJ[maxNTrk];
       float    wgtB[maxNTrk];
       float    wgtL[maxNTrk];
       float    wgtG[maxNTrk];
       float   Sig3D[maxNTrk];
       int    chg[maxNTrk];
       int  nVrtT[maxNTrk];
       float TotM;
       int   nVrt;
       float VrtDist2D[maxNVrt];
       float VrtSig3D[maxNVrt];
       float VrtSig2D[maxNVrt];
       float VrtDR[maxNVrt];
       float mass[maxNVrt];
       float Chi2[maxNVrt];
       int   itrk[maxNVrt];
       int   jtrk[maxNVrt];
       int badVrt[maxNVrt];
       int    ibl[maxNVrt];
       int     bl[maxNVrt];
       int        NTHF;
       int   itHF[maxNVrt];
       //---
       int   nNVrt;
       float NVrtDist2D[maxNVrt];
       int   NVrtNT[maxNVrt];
       int   NVrtTrkI[maxNVrt];
       float NVrtM[maxNVrt];
       float NVrtChi2[maxNVrt];
       float NVrtMaxW[maxNVrt];
       float NVrtAveW[maxNVrt];
       float NVrtDR[maxNVrt];
     };
     DevTuple*  m_curTup;

     struct Vrt2Tr 
     {   int i=0, j=0;
         int badVrt=0;
         Amg::Vector3D     FitVertex;
         TLorentzVector    Momentum;
         long int   vertexCharge;
         std::vector<double> ErrorMatrix;
         std::vector<double> Chi2PerTrk;
         std::vector< std::vector<double> > TrkAtVrt;
         double Signif3D=0.;
         double Signif3DProj=0.;
         double Signif2D=0.;
         double Chi2=0.;
         double dRSVPV=-1.;
     };


// For multivertex version only

      boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> *m_compatibilityGraph{};
      float m_chiScale[11]{};
      VKalVxInJetTemp*  m_WorkArray{};     
      struct WrkVrt 
     {   bool Good=true;
         std::deque<long int> SelTrk;
         Amg::Vector3D     vertex;
         TLorentzVector    vertexMom;
         long int   vertexCharge{};
         std::vector<double> vertexCov;
         std::vector<double> Chi2PerTrk;
         std::vector< std::vector<double> > TrkAtVrt;
         double Chi2{};
         int nCloseVrt=0;
         double dCloseVrt=1000000.;
	 double ProjectedVrt=0.;
         int detachedTrack=-1;
      };


//
//   Private struct for track oredering (happened to be not needed)
//
//     struct compareTrk {
//        compareTrk(const TLorentzVector J):JetDir(J.Vect()){};
//        bool operator() (const xAOD::TrackParticle* p1, const xAOD::TrackParticle*p2) { 
//                double cmp1=p1->p4().Rho()*(1.-sin(p1->p4().Angle(JetDir)));
//                double cmp2=p2->p4().Rho()*(1.-sin(p2->p4().Angle(JetDir)));
//                return (cmp1>cmp2);  }
//	TVector3 JetDir;
//     };

//   Private technical functions
//
//
      xAOD::Vertex*      GetVrtSec(const std::vector<const xAOD::TrackParticle*> & InpTrk,
                                   const xAOD::Vertex                            & PrimVrt,
                                   const TLorentzVector                          & JetDir,
                                   std::vector<double>                           & Results,
                                   std::vector<const xAOD::TrackParticle*>       & SelSecTrk,
                                   std::vector<const xAOD::TrackParticle*>       & TrkFromV0) const;

      std::vector<xAOD::Vertex*> GetVrtSecMulti(
        workVectorArrxAOD*,
        const xAOD::Vertex& PrimVrt,
        const TLorentzVector& JetDir,
        std::vector<double>& Results) const;

      void  TrackClassification(std::vector<WrkVrt> *WrkVrtSet, 
                                std::vector< std::deque<long int> > *TrkInVrt) const;

      double MaxOfShared(std::vector<WrkVrt> *WrkVrtSet, 
                         std::vector< std::deque<long int> > *TrkInVrt,
			 long int & SelectedTrack,
			 long int & SelectedVertex) const;
      void RemoveTrackFromVertex(std::vector<WrkVrt> *WrkVrtSet, 
                                 std::vector< std::deque<long int> > *TrkInVrt,
				 long int & SelectedTrack,
				 long int & SelectedVertex) const;
//
//

      void printWrkSet(const std::vector<WrkVrt> * WrkSet, const std::string& name ) const;


      StatusCode CutTrk(double,double, double , double , double , 
         long int ,long int ,long int , long int ) const;
      double ConeDist(const AmgVector(5) & , const TLorentzVector & ) const;
//
// Gives correct mass assignment in case of nonequal masses
      double massV0( std::vector< std::vector<double> >& TrkAtVrt, double massP, double massPi ) const;
      int FindMax( std::vector<double>& Chi2PerTrk, std::vector<float>&  rank) const;


      TLorentzVector TotalMom(const std::vector<const Trk::Perigee*>& InpTrk) const; 
      TLorentzVector TotalMom(const std::vector<const xAOD::TrackParticle*>& InpTrk) const; 
      TLorentzVector MomAtVrt(const std::vector<double>& InpTrk) const; 
      double           TotalTMom(const std::vector<const Trk::Perigee*> & InpTrk) const; 
      double           TotalTVMom(const Amg::Vector3D &Dir, const std::vector<const Trk::Perigee*>& InpTrk) const; 
      double           pTvsDir(const Amg::Vector3D &Dir, const std::vector<double>& InpTrk) const; 
      double           VrtRadiusError(const Amg::Vector3D & SecVrt, const std::vector<double>  & VrtErr) const;

      bool  insideMatLayer(float ,float ) const;
      void  fillVrtNTup( std::vector<Vrt2Tr> & all2TrVrt) const;
      void  fillNVrtNTup(std::vector<WrkVrt> & VrtSet, std::vector< std::vector<float> > & trkScore,
                         const xAOD::Vertex   & PrimVrt, const TLorentzVector & JetDir)const;

      TLorentzVector GetBDir( const xAOD::TrackParticle* trk1,
                              const xAOD::TrackParticle* trk2,
                              const xAOD::Vertex    & PrimVrt,
			      Amg::Vector3D &V1, Amg::Vector3D &V2) const;

      int   nTrkCommon( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2) const;
      double minVrtVrtDist( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2) const;
      double minVrtVrtDistNext( std::vector<WrkVrt> *WrkVrtSet, int & V1, int & V2) const;
      bool isPart( std::deque<long int> test, std::deque<long int> base) const;
      void Clean1TrVertexSet(std::vector<WrkVrt> *WrkVrtSet) const;
      double JetProjDist(Amg::Vector3D &SecVrt, const xAOD::Vertex &PrimVrt, const TLorentzVector &JetDir) const;

      double VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& VrtErr,double& Signif ) const;
      double VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& VrtErr,double& Signif ) const;
      double VrtVrtDist2D(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& VrtErr,double& Signif ) const;
      double VrtVrtDist(const Trk::RecVertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& SecVrtErr, const TLorentzVector & JetDir) const;
      double VrtVrtDist(const xAOD::Vertex & PrimVrt, const Amg::Vector3D & SecVrt, 
                                  const std::vector<double>& SecVrtErr, const TLorentzVector & JetDir) const;
      double VrtVrtDist(const Amg::Vector3D & Vrt1, const std::vector<double>& VrtErr1,
                        const Amg::Vector3D & Vrt2, const std::vector<double>& VrtErr2) const;
 
      template <class Particle>
      void DisassembleVertex(std::vector<WrkVrt> *WrkVrtSet, int iv, 
                             std::vector<const Particle*>  AllTracks,
                             Trk::IVKalState& istate) const;
					  
      double ProjSV_PV(const Amg::Vector3D & SV, const xAOD::Vertex & PV, const TLorentzVector & Jet) const;

      double RankBTrk(double TrkPt, double JetPt, double Signif) const;
 

      const Trk::Perigee* GetPerigee( const xAOD::TrackParticle* ) const;
      std::vector<const Trk::Perigee*> GetPerigeeVector( const std::vector<const Trk::TrackParticleBase*>& ) const;


      template <class Trk>
      double FitCommonVrt(std::vector<const Trk*>& ListSecondTracks,
                          std::vector<float>   & trkRank,
                          const xAOD::Vertex   & PrimVrt,
 	                  const TLorentzVector & JetDir,
                          std::vector<double>  & InpMass, 
	                  Amg::Vector3D        & FitVertex,
                          std::vector<double>  & ErrorMatrix,
	                  TLorentzVector       & Momentum,
		     std::vector< std::vector<double> >  & TrkAtVrt) const; 

      template <class Trk>
      void RemoveEntryInList(std::vector<const Trk*>& , std::vector<float>&, int) const;
      template <class Trk>
      void RemoveDoubleEntries(std::vector<const Trk*>& ) const;

      template <class Particle>
      StatusCode RefitVertex( std::vector<WrkVrt> *WrkVrtSet, int SelectedVertex,
                              std::vector<const Particle*> & SelectedTracks,
                              Trk::IVKalState& istate,
                              bool ifCovV0) const;

      template <class Particle>
      double RefitVertex( WrkVrt &Vrt,std::vector<const Particle*> & SelectedTracks,
                          Trk::IVKalState& istate,
                          bool ifCovV0) const;

      template <class Particle>
      double mergeAndRefitVertices( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2, WrkVrt & newvrt,
                                    std::vector<const Particle*> & AllTrackList,
                                    Trk::IVKalState& istate) const;
      template <class Particle>
      void   mergeAndRefitOverlapVertices( std::vector<WrkVrt> *WrkVrtSet, int V1, int V2,
                                           std::vector<const Particle*> & AllTrackLis,
                                           Trk::IVKalState& istate) const;

      template <class Particle>
      double  improveVertexChi2( std::vector<WrkVrt> *WrkVrtSet, int V, std::vector<const Particle*> & AllTracks,
                                 Trk::IVKalState& istate,
                                 bool ifCovV0) const;

     int   SelGoodTrkParticle( const std::vector<const xAOD::TrackParticle*>& InpPart,
                                const xAOD::Vertex                           & PrimVrt,
	                        const TLorentzVector                         & JetDir,
                                      std::vector<const xAOD::TrackParticle*>& SelPart) const;


      template <class Trk>
      void Select2TrVrt(std::vector<const Trk*>  & SelectedTracks,
                        std::vector<const Trk*>  & TracksForFit,
                        const xAOD::Vertex       & PrimVrt,
 	                const TLorentzVector     & JetDir,
                        std::vector<double>      & InpMass, 
	                std::vector<const Trk*>  & TrkFromV0,
	                std::vector<const Trk*>  & ListSecondTracks) const;

     Amg::MatrixX  makeVrtCovMatrix( std::vector<double> & ErrorMatrix ) const;

     Amg::MatrixX SetFullMatrix(int NTrk, std::vector<double> & Matrix) const;


//////////////////////////////////////////////////////////////////////////////////////////
//   Needed for TrackParticle<->TrackParticleBase story!!!!
//
//     template <class Track>
//     StatusCode VKalVrtFitFastBase(const std::vector<const Track*>& listPart,Amg::Vector3D& Vertex) const;
     StatusCode VKalVrtFitFastBase(const std::vector<const xAOD::TrackParticle*>& listPart,Amg::Vector3D& Vertex, Trk::IVKalState& istate) const;

     template <class Track>
     bool  Check2TrVertexInPixel( const Track* p1, const Track* p2, Amg::Vector3D &, std::vector<double> &) const;
     template <class Track>
     bool  Check1TrVertexInPixel( const Track* p1, Amg::Vector3D &, std::vector<double> & ) const;

     void  getPixelLayers(const xAOD::TrackParticle* Part, int &blHit, int &l1Hit, int &l2Hit, int &nLay) const;
     void  getPixelDiscs(const xAOD::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const;
     void  getPixelProblems(const xAOD::TrackParticle* Part, int &splshIBL, int &splshBL ) const;


     StatusCode VKalVrtFitBase(const std::vector<const xAOD::TrackParticle*> & listPart,
                                Amg::Vector3D&                 Vertex,
                                TLorentzVector&                Momentum,
                                long int&                      Charge,
                                std::vector<double>&           ErrorMatrix,
                                std::vector<double>&           Chi2PerTrk,
                                std::vector< std::vector<double> >& TrkAtVrt,
                                double& Chi2,
                                Trk::IVKalState& istate,
                                bool ifCovV0) const;

     StatusCode GetTrkFitWeights(std::vector<double> & wgt,
                                 const Trk::IVKalState& istate) const;
   };


  template <class Trk>
  void InDetVKalVxInJetTool::RemoveEntryInList(std::vector<const Trk*>& ListTracks, std::vector<float> & rank, int Outlier) const
  {
    if(Outlier < 0 ) return;
    if(Outlier >= (int)ListTracks.size() ) return;
    ListTracks.erase( ListTracks.begin()+Outlier);
    rank.erase( rank.begin()+Outlier);
  }     

  template <class Trk>
  void InDetVKalVxInJetTool::RemoveDoubleEntries(std::vector<const Trk*>& ListTracks) const
  {
    typename std::vector<const Trk*>::iterator   TransfEnd;
    sort(ListTracks.begin(),ListTracks.end());
    TransfEnd =  unique(ListTracks.begin(),ListTracks.end());
    ListTracks.erase( TransfEnd, ListTracks.end());
  }     

  struct clique_visitor
  {
    clique_visitor(std::vector< std::vector<int> > & input): m_allCliques(input){ input.clear();}
    
    template <typename Clique, typename Graph>
    void clique(const Clique& clq, Graph& )
    { 
      std::vector<int> new_clique(0);
      for(auto i = clq.begin(); i != clq.end(); ++i) new_clique.push_back(*i);
      m_allCliques.push_back(new_clique);
    }

    std::vector< std::vector<int> > & m_allCliques;

  };

   template <class Track>
   bool InDetVKalVxInJetTool::Check1TrVertexInPixel( const Track* p1, Amg::Vector3D &FitVertex, std::vector<double> &VrtCov)
   const
   {
	int blTrk=0, blP=0, l1Trk=0, l1P=0, l2Trk=0, nLays=0; 
        getPixelLayers( p1, blTrk , l1Trk, l2Trk, nLays );
        getPixelProblems(p1, blP, l1P );
        double xDif=FitVertex.x()-m_xLayerB, yDif=FitVertex.y()-m_yLayerB ; 
        double Dist2DBL=sqrt(xDif*xDif+yDif*yDif);
        if      (Dist2DBL < m_rLayerB-VrtRadiusError(FitVertex, VrtCov)){       //----------------------------------------- Inside B-layer
          if( blTrk<1  && l1Trk<1  )  return false;
          if(  nLays           <2 )   return false;  // Less than 2 layers on track 0
	  return true;
        }else if(Dist2DBL > m_rLayerB+VrtRadiusError(FitVertex, VrtCov)){      //----------------------------------------- Outside b-layer
          if( blTrk>0 && blP==0 ) return false;  // Good hit in b-layer is present
       }
// 
// L1 and L2 are considered only if vertex is in acceptance
//
	if(fabs(FitVertex.z())<400.){
          xDif=FitVertex.x()-m_xLayer1, yDif=FitVertex.y()-m_yLayer1 ;
	  double Dist2DL1=sqrt(xDif*xDif+yDif*yDif);
          xDif=FitVertex.x()-m_xLayer2, yDif=FitVertex.y()-m_yLayer2 ; 
	  double Dist2DL2=sqrt(xDif*xDif+yDif*yDif);
          if      (Dist2DL1 < m_rLayer1-VrtRadiusError(FitVertex, VrtCov)) {   //------------------------------------------ Inside 1st-layer
             if( l1Trk<1  && l2Trk<1  )     return false;  // Less than 1 hits on track 0
             return true;
          }else if(Dist2DL1 > m_rLayer1+VrtRadiusError(FitVertex, VrtCov)) {  //------------------------------------------- Outside 1st-layer
	     if( l1Trk>0 && l1P==0 )       return false;  //  Good L1 hit is present
          }
          
          if      (Dist2DL2 < m_rLayer2-VrtRadiusError(FitVertex, VrtCov)) {  //------------------------------------------- Inside 2nd-layer
	     if( l2Trk==0 )  return false;           // At least one L2 hit must be present
          }else if(Dist2DL2 > m_rLayer2+VrtRadiusError(FitVertex, VrtCov)) {  
	  //   if( l2Trk>0  )  return false;           // L2 hits are present
	  }           
        } else {
	  int d0Trk=0, d1Trk=0, d2Trk=0; 
          getPixelDiscs( p1, d0Trk , d1Trk, d2Trk );
          if( d0Trk+d1Trk+d2Trk ==0 )return false;
        }
        return true;
   }

}  //end namespace

#endif
