/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///
///  @author  Vadim Kostyukhin <vadim.kostyukhin@cern.ch>
///


// Header include
#include "VrtSecInclusive/MultiSecVertexTool.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "PathResolver/PathResolver.h"
//---------------------------------------------
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "CxxUtils/sincos.h"
#include <cmath>
#include "boost/graph/bron_kerbosch_all_cliques.hpp"
 
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "MVAUtils/BDT.h" 


namespace Rec {

//
//Constructor-------------------------------------------------------------- 
MultiSecVertexTool::MultiSecVertexTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_cutSctHits(4),
    m_cutPixelHits(2),
    m_cutSiHits(8),
    m_cutBLayHits(0),
    m_cutSharedHits(1),
    m_cutPt(500.),
    m_cutZVrt(15.),
    m_cutA0(10.),
    m_cutChi2(5.),
    m_sel2VrtProbCut(0.02),
    m_globVrtProbCut(0.005),
    m_maxSVRadiusCut(140.),
    m_selVrtSigCut(3.0),
    m_trkSigCut(2.0),
    m_a0TrkErrorCut(1.0),
    m_zTrkErrorCut(5.0),
    m_VrtMassLimit(5500.),
    m_Vrt2TrMassLimit(4000.),
    m_Vrt2TrPtLimit(5.e5),
    m_antiPileupSigRCut(2.0),
    m_dRdZRatioCut(0.25),
    m_v2tIniBDTCut(-0.6),
    m_v2tFinBDTCut(0.),
    m_vertexMergeCut(3.),
    m_trackDetachCut(6.),
    m_beampipeR(24.3),
    m_removeTrkMatSignif(0.),
    m_fillHist(false),
    m_useVertexCleaning(true),
    m_multiWithOneTrkVrt(true),
    m_calibFileName("Fake2TrVertexReject.MVA.v01.root"),
    m_SV2T_BDT(nullptr),
    m_beamService("BeamCondSvc",name),
    m_extrapolator("Trk::Extrapolator/Extrapolator",this),
    m_fitSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
   {
    declareInterface< IVrtInclusive >(this);
    declareProperty("CutSctHits",    m_cutSctHits ,  "Remove track is it has less SCT hits");
    declareProperty("CutPixelHits",  m_cutPixelHits, "Remove track is it has less Pixel hits");
    declareProperty("CutSiHits",     m_cutSiHits,    "Remove track is it has less Pixel+SCT hits");
    declareProperty("CutBLayHits",   m_cutBLayHits,  "Remove track is it has less B-layer hits");
    declareProperty("CutSharedHits", m_cutSharedHits,"Reject final 2tr vertices if tracks have shared hits");

    declareProperty("CutPt",         m_cutPt,     "Track Pt selection cut");
    declareProperty("CutA0",         m_cutA0,     "Track A0 selection cut");
    declareProperty("CutZVrt",       m_cutZVrt,   "Track Z impact selection cut");
    declareProperty("CutChi2",       m_cutChi2,   "Track Chi2 selection cut");
    declareProperty("TrkSigCut",     m_trkSigCut, "Track 3D impact significance w/r primary vertex. Should be >=AntiPileupSigRCut");

    declareProperty("A0TrkErrorCut",  m_a0TrkErrorCut,  "Track A0 error cut");
    declareProperty("ZTrkErrorCut",   m_zTrkErrorCut,   "Track Z impact error cut");
    declareProperty("VrtMassLimit",   m_VrtMassLimit,   "Maximal allowed mass for found vertices");
    declareProperty("Vrt2TrMassLimit",m_Vrt2TrMassLimit,"Maximal allowed mass for 2-track vertices");
    declareProperty("Vrt2TrPtLimit",  m_Vrt2TrPtLimit,  "Maximal allowed Pt for 2-track vertices. Calibration limit");

    declareProperty("Sel2VrtProbCut",      m_sel2VrtProbCut, "Cut on probability of 2-track vertex for initial selection");
    declareProperty("GlobVrtProbCut",      m_globVrtProbCut, "Cut on probability of any vertex for final selection");
    declareProperty("MaxSVRadiusCut",      m_maxSVRadiusCut, "Cut on maximal radius of SV (def = Pixel detector size)");
    declareProperty("SelVrtSigCut",        m_selVrtSigCut,  "Cut on significance of 3D distance between vertex and PV");
    declareProperty("AntiPileupSigRCut",   m_antiPileupSigRCut,  "Upper cut on significance of 2D distance between beam and perigee");
    declareProperty("dRdZRatioCut",        m_dRdZRatioCut,  "Cut on dR/dZ ratio to remove pileup tracks"  );
    declareProperty("v2tIniBDTCut",        m_v2tIniBDTCut,  "Initial BDT cut for 2track vertices selection "  );
    declareProperty("v2tFinBDTCut",        m_v2tFinBDTCut,  "Final BDT cut for 2track vertices selection ");
    declareProperty("FillHist",            m_fillHist, "Fill technical histograms");
    declareProperty("useVertexCleaning",   m_useVertexCleaning,    "Clean vertices by requiring pixel hit presence according to vertex position");
    declareProperty("MultiWithOneTrkVrt",  m_multiWithOneTrkVrt,"Allow one-track-vertex addition to already found secondary vertices");
    declareProperty("VertexMergeCut",	   m_vertexMergeCut, "To allow vertex merging for MultiVertex Finder");
    declareProperty("TrackDetachCut",	   m_trackDetachCut, "To allow track from vertex detachment for MultiVertex Finder");
    declareProperty("beampipeR",	   m_beampipeR, "Radius of the beampipe material" );
    declareProperty("removeTrkMatSignif",  m_removeTrkMatSignif, "Significance of Vertex-TrackingMaterial distance for removal. No removal if <=0.");
    declareProperty("calibFileName",       m_calibFileName, " MVA calibration file for 2-track fake vertices removal" );
    declareProperty("BeamSpotSvc",         m_beamService, "Name of the BeamSpot service");
    declareProperty("VertexFitterTool",    m_fitSvc, "Name of the Vertex Fitter tool");
    m_massPi  =  Trk::ParticleMasses().mass[Trk::pion];
    m_massP   =  Trk::ParticleMasses().mass[Trk::proton];
    m_massE   =  Trk::ParticleMasses().mass[Trk::electron];
    m_massK0  =  Trk::ParticleMasses().mass[Trk::k0];
    m_massLam =  1115.683  ;
    m_compatibilityGraph = nullptr;
    m_instanceName=name;

   }

    MultiSecVertexTool::~MultiSecVertexTool(){
     ATH_MSG_DEBUG("MultiSecVertexTool destructor called");
   }

   StatusCode MultiSecVertexTool::initialize(){
     ATH_MSG_DEBUG( "Initialising MultiSecVertexTool- Package version: " << PACKAGE_VERSION ); 
     m_compatibilityGraph = new boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>();

     ATH_CHECK( m_beamService.retrieve() );
     ATH_CHECK( m_extrapolator.retrieve() );

     ATH_CHECK( m_fitSvc.retrieve() );
     ATH_MSG_DEBUG("MultiSecVertexTool TrkVKalVrtFitter found");

     ITHistSvc*     hist_root=0;
     if(m_fillHist){

       StatusCode sc = service( "THistSvc", hist_root); 
       if( sc.isFailure() )  ATH_MSG_DEBUG("Could not find THistSvc service");
       else                  ATH_MSG_DEBUG("MultiSecVertexTool Histograms found");
 
       m_hb_massPiPi   = new TH1D("massPiPi"," mass PiPi",200,0., 4000.);
       m_hb_massPiPi1  = new TH1D("massPiPi1"," mass PiPi",200,0., 4000.);
       m_hb_massPPi    = new TH1D("massPPi"," massPPi", 100,1000., 1250.);
       m_hb_massEE     = new TH1D("massEE"," massEE", 100,0., 200.);
       m_hb_nvrt2      = new TH1D("nvrt2"," vertices2", 50,0., 50.);
       m_hb_ratio      = new TH1D("ratio"," ratio", 51,0., 1.02);
       m_hb_totmass    = new TH1D("totmass"," totmass", 250,0., 10000.);
       m_hb_impact     = new TH1D("impact", " impact", 100,0., 20.);
       m_hb_impactR    = new TH1D("impactR"," impactR", 400,-30., 70.);
       m_hb_impactZ    = new TH1D("impactZ"," impactZ", 100,-30., 70.);
       m_hb_impactRZ   = new TH2D("impactRZ"," impactRZ", 40,-10., 10., 60, -30.,30. );
       m_hb_pileupRat  = new TH1D("pileupRatio"," dR/dZ ratio", 100, 0., 1.);
       m_hb_trkD0      = new TH1D("trkD0"," d0 of tracks", 100, 0., 10.);
       m_hb_trkZ       = new TH1D("trkZ"," Z of tracks", 120,-60., 60.);
       m_hb_r2d        = new TH1D("r2interact","Interaction radius 2tr selected", 150,0., 150.);
       m_hb_ntrksel    = new TH1F("NTrkSel","Number of selected tracks", 200,0., 200.);
       m_hb_ntrkInput  = new TH1F("NTrkInput","Number of provided tracks", 200,0., 1000.);
       m_hb_trkSelect  = new TH1F("TrkSelect","Track selection efficiency", 15,0., 15.);
       m_hb_signif3D   = new TH1D("signif3D"," Signif3D for initial 2tr vertex", 140,-20., 50.);
       m_hb_sig3DTot   = new TH1D("sig3dcommon"," Signif3D for common vertex", 140,-20., 50.);
       m_hb_sig3D1tr   = new TH1D("sig3D1tr","Signif3D for 1tr  vertices", 140,-20., 50.);
       m_hb_sig3D2tr   = new TH1D("sig3D2tr","Signif3D for 2tr single vertex", 140,-20., 50.);
       m_hb_sig3DNtr   = new TH1D("sig3DNtr","Signif3D for many-tr single vertex", 140,-20., 50.);
       m_hb_goodvrtN   = new TH1F("goodvrtN","Number of good vertices", 20,0., 20.);
       m_hb_goodvrt1N  = new TH1F("goodvrt1N","Number of good 1-track vertices", 20,0., 20.);
       m_hb_distVV     = new TH1D("distvv","Vertex-Vertex dist", 100,0., 20.);
       m_hb_diffPS     = new TH1D("diffPS","Primary-Secondary assoc", 200,-20., 20.);
       m_hb_rawVrtN    = new TH1F("rawVrtN","Number of raw vertices multivertex case", 20, 0., 20.);
       m_hb_cosSVMom   = new TH1F("cosSVMom","SV-PV vs SV momentum ", 100, 0., 1.);
       m_hb_etaSV      = new TH1F("etaSV"," Eta of SV-PV ", 100, -5., 5.);
       m_hb_fakeSVBDT  = new TH1F("fakeSVBDT"," BDT for fake SV rejection", 100, -1., 1.);
       std::string histDir;
       histDir="/file1/stat/MultiSVrt"+m_instanceName+"/";
       sc = hist_root->regHist(histDir+"massPiPi", m_hb_massPiPi);
       sc = hist_root->regHist(histDir+"massPiPi1", m_hb_massPiPi1);
       sc = hist_root->regHist(histDir+"massPPi", m_hb_massPPi);
       sc = hist_root->regHist(histDir+"massEE", m_hb_massEE );
       sc = hist_root->regHist(histDir+"nvrt2", m_hb_nvrt2);
       sc = hist_root->regHist(histDir+"ratio", m_hb_ratio);
       sc = hist_root->regHist(histDir+"totmass", m_hb_totmass);
       sc = hist_root->regHist(histDir+"impact",    m_hb_impact);
       sc = hist_root->regHist(histDir+"impactR",   m_hb_impactR);
       sc = hist_root->regHist(histDir+"impactZ",   m_hb_impactZ);
       sc = hist_root->regHist(histDir+"impactRZ",  m_hb_impactRZ);
       sc = hist_root->regHist(histDir+"pileupRatio", m_hb_pileupRat);
       sc = hist_root->regHist(histDir+"trkD0",     m_hb_trkD0);
       sc = hist_root->regHist(histDir+"trkZ",      m_hb_trkZ);
       sc = hist_root->regHist(histDir+"r2interact",m_hb_r2d);
       sc = hist_root->regHist(histDir+"NTrkSel",   m_hb_ntrksel);
       sc = hist_root->regHist(histDir+"NTrkInput", m_hb_ntrkInput);
       sc = hist_root->regHist(histDir+"TrkSelect", m_hb_trkSelect);
       sc = hist_root->regHist(histDir+"signif3D",  m_hb_signif3D);
       sc = hist_root->regHist(histDir+"sig3dcommon", m_hb_sig3DTot);
       sc = hist_root->regHist(histDir+"sig3D1tr",  m_hb_sig3D1tr);
       sc = hist_root->regHist(histDir+"sig3D2tr",  m_hb_sig3D2tr);
       sc = hist_root->regHist(histDir+"sig3DNtr",  m_hb_sig3DNtr);
       sc = hist_root->regHist(histDir+"goodvrtN",  m_hb_goodvrtN);
       sc = hist_root->regHist(histDir+"goodvrt1N", m_hb_goodvrt1N);
       sc = hist_root->regHist(histDir+"distVV",    m_hb_distVV);
       sc = hist_root->regHist(histDir+"diffPS",    m_hb_diffPS);
       sc = hist_root->regHist(histDir+"rawVrtN",   m_hb_rawVrtN);
       sc = hist_root->regHist(histDir+"cosSVMom",  m_hb_cosSVMom);
       sc = hist_root->regHist(histDir+"etaSV",     m_hb_etaSV);
       sc = hist_root->regHist(histDir+"fakeSVBDT", m_hb_fakeSVBDT);
       if( sc.isFailure() ) ATH_MSG_INFO( "NewVrtSecInclusive Histogram registration failure!!!");
       m_w_1 = 1.;
       m_curTup=new DevTuple();
       m_tuple = new TTree("Vertices","Vertices");
       std::string TreeDir;
       TreeDir="/file1/stat/MultiSVrt"+m_instanceName+"/";
       sc = hist_root->regTree(TreeDir,m_tuple);
       if (sc.isSuccess()) {
          m_tuple->Branch("ntrk",       &m_curTup->nTrk,    "ntrk/I");
          m_tuple->Branch("pttrk",      &m_curTup->pttrk,   "pttrk[ntrk]/F");
          m_tuple->Branch("Sig3D",      &m_curTup->Sig3D,   "Sig3D[ntrk]/F");
          m_tuple->Branch("n2Vrt",      &m_curTup->n2Vrt,      "n2Vrt/I");
          m_tuple->Branch("VrtTrkHF",   &m_curTup->VrtTrkHF,   "VrtTrkHF[n2Vrt]/I");
          m_tuple->Branch("VrtTrkI",    &m_curTup->VrtTrkI,    "VrtTrkI[n2Vrt]/I");
          m_tuple->Branch("VrtCh",      &m_curTup->VrtCh,      "VrtCh[n2Vrt]/I");
          m_tuple->Branch("VrtDist2D",  &m_curTup->VrtDist2D,  "VrtDist2D[n2Vrt]/F");
          m_tuple->Branch("VrtSig3D",   &m_curTup->VrtSig3D,   "VrtSig3D[n2Vrt]/F");
          m_tuple->Branch("VrtSig2D",   &m_curTup->VrtSig2D,   "VrtSig2D[n2Vrt]/F");
          m_tuple->Branch("VrtM",       &m_curTup->VrtM,       "VrtM[n2Vrt]/F");
          m_tuple->Branch("VrtZ",       &m_curTup->VrtZ,       "VrtZ[n2Vrt]/F");
          m_tuple->Branch("VrtPt",      &m_curTup->VrtPt,      "VrtPt[n2Vrt]/F");
          m_tuple->Branch("VrtEta",     &m_curTup->VrtEta,     "VrtEta[n2Vrt]/F");
          m_tuple->Branch("VrtIBL",     &m_curTup->VrtIBL,     "VrtIBL[n2Vrt]/I");
          m_tuple->Branch("VrtBL",      &m_curTup->VrtBL,      "VrtBL[n2Vrt]/I");
          m_tuple->Branch("VrtSinSPM",  &m_curTup->VrtSinSPM,  "VrtSinSPM[n2Vrt]/F");
          m_tuple->Branch("VMinPtT",    &m_curTup->VMinPtT,    "VMinPtT[n2Vrt]/F");
          m_tuple->Branch("VMinS3DT",   &m_curTup->VMinS3DT,   "VMinS3DT[n2Vrt]/F");
          m_tuple->Branch("VMaxS3DT",   &m_curTup->VMaxS3DT,   "VMaxS3DT[n2Vrt]/F");
          m_tuple->Branch("VrtProb",    &m_curTup->VrtProb,    "VrtProb[n2Vrt]/F");
          m_tuple->Branch("VrtHR1",     &m_curTup->VrtHR1,     "VrtHR1[n2Vrt]/F");
          m_tuple->Branch("VrtHR2",     &m_curTup->VrtHR2,     "VrtHR2[n2Vrt]/F");
          m_tuple->Branch("VrtBDT",     &m_curTup->VrtBDT,     "VrtBDT[n2Vrt]/F");
          m_tuple->Branch("VrtDisk",    &m_curTup->VrtDisk,    "VrtDisk[n2Vrt]/I");
          m_tuple->Branch("VSigMat",    &m_curTup->VSigMat,    "VSigMat[n2Vrt]/F");
          m_tuple->Branch("nNVrt",       &m_curTup->nNVrt,       "nNVrt/I");
          m_tuple->Branch("NVrtTrk",     &m_curTup->NVrtTrk,     "NVrtTrk[nNVrt]/I");
          m_tuple->Branch("NVrtTrkHF",   &m_curTup->NVrtTrkHF,   "NVrtTrkHF[nNVrt]/I");
          m_tuple->Branch("NVrtTrkI",    &m_curTup->NVrtTrkI,    "NVrtTrkI[nNVrt]/I");
          m_tuple->Branch("NVrtCh",      &m_curTup->NVrtCh,      "NVrtCh[nNVrt]/I");
          m_tuple->Branch("NVrtDist2D",  &m_curTup->NVrtDist2D,  "NVrtDist2D[nNVrt]/F");
          m_tuple->Branch("NVrtSig3D",   &m_curTup->NVrtSig3D,   "NVrtSig3D[nNVrt]/F");
          m_tuple->Branch("NVrtSig2D",   &m_curTup->NVrtSig2D,   "NVrtSig2D[nNVrt]/F");
          m_tuple->Branch("NVrtM",       &m_curTup->NVrtM,       "NVrtM[nNVrt]/F");
          m_tuple->Branch("NVrtPt",      &m_curTup->NVrtPt,      "NVrtPt[nNVrt]/F");
          m_tuple->Branch("NVrtEta",     &m_curTup->NVrtEta,     "NVrtEta[nNVrt]/F");
          m_tuple->Branch("NVrtIBL",     &m_curTup->NVrtIBL,     "NVrtIBL[nNVrt]/I");
          m_tuple->Branch("NVrtBL",      &m_curTup->NVrtBL,      "NVrtBL[nNVrt]/I");
          m_tuple->Branch("NVrtSinSPM",  &m_curTup->NVrtSinSPM,  "NVrtSinSPM[nNVrt]/F");
          m_tuple->Branch("NVMinPtT",    &m_curTup->NVMinPtT,    "NVMinPtT[nNVrt]/F");
          m_tuple->Branch("NVMinS3DT",   &m_curTup->NVMinS3DT,   "NVMinS3DT[nNVrt]/F");
          m_tuple->Branch("NVrtProb",    &m_curTup->NVrtProb,    "NVrtProb[nNVrt]/F");
          m_tuple->Branch("NVrtBDT",     &m_curTup->NVrtBDT,     "NVrtBDT[nNVrt]/F");
       }
     }
     std::string rootFilePath = PathResolver::find_calib_file("NewVrtSecInclusiveTool/"+m_calibFileName);
     TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");    
     if (!rootFile) {
        ATH_MSG_FATAL("Could not retrieve root file: " << m_calibFileName);
        return StatusCode::FAILURE;
     }
     TTree * training = (TTree*)rootFile->Get("BDT");
     m_SV2T_BDT = std::make_unique<MVAUtils::BDT>(training);
     return StatusCode::SUCCESS;

   }

  StatusCode MultiSecVertexTool::finalize()
  {
    if(m_compatibilityGraph)delete m_compatibilityGraph;
    ATH_MSG_DEBUG("MultiSecVertexTool finalize()");
    return StatusCode::SUCCESS; 
  }


  std::unique_ptr<Trk::VxSecVertexInfo> MultiSecVertexTool::findAllVertices (
           const std::vector<const xAOD::TrackParticle*> & inpTrk,
           const xAOD::Vertex & primVrt ) const 
  {
    std::vector<xAOD::Vertex*> listVrtSec(0);

    if(m_fillHist && m_curTup){  
      m_curTup->nTrk=0;
      m_curTup->n2Vrt=0;
      m_curTup->nNVrt=0;
    };

    workVectorArrxAOD * tmpVectxAOD=new workVectorArrxAOD();
    tmpVectxAOD->inpTrk.resize(inpTrk.size());
    std::copy(inpTrk.begin(),inpTrk.end(), tmpVectxAOD->inpTrk.begin());
    tmpVectxAOD->beamX=m_beamService->beamPos().x();
    tmpVectxAOD->beamY=m_beamService->beamPos().y();
    tmpVectxAOD->beamZ=m_beamService->beamPos().z();
    tmpVectxAOD->tanBeamTiltX=tan(m_beamService->beamTilt(0));
    tmpVectxAOD->tanBeamTiltY=tan(m_beamService->beamTilt(1));

    listVrtSec = getVrtSecMulti(tmpVectxAOD,primVrt);
    delete tmpVectxAOD;

    std::vector<const xAOD::IParticle*>  iparTrkV0(0); 
    std::unique_ptr<Trk::VxSecVertexInfo> res = std::make_unique<Trk::VxSecVertexInfo>(Trk::VxSecVertexInfo(listVrtSec));
    if(m_fillHist && m_tuple){  m_tuple->Fill(); };
    m_compatibilityGraph->clear();
    return res;
 }


   std::vector<xAOD::Vertex*> MultiSecVertexTool::getVrtSecMulti(  workVectorArrxAOD * xAODwrk,
                                                                      const xAOD::Vertex & PrimVrt )
   const
   {

      const double probVrtMergeLimit=0.01;

      int inpNPart=0; 
      int i,j;
      inpNPart=xAODwrk->inpTrk.size();
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      ATH_MSG_DEBUG( "getVrtSecMulti() called with NPart=" << inpNPart);
   
      std::vector<xAOD::Vertex*>  finalVertices(0); 

      if( inpNPart < 2 ) { return finalVertices;}   // 0,1 track => nothing to do!
   
   
      long int NTracks = 0;
      selGoodTrkParticle( xAODwrk, PrimVrt);
      NTracks = xAODwrk->listSelTracks.size();

      if( NTracks < 2 ) { return finalVertices;} // 0,1 selected track => nothing to do!

      ATH_MSG_DEBUG( "Number of selected tracks = " <<NTracks);
      
      if(m_fillHist){ m_hb_ntrksel->Fill( (double) NTracks, m_w_1); }

//
//  inpTrk[]           - input track list
//  listSelTracks[]    - list of good tracks in jet for vertex search
//------------------------------------------------------------	 
//                     Initial track list ready
//                     Find 2track vertices
//

      select2TrVrt(xAODwrk->listSelTracks, PrimVrt);

//---
      ATH_MSG_DEBUG(" Defined edges in the graph="<< num_edges(*m_compatibilityGraph));

//
//  m_Incomp[]           -  main vector of pointers for multivertex search
//-----------------------------------------------------------------------------------------------------
//            Secondary track list is ready
//            Creation of initial vertex set
//


      std::vector<WrkVrt> *WrkVrtSet= new std::vector<WrkVrt>;
      WrkVrt newvrt; newvrt.Good=true;
      StatusCode sc; sc.setChecked(); 
      long int NPTR=0, nth=2; // VK nth=2 to speed up PGRAPH when it's used


      std::vector< std::vector<int> > allCliques;
      bron_kerbosch_all_cliques(*m_compatibilityGraph, Clique_visitor(allCliques));
      for(int cq=0; cq<(int)allCliques.size();cq++){
          newvrt.selTrk.clear();
          NPTR=allCliques[cq].size();
          for(i=0;i<NPTR;i++){ newvrt.selTrk.push_back(allCliques[cq][i]);}
//==================================================
          xAODwrk->tmpListTracks.clear();
	  TLorentzVector vpsum;
          for(i=0;i<NPTR;i++) {
             xAODwrk->tmpListTracks.push_back( xAODwrk->listSelTracks.at(newvrt.selTrk[i]) );
	     vpsum += xAODwrk->listSelTracks.at(newvrt.selTrk[i])->p4();
          }
          sc = m_fitSvc->VKalVrtFitFast(xAODwrk->tmpListTracks,newvrt.vertex);            /* Fast crude estimation*/

          if( sc.isFailure()  ) {   /* No initial estimation */ 
              m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); /*Use as starting point*/
 	  } else {
	      Amg::Vector3D vDist=newvrt.vertex-PrimVrt.position();
              double MomVrtDir = vpsum.Px()*vDist.x() + vpsum.Py()*vDist.y() + vpsum.Pz()*vDist.z();
              if( MomVrtDir>0. ) {                           /* Good initial estimation */ 
                  m_fitSvc->setApproximateVertex(newvrt.vertex.x(),newvrt.vertex.y(),newvrt.vertex.z()); /*Use as starting point*/
	      }else{
                  m_fitSvc->setApproximateVertex(PrimVrt.x(), PrimVrt.y(), PrimVrt.z()); 
              }
          }
          ATH_MSG_VERBOSE("Found IniVertex="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]);
	  sc = StatusCode::FAILURE;
          m_fitSvc->setMomCovCalc(0); // No full covariance to save CPU
          sc=m_fitSvc->VKalVrtFit(xAODwrk->tmpListTracks, neutralPartDummy,
	                                     newvrt.vertex,     newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                             newvrt.chi2PerTrk, newvrt.trkAtVrt,  newvrt.chi2);

          ATH_MSG_VERBOSE("Found IniVertex="<<newvrt.vertex[0]<<", "<<newvrt.vertex[1]<<", "<<newvrt.vertex[2]);
          ATH_MSG_VERBOSE("with Chi2="<<newvrt.chi2<<" Ntrk="<<NPTR<<" trk1,2="<<newvrt.selTrk[0]<<", "<<newvrt.selTrk[1]); 
          if( sc.isFailure() )           continue;   /* Bad fit - goto next solution */
          if(NPTR==2 && newvrt.chi2>10.) continue;   /* Bad 2track vertex */
          if(newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
          newvrt.Good         = true;
          newvrt.nCloseVrt    = 0;
          newvrt.dCloseVrt    = 1000000.;
          newvrt.projectedVrt=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom); //3D SV-PV distance
          WrkVrtSet->push_back(newvrt);
    } 
    bool disassembled=false;
    int NSoluI=0;
    do{ 
      disassembled=false;
      NSoluI=(*WrkVrtSet).size();
      for(int iv=0; iv<NSoluI; iv++){
        if(!(*WrkVrtSet)[iv].Good || (*WrkVrtSet)[iv].selTrk.size() == 2 ) continue;
        if( TMath::Prob( (*WrkVrtSet)[iv].chi2, 2*(*WrkVrtSet)[iv].selTrk.size()-3) <1.e-3){
          if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"BigChi2Vertex present");
          DisassembleVertex(WrkVrtSet,iv,xAODwrk->listSelTracks);
          disassembled=true;
      } }
    }while(disassembled);
//-Modify too heavy vertices 
    for(auto & iv : (*WrkVrtSet)){
      if( iv.vertexMom.M()>m_VrtMassLimit ) {
        int it_bad=mostHeavyTrk(iv,xAODwrk->listSelTracks);
        if(it_bad>-1){
	  iv.selTrk.erase( iv.selTrk.begin() + it_bad );
	  refitVertex(iv, xAODwrk->listSelTracks, false);
     } } }
//-Remove vertices fully contained in other vertices 
    while( (*WrkVrtSet).size()>1 ){
      int tmpN=(*WrkVrtSet).size();  int iv=0;
      for(; iv<tmpN-1; iv++){        int jv=iv+1;
        if(!(*WrkVrtSet)[iv].Good )               continue;
        for(; jv<tmpN; jv++){
          if(!(*WrkVrtSet)[jv].Good )             continue;
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);
          if(      nTCom==(int)(*WrkVrtSet)[iv].selTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+iv); break; }
          else if( nTCom==(int)(*WrkVrtSet)[jv].selTrk.size()){  (*WrkVrtSet).erase((*WrkVrtSet).begin()+jv); break; }
        }   if(jv!=tmpN)   break;  // One vertex is erased. Restart check
      }     if(iv==tmpN-1) break;  // No vertex deleted
    }
//
//- Try to merge all vertices with common tracks
    std::multimap<int,std::pair<int,int>> vrtWithCommonTrk;
    std::multimap<int,std::pair<int,int>>::reverse_iterator icvrt;
    do{
      NSoluI=(*WrkVrtSet).size();
      vrtWithCommonTrk.clear();
      for(int iv=0; iv<NSoluI-1; iv++ ){  for(int jv=iv+1; jv<NSoluI; jv++){
          if(!(*WrkVrtSet)[iv].Good || !(*WrkVrtSet)[jv].Good)    continue;
          int nTCom=nTrkCommon( WrkVrtSet, iv, jv);     if(!nTCom)continue;
          vrtWithCommonTrk.emplace(nTCom,std::make_pair(iv,jv));
      } }
      if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"Initial Vertices");
      for(icvrt=vrtWithCommonTrk.rbegin(); icvrt!=vrtWithCommonTrk.rend(); icvrt++){ 
          int nTCom=(*icvrt).first;
          int iv=(*icvrt).second.first;
          int jv=(*icvrt).second.second;
          int nTrkI=(*WrkVrtSet)[iv].selTrk.size();
          int nTrkJ=(*WrkVrtSet)[jv].selTrk.size();
	  double probV=-1.;
          probV=mergeAndRefitVertices( WrkVrtSet, iv, jv, newvrt, xAODwrk->listSelTracks);
          if(newvrt.vertexMom.M()>m_VrtMassLimit) continue; //Do not merge vertices if summary one is too heavy
	  if(probV<probVrtMergeLimit){
            if(nTrkI==2 || nTrkJ==2 || nTCom<2) continue;
            if((nTCom>nTrkI-nTCom || nTCom>nTrkJ-nTCom)){  //2 and more common tracks for NTr>=3 vertices. Merge anyway.
              mergeAndRefitOverlapVertices( WrkVrtSet, iv, jv, xAODwrk->listSelTracks);
              break; //Vertex list is changed. Restart merging from scratch.
            }
            continue;  //Continue merging loop 
          }
          newvrt.Good = true;
          (*WrkVrtSet).push_back(newvrt);
	  (*WrkVrtSet)[iv].Good=false;      
	  (*WrkVrtSet)[jv].Good=false;
          break;  //Merging successful. Break merging loop and remake list of connected vertices
       }
    } while( icvrt != vrtWithCommonTrk.rend() );
    if(m_fillHist){ int cvgood=0; for(int iv=0; iv<(int)(*WrkVrtSet).size(); iv++) if((*WrkVrtSet)[iv].Good)cvgood++;
                    m_hb_rawVrtN->Fill( (float)cvgood, m_w_1); }
//
//-Remove all bad vertices from the working set    
    int tmpV=0; while( tmpV<(int)(*WrkVrtSet).size() )if( !(*WrkVrtSet)[tmpV].Good ) { (*WrkVrtSet).erase((*WrkVrtSet).begin()+tmpV);} else {tmpV++;}
    if(WrkVrtSet->empty()){             // No vertices at all
      delete WrkVrtSet;
      return finalVertices;
    }
    //------
    if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"Intermediate Vertices");
    //------
    for( auto &tmpV : (*WrkVrtSet) ) tmpV.projectedVrt=MomProjDist(tmpV.vertex, PrimVrt, tmpV.vertexMom );  //Setup ProjectedVrt
//----------------------------------------------------------------------------
//             Here we have the overlapping solutions.
//             Vertices may have only 1 common track. 
//              Now solution cleaning

    int nGoodVertices=0;         // Final number of good vertices
    int n2trVrt=0;               // N of vertices with 2  tracks
    int nNtrVrt=0;               // N vertices with 3 and more tracks
    std::vector< std::deque<long int> > *TrkInVrt  =new std::vector< std::deque<long int> >(NTracks);  
    double foundMaxT; long int SelectedTrack, SelectedVertex;
    int foundV1, foundV2;
    
    trackClassification( WrkVrtSet, TrkInVrt);

    while((foundMaxT=maxOfShared( WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex))>0) {
         if(msgLvl(MSG::VERBOSE)) { 
           printWrkSet(WrkVrtSet,"Iteration");
	   ATH_MSG_VERBOSE("MAX="<<foundMaxT<<", "<<SelectedTrack<<", "<<SelectedVertex<<
	                   "VRT="<<minVrtVrtDist( WrkVrtSet, foundV1, foundV2)<<", "<<foundV1<<", "<<foundV2);
         }
         double foundMinVrtDst = 1000000.;
         if(foundMaxT<m_trackDetachCut) foundMinVrtDst = minVrtVrtDist( WrkVrtSet, foundV1, foundV2);

//Choice of action
          if( foundMaxT<m_trackDetachCut && foundMinVrtDst<m_vertexMergeCut && nTrkCommon( WrkVrtSet, foundV1, foundV2)){
          //if( foundMaxT<m_trackDetachCut && foundMinVrtDst<m_vertexMergeCut){
             bool vrtMerged=false;   //to check whether something is really merged
             while(foundMinVrtDst<m_vertexMergeCut){
               if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;} /*Always drop vertex with smallest number*/
	       double probV=0.;
               probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listSelTracks);
	       if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<m_VrtMassLimit){        //  Good merged vertex found
                 double tstDst=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom);
	         if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
                   std::swap((*WrkVrtSet)[foundV1],newvrt);
                   (*WrkVrtSet)[foundV1].projectedVrt=tstDst;
	           (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
                   (*WrkVrtSet)[foundV2].selTrk.clear();     //Clean dropped vertex
	           vrtMerged=true;
	         }
               }
               (*WrkVrtSet)[foundV1].nCloseVrt=-1; (*WrkVrtSet)[foundV1].dCloseVrt=1000000.; //For minVrtVrtDistNext optimisation(!)
               (*WrkVrtSet)[foundV2].nCloseVrt=-1; (*WrkVrtSet)[foundV2].dCloseVrt=1000000.; //Exclude given pair
                foundMinVrtDst=minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2);     //Check other vertices
             }
	     if(vrtMerged){
               delete TrkInVrt;
               TrkInVrt = new std::vector< std::deque<long int> >(NTracks);  
               trackClassification( WrkVrtSet, TrkInVrt);
	       continue;  // Something was merged => goto next cycle. Otherwise break the found track-vertex link
	     }
	  }
          removeTrackFromVertex(WrkVrtSet, TrkInVrt, SelectedTrack, SelectedVertex);
          sc = refitVertex( WrkVrtSet, SelectedVertex, xAODwrk->listSelTracks, false);
          if( sc.isFailure() ) { (*WrkVrtSet)[SelectedVertex].Good=false;  continue; } //bad vertex
          (*WrkVrtSet)[SelectedVertex].projectedVrt=MomProjDist((*WrkVrtSet)[SelectedVertex].vertex, PrimVrt, (*WrkVrtSet)[SelectedVertex].vertexMom);
          if( (*WrkVrtSet)[SelectedVertex].projectedVrt<0. && (*WrkVrtSet)[SelectedVertex].selTrk.size()==2 )
	      (*WrkVrtSet)[SelectedVertex].Good=false;  // 2track vertex migrates behind PV - drop it.
    }
//
// Final check/merge for close vertices
//
    double minDistVV=minVrtVrtDist( WrkVrtSet, foundV1, foundV2); //recalculate VV distances
    while ( minDistVV < m_vertexMergeCut) {
        if(foundV1<foundV2) { int tmp=foundV1; foundV1=foundV2; foundV2=tmp;}
	double probV=0.;
        probV=mergeAndRefitVertices( WrkVrtSet, foundV1, foundV2, newvrt, xAODwrk->listSelTracks);
	if(probV>probVrtMergeLimit && newvrt.vertexMom.M()<m_VrtMassLimit){        //  Good merged vertex found
           double tstDst=MomProjDist(newvrt.vertex, PrimVrt, newvrt.vertexMom);
	   if(tstDst>0.){                               // only positive vertex directions are accepted as merging result
              std::swap((*WrkVrtSet)[foundV1],newvrt);
              (*WrkVrtSet)[foundV1].projectedVrt=tstDst;
	      (*WrkVrtSet)[foundV2].Good=false;         //Drop vertex
              (*WrkVrtSet)[foundV2].selTrk.clear();     //Clean dropped vertex
           }
        }
        (*WrkVrtSet)[foundV1].nCloseVrt=-1; (*WrkVrtSet)[foundV1].dCloseVrt=1000000.; //For minVrtVrtDistNext optimisation(!)
        (*WrkVrtSet)[foundV2].nCloseVrt=-1; (*WrkVrtSet)[foundV2].dCloseVrt=1000000.; //Exclude given pair
        minDistVV=minVrtVrtDistNext( WrkVrtSet, foundV1, foundV2);
    }
//
// Try to improve vertices with big Chi2 if something went wrong. Just precaution.
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
       if(!(*WrkVrtSet)[iv].Good )                 continue;  //don't work on vertex which is already bad
       if( (*WrkVrtSet)[iv].selTrk.size()<3 )      continue;
       double tmpProb=TMath::Prob( (*WrkVrtSet)[iv].chi2, 2*(*WrkVrtSet)[iv].selTrk.size()-3 ); //Chi2 of the original vertex
       if(tmpProb<m_globVrtProbCut){
         tmpProb=improveVertexChi2( WrkVrtSet, iv, xAODwrk->listSelTracks);
         if(tmpProb<m_globVrtProbCut)(*WrkVrtSet)[iv].Good=false;
         (*WrkVrtSet)[iv].projectedVrt=MomProjDist((*WrkVrtSet)[iv].vertex, PrimVrt, (*WrkVrtSet)[iv].vertexMom);
       }
    }
    if(msgLvl(MSG::DEBUG))printWrkSet(WrkVrtSet,"Joined vertices");
//--------------------------------------------------------------------------------------------------------
// Final vertex selection/cleaning
//
    double signif3D=0., signif2D=0., vProb=0.;
    //double Dist3D=0;

//--------- Start with 1-track vertices
//=First check if the track was detached from a multitrack vertex. If so - reattach. 
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good || ntrVrt.selTrk.size()<=1)      continue;
      for(auto &onetVrt : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.selTrk.size()!=1) continue;
        if(ntrVrt.detachedTrack==onetVrt.selTrk[0]){   
	  WrkVrt newV(ntrVrt); newV.selTrk.push_back(ntrVrt.detachedTrack);
          vProb = refitVertex( newV, xAODwrk->listSelTracks, false);
	  if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ onetVrt.Good=false; ntrVrt=newV;  ntrVrt.detachedTrack=-1;}
	  break;
    } } }
//=Recheck if the track was detached from a 2track vertex. If so - reattach. 
    for(auto &iVrt : (*WrkVrtSet)){   if(!iVrt.Good || iVrt.selTrk.size()!=1) continue;
      for(auto &jVrt : (*WrkVrtSet)){ if(!jVrt.Good || jVrt.selTrk.size()!=1) continue;
        if(iVrt.detachedTrack==jVrt.selTrk[0]){   
	  WrkVrt newV(iVrt); newV.selTrk.push_back(iVrt.detachedTrack);
          vProb = refitVertex( newV, xAODwrk->listSelTracks, false);
	  if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ jVrt.Good=false; iVrt=newV;  iVrt.detachedTrack=-1;}
	  break;
    } } }
    for(auto &ntrVrt : (*WrkVrtSet)){ if(!ntrVrt.Good  || ntrVrt.selTrk.size()<=1)  continue;
      for(auto tr : ntrVrt.selTrk){ 
        for(auto &onetVrt  : (*WrkVrtSet)){ if(!onetVrt.Good || onetVrt.selTrk.size()!=1) continue;
	  if(onetVrt.detachedTrack==tr){
	    WrkVrt newV(ntrVrt); newV.selTrk.push_back(onetVrt.selTrk[0]);
            vProb = refitVertex( newV, xAODwrk->listSelTracks, false);
	    if( vProb>probVrtMergeLimit && newV.vertexMom.M()<m_VrtMassLimit ){ onetVrt.Good=false; ntrVrt=newV;}
    } } } }
    for(auto & curVrt : (*WrkVrtSet) ) {
       if(!curVrt.Good )                 continue;  //don't work on vertex which is already bad
       if(curVrt.selTrk.size() != 1)     continue;
       curVrt.Good=false;       // Make them bad by default
       if(m_multiWithOneTrkVrt){          /* 1track vertices left unassigned from good 2tr vertices */
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); //VK non-projected signif3D is worse
          double tmpProb=TMath::Prob( curVrt.chi2, 1);                 //Chi2 of the original 2tr vertex
          if(tmpProb>0.01){  /* accept only good tracks coming from good 2tr vertex*/
             std::vector<double> impact,impactError;   double signif3DP = 0;
             signif3DP=m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[curVrt.selTrk[0]],PrimVrt.position(), 1, impact, impactError);
             if(m_fillHist){
               m_hb_diffPS->Fill( signif3DP, m_w_1); 
               m_hb_sig3D1tr->Fill( signif3D, m_w_1);
             }
             if( signif3DP>2.*m_trkSigCut && signif3D>m_selVrtSigCut) curVrt.Good=true; // accept only tracks which are far from PV
          }
       }
    }
//-----  Vertices with >1 tracks
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*WrkVrtSet)[iv];
          if(!curVrt.Good )                      continue;  //don't work on vertex which is already bad
          nth=(*WrkVrtSet)[iv].selTrk.size(); if(nth == 1) continue;  // 1track vertices are treated already
          VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
          xAODwrk->tmpListTracks.resize(nth);
          for(i=0;i<nth;i++) {
            xAODwrk->tmpListTracks[i]=xAODwrk->listSelTracks[curVrt.selTrk[i]];
          }
          (*WrkVrtSet)[iv].Good = false;                                     /* Make all vertices bad for futher selection */
          if(nth <= 1)                          continue;                    /* Definitely bad vertices */
          if((*WrkVrtSet)[iv].projectedVrt<0.)  continue;                    /* Remove vertices behind primary one */ 
          if( TMath::Prob( curVrt.chi2, 2*nth-3)<m_globVrtProbCut) continue;           /* Bad Chi2 of refitted vertex  */
          if(m_fillHist){ 
             if(nth==2 && curVrt.vertexCharge==0) m_hb_massPiPi1->Fill(curVrt.vertexMom.M(), m_w_1);
 	     m_hb_sig3DTot->Fill( signif3D, m_w_1);
             if(nth==2)m_hb_sig3D2tr->Fill( signif3D, m_w_1);
             if(nth >2)m_hb_sig3DNtr->Fill( signif3D, m_w_1);
          }
          if(signif3D<m_selVrtSigCut+1.)              continue;      //Main PV-SV distance quality cut 
          if(curVrt.vertex.perp() > m_maxSVRadiusCut) continue;      // Too far from interaction point
          curVrt.Good = true;  /* Vertex is absolutely good */
    }
    std::vector<double> impact,impactError;
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
          WrkVrt & curVrt=(*WrkVrtSet)[iv];
          nth=curVrt.selTrk.size();
          if(!curVrt.Good )      continue;  //don't work on vertex which is already bad
          double minPtT=1.e6, minSig3DT=1.e6, maxSig3DT=0.;
          int ntrkBC=0,ntrkI=0,sumIBLHits=0,sumBLHits=0;
          for(i=0;i<nth;i++) {
             j=curVrt.selTrk[i];                           /*Track number*/
             minPtT=std::min( minPtT, xAODwrk->listSelTracks[j]->pt());
             m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[j], PrimVrt.position(), 1, impact, impactError);
	     double SigR2 = impact[0]*impact[0]/impactError[0];
	     double SigZ2 = impact[1]*impact[1]/impactError[2];
             minSig3DT=std::min( minSig3DT, std::sqrt( SigR2 + SigZ2) );
             maxSig3DT=std::max( maxSig3DT, std::sqrt( SigR2 + SigZ2) );
	     sumIBLHits += std::max(getIBLHit(xAODwrk->listSelTracks[j]),0);
	     sumBLHits  += std::max(getBLHit(xAODwrk->listSelTracks[j]),0);
             if(m_fillHist && m_curTup) {
	        ntrkBC += getIdHF(xAODwrk->listSelTracks[j]);
	        ntrkI  += getG4Inter(xAODwrk->listSelTracks[j]);
             }
          }
	  float vProb=TMath::Prob(curVrt.chi2, 2*nth-3);
          float cosSVPVM=ProjSV_PV(curVrt.vertex, PrimVrt, curVrt.vertexMom);
	  float vrtR=curVrt.vertex.perp();
	  TLorentzVector SVPV(curVrt.vertex.x()-PrimVrt.x(),curVrt.vertex.y()-PrimVrt.y(),curVrt.vertex.z()-PrimVrt.z(), 10.);
          if(m_fillHist){
             if( m_curTup && nth>1 ){
                VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
                float Dist2D=VrtVrtDist2D(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif2D); 
                m_curTup->NVrtTrk   [m_curTup->nNVrt] = nth;          
                m_curTup->NVrtTrkHF [m_curTup->nNVrt] = ntrkBC;          
                m_curTup->NVrtTrkI  [m_curTup->nNVrt] = ntrkI;          
                m_curTup->NVrtProb  [m_curTup->nNVrt] = vProb;          
                m_curTup->NVrtSig3D [m_curTup->nNVrt] = signif3D;
                m_curTup->NVrtSig2D [m_curTup->nNVrt] = signif2D;
                m_curTup->NVrtDist2D[m_curTup->nNVrt] = vrtR<20. ? Dist2D : vrtR;
                m_curTup->NVrtM     [m_curTup->nNVrt] = curVrt.vertexMom.M();
                m_curTup->NVrtPt    [m_curTup->nNVrt] = curVrt.vertexMom.Pt();
                m_curTup->NVrtEta   [m_curTup->nNVrt] = SVPV.Eta();
                m_curTup->NVrtIBL   [m_curTup->nNVrt] = sumIBLHits;
                m_curTup->NVrtBL    [m_curTup->nNVrt] = sumBLHits;
                m_curTup->NVrtSinSPM[m_curTup->nNVrt] = std::sqrt(1.-cosSVPVM*cosSVPVM);
                m_curTup->NVrtCh    [m_curTup->nNVrt] = curVrt.vertexCharge;
                m_curTup->NVMinPtT  [m_curTup->nNVrt] = minPtT;
                m_curTup->NVMinS3DT [m_curTup->nNVrt] = minSig3DT;
                m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
                if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
            }
          }
//-------------------BDT based rejection
          if(nth==2){
	     if(curVrt.vertexMom.Pt() > m_Vrt2TrPtLimit){ curVrt.Good = false; continue; }
             float rhit0=xAODwrk->listSelTracks[curVrt.selTrk[0]]->radiusOfFirstHit();
             float rhit1=xAODwrk->listSelTracks[curVrt.selTrk[1]]->radiusOfFirstHit();
	     VrtVrtDist(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif3D); 
	     float Dist2D=VrtVrtDist2D(PrimVrt,curVrt.vertex, curVrt.vertexCov, signif2D); 
	     std::vector<float> VARS(10);
	     VARS[0]=vProb;
	     VARS[1]=std::log(curVrt.vertexMom.Pt());
	     VARS[2]=std::log(std::max(minPtT,m_cutPt));
	     VARS[3]=std::log(vrtR<20. ? Dist2D : vrtR);
	     VARS[4]=std::log(std::max(minSig3DT,m_trkSigCut));
	     VARS[5]=std::log(maxSig3DT);
	     VARS[6]=curVrt.vertexMom.M();
	     VARS[7]=std::sqrt(std::abs(1.-cosSVPVM*cosSVPVM));
	     VARS[8]=SVPV.Eta();
	     VARS[9]=std::max(rhit0,rhit1);
	     float wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
	     if(m_fillHist){
	       m_hb_fakeSVBDT->Fill(wgtSelect,1.);
	       if( m_curTup ) m_curTup->NVrtBDT[m_curTup->nNVrt-1] = wgtSelect;
	     }
	     if(wgtSelect<m_v2tFinBDTCut) curVrt.Good = false;
	   }
    }
//
//--Final cleaning of the 1-track vertices set. Must be behind all other cleanings.
//-- Debug ntuple for 1track vertex is filled here as well
//
    if(m_multiWithOneTrkVrt) Clean1TrVertexSet(WrkVrtSet);
    if(m_fillHist && m_curTup && m_multiWithOneTrkVrt){
      for(auto & V : (*WrkVrtSet)) {
        nth=V.selTrk.size();
        if( !V.Good || nth != 1 ) continue;  // Good 1track vertices
        m_fitSvc->VKalGetImpact(xAODwrk->listSelTracks[V.selTrk[0]], PrimVrt.position(), 1, impact, impactError);
        int ntrkBC = getIdHF(xAODwrk->listSelTracks[V.selTrk[0]]);
	double SigR2 = impact[0]*impact[0]/impactError[0];
	double SigZ2 = impact[1]*impact[1]/impactError[2];
        float Dist2D=VrtVrtDist2D(PrimVrt,V.vertex, V.vertexCov, signif2D); 
        m_curTup->NVrtTrk   [m_curTup->nNVrt] = nth;          
        m_curTup->NVrtTrkHF [m_curTup->nNVrt] = ntrkBC;          
        m_curTup->NVrtProb  [m_curTup->nNVrt] = TMath::Prob(V.chi2, 1);        
        m_curTup->NVrtSig3D [m_curTup->nNVrt] = 0.;
        m_curTup->NVrtSig2D [m_curTup->nNVrt] = signif2D;
        m_curTup->NVrtDist2D[m_curTup->nNVrt] = Dist2D;
        m_curTup->NVrtM     [m_curTup->nNVrt] = V.vertexMom.M();
        m_curTup->NVrtPt    [m_curTup->nNVrt] = V.vertexMom.Pt();
        m_curTup->NVrtSinSPM[m_curTup->nNVrt] = 0.;
        m_curTup->NVrtCh    [m_curTup->nNVrt] = V.vertexCharge;
        m_curTup->NVMinPtT  [m_curTup->nNVrt] = xAODwrk->listSelTracks[V.selTrk[0]]->pt();
        m_curTup->NVMinS3DT [m_curTup->nNVrt] = std::sqrt(SigR2 + SigZ2);
        m_curTup->NVrtBDT   [m_curTup->nNVrt] = 1.1;
        if( m_curTup->nNVrt < DevTuple::maxNVrt-1 )m_curTup->nNVrt++;
   }  }
//-------------------------------------------
//Checks
    std::vector<WrkVrt> GoodVertices(0);
    nGoodVertices=0;         // Final number of good vertices
    n2trVrt=nNtrVrt=0;       // N of vertices with different amount of tracks
    for(auto & iv : (*WrkVrtSet) ) {
       nth=iv.selTrk.size(); if(nth == 0) continue;   /* Definitely bad vertices */
       if( iv.Good) {
	  nGoodVertices++;                                    
	  GoodVertices.emplace_back(iv);    /* add it */
	  if(nth==2)n2trVrt++;
	  if(nth>=3)nNtrVrt++;
       }
    }
    if(nGoodVertices == 0 || (n2trVrt+nNtrVrt)==0){  // No good vertices at all
      delete WrkVrtSet;  delete TrkInVrt;
      if(m_fillHist && m_curTup ) m_curTup->nNVrt=0;
      return finalVertices;
    }
//
//sorting
    while(1){ bool swapDone=false;                              // Sort on XY distance from (0.,0.)
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        if( GoodVertices[iv].vertex.perp() > GoodVertices[iv+1].vertex.perp()){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    while(1){ bool swapDone=false;                            // Then sort on Projected dist if R<20.
      for(int iv=0; iv<(int)GoodVertices.size()-1; iv++) {
        if(GoodVertices[iv+1].vertex.perp() > 400.) continue;
        if(GoodVertices[iv].projectedVrt > GoodVertices[iv+1].projectedVrt){
	  std::swap( GoodVertices[iv], GoodVertices[iv+1] );
          swapDone=true; 
        }
      }    if(!swapDone) break;
    }
    if(nGoodVertices>1){
      if( GoodVertices[1].vertexMom.M()-GoodVertices[0].vertexMom.M() > 5000.) std::swap( GoodVertices[0], GoodVertices[1] );
    }
    if(m_fillHist){m_hb_distVV->Fill( minVrtVrtDist( WrkVrtSet, foundV1, foundV2), m_w_1); }
//-------------------------------------------
// Saving and final cleaning
//
    nGoodVertices=0;         // Final number of good vertices
    int n1trVrt=0;           // Final number of good 1-track vertices
    TLorentzVector VertexMom;
    for(int iv=0; iv<(int)GoodVertices.size(); iv++) {
          nth=GoodVertices[iv].selTrk.size();
          xAODwrk->tmpListTracks.clear();
          for(i=0;i<nth;i++) {
             j=GoodVertices[iv].selTrk[i];                           /*Track number*/
             xAODwrk->tmpListTracks.push_back( xAODwrk->listSelTracks[j] );
          }
          if(m_fillHist){ 
	     m_hb_totmass->Fill(GoodVertices[iv].vertexMom.M(), m_w_1);
             m_hb_r2d->Fill( GoodVertices[iv].vertex.perp(), m_w_1); } 
// xAOD::Vertex creation-----------------------------
          xAOD::Vertex * tmpVertex=new xAOD::Vertex();
          tmpVertex->makePrivateStore();
          tmpVertex->setPosition(GoodVertices[iv].vertex);
          int NERMS=GoodVertices[iv].vertexCov.size();
	  NERMS=6;
          std::vector<float> floatErrMtx;   floatErrMtx.resize(NERMS);
          for(int i=0; i<NERMS; i++) floatErrMtx[i]=GoodVertices[iv].vertexCov[i];
          tmpVertex->setCovariance(floatErrMtx);
          tmpVertex->setFitQuality(GoodVertices[iv].chi2, (float)(nth*2-3));

          std::vector<Trk::VxTrackAtVertex> & tmpVTAV=tmpVertex->vxTrackAtVertex();    tmpVTAV.clear();
          for(int ii=0; ii<nth; ii++) {
             AmgSymMatrix(5) *CovMtxP=new AmgSymMatrix(5);    (*CovMtxP).setIdentity(); 
             Trk::Perigee * tmpMeasPer  =  new Trk::Perigee( 0.,0.,  GoodVertices[iv].trkAtVrt[ii][0], 
	                                                             GoodVertices[iv].trkAtVrt[ii][1],
                                                                     GoodVertices[iv].trkAtVrt[ii][2],
                                                                Trk::PerigeeSurface(GoodVertices[iv].vertex), CovMtxP );
             tmpVTAV.push_back( Trk::VxTrackAtVertex( 1., tmpMeasPer) );
	     if(xAODwrk){            //No way to store a link to Rec::TrackParticleContainer 
               ElementLink<xAOD::TrackParticleContainer> TEL;  TEL.setElement( xAODwrk->tmpListTracks[ii] );
               const xAOD::TrackParticleContainer* cont = (const xAOD::TrackParticleContainer* ) (xAODwrk->tmpListTracks[ii]->container() );
	       TEL.setStorableObject(*cont);
               tmpVertex->addTrackAtVertex(TEL,1.);
	     }
          }
          finalVertices.push_back(tmpVertex);
//-----
          nGoodVertices++;
          if(nth==1)n1trVrt++;
//-----
           VertexMom += GoodVertices[iv].vertexMom;
    }
    if(m_fillHist){
      m_hb_goodvrtN->Fill( nGoodVertices+0.1, m_w_1);
      m_hb_goodvrt1N->Fill( n1trVrt+0.1, m_w_1);
    }


    if(nGoodVertices == 0){
      delete WrkVrtSet;
      delete TrkInVrt;
      return finalVertices;
    }

//-----------------------------------------------------------------------------------
//  Saving of results
//
//


      delete WrkVrtSet; delete TrkInVrt;

      return finalVertices;


  }


    void MultiSecVertexTool::select2TrVrt(std::vector<const xAOD::TrackParticle*>  & selectedTracks,
                                  const xAOD::Vertex                 & PrimVrt)
    const
    {
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  tracksForFit(2,0);
      std::vector<double> impact,impactError;
      std::vector<double> inpMass(2,m_massPi);
      long int      Charge;
      int i,j;
      StatusCode sc; sc.setChecked();
      Vrt2Tr tmpVrt;
      std::vector<Vrt2Tr> all2TrVrt(0);
      TLorentzVector PSum2T; 
      Amg::Vector3D iniVrt(0.,0.,0.);
      int NTracks = (int) (selectedTracks.size());
      std::vector<float> covPV=PrimVrt.covariance(); 
      double signifR=0.,signifZ=0.;
      std::vector<int> hitIBL(NTracks,0), hitBL(NTracks,0);
      std::vector<double> trackSignif(NTracks),dRdZratio(NTracks);
      for (i=0; i<NTracks; i++) {
         m_fitSvc->VKalGetImpact(selectedTracks[i], PrimVrt.position(), 1, impact, impactError);
	 signifR = impact[0]/ std::sqrt(impactError[0]);
	 signifZ = impact[1]/ std::sqrt(impactError[2]);
  	 trackSignif[i] = std::sqrt( signifR*signifR + signifZ*signifZ);
	 dRdZratio[i] = std::abs(signifR/signifZ);
         if(m_fillHist){
	    m_hb_impactR->Fill( signifR, m_w_1); 
            m_hb_impactZ->Fill( signifZ, m_w_1); 
            m_hb_impactRZ->Fill(signifR, signifZ, m_w_1); 
	    m_hb_impact->Fill( trackSignif[i], m_w_1);
	    if(trackSignif[i]>2.) m_hb_pileupRat->Fill(dRdZratio[i],1.);
            if( i<DevTuple::maxNTrk && m_curTup){
	       m_curTup->pttrk[i]=selectedTracks[i]->pt();
	       m_curTup->Sig3D[i]=trackSignif[i];
	    }
	 }
      }

      if( m_fillHist && m_curTup ){
          m_curTup->nTrk=std::min(NTracks,DevTuple::maxNTrk);
          m_curTup->n2Vrt=0;
      }

      for (i=0; i<NTracks-1; i++) {
         if(trackSignif[i]<m_trkSigCut || dRdZratio[i]<m_dRdZRatioCut )continue;
         for (j=i+1; j<NTracks; j++) {
             if(trackSignif[j]<m_trkSigCut || dRdZratio[j]<m_dRdZRatioCut )continue;
             PSum2T=selectedTracks[i]->p4()+selectedTracks[j]->p4();
             if(PSum2T.M()>m_Vrt2TrMassLimit)continue;
             
             m_fitSvc->setDefault();
             m_fitSvc->setMomCovCalc(0); //No full covariance to save CPU
             m_fitSvc->setMassInputParticles(inpMass);     // Use pion masses for fit
             tracksForFit[0]=selectedTracks[i];
             tracksForFit[1]=selectedTracks[j];
             sc=m_fitSvc->VKalVrtFitFast(tracksForFit,tmpVrt.fitVertex);            /* Fast crude estimation*/

             if( sc.isFailure()  ) {   /* No initial estimation */ 
                iniVrt=PrimVrt.position();
 	     } else {
                double PMomVrtDir = ProjSV_PV(tmpVrt.fitVertex,PrimVrt,PSum2T);
                if( PMomVrtDir>0. ) iniVrt=tmpVrt.fitVertex;                /* Good initial estimation */ 
                else                iniVrt=PrimVrt.position();
             }
             m_fitSvc->setApproximateVertex(iniVrt.x(), iniVrt.y(), iniVrt.z());
             sc=m_fitSvc->VKalVrtFit(tracksForFit, neutralPartDummy, tmpVrt.fitVertex, tmpVrt.momentum, Charge,
                                  tmpVrt.errorMatrix, tmpVrt.chi2PerTrk, tmpVrt.trkAtVrt, tmpVrt.chi2);
             if(sc.isFailure())                       continue;          /* No fit */ 
             double Prob2v=TMath::Prob(tmpVrt.chi2,1);
             if( Prob2v < m_sel2VrtProbCut )    continue;
	     if( tmpVrt.momentum.M()> m_Vrt2TrMassLimit )      continue; 
  	     if( ProjSV_PV(tmpVrt.fitVertex, PrimVrt, tmpVrt.momentum) < 0.) continue;  // SV-PV don't coincide with summary momentum direction
             if( tmpVrt.fitVertex.perp() > m_maxSVRadiusCut) continue;                  // Too far from interaction point
             double cosSVPV=ProjSV_PV(tmpVrt.fitVertex, PrimVrt, tmpVrt.momentum);
	     TLorentzVector SVPV(tmpVrt.fitVertex.x()-PrimVrt.x(),
	                         tmpVrt.fitVertex.y()-PrimVrt.y(),
	                         tmpVrt.fitVertex.z()-PrimVrt.z(), 10.);
             if(m_fillHist){
               if(Charge==0){m_hb_massPiPi->Fill(tmpVrt.momentum.M(),1.);}
               m_hb_cosSVMom->Fill(cosSVPV,1.);
               m_hb_etaSV->Fill(SVPV.Eta(),1.);
             }
	     if(cosSVPV<0.8)continue;
	     if(tmpVrt.momentum.Pt()<1000.)continue;
	     float vrtR=tmpVrt.fitVertex.perp();
             double dstMatSignif=1.e4;
	     if(m_removeTrkMatSignif>0. && vrtR>20.){
		if(vrtR<30.){ dstMatSignif=std::abs(vrtR-m_beampipeR)/VrtRadiusError(tmpVrt.fitVertex,tmpVrt.errorMatrix );} //beampipe
		else        { dstMatSignif=distToMatLayerSignificance(tmpVrt);}     //Material in Pixel volume
		if(dstMatSignif<m_removeTrkMatSignif)continue;
	     }
             int ihitIBL  = getIBLHit(selectedTracks[i]);
	     int jhitIBL  = getIBLHit(selectedTracks[j]);
             if( ihitIBL<0 && jhitIBL<0)continue;
             float ihitR  = selectedTracks[i]->radiusOfFirstHit();
	     float jhitR  = selectedTracks[j]->radiusOfFirstHit();
             if(m_useVertexCleaning){
               if(std::abs(ihitR-jhitR)>15.) continue;
               if( std::min(ihitR,jhitR)-vrtR > 36.) continue; // Too big dR between vertex and hit in pixel
							       // Should be another layer in between 
               if( std::max(ihitR,jhitR)-vrtR <-10.) continue; // Vertex is behind hit in pixel 
             }
	     double minPtT = std::min(tracksForFit[0]->pt(),tracksForFit[1]->pt());
	     double Sig3D=0.,Sig2D=0., Dist2D=0.; 
             if( m_fillHist && m_curTup ){
                int ihitBL   = getBLHit (selectedTracks[i]);
	        int jhitBL   = getBLHit (selectedTracks[j]);
	        int idisk1=0,idisk2=0,idisk3=0,jdisk1=0,jdisk2=0,jdisk3=0;
                int sumIBLHits =  std::max(ihitIBL,0)+std::max(jhitIBL,0);
                int sumBLHits  =  std::max(ihitBL, 0)+std::max(jhitBL, 0);
                getPixelDiscs(selectedTracks[i],idisk1,idisk2,idisk3);
                getPixelDiscs(selectedTracks[j],jdisk1,jdisk2,jdisk3);
                VrtVrtDist(PrimVrt, tmpVrt.fitVertex, tmpVrt.errorMatrix, Sig3D);
                Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.fitVertex, tmpVrt.errorMatrix, Sig2D);
                m_hb_signif3D->Fill(Sig3D,1.);
		int curNV=m_curTup->n2Vrt;
                m_curTup->VrtTrkHF [curNV] = getIdHF(tracksForFit[0])+ getIdHF(tracksForFit[1]);       
                m_curTup->VrtTrkI  [curNV] = getG4Inter(tracksForFit[0])+ getG4Inter(tracksForFit[1]);       
                m_curTup->VrtCh    [curNV] = Charge;
                m_curTup->VrtProb  [curNV] = Prob2v;          
                m_curTup->VrtSig3D [curNV] = Sig3D;
                m_curTup->VrtSig2D [curNV] = Sig2D;
                m_curTup->VrtDist2D[curNV] = vrtR<20. ? Dist2D : vrtR;
                m_curTup->VrtM     [curNV] = tmpVrt.momentum.M();
                m_curTup->VrtZ     [curNV] = tmpVrt.fitVertex.z();
                m_curTup->VrtPt    [curNV] = tmpVrt.momentum.Pt();
                m_curTup->VrtEta   [curNV] = SVPV.Eta();
                m_curTup->VrtIBL   [curNV] = sumIBLHits;
                m_curTup->VrtBL    [curNV] = sumBLHits;
                m_curTup->VrtSinSPM[curNV] = std::sqrt(1.-cosSVPV*cosSVPV);
                m_curTup->VMinPtT  [curNV] = minPtT;
                m_curTup->VMinS3DT [curNV] = std::min(trackSignif[i],trackSignif[j]);
                m_curTup->VMaxS3DT [curNV] = std::max(trackSignif[i],trackSignif[j]);
                m_curTup->VrtBDT   [curNV] = 1.1;
                m_curTup->VrtHR1   [curNV] = ihitR;
                m_curTup->VrtHR2   [curNV] = jhitR;
                m_curTup->VrtDisk  [curNV] = idisk1+10*idisk2+20*idisk3+30*jdisk1+40*jdisk2+50*jdisk3;
                m_curTup->VSigMat  [curNV] = dstMatSignif;
                if(curNV<DevTuple::maxNVrt-1)m_curTup->n2Vrt++;
             }
	     if(tmpVrt.momentum.Pt() > m_Vrt2TrPtLimit) continue;
             VrtVrtDist(PrimVrt, tmpVrt.fitVertex, tmpVrt.errorMatrix, Sig3D);
	     Dist2D=VrtVrtDist2D(PrimVrt, tmpVrt.fitVertex, tmpVrt.errorMatrix, Sig2D);
             std::vector<float> VARS(10);
	     VARS[0]=Prob2v;
	     VARS[1]=std::log(tmpVrt.momentum.Pt());
	     VARS[2]=std::log(std::max(minPtT,m_cutPt));
	     VARS[3]=std::log(vrtR<20. ? Dist2D : vrtR);
	     VARS[4]=std::log(std::max(std::min(trackSignif[i],trackSignif[j]),m_trkSigCut));
	     VARS[5]=std::log(std::max(trackSignif[i],trackSignif[j]));
	     VARS[6]=tmpVrt.momentum.M();
	     VARS[7]=std::sqrt(std::abs(1.-cosSVPV*cosSVPV));
	     VARS[8]=SVPV.Eta();
	     VARS[9]=std::max(ihitR,jhitR);
             float wgtSelect=m_SV2T_BDT->GetGradBoostMVA(VARS);
	     if( m_fillHist && m_curTup ) m_curTup->VrtBDT[m_curTup->n2Vrt-1] = wgtSelect;
	     if(wgtSelect<m_v2tIniBDTCut) continue;
             add_edge(i,j,*m_compatibilityGraph);
         }
      } 
      return;
   }

   void MultiSecVertexTool::selGoodTrkParticle( workVectorArrxAOD * xAODwrk,
                                                    const xAOD::Vertex & PrimVrt)
   const
   {    
    std::vector<const xAOD::TrackParticle*>& inpTrk          = xAODwrk->inpTrk;
    std::vector<const xAOD::TrackParticle*>& selectedTracks  = xAODwrk->listSelTracks;
    std::vector<const xAOD::TrackParticle*>::const_iterator   i_ntrk;
    std::vector<double> impact,impactError;
    std::multimap<double,const xAOD::TrackParticle*> orderedTrk;
    if(m_fillHist){ m_hb_ntrkInput->Fill( inpTrk.size()+0.1, m_w_1);}
    for (i_ntrk = inpTrk.begin(); i_ntrk < inpTrk.end(); ++i_ntrk) {
//
//-- Perigee in TrackParticle
//
          if(m_fillHist){ m_hb_trkSelect->Fill( 0., m_w_1);}
          if((*i_ntrk)->numberDoF() == 0) continue; //Protection
          double trkChi2 = (*i_ntrk)->chiSquared() / (*i_ntrk)->numberDoF();
          if(trkChi2 	      > m_cutChi2)           continue;
          if( (*i_ntrk)->pt()  < m_cutPt)            continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 1., m_w_1);}
 
          const Trk::Perigee mPer=(*i_ntrk)->perigeeParameters() ;
          double CovTrkMtx00 = (*(mPer.covariance()))(0,0);
 
          uint8_t PixelHits,SctHits,BLayHits;
          if( !((*i_ntrk)->summaryValue(PixelHits,xAOD::numberOfPixelHits)) )   continue; // Track is 
          if( !((*i_ntrk)->summaryValue(  SctHits,xAOD::numberOfSCTHits))   )   continue; // definitely  
          if( SctHits<3 )                                                       continue; // bad
          if( !((*i_ntrk)->summaryValue(BLayHits,xAOD::numberOfInnermostPixelLayerHits)))  BLayHits=0;
          if(m_fillHist){ m_hb_trkSelect->Fill( 2., m_w_1);}

          uint8_t splSCTHits,outSCTHits,splPixHits,outPixHits;
          if( !((*i_ntrk)->summaryValue(splSCTHits,xAOD::numberOfSCTSpoiltHits)))  splSCTHits=0;
          if( !((*i_ntrk)->summaryValue(outSCTHits,xAOD::numberOfSCTOutliers)))    outSCTHits=0;
          if( !((*i_ntrk)->summaryValue(splPixHits,xAOD::numberOfPixelSpoiltHits)))splPixHits=0;
          if( !((*i_ntrk)->summaryValue(outPixHits,xAOD::numberOfPixelOutliers)))  outPixHits=0;

          Amg::Vector3D perigeePos=mPer.position();
          double impactA0=std::sqrt( (perigeePos.x()-PrimVrt.x())*(perigeePos.x()-PrimVrt.x())
                               +(perigeePos.y()-PrimVrt.y())*(perigeePos.y()-PrimVrt.y()) );
          double impactZ=perigeePos.z()-PrimVrt.z();
          if(m_fillHist){  
            m_hb_trkD0->Fill( impactA0, m_w_1);
            m_hb_trkZ ->Fill( impactZ, m_w_1);
          }
          if(std::abs(impactZ)>m_cutZVrt) continue;
          if(impactA0>m_cutA0)        continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 3., m_w_1);}
     
          double bX=xAODwrk->beamX + (perigeePos.z()-xAODwrk->beamZ)*xAODwrk->tanBeamTiltX;
          double bY=xAODwrk->beamY + (perigeePos.z()-xAODwrk->beamZ)*xAODwrk->tanBeamTiltY;
          double impactBeam=std::sqrt( (perigeePos.x()-bX)*(perigeePos.x()-bX) + (perigeePos.y()-bY)*(perigeePos.y()-bY));
          double signifBeam = impactBeam    / std::sqrt(CovTrkMtx00);
          if(signifBeam < m_antiPileupSigRCut) continue;   // cut against tracks from pileup vertices
          if(m_fillHist){ m_hb_trkSelect->Fill( 4., m_w_1);}
          if(PixelHits	    < m_cutPixelHits) 		continue;
          if(SctHits	    < m_cutSctHits) 		continue;
          if((PixelHits+SctHits) < m_cutSiHits) 	continue;
          if(BLayHits	    < m_cutBLayHits) 		continue;
          if(m_fillHist){ m_hb_trkSelect->Fill( 5., m_w_1);}
          orderedTrk.emplace(signifBeam,*i_ntrk);
          selectedTracks.push_back(*i_ntrk);
      }
      std::map<double,const xAOD::TrackParticle*>::reverse_iterator rt=orderedTrk.rbegin();
      selectedTracks.resize(orderedTrk.size());
      for ( int cntt=0; rt!=orderedTrk.rend(); ++rt,++cntt) {selectedTracks[cntt]=(*rt).second;}
      return;
   }



                      /* Service routines*/

//-----------------------------------------------------------------------------------
//  Find track contributing most to the vertex invariant mass
   int MultiSecVertexTool::mostHeavyTrk(WrkVrt v, std::vector<const xAOD::TrackParticle*> allTracks)
   const
   {
      int nTrk=v.selTrk.size(), selT=-1;
      if(nTrk<3)return -1;
      TLorentzVector tSum;
      for(int i=0; i<nTrk; i++){
         tSum += allTracks.at(v.selTrk[i])->p4();
      }
      double massMin=1.e9;
      for(int i=0; i<nTrk; i++){
         TLorentzVector tSum_m1 = tSum - allTracks[v.selTrk[i]]->p4();
         if(tSum_m1.M()<massMin){massMin=tSum_m1.M(); selT=i;}
      }
      return selT;
    }
//-----------------------------------------------------------------------------------
//  Detach the worst Chi2 track from the vertex and join it (if possible) with other track 
//    from the vertex into additional 2-track vertices
//
   void MultiSecVertexTool::DisassembleVertex(std::vector<WrkVrt> *wrkVrtSet, int iv, 
                                                std::vector<const xAOD::TrackParticle*>  AllTracks)
   const
   {
      WrkVrt newvrt; newvrt.Good=true;
      int NTrk=(*wrkVrtSet)[iv].selTrk.size(), SelT=-1;
      if(NTrk<3)return;

      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListBaseTracks;
      StatusCode sc; sc.setChecked();
      m_fitSvc->setRobustness(5);
      sc = refitVertex( wrkVrtSet, iv, AllTracks, false);
      if(sc.isFailure()){ (*wrkVrtSet)[iv].Good=false; return; }
      m_fitSvc->setRobustness(0);
      double Chi2Max=0.;
      for(int i=0; i<NTrk; i++){
         if( (*wrkVrtSet)[iv].chi2PerTrk[i]>Chi2Max) { Chi2Max=(*wrkVrtSet)[iv].chi2PerTrk[i];  SelT=i;}
      }	    
      int NVrtCur=wrkVrtSet->size();
      for(int i=0; i<NTrk; i++){
	   if(i==SelT)continue;
           ListBaseTracks.clear();
	   ListBaseTracks.push_back( AllTracks[(*wrkVrtSet)[iv].selTrk[i]] );
	   ListBaseTracks.push_back( AllTracks[(*wrkVrtSet)[iv].selTrk[SelT]] );
           newvrt.selTrk.resize(2); 
	   newvrt.selTrk[0]=(*wrkVrtSet)[iv].selTrk[i]; 
	   newvrt.selTrk[1]=(*wrkVrtSet)[iv].selTrk[SelT];
           sc = m_fitSvc->VKalVrtFitFast(ListBaseTracks,newvrt.vertex);            /* Fast crude estimation*/
           if( sc.isFailure() )  continue;
           if( newvrt.vertex.perp() > m_maxSVRadiusCut )  newvrt.vertex=Amg::Vector3D(0.,0.,0.);
           m_fitSvc->setMomCovCalc(0);
           m_fitSvc->setApproximateVertex(newvrt.vertex[0],newvrt.vertex[1],newvrt.vertex[2]);
           sc=m_fitSvc->VKalVrtFit(ListBaseTracks, neutralPartDummy,
                             newvrt.vertex,
                             newvrt.vertexMom,
                             newvrt.vertexCharge,
                             newvrt.vertexCov,
                             newvrt.chi2PerTrk, 
                             newvrt.trkAtVrt,
                             newvrt.chi2);
           if( sc.isFailure() )  continue;  
           if( newvrt.chi2>10.)  continue;  // Too bad 2-track vertex fit
           newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
           newvrt.nCloseVrt    = 0;
           newvrt.dCloseVrt    = 1000000.;
           newvrt.projectedVrt = 0.9999;
           if((int)wrkVrtSet->size()==NVrtCur) { wrkVrtSet->push_back(newvrt); continue;}  // just the first added vertex
           if( (*wrkVrtSet).at(NVrtCur).chi2<newvrt.chi2 ) continue;  // previously added 2tr vertex was better
           wrkVrtSet->pop_back();
           wrkVrtSet->push_back(newvrt);
      }
      (*wrkVrtSet)[iv].selTrk.erase( (*wrkVrtSet)[iv].selTrk.begin() + SelT ); //remove track
      sc = refitVertex( wrkVrtSet, iv, AllTracks, false);
      if( sc.isFailure() ) {(*wrkVrtSet)[iv].Good=false;}
   }


   void MultiSecVertexTool::Clean1TrVertexSet(std::vector<WrkVrt> *wrkVrtSet)
   const
   {
     std::vector<int> countVT(wrkVrtSet->size(),0);
     std::vector<int> linkedVrt(wrkVrtSet->size(),0);
//--- Mark as bad the 1track vertex if the detached track is NOT present in any remaining good vertex (>=2tr)    
     for(int i1tv=0; i1tv<(int)wrkVrtSet->size(); i1tv++) {
       if( (*wrkVrtSet)[i1tv].selTrk.size()!=1) continue;
       if(!(*wrkVrtSet)[i1tv].Good)             continue;   
       int Trk1=(*wrkVrtSet)[i1tv].detachedTrack; 
       int foundInGoodVrt=0;
       for(int mtv=0; mtv<(int)wrkVrtSet->size(); mtv++) {                        //cycle over good vertices with many tracks
         if( (*wrkVrtSet)[mtv].selTrk.size()<2) continue;
	 if(!(*wrkVrtSet)[mtv].Good)            continue;   
         if( std::find((*wrkVrtSet)[mtv].selTrk.begin(),(*wrkVrtSet)[mtv].selTrk.end(), Trk1) != (*wrkVrtSet)[mtv].selTrk.end()){
	   foundInGoodVrt++; countVT[mtv]++; linkedVrt[i1tv]=mtv;  //Linked vertex found
         }
       }
       if(!foundInGoodVrt)(*wrkVrtSet)[i1tv].Good=false;             // Make the vertex bad
     }
//
//---Select SINGLE 1tr-vertex from many pointing to one multi-track vertex
     for(int mtv=0; mtv<(int)wrkVrtSet->size(); mtv++) {
       if( (*wrkVrtSet)[mtv].selTrk.size()<2 ) continue;
       if(!(*wrkVrtSet)[mtv].Good )            continue;   
       if(      countVT[mtv] < 1 )             continue;
       double distM=1.e9;
       int    best1TVrt=-1;
       for(int i1tv=0; i1tv<(int)wrkVrtSet->size(); i1tv++) {
         if( (*wrkVrtSet)[i1tv].selTrk.size()!=1 ) continue;
         if(!(*wrkVrtSet)[i1tv].Good )             continue;   
	 if( linkedVrt[i1tv] != mtv )              continue;
         double dist=((*wrkVrtSet)[mtv].vertexMom+(*wrkVrtSet)[i1tv].vertexMom).M(); //Use 
         if( dist < distM ) {distM=dist; best1TVrt=i1tv;}
         (*wrkVrtSet)[i1tv].Good=false;   
       }
       if(best1TVrt>-1 && distM<m_VrtMassLimit)(*wrkVrtSet)[best1TVrt].Good=true;
     }
   }
   
   
   void MultiSecVertexTool::trackClassification(std::vector<WrkVrt> *wrkVrtSet, 
                                             std::vector< std::deque<long int> > *TrkInVrt)
   const
   { 
      int NSet=wrkVrtSet->size(); 
      for(int iv=0; iv<NSet; iv++) {
         if(!(*wrkVrtSet)[iv].Good) continue;
         int NTrkAtVrt=(*wrkVrtSet)[iv].selTrk.size();
         if(NTrkAtVrt<2) continue;
         for(int jt=0; jt<NTrkAtVrt; jt++){
           int tracknum=(*wrkVrtSet)[iv].selTrk[jt];
	   (*TrkInVrt).at(tracknum).push_back(iv);
         }
      }
   }


   double MultiSecVertexTool::maxOfShared(std::vector<WrkVrt> *wrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int & SelectedTrack,
				       long int & SelectedVertex)
   const
   {
      long int NTrack=TrkInVrt->size(); 
      int it, jv, itmp, NVrt, VertexNumber;
 
      double MaxOf=-999999999, Chi2Red=0., SelectedProb=1.;
//
      int NShareMax=0;
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt > NShareMax ) NShareMax=NVrt;
      }
      if(NShareMax<=1)return MaxOf;              /* No shared tracks */
//      
      for( it=0; it<NTrack; it++) {
         NVrt=(*TrkInVrt)[it].size();         /*Number of vertices for this track*/
         if(  NVrt <= 1 )        continue;    /*Should ALWAYS be checked for safety*/
         if(  NVrt < NShareMax ) continue;    /*Not a shared track with maximal sharing*/
         double maxRadius=0;
         for(auto &vrtn :(*TrkInVrt)[it] ){ maxRadius=std::max((*wrkVrtSet).at(vrtn).vertex.perp(),maxRadius); } //count number of 2-track vertices
         for( jv=0; jv<NVrt; jv++ ){
	    VertexNumber=(*TrkInVrt)[it][jv];
	    if(!(*wrkVrtSet).at(VertexNumber).Good)continue;
	    int NTrkInVrt=(*wrkVrtSet)[VertexNumber].selTrk.size();
	    if( NTrkInVrt <= 1) continue;                                // one track vertex - nothing to do
	    for( itmp=0; itmp<NTrkInVrt; itmp++){
	       if( (*wrkVrtSet)[VertexNumber].selTrk[itmp] == it ) {         /* Track found*/        
	          Chi2Red=(*wrkVrtSet)[VertexNumber].chi2PerTrk.at(itmp)*(*wrkVrtSet)[VertexNumber].vertex.perp()/maxRadius; // Works much better
                  double prob_vrt = TMath::Prob( (*wrkVrtSet)[VertexNumber].chi2, 2*(*wrkVrtSet)[VertexNumber].selTrk.size()-3);
                  if( MaxOf < Chi2Red ){
		      if(MaxOf>0 && prob_vrt>0.01 && SelectedProb<0.01 ) continue; // Don't disassemble good vertices if a bad one is present
		      MaxOf = Chi2Red;
		      SelectedTrack=it; SelectedVertex=VertexNumber;
                      SelectedProb = prob_vrt;
		  }
               }
            }
	 }
      }
//-- Additional check for a common track in 2tr-2tr/Ntr vertex topology
      if( (*TrkInVrt)[SelectedTrack].size() == 2){
          int v1=(*TrkInVrt)[SelectedTrack][0]; int v2=(*TrkInVrt)[SelectedTrack][1];
          double prb1=TMath::Prob( (*wrkVrtSet)[v1].chi2, 2*(*wrkVrtSet)[v1].selTrk.size()-3),
                 prb2=TMath::Prob( (*wrkVrtSet)[v2].chi2, 2*(*wrkVrtSet)[v2].selTrk.size()-3);
	  double dst1=(*wrkVrtSet)[v1].projectedVrt, dst2=(*wrkVrtSet)[v2].projectedVrt; 
          if(prb1>0.05 && prb2>0.05){
            if( (*wrkVrtSet)[v1].selTrk.size()==2 && (*wrkVrtSet)[v2].selTrk.size()==2){
              if     (SelectedVertex==v1 && dst2<dst1)  SelectedVertex=v2;  // Swap to remove the closest to PV vertex
              else if(SelectedVertex==v2 && dst1<dst2)  SelectedVertex=v1;  // Swap to remove the closest to PV vertex
              double M1=(*wrkVrtSet)[v1].vertexMom.M();  double M2=(*wrkVrtSet)[v2].vertexMom.M();
              if( M1>m_VrtMassLimit && M2<m_VrtMassLimit ) SelectedVertex=v1;
              if( M1<m_VrtMassLimit && M2>m_VrtMassLimit ) SelectedVertex=v2;
            }
            if( (*wrkVrtSet)[v1].selTrk.size()+(*wrkVrtSet)[v2].selTrk.size() > 4){
	       if( (*wrkVrtSet)[v1].selTrk.size()==2 && dst1>dst2) SelectedVertex=v2;
	       if( (*wrkVrtSet)[v2].selTrk.size()==2 && dst2>dst1) SelectedVertex=v1;
          } }	
      }
//
      return MaxOf;
   }


   void MultiSecVertexTool::removeTrackFromVertex(std::vector<WrkVrt> *wrkVrtSet, 
                                       std::vector< std::deque<long int> > *TrkInVrt,
				       long int selectedTrack,
				       long int selectedVertex)
   const
   {   
        int posInVrtFit=0;                    //Position of SelectedTrack in vertex fit track list.
	std::deque<long int>::iterator it;
        WrkVrt & CVRT=(*wrkVrtSet).at(selectedVertex);
	for(it=CVRT.selTrk.begin(); 
	    it!=CVRT.selTrk.end();     it++) {
	    if( (*it) == selectedTrack ) { 
	       CVRT.selTrk.erase(it); break;
	    }     
            posInVrtFit++;
	}   

	for(it=(*TrkInVrt).at(selectedTrack).begin(); 
	    it!=(*TrkInVrt)[selectedTrack].end();     it++) {
	    if( (*it) == selectedVertex ) { 
	       (*TrkInVrt)[selectedTrack].erase(it); break;
	    }     
	}   
        (*wrkVrtSet)[selectedVertex].detachedTrack=selectedTrack;
//Check if track is removed from 2tr vertex => then sharing of track left should also be decreased
        if( CVRT.selTrk.size() == 1){
	   long int LeftTrack=CVRT.selTrk[0];  // track left in 1tr vertex
	   for(it=(*TrkInVrt).at(LeftTrack).begin();  it!=(*TrkInVrt)[LeftTrack].end();  it++) {
	      if( (*it) == selectedVertex ) { 
	       (*TrkInVrt)[LeftTrack].erase(it); break;
	      }     
	   }   
           if( TMath::Prob( CVRT.chi2, 1) < 0.05 ) CVRT.Good=false; // Not really good Chi2 for one-track vertex
	   if( CVRT.vertexMom.M()>m_VrtMassLimit)CVRT.Good=false;   // Vertex is too heavy
           int ipos=0; if(posInVrtFit==0)ipos=1;                    // Position of remaining track in previous 2track vertex fit
	   CVRT.vertexMom=MomAtVrt(CVRT.trkAtVrt[ipos]);            //Redefine vertexMom using remaining track
	   if((*TrkInVrt)[LeftTrack].size()>0)CVRT.Good=false;      //Vertex is declared false only if remaining track 
	    						            // is still linked to another vertex
           if(CVRT.vertexMom.Pt()<2.*m_cutPt) CVRT.Good=false;      //1track vertex should have twice momentum of track pt cut
	   CVRT.trkAtVrt.erase((*wrkVrtSet)[selectedVertex].trkAtVrt.begin()+posInVrtFit);  //Remove also TrkAtVrt
        }

   }
//
//  Number of common tracks for 2 vertices
//
   int MultiSecVertexTool::nTrkCommon( std::vector<WrkVrt> *wrkVrtSet, 
                                         int V1, int V2)
   const
   {
      int NTrk_V1 = (*wrkVrtSet).at(V1).selTrk.size(); if( NTrk_V1< 2) return 0;   /* Bad vertex */
      int NTrk_V2 = (*wrkVrtSet).at(V2).selTrk.size(); if( NTrk_V2< 2) return 0;   /* Bad vertex */
      int nTrkCom=0;
      if(NTrk_V1 < NTrk_V2){
        for(int i=0; i<NTrk_V1; i++){
          int trk=(*wrkVrtSet)[V1].selTrk[i];
          if( std::find((*wrkVrtSet)[V2].selTrk.begin(),(*wrkVrtSet)[V2].selTrk.end(),trk) != (*wrkVrtSet)[V2].selTrk.end()) nTrkCom++;
        }
      }else{
        for(int i=0; i<NTrk_V2; i++){
          int trk=(*wrkVrtSet)[V2].selTrk[i];
          if( std::find((*wrkVrtSet)[V1].selTrk.begin(),(*wrkVrtSet)[V1].selTrk.end(),trk) != (*wrkVrtSet)[V1].selTrk.end()) nTrkCom++;
        }
      }
      return nTrkCom;
   }
//
//  Minimal normalized vertex-vertex distance
//
   double MultiSecVertexTool::minVrtVrtDist( std::vector<WrkVrt> *wrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=V2=0;
     for(int iv=0; iv<(int)wrkVrtSet->size(); iv++) { (*wrkVrtSet).at(iv).dCloseVrt=1000000.; (*wrkVrtSet)[iv].nCloseVrt=-1;}
     double foundMinVrtDst=1000000.;

     for(int iv=0; iv<(int)wrkVrtSet->size()-1; iv++) {
        if( (*wrkVrtSet)[iv].selTrk.size()< 2)    continue;   /* Bad vertices */
        if(!(*wrkVrtSet)[iv].Good )               continue;   /* Bad vertices */
        for(int jv=iv+1; jv<(int)wrkVrtSet->size(); jv++) {
           if( (*wrkVrtSet)[jv].selTrk.size()< 2) continue;   /* Bad vertices */
           if(!(*wrkVrtSet)[jv].Good )            continue;   /* Bad vertices */
           double tmp= std::abs((*wrkVrtSet)[iv].vertex.x()-(*wrkVrtSet)[jv].vertex.x())
                      +std::abs((*wrkVrtSet)[iv].vertex.y()-(*wrkVrtSet)[jv].vertex.y())
                      +std::abs((*wrkVrtSet)[iv].vertex.z()-(*wrkVrtSet)[jv].vertex.z());
           if(tmp>20.) continue;
           double tmpDst = VrtVrtDist((*wrkVrtSet)[iv].vertex,(*wrkVrtSet)[iv].vertexCov,
                                      (*wrkVrtSet)[jv].vertex,(*wrkVrtSet)[jv].vertexCov);
           if( tmpDst < foundMinVrtDst){foundMinVrtDst = tmpDst; V1=iv; V2=jv;} 
           if( tmpDst < (*wrkVrtSet)[iv].dCloseVrt ) {(*wrkVrtSet)[iv].dCloseVrt=tmpDst; (*wrkVrtSet)[iv].nCloseVrt=jv;}
           if( tmpDst < (*wrkVrtSet)[jv].dCloseVrt ) {(*wrkVrtSet)[jv].dCloseVrt=tmpDst; (*wrkVrtSet)[jv].nCloseVrt=iv;}
         }
      }
      return foundMinVrtDst;
   }
//   
// Give minimal distance between nonmodifed yet vertices
//   
   double MultiSecVertexTool::minVrtVrtDistNext( std::vector<WrkVrt> *wrkVrtSet, int & V1, int & V2)
   const
   {  
     V1=0; V2=0;
     double foundMinVrtDst=1000000.;
     for(int iv=0; iv<(int)wrkVrtSet->size()-1; iv++) {
        if( (*wrkVrtSet)[iv].selTrk.size()< 2) continue;   /* Bad vertex */
        if( (*wrkVrtSet)[iv].nCloseVrt==-1)    continue;   /* Used vertex */
        if( (*wrkVrtSet)[iv].dCloseVrt <foundMinVrtDst ) {
	   int vtmp=(*wrkVrtSet)[iv].nCloseVrt;
           if((*wrkVrtSet)[vtmp].nCloseVrt==-1) { continue;}  // Close vertex to given [iv] one is modified already 
	   foundMinVrtDst=(*wrkVrtSet)[iv].dCloseVrt;
	   V1=iv;
	   V2=vtmp;
	}
      }
      return foundMinVrtDst;
   }

//
//  Check two close vertices by explicit vertex fit and create combined vertex if successful
//
   double  MultiSecVertexTool::mergeAndRefitVertices( std::vector<WrkVrt> *wrkVrtSet, int V1, int V2, WrkVrt & newvrt,
                                                        std::vector<const xAOD::TrackParticle*> & AllTrackList)
   const
   {
      if(!(*wrkVrtSet).at(V1).Good)return -1.;         //bad vertex
      if(!(*wrkVrtSet).at(V2).Good)return -1.;         //bad vertex
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      newvrt.Good=true;
      int NTrk_V1=(*wrkVrtSet)[V1].selTrk.size();
      int NTrk_V2=(*wrkVrtSet)[V2].selTrk.size();
      newvrt.selTrk.resize(NTrk_V1+NTrk_V2);
      std::copy((*wrkVrtSet)[V1].selTrk.begin(),(*wrkVrtSet)[V1].selTrk.end(), newvrt.selTrk.begin());
      std::copy((*wrkVrtSet)[V2].selTrk.begin(),(*wrkVrtSet)[V2].selTrk.end(), newvrt.selTrk.begin()+NTrk_V1);

      std::deque<long int>::iterator   TransfEnd ;
      sort( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      TransfEnd =  unique( newvrt.selTrk.begin(), newvrt.selTrk.end() );
      newvrt.selTrk.erase( TransfEnd, newvrt.selTrk.end());
      std::vector<const xAOD::TrackParticle*>  fitTrackList(0);
      for(int it=0; it<(int)newvrt.selTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.selTrk[it]] );
      m_fitSvc->setApproximateVertex( 0.5*((*wrkVrtSet)[V1].vertex[0]+(*wrkVrtSet)[V2].vertex[0]),
                                      0.5*((*wrkVrtSet)[V1].vertex[1]+(*wrkVrtSet)[V2].vertex[1]),
                                      0.5*((*wrkVrtSet)[V1].vertex[2]+(*wrkVrtSet)[V2].vertex[2]));
      
      m_fitSvc->setMomCovCalc(0); // No full covariance to save CPU
      StatusCode sc=m_fitSvc->VKalVrtFit(fitTrackList,neutralPartDummy,
                                   newvrt.vertex, newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                   newvrt.chi2PerTrk,  newvrt.trkAtVrt, newvrt.chi2);
      if( sc.isFailure() )             return -1.;  
      if( newvrt.chi2>500. )           return -1.;  //VK protection
      if( newvrt.chi2PerTrk.size()==2) newvrt.chi2PerTrk[0]=newvrt.chi2PerTrk[1]=newvrt.chi2/2.;
      return TMath::Prob( newvrt.chi2, 2*newvrt.selTrk.size()-3);
   }
   //
   //
   //================== Intelligent merge of multitrack vertices with 2 and more common tracks
   void  MultiSecVertexTool::mergeAndRefitOverlapVertices( std::vector<WrkVrt> *wrkVrtSet, int V1, int V2,
                                                             std::vector<const xAOD::TrackParticle*> & AllTrackList) const
   {
      if(!(*wrkVrtSet).at(V1).Good)return;         //bad vertex
      if(!(*wrkVrtSet).at(V2).Good)return;         //bad vertex
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      m_fitSvc->setMomCovCalc(0);  //No full covariance to save CPU
      WrkVrt newvrt;
      newvrt.Good=true;
      if( nTrkCommon( wrkVrtSet, V1, V2)<2 )return;       //No overlap
      //- V1 should become ref. vertex. Another Vrt tracks will be added to it. 
      if(      (*wrkVrtSet)[V1].selTrk.size() <  (*wrkVrtSet)[V2].selTrk.size() )
                                                                           {int itmp=V2; V2=V1; V1=itmp;}   //Vertex with NTrk=max is chosen
      else if( (*wrkVrtSet)[V1].selTrk.size() == (*wrkVrtSet)[V2].selTrk.size() )
         { if( (*wrkVrtSet)[V1].chi2           > (*wrkVrtSet)[V2].chi2 )   {int itmp=V2; V2=V1; V1=itmp;} } // Vertex with minimal Chi2 is chosen
      //- Fill base track list for new vertex
      newvrt.selTrk.resize( (*wrkVrtSet)[V1].selTrk.size() );
      std::copy((*wrkVrtSet)[V1].selTrk.begin(),(*wrkVrtSet)[V1].selTrk.end(), newvrt.selTrk.begin());
      //- Identify non-common tracks in second vertex
      std::vector<int> noncommonTrk(0);
      for(auto &it : (*wrkVrtSet)[V2].selTrk){
        if( std::find((*wrkVrtSet)[V1].selTrk.begin(), (*wrkVrtSet)[V1].selTrk.end(), it) == (*wrkVrtSet)[V1].selTrk.end() )
           noncommonTrk.push_back(it);
      }
      //      
      // Try to add non-common tracks one by one
      std::vector<const xAOD::TrackParticle*>  fitTrackList(0);
      std::vector<int> detachedTrk(0);
      StatusCode sc; sc.setChecked();
      WrkVrt bestVrt;
      bool foundMerged=false;
      for( auto nct : noncommonTrk){  
         fitTrackList.clear();
         for(int it=0; it<(int)newvrt.selTrk.size(); it++)fitTrackList.push_back( AllTrackList[newvrt.selTrk[it]] );
         fitTrackList.push_back( AllTrackList.at(nct) );
         m_fitSvc->setApproximateVertex( (*wrkVrtSet)[V1].vertex[0],(*wrkVrtSet)[V1].vertex[1],(*wrkVrtSet)[V1].vertex[2]);
         m_fitSvc->setMomCovCalc(0); // No full covariance to save CPU
         sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, newvrt.vertex, newvrt.vertexMom, newvrt.vertexCharge, newvrt.vertexCov,
                                 newvrt.chi2PerTrk, newvrt.trkAtVrt, newvrt.chi2);
         if( sc.isFailure() || TMath::Prob( newvrt.chi2, 2*newvrt.selTrk.size()+2-3)<m_globVrtProbCut ) {
           detachedTrk.push_back(nct);
           continue;
         }
         newvrt.selTrk.push_back(nct);   // Compatible track. Add to common vertex.
         bestVrt=newvrt;
         foundMerged=true;
      }
      if(foundMerged)(*wrkVrtSet)[V1]=bestVrt;
      (*wrkVrtSet)[V2].Good=false;
      //
      // Now detached tracks
      if(detachedTrk.size()>1){
         WrkVrt nVrt;
         fitTrackList.clear(); nVrt.selTrk.clear();
         for(auto nct : detachedTrk){ fitTrackList.push_back( AllTrackList[nct] );  nVrt.selTrk.push_back(nct); }
         m_fitSvc->setApproximateVertex( (*wrkVrtSet)[V2].vertex[0],(*wrkVrtSet)[V2].vertex[1],(*wrkVrtSet)[V2].vertex[2]);
         m_fitSvc->setMomCovCalc(0); // No full covariance to save CPU
         sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, nVrt.vertex, nVrt.vertexMom, nVrt.vertexCharge, nVrt.vertexCov,
                                 nVrt.chi2PerTrk, nVrt.trkAtVrt, nVrt.chi2);
         if(sc.isSuccess()) (*wrkVrtSet).push_back(nVrt);
      } else if( detachedTrk.size()==1 ){
         bool tFound=false;
         for( auto &vrt : (*wrkVrtSet) ){  
           if( !vrt.Good || vrt.selTrk.size()<2 )continue;
           if( std::find(vrt.selTrk.begin(), vrt.selTrk.end(), detachedTrk[0]) != vrt.selTrk.end() ) { tFound=true; break; }
         }
         if( !tFound ) {   //Track is not present in other vertices. 
           double Chi2min=1.e9; int selectedTrk=-1;
           WrkVrt saveVrt;
           fitTrackList.resize(2);
           fitTrackList[0]= AllTrackList[detachedTrk[0]];
           for(auto trk : (*wrkVrtSet)[V2].selTrk){
              if(trk==detachedTrk[0])continue;
              WrkVrt nVrt; nVrt.Good=true;
              fitTrackList[1]=AllTrackList[trk];
              m_fitSvc->setApproximateVertex( (*wrkVrtSet)[V2].vertex[0],(*wrkVrtSet)[V2].vertex[1],(*wrkVrtSet)[V2].vertex[2]);
              m_fitSvc->setMomCovCalc(0); // No full covariance to save CPU
              sc=m_fitSvc->VKalVrtFit(fitTrackList, neutralPartDummy, nVrt.vertex, nVrt.vertexMom, nVrt.vertexCharge, nVrt.vertexCov,
                                      nVrt.chi2PerTrk, nVrt.trkAtVrt, nVrt.chi2);
              if(sc.isSuccess() &&   nVrt.chi2<Chi2min) {Chi2min=nVrt.chi2;  saveVrt=nVrt; selectedTrk=trk; }
           }    
	   if(Chi2min<1.e9){
             saveVrt.selTrk.resize(1); saveVrt.selTrk[0]=detachedTrk[0];
             saveVrt.detachedTrack=selectedTrk;
             saveVrt.vertexMom=MomAtVrt(saveVrt.trkAtVrt[0]);  //redefine vertex momentum
             (*wrkVrtSet).push_back(saveVrt);
           }
         }
      }
      return ;
   }

//
//  Iterate track removal until vertex get good Chi2
//

   double  MultiSecVertexTool::improveVertexChi2( std::vector<WrkVrt> *wrkVrtSet, int V,
                                                      std::vector<const xAOD::TrackParticle*> & AllTrackList)
   const
   {

      int NTrk=(*wrkVrtSet)[V].selTrk.size();
      if( NTrk<2 )return 0.;
      double Prob=TMath::Prob( (*wrkVrtSet)[V].chi2, 2*NTrk-3);
      //
      //----Start track removal iterations
      while(Prob<0.01){
        NTrk=(*wrkVrtSet)[V].selTrk.size();
        if(NTrk==2)return Prob;
        int SelT=-1; double Chi2Max=0.;
        for(int i=0; i<NTrk; i++){
          if( (*wrkVrtSet)[V].chi2PerTrk[i]>Chi2Max) { 
            Chi2Max=(*wrkVrtSet)[V].chi2PerTrk[i];  
            SelT=i;
          }
        }
        if (SelT<0) return 0; 
        (*wrkVrtSet)[V].detachedTrack=(*wrkVrtSet)[V].selTrk[SelT];
        (*wrkVrtSet)[V].selTrk.erase( (*wrkVrtSet)[V].selTrk.begin() + SelT ); //remove track
        StatusCode sc = refitVertex( wrkVrtSet, V, AllTrackList, false);
        if(sc.isFailure())return 0.;
        Prob=TMath::Prob( (*wrkVrtSet)[V].chi2, 2*(NTrk-1)-3);
      }
      return Prob;
   }




   StatusCode MultiSecVertexTool::refitVertex( std::vector<WrkVrt> *wrkVrtSet,
                                                 int SelectedVertex,
                                                 std::vector<const xAOD::TrackParticle*> & selectedTracks,
						 bool ifCovM) const
   {
      int nth = (*wrkVrtSet)[SelectedVertex].selTrk.size();
 
      if(nth<2) return StatusCode::SUCCESS;
 
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListTracks(0);
      for(int i=0;i<nth;i++) {
	  int j=(*wrkVrtSet)[SelectedVertex].selTrk[i];                           /*Track number*/
          ListTracks.push_back( selectedTracks[j] );
      }
      (*wrkVrtSet)[SelectedVertex].Good = false;
      (*wrkVrtSet)[SelectedVertex].chi2PerTrk.resize(nth);
      for(int i=0;i<nth;i++)(*wrkVrtSet)[SelectedVertex].chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setDefault();
      if(ifCovM)m_fitSvc->setMomCovCalc(1);
      else      m_fitSvc->setMomCovCalc(0);
      m_fitSvc->setApproximateVertex((*wrkVrtSet)[SelectedVertex].vertex[0],
	                             (*wrkVrtSet)[SelectedVertex].vertex[1],
	                             (*wrkVrtSet)[SelectedVertex].vertex[2]);
      StatusCode sc=m_fitSvc->VKalVrtFit(ListTracks, neutralPartDummy,
                                (*wrkVrtSet)[SelectedVertex].vertex,
                                (*wrkVrtSet)[SelectedVertex].vertexMom,
				(*wrkVrtSet)[SelectedVertex].vertexCharge,
		                (*wrkVrtSet)[SelectedVertex].vertexCov,
				(*wrkVrtSet)[SelectedVertex].chi2PerTrk, 
				(*wrkVrtSet)[SelectedVertex].trkAtVrt,
				(*wrkVrtSet)[SelectedVertex].chi2);
      if(sc.isSuccess())(*wrkVrtSet)[SelectedVertex].Good = true;
      if((*wrkVrtSet)[SelectedVertex].chi2PerTrk.size()==2) 
         (*wrkVrtSet)[SelectedVertex].chi2PerTrk[0]=
	 (*wrkVrtSet)[SelectedVertex].chi2PerTrk[1]=(*wrkVrtSet)[SelectedVertex].chi2/2.;
      return sc;
   }



   double MultiSecVertexTool::refitVertex(WrkVrt &Vrt,
                                            std::vector<const xAOD::TrackParticle*> & selectedTracks,
					    bool ifCovM) const
   {
      int i,j;
      int nth = Vrt.selTrk.size();
 
      if(nth<2) return -1.;
 
      std::vector<const xAOD::NeutralParticle*> neutralPartDummy(0);
      std::vector<const xAOD::TrackParticle*>  ListTracks(0);
      for(i=0;i<nth;i++) {
	  j=Vrt.selTrk[i];                           /*Track number*/
          ListTracks.push_back( selectedTracks[j] );
      }
      Vrt.Good = false;
      Vrt.chi2PerTrk.resize(nth);
      for(i=0;i<nth;i++)Vrt.chi2PerTrk[i]=100000.+i; //VK safety

      m_fitSvc->setDefault();
      if(ifCovM)m_fitSvc->setMomCovCalc(1);
      else      m_fitSvc->setMomCovCalc(0);
      m_fitSvc->setApproximateVertex(Vrt.vertex[0],Vrt.vertex[1],Vrt.vertex[2]);
      StatusCode sc=m_fitSvc->VKalVrtFit(ListTracks,neutralPartDummy,Vrt.vertex,Vrt.vertexMom,Vrt.vertexCharge,
                                         Vrt.vertexCov,Vrt.chi2PerTrk, Vrt.trkAtVrt,Vrt.chi2);
      if(sc.isSuccess())Vrt.Good = true;
      else {Vrt.Good = false; return -1.;}
      if(Vrt.chi2PerTrk.size()==2) Vrt.chi2PerTrk[0]=Vrt.chi2PerTrk[1]=Vrt.chi2/2.;
      return TMath::Prob( Vrt.chi2, 2*nth-1);
   }


   bool MultiSecVertexTool::isPart( std::deque<long int> test, std::deque<long int> base)
   const
   {
      std::deque<long int>::iterator trk=test.begin();
      for(trk=test.begin(); trk!=test.end(); trk++)
         if(std::find(base.begin(), base.end(), (*trk)) == base.end()) return false;  //element not found => test is not part of base
      return true;
   }

   double MultiSecVertexTool::MomProjDist(const Amg::Vector3D & SecVrt,const xAOD::Vertex &PrimVrt,const TLorentzVector &Mom)
   const
   {
      Amg::Vector3D vv=SecVrt-PrimVrt.position();
      return ( vv.x()*Mom.X()+vv.y()*Mom.Y()+vv.z()*Mom.Z() )/ Mom.P();
   }



  void MultiSecVertexTool::printWrkSet(const std::vector<WrkVrt> *WrkVrtSet, const std::string &name) const {
    int nGoodV=0;
    if(msgLvl(MSG::INFO)){
    for(int iv=0; iv<(int)WrkVrtSet->size(); iv++) {
      msg(MSG::INFO)<<name
      <<"= "<<(*WrkVrtSet)[iv].vertex[0]
      <<", "<<(*WrkVrtSet)[iv].vertex[1]
      <<", "<<(*WrkVrtSet)[iv].vertex[2]
      <<" NTrk="<<(*WrkVrtSet)[iv].selTrk.size()
      <<" is good="<<std::boolalpha<<(*WrkVrtSet)[iv].Good<<std::noboolalpha
      <<"  Chi2="<<(*WrkVrtSet)[iv].chi2
      <<"  Mass="<<(*WrkVrtSet)[iv].vertexMom.M()
      <<"  detached="<<(*WrkVrtSet)[iv].detachedTrack
      <<"  proj.dist="<<(*WrkVrtSet)[iv].projectedVrt
      <<" trk=";
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].selTrk.size(); kk++) {
                msg(MSG::INFO)<<", "<<(*WrkVrtSet)[iv].selTrk[kk];}
      for(int kk=0; kk<(int)(*WrkVrtSet)[iv].selTrk.size(); kk++) {
                msg(MSG::INFO)<<", "<<MomAtVrt((*WrkVrtSet)[iv].trkAtVrt[kk]).Perp();}
      msg(MSG::INFO)<<endreq;
      if((*WrkVrtSet)[iv].Good)nGoodV++;
    }
    msg(MSG::INFO)<<name<<" N="<<nGoodV<<endreq; 
    }
  }

               /*  Technicalities */
  double MultiSecVertexTool::ProjSV_PV(const Amg::Vector3D & SV, const xAOD::Vertex & PV, const TLorentzVector & Direction) const
  {  
     TVector3 SV_PV( SV.x()-PV.x(), SV.y()-PV.y(), SV.z()-PV.z() );
     return Direction.Vect().Unit()*SV_PV.Unit();
  }

  
  double MultiSecVertexTool::VrtVrtDist(const xAOD::Vertex & primVrt, const Amg::Vector3D & secVrt, 
                                          const std::vector<double> secVrtErr, double& signif)
  const
  {
    double distx =  primVrt.x()- secVrt.x();
    double disty =  primVrt.y()- secVrt.y();
    double distz =  primVrt.z()- secVrt.z();


    AmgSymMatrix(3)  primCovMtx=primVrt.covariancePosition();  //Create
    primCovMtx(0,0) += secVrtErr[0];
    primCovMtx(0,1) += secVrtErr[1];
    primCovMtx(1,0) += secVrtErr[1];
    primCovMtx(1,1) += secVrtErr[2];
    primCovMtx(0,2) += secVrtErr[3];
    primCovMtx(2,0) += secVrtErr[3];
    primCovMtx(1,2) += secVrtErr[4];
    primCovMtx(2,1) += secVrtErr[4];
    primCovMtx(2,2) += secVrtErr[5];

    AmgSymMatrix(3)  wgtMtx = primCovMtx.inverse();

    signif = distx*wgtMtx(0,0)*distx
            +disty*wgtMtx(1,1)*disty
            +distz*wgtMtx(2,2)*distz
         +2.*distx*wgtMtx(0,1)*disty
         +2.*distx*wgtMtx(0,2)*distz
         +2.*disty*wgtMtx(1,2)*distz;
    signif=std::sqrt(std::abs(signif));
    if( signif!=signif ) signif = 0.;
    return std::sqrt(distx*distx+disty*disty+distz*distz);
  }

  double MultiSecVertexTool::VrtVrtDist2D(const xAOD::Vertex & primVrt, const Amg::Vector3D & secVrt, 
                                          const std::vector<double> secVrtErr, double& signif)
  const
  {
    double distx =  primVrt.x()- secVrt.x();
    double disty =  primVrt.y()- secVrt.y();


    AmgSymMatrix(3)  primCovMtx=primVrt.covariancePosition();  //Create
    AmgSymMatrix(2)  covMtx;
    covMtx(0,0) = primCovMtx(0,0) + secVrtErr[0];
    covMtx(0,1) = primCovMtx(0,1) + secVrtErr[1];
    covMtx(1,0) = primCovMtx(1,0) + secVrtErr[1];
    covMtx(1,1) = primCovMtx(1,1) + secVrtErr[2];

    AmgSymMatrix(2)  wgtMtx = covMtx.inverse();

    signif = distx*wgtMtx(0,0)*distx
            +disty*wgtMtx(1,1)*disty
         +2.*distx*wgtMtx(0,1)*disty;
    signif=std::sqrt(std::abs(signif));
    if( signif!=signif ) signif = 0.;
    return std::sqrt(distx*distx+disty*disty);
  }


  double MultiSecVertexTool::VrtVrtDist(const Amg::Vector3D & vrt1, const std::vector<double>  & vrtErr1,
                                            const Amg::Vector3D & vrt2, const std::vector<double>  & vrtErr2)
  const
  {
    double distx =  vrt1.x()- vrt2.x();
    double disty =  vrt1.y()- vrt2.y();
    double distz =  vrt1.z()- vrt2.z();

    AmgSymMatrix(3)  primCovMtx;
    primCovMtx(0,0) =                   vrtErr1[0]+vrtErr2[0];
    primCovMtx(0,1) = primCovMtx(1,0) = vrtErr1[1]+vrtErr2[1];
    primCovMtx(1,1) =                   vrtErr1[2]+vrtErr2[2];
    primCovMtx(0,2) = primCovMtx(2,0) = vrtErr1[3]+vrtErr2[3];
    primCovMtx(1,2) = primCovMtx(2,1) = vrtErr1[4]+vrtErr2[4];
    primCovMtx(2,2) =                   vrtErr1[5]+vrtErr2[5];

    AmgSymMatrix(3)  wgtMtx = primCovMtx.inverse();


    double signif = 
               distx*wgtMtx(0,0)*distx
              +disty*wgtMtx(1,1)*disty
              +distz*wgtMtx(2,2)*distz
           +2.*distx*wgtMtx(0,1)*disty
           +2.*distx*wgtMtx(0,2)*distz
           +2.*disty*wgtMtx(1,2)*distz;
    signif=std::sqrt(std::abs(signif));
    if(signif != signif)  signif = 0.;
    return signif;
  }
//
  double MultiSecVertexTool::PntPntDist(const Amg::Vector3D & vrt1, const Amg::Vector3D & vrt2) const
  { 
    double dx =  vrt1.x()- vrt2.x();
    double dy =  vrt1.y()- vrt2.y();
    double dz =  vrt1.z()- vrt2.z();
    return std::sqrt(dx*dx+dy*dy*dz*dz);
  }
//--------------------------------------------------
// significance along some direction
//--------------------------------------------------
double MultiSecVertexTool::VrtVrtDist(const xAOD::Vertex & primVrt, const Amg::Vector3D & secVrt, 
                                           const std::vector<double> secVrtErr, const TLorentzVector & Dir)
   const
   {
     Amg::Vector3D dir(Dir.Vect().Unit().X(), Dir.Vect().Unit().Y(), Dir.Vect().Unit().Z());
     double projDist=(secVrt-primVrt.position()).dot(dir);
     double distx =  dir.x()*projDist;
     double disty =  dir.y()*projDist;
     double distz =  dir.z()*projDist;

     AmgSymMatrix(3)  primCovMtx=primVrt.covariancePosition();  //Create
     primCovMtx(0,0) += secVrtErr[0];
     primCovMtx(0,1) += secVrtErr[1];
     primCovMtx(1,0) += secVrtErr[1];
     primCovMtx(1,1) += secVrtErr[2];
     primCovMtx(0,2) += secVrtErr[3];
     primCovMtx(2,0) += secVrtErr[3];
     primCovMtx(1,2) += secVrtErr[4];
     primCovMtx(2,1) += secVrtErr[4];
     primCovMtx(2,2) += secVrtErr[5];
 
     AmgSymMatrix(3)  wgtMtx = primCovMtx.inverse();
 
     double signif = distx*wgtMtx(0,0)*distx
                    +disty*wgtMtx(1,1)*disty
                    +distz*wgtMtx(2,2)*distz
                 +2.*distx*wgtMtx(0,1)*disty
                 +2.*distx*wgtMtx(0,2)*distz
                 +2.*disty*wgtMtx(1,2)*distz;
     signif=std::sqrt(std::abs(signif));
     if( signif!=signif ) signif = 0.;
     if(projDist<0)signif=-signif;
     return signif;
   }

//----------------------------
//   Vertex error along radius
//----------------------------
  double MultiSecVertexTool::VrtRadiusError(const Amg::Vector3D & SecVrt, const std::vector<double>  & VrtErr) const
  {
    double DirX=SecVrt.x(), DirY=SecVrt.y(); 
    double Covar =    DirX*VrtErr[0]*DirX
                  +2.*DirX*VrtErr[1]*DirY
                     +DirY*VrtErr[2]*DirY;
    Covar /= DirX*DirX + DirY*DirY;
    Covar=std::sqrt(std::abs(Covar));
    if(Covar != Covar)  Covar = 0.;
    return Covar;
  }


  TLorentzVector MultiSecVertexTool::MomAtVrt(const std::vector< double >& inpTrk) 
  const
  {
     double api=1./std::abs(inpTrk[2]);
     CxxUtils::sincos phi  (inpTrk[0]);
     CxxUtils::sincos theta(inpTrk[1]);
     double px = phi.cs * theta.sn*api;
     double py = phi.sn * theta.sn*api;
     double pz =          theta.cs*api;
     double ee = std::sqrt( px*px + py*py + pz*pz + m_massPi*m_massPi);
     return TLorentzVector(px,py,pz,ee); 
   }


/*************************************************************************************************************/
  int   MultiSecVertexTool::getIBLHit(const xAOD::TrackParticle* Part) const
  {
        uint8_t IBLhit,IBLexp;
        if(!Part->summaryValue( IBLexp,  xAOD::expectInnermostPixelLayerHit) )           IBLexp = 0;
	if( IBLexp==0 ) return -1;
        if(!Part->summaryValue( IBLhit,  xAOD::numberOfInnermostPixelLayerHits) )        IBLhit = 0;
        if(IBLhit) return 1;
	else       return 0;
  }
  int   MultiSecVertexTool::getBLHit(const xAOD::TrackParticle* Part) const
  {
        uint8_t BLhit,BLexp;
        if(!Part->summaryValue( BLexp,  xAOD::expectNextToInnermostPixelLayerHit) )           BLexp = 0;
	if( BLexp==0 ) return -1;
        if(!Part->summaryValue( BLhit,  xAOD::numberOfNextToInnermostPixelLayerHits) )        BLhit = 0;
        if(BLhit) return 1;
	else      return 0;
  }

  void   MultiSecVertexTool::getPixelDiscs(const xAOD::TrackParticle* Part, int &d0Hit, int &d1Hit, int &d2Hit) const
  {
        uint32_t HitPattern=Part->hitPattern();
	d0Hit=0; if( HitPattern&((1<<Trk::pixelEndCap0)) ) d0Hit=1;
	d1Hit=0; if( HitPattern&((1<<Trk::pixelEndCap1)) ) d1Hit=1;
	d2Hit=0; if( HitPattern&((1<<Trk::pixelEndCap2)) ) d2Hit=1;
  }
/*************************************************************************************************************/

  int MultiSecVertexTool::getIdHF(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( !tplink.isValid() ) return 0;
        if( TP->auxdata< float >( "truthMatchProbability" ) < 0.75 ) return 0;
        if( (*tplink)->barcode() > 200000) return 0;
        if( (*tplink)->hasProdVtx()){
          if( (*tplink)->prodVtx()->nIncomingParticles()==1){
             int PDGID1=0, PDGID2=0, PDGID3=0;
	     const xAOD::TruthParticle * parTP1=getPreviousParent(*tplink, PDGID1);
	     const xAOD::TruthParticle * parTP2=0;
	     int noBC1=notFromBC(PDGID1);
             if(noBC1)  parTP2 = getPreviousParent(parTP1, PDGID2);
	     int noBC2=notFromBC(PDGID2);
             if(noBC2 && parTP2) getPreviousParent(parTP2, PDGID3);
	     int noBC3=notFromBC(PDGID3);
             if(noBC1 && noBC2 && noBC3)return 0;
             return 1;  //This is a reconstructed track from B/C decays
      } } }
      return 0;
  }

  int MultiSecVertexTool::notFromBC(int PDGID) const {
    int noBC=0;
    if(PDGID<=0)return 1;
    if(PDGID>600 && PDGID<4000)noBC=1;
    if(PDGID<400 || PDGID>5600)noBC=1;
    if(PDGID==513  || PDGID==523  || PDGID==533  || PDGID==543)noBC=1;  //Remove tracks from B* (they are in PV)
    if(PDGID==5114 || PDGID==5214 || PDGID==5224 || PDGID==5314 || PDGID==5324)noBC=1; //Remove tracks from B_Barions* (they are in PV)
    // PDGID==413,423,433,4114,4214,4224,4314,4324 - tracks from excited charmed mesons and baryons are kept now
    // as if they are produced in B-vertex only. Wrong for prompt production! Should be corrected in future.
    return noBC;
  }
  const xAOD::TruthParticle * MultiSecVertexTool::getPreviousParent(const xAOD::TruthParticle * child, int & ParentPDG) const {
    ParentPDG=0;
    if( child->hasProdVtx() ){
       if( child->prodVtx()->nIncomingParticles()==1 ){
            ParentPDG = std::abs((*(child->prodVtx()->incomingParticleLinks())[0])->pdgId());
            return *(child->prodVtx()->incomingParticleLinks())[0];
       }
    }
    return 0;
  }


  int MultiSecVertexTool::getG4Inter(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( tplink.isValid() && (*tplink)->barcode()>200000) return 1;
      }
      return 0;
  }
  int MultiSecVertexTool::getMCPileup(const xAOD::TrackParticle* TP ) const {
      if( TP->isAvailable< ElementLink< xAOD::TruthParticleContainer> >( "truthParticleLink") ) {
        const ElementLink<xAOD::TruthParticleContainer>& tplink = 
                               TP->auxdata< ElementLink< xAOD::TruthParticleContainer > >("truthParticleLink");
        if( !tplink.isValid() ) return 1;
      } else { return 1; }
      return 0;
  }

  double MultiSecVertexTool::distToMatLayerSignificance(Vrt2Tr & Vrt) const
  {
     if(Vrt.fitVertex.perp()<20.) return 1.e9;
     double normP=1./Vrt.momentum.P();
     Amg::Vector3D momentumP(Vrt.momentum.Px()*normP,Vrt.momentum.Py()*normP,Vrt.momentum.Pz()*normP);
     Amg::Vector3D momentumN=-momentumP;
     
     const Trk::Layer * someLayer  = nullptr;
     const Trk::Layer * nextLayerP = nullptr;
     const Trk::Layer * nextLayerN = nullptr;
     auto volume = m_extrapolator->trackingGeometry()->lowestTrackingVolume(Vrt.fitVertex);
     someLayer = volume->associatedLayer(Vrt.fitVertex);
     auto material =  someLayer->layerMaterialProperties();
     if(material){
       nextLayerP=someLayer;
     } else {
       nextLayerP = someLayer->nextLayer(Vrt.fitVertex,momentumP);
       if(nextLayerP){ if(!nextLayerP->layerMaterialProperties())nextLayerP=0; }
       nextLayerN = someLayer->nextLayer(Vrt.fitVertex,momentumN);
       if(nextLayerN){ if(!nextLayerN->layerMaterialProperties())nextLayerN=0; }
     }
     momentumP *= 1.e5;   //100GeV to have straight trajectory
     double charge = 1.;
     const Trk::Perigee pseudoVrtPart(Vrt.fitVertex, momentumP, charge, Vrt.fitVertex);

     const Trk::TrackParameters * extrapParP=0; //along momentum
     const Trk::TrackParameters * extrapParN=0; //backward
     if(nextLayerP){ extrapParP = m_extrapolator->extrapolate(pseudoVrtPart,
                     nextLayerP->surfaceRepresentation(), Trk::anyDirection, false, Trk::nonInteractingMuon) ;}
     if(nextLayerN){ extrapParN = m_extrapolator->extrapolate(pseudoVrtPart,
                     nextLayerN->surfaceRepresentation(), Trk::anyDirection, false, Trk::nonInteractingMuon) ;}

     float distanceP=1.e9, distanceN=1.e9;
     if(extrapParP)distanceP=PntPntDist(extrapParP->position(), Vrt.fitVertex);
     if(extrapParN)distanceN=PntPntDist(extrapParN->position(), Vrt.fitVertex);
     if(distanceP==1.e9 && distanceN==1.e9) return 1.e9;

     //std::pair<const Trk::TrackParameters*,const Trk::Layer*> next= 
     //         m_extrapolator->extrapolateToNextActiveLayer(pseudoVrtPart,Trk::anyDirection,true,Trk::pion) ;

     double signif=1.e9;
     std::vector<double> pntCovar={1.e-2,0.,1.e-2,0.,0.,4.e-2};
     if(distanceP<distanceN)signif=VrtVrtDist(Vrt.fitVertex, Vrt.errorMatrix, extrapParP->position(), pntCovar);
     else                   signif=VrtVrtDist(Vrt.fitVertex, Vrt.errorMatrix, extrapParN->position(), pntCovar);
     delete extrapParP;
     delete extrapParN;
     return signif;
  }

 

 

}  //end namespace
