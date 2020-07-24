/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalVxInJetTool/InDetVKalVxInJetTool.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include  "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TProfile.h"

#include "TMath.h"
//
//-------------------------------------------------
// Other stuff
//#include<iostream>

//extern "C" {
//  float chisin_(const float & prob, const long int& ndf);             
//}

namespace InDet {

const int InDetVKalVxInJetTool::DevTuple::maxNTrk;

//
//Constructor-------------------------------------------------------------- 
InDetVKalVxInJetTool::InDetVKalVxInJetTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_cutSctHits(4),
    m_cutPixelHits(1),
    m_cutSiHits(7),
    m_cutBLayHits(0),
    m_cutSharedHits(1),
    m_cutPt(700.),
    m_cutZVrt(15.),
    m_cutA0(5.),
    m_cutChi2(5.),
    m_secTrkChi2Cut(10.),
    m_coneForTag(0.4),
    m_sel2VrtChi2Cut(10.0),
    m_sel2VrtSigCut(4.0),
    m_trkSigCut(2.0),
    m_a0TrkErrorCut(1.0),
    m_zTrkErrorCut(5.0),
    m_cutHFClass(0.15),
    m_antiGarbageCut(0.75),
    m_antiFragmentCut(0.70),
    m_Vrt2TrMassLimit(4000.),
    m_fillHist(false),
    m_existIBL(true),
    m_RobustFit(1),
    m_beampipeX (0.),
    m_beampipeY (0.),
    m_xLayerB (0.),
    m_yLayerB (0.),
    m_xLayer1 (0.),
    m_yLayer1 (0.),
    m_xLayer2 (0.),
    m_yLayer2 (0.),
    m_beampipeR (0.),  //Correct values are filled     
    m_rLayerB   (0.),  // in jobO or initialize()
    m_rLayer1   (0.),
    m_rLayer2   (0.),
    m_useVertexCleaning(false),
    m_multiVertex(false),
    m_multiWithPrimary(false),
    m_getNegativeTail(false),
    m_getNegativeTag(false),
    m_multiWithOneTrkVrt(true),
    m_vertexMergeCut(3.),
    m_trackDetachCut(6.),
    m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
//    m_useMaterialRejection(true),
//    m_materialMap ("InDet::InDetMaterialRejTool", this)
//    m_fitSvc("Trk::TrkVKalVrtFitter/VKalVrtFitter",this)
    m_trackClassificator("InDet::InDetTrkInJetType",this)
   {
//
// Declare additional interface
//
    declareInterface< ISecVertexInJetFinder >(this);
// Properties
//
//
    declareProperty("CutSctHits",    m_cutSctHits ,  "Remove track is it has less SCT hits" );
    declareProperty("CutPixelHits",  m_cutPixelHits, "Remove track is it has less Pixel hits");
    declareProperty("CutSiHits",     m_cutSiHits,    "Remove track is it has less Pixel+SCT hits"  );
    declareProperty("CutBLayHits",   m_cutBLayHits,  "Remove track is it has less B-layer hits"   );
    declareProperty("CutSharedHits", m_cutSharedHits,"Reject final 2tr vertices if tracks have shared hits" );

    declareProperty("CutPt",         m_cutPt,     "Track Pt selection cut"  );
    declareProperty("CutA0",         m_cutA0,     "Track A0 selection cut"  );
    declareProperty("CutZVrt",       m_cutZVrt,   "Track Z impact selection cut");
    declareProperty("ConeForTag",    m_coneForTag,"Cone around jet direction for track selection");
    declareProperty("CutChi2",       m_cutChi2,   "Track Chi2 selection cut" );
    declareProperty("TrkSigCut",     m_trkSigCut, "Track 3D impact significance w/r primary vertex" );
    declareProperty("SecTrkChi2Cut", m_secTrkChi2Cut,"Track - common secondary vertex association cut. Single Vertex Finder only");

    declareProperty("A0TrkErrorCut",  m_a0TrkErrorCut, "Track A0 error cut" );
    declareProperty("ZTrkErrorCut",   m_zTrkErrorCut,  "Track Z impact error cut" );
    declareProperty("CutHFClass",     m_cutHFClass,  "Cut on HF classification weight" );
    declareProperty("AntiGarbageCut", m_antiGarbageCut,   "Cut on Garbage classification weight for track removal" );
    declareProperty("AntiFragmentCut",m_antiFragmentCut,  "Cut on Fragmentation classification weight for track removal" );
    declareProperty("Vrt2TrMassLimit",m_Vrt2TrMassLimit,  "Maximal allowed mass for 2-track vertices" );

    declareProperty("Sel2VrtChi2Cut",    m_sel2VrtChi2Cut, "Cut on Chi2 of 2-track vertex for initial selection"  );
    declareProperty("Sel2VrtSigCut",     m_sel2VrtSigCut,  "Cut on significance of 3D distance between initial 2-track vertex and PV"  );

    declareProperty("FillHist",   m_fillHist, "Fill technical histograms"  );
    declareProperty("ExistIBL",   m_existIBL, "Inform whether 3-layer or 4-layer detector is used "  );

    declareProperty("RobustFit",  m_RobustFit, "Use vertex fit with RobustFit functional(VKalVrt) for common secondary vertex fit" );

    declareProperty("Xbeampipe", m_beampipeX);
    declareProperty("Ybeampipe", m_beampipeY);
    declareProperty("XlayerB",   m_xLayerB  );
    declareProperty("YlayerB",   m_yLayerB  );
    declareProperty("Xlayer1",   m_xLayer1  );
    declareProperty("Ylayer1",   m_yLayer1  );
    declareProperty("Xlayer2",   m_xLayer2  );
    declareProperty("Ylayer2",   m_yLayer2  );
    declareProperty("Rbeampipe", m_beampipeR);
    declareProperty("RlayerB",   m_rLayerB  );
    declareProperty("Rlayer1",   m_rLayer1  );
    declareProperty("Rlayer2",   m_rLayer2  );

//    declareProperty("useMaterialRejection",  m_useMaterialRejection, "Reject vertices from hadronic interactions in detector material" );
    declareProperty("useVertexCleaning",     m_useVertexCleaning,    "Clean vertices by requiring pixel hit presence according to vertex position" );

    declareProperty("MultiVertex",        m_multiVertex,       "Run Multiple Secondary Vertices in jet finder"  );
    declareProperty("MultiWithPrimary",   m_multiWithPrimary,  "Find Multiple Secondary Vertices + primary vertex in jet. MultiVertex Finder only!"  );
    declareProperty("MultiWithOneTrkVrt", m_multiWithOneTrkVrt,"Allow one-track-vertex addition to already found secondary vertices. MultiVertex Finder only! ");
    declareProperty("getNegativeTail", m_getNegativeTail, "Allow secondary vertex behind the primary one (negative) w/r jet direction (not for multivertex!)" );
    declareProperty("getNegativeTag",  m_getNegativeTag,  "Return ONLY negative secondary vertices (not for multivertex!)"   );

    declareProperty("VertexMergeCut",	  m_vertexMergeCut, "To allow vertex merging for MultiVertex Finder" );
    declareProperty("TrackDetachCut",	  m_trackDetachCut, "To allow track from vertex detachment for MultiVertex Finder" );

    declareProperty("VertexFitterTool",  m_fitterSvc);
    declareProperty("TrackClassTool",  m_trackClassificator);
//    declareProperty("MaterialMap", m_materialMap);
//    declareProperty("TrkVKalVrtFitter", m_fitSvc);
//
    m_iflag=0;
    m_massPi  = 139.5702 ;
    m_massP   = 938.272  ;
    m_massE   =   0.511  ;
    m_massK0  = 497.648  ;
    m_massLam =1115.683  ;
    m_massB   =5279.400  ;
    m_WorkArray = 0;
    m_compatibilityGraph = nullptr;
    m_instanceName=name;
    m_curTup = 0;

   }

//Destructor---------------------------------------------------------------
    InDetVKalVxInJetTool::~InDetVKalVxInJetTool(){
     if(m_WorkArray) delete m_WorkArray;
     if(m_compatibilityGraph)delete m_compatibilityGraph;
     ATH_MSG_DEBUG("InDetVKalVxInJetTool destructor called");
   }

//Initialize---------------------------------------------------------------
   StatusCode InDetVKalVxInJetTool::initialize(){
     ATH_MSG_DEBUG("InDetVKalVxInJetTool initialize() called");
     m_WorkArray = new VKalVxInJetTemp;
     m_compatibilityGraph = new boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>();

//     IToolSvc* toolSvc;                    //needed for old style TrkVKalVrtFitter retrieval
//     StatusCode sc = service("ToolSvc",toolSvc);
//     if (sc.isFailure()) { 
//        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << " No ToolSvc for InDetVKalVxInJetTool !" << endmsg;
//        return StatusCode::SUCCESS;
//     }

 
     if (m_fitterSvc.retrieve().isFailure()) {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Could not find Trk::TrkVKalVrtFitter" << endmsg;
        return StatusCode::SUCCESS;
     } else {
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDetVKalVxInJetTool TrkVKalVrtFitter found" << endmsg;
     }
     m_fitSvc = dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_fitterSvc));
     if(!m_fitSvc){
        if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG)<<" No implemented Trk::ITrkVKalVrtFitter interface" << endmsg;
        return StatusCode::SUCCESS;
     }
     //if (m_materialMap.retrieve().isFailure()) {
     //   if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "Could not find InDetMaterialRejTool."
     //                                      << "Use radial rejection"<< endmsg;
     //} else {
     //   if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDetMaterialRejTool found" << endmsg;
     //}
//-------
//     Trk::IVertexFitter * tmp;
//     sc = toolSvc->retrieveTool("Trk::TrkVKalVrtFitter",tmp,this);
//     if (sc.isFailure()) { 
//        log << MSG::DEBUG << " No Trk::TrkVKalVrtFitter for InDetVKalVxInJetTool !" << endmsg;
//     }else{
//        m_fitSvc = dynamic_cast<Trk::TrkVKalVrtFitter*>(tmp);
//        if(!m_fitSvc)log<<MSG::DEBUG<<" No Trk::TrkVKalVrtFitter" << endmsg;
//     }

//------------------------------------------
     if(msgLvl(MSG::DEBUG)) ATH_CHECK(service("ChronoStatSvc", m_timingProfile));
//------------------------------------------
// Chose whether IBL is installed
     if(m_existIBL){ // 4-layer pixel detector
       if( m_beampipeR==0.)  m_beampipeR=24.0;    
       if( m_rLayerB  ==0.)  m_rLayerB  =34.0;
       if( m_rLayer1  ==0.)  m_rLayer1  =51.6;
       if( m_rLayer2  ==0.)  m_rLayer2  =90.0;
       m_rLayer3  =122.5;
     } else {   // 3-layer pixel detector
       if( m_beampipeR==0.)  m_beampipeR=29.4;    
       if( m_rLayerB  ==0.)  m_rLayerB  =51.5;
       if( m_rLayer1  ==0.)  m_rLayer1  =90.0;
       if( m_rLayer2  ==0.)  m_rLayer2  =122.5;
     }       
       
//
//
     ITHistSvc*     hist_root=0;
     if(m_fillHist){

       StatusCode sc = service( "THistSvc", hist_root); 
       if( sc.isFailure() ) {
          if(msgLvl(MSG::ERROR))msg(MSG::ERROR)<< "Could not find THistSvc service" << endmsg;
       }
       if(msgLvl(MSG::DEBUG))msg(MSG::DEBUG) << "InDetVKalVxInJetTool Histograms found" << endmsg;
 
       m_hb_massPiPi   = new TH1D("massPiPi"," mass PiPi",160,200., 1000.);
       m_hb_massPiPi1  = new TH1D("massPiPi1"," mass PiPi",100,200., 2000.);
       m_hb_massPPi    = new TH1D("massPPi"," massPPi", 100,1000., 1250.);
       m_hb_massEE     = new TH1D("massEE"," massEE", 100,0., 200.);
       m_hb_nvrt2      = new TH1D("nvrt2"," vertices2", 50,0., 50.);
       m_hb_ratio      = new TH1D("ratio"," ratio", 51,0., 1.02);
       m_hb_totmass    = new TH1D("totmass"," totmass", 250,0., 10000.);
       m_hb_totmassEE  = new TH1D("massEEcomvrt"," totmass EE common vertex", 100,0., 1000.);
       m_hb_totmass2T0 = new TH1D("mass2trcomvrt0"," totmass 2tr common vertex", 800,0., 4000.);
       m_hb_totmass2T1 = new TH1D("mass2trcomvrt1"," totmass 2tr common vertex", 200,0., 10000.);
       m_hb_totmass2T2 = new TH1D("mass2trcomvrt2"," totmass 2tr common vertex", 200,0., 10000.);
       m_hb_impact     = new TH1D("impact", " impact", 100,0., 20.);
       m_hb_impactR    = new TH1D("impactR"," impactR", 400,-30., 70.);
       m_hb_impactZ    = new TH1D("impactZ"," impactZ", 100,-30., 70.);
       m_hb_impactRZ   = new TH2D("impactRZ"," impactRZ", 40,-10., 10., 60, -30.,30. );
       m_hb_trkD0      = new TH1D("trkD0"," d0 of tracks", 100,-20., 20.);
       m_hb_r2d        = new TH1D("r2interact","Interaction radius 2tr selected", 150,0., 150.);
       m_hb_r1dc       = new TH1D("r1interactCommon","Interaction 1tr radius common", 150,0., 150.);
       m_hb_r2dc       = new TH1D("r2interactCommon","Interaction 2tr radius common", 150,0., 150.);
       m_hb_r3dc       = new TH1D("r3interactCommon","Interaction 3tr radius common", 150,0., 150.);
       m_hb_rNdc       = new TH1D("rNinteractCommon","Interaction Ntr radius common", 150,0., 150.);
       m_hb_dstToMat   = new TH1D("dstToMatL","Distance to material layer", 80,0., 40.);
       m_hb_impV0      = new TH1D("impactV0"," V0 impact", 100,0., 50.);
       m_hb_ntrkjet    = new TH1D("ntrkjet"," NTrk in jet", 50,0., 50.);
       m_hb_jmom       = new TH1D("jetmom"," Jet mom", 200,0., 2000000.);
       m_hb_mom        = new TH1D("jetmomvrt"," Jet mom with sec. vertex", 200,0., 2000000.);
       m_hb_signif3D   = new TH1D("signif3D"," Signif3D for initial 2tr vertex", 140,-20., 50.);
       m_hb_sig3DTot   = new TH1D("sig3dcommon"," Signif3D for common vertex", 140,-20., 50.);
       m_hb_sig3D1tr   = new TH1D("sig3D1tr","Signif3D for 1tr  vertices", 140,-20., 50.);
       m_hb_sig3D2tr   = new TH1D("sig3D2tr","Signif3D for 2tr single vertex", 140,-20., 50.);
       m_hb_sig3DNtr   = new TH1D("sig3DNtr","Signif3D for many-tr single vertex", 140,-20., 50.);
       m_hb_goodvrtN   = new TH1F("goodvrtN","Number of good vertices", 20,0., 20.);
       m_hb_distVV     = new TH1D("distvv","Vertex-Vertex dist", 100,0., 20.);
       m_hb_diffPS     = new TH1D("diffPS","Primary-Secondary assoc", 200,-20., 20.);
       m_hb_trkPtMax   = new TH1D("trkPtMax","Maximal track Pt to jet", 100, 0., 5000.);
       m_hb_blshared   = new TH1F("blshared","Number of shared hits in B-layer for R<BL", 5, 0., 5.);
       m_hb_pxshared   = new TH1F("pxshared","Number of shared hits in pixel for R>BL", 5, 0., 5.);
       m_hb_rawVrtN    = new TH1F("rawVrtN","Number of raw vertices multivertex case", 20, 0., 20.);
       m_hb_lifetime   = new TH1F("lifetime","Distance/momentum", 100, 0., 5.);
       m_hb_trkPErr    = new TH1F("trkPErr","Track momentum error for P>10 GeV", 100, 0., 0.5);
       m_hb_deltaRSVPV = new TH1F("deltaRSVPV","SV-PV vs jet dR ", 200, 0., 1.);
//---
       m_pr_NSelTrkMean = new TProfile("NSelTrkMean"," NTracks selected vs jet pt", 80, 0., 1600000.);
       m_pr_effVrt2tr   = new TProfile("effVrt2tr"," 2tr vertex efficiency vs Ntrack", 50, 0., 50.);
       m_pr_effVrt2trEta= new TProfile("effVrt2trEta"," 2tr vertex efficiency vs eta", 50, -3., 3.);
       m_pr_effVrt   = new TProfile("effVrt","Full vertex efficiency vs Ntrack", 50, 0., 50.);
       m_pr_effVrtEta= new TProfile("effVrtEta","Full vertex efficiency vs eta", 50, -3., 3.);
       std::string histDir;
       if(m_multiVertex) histDir="/file1/stat/MSVrtInJet"+m_instanceName+"/";
       else              histDir="/file1/stat/SVrtInJet"+m_instanceName+"/";
       sc = hist_root->regHist(histDir+"massPiPi", m_hb_massPiPi);
       sc = hist_root->regHist(histDir+"massPiPi1", m_hb_massPiPi1);
       sc = hist_root->regHist(histDir+"massPPi", m_hb_massPPi);
       sc = hist_root->regHist(histDir+"massEE", m_hb_massEE );
       sc = hist_root->regHist(histDir+"nvrt2", m_hb_nvrt2);
       sc = hist_root->regHist(histDir+"ratio", m_hb_ratio);
       sc = hist_root->regHist(histDir+"totmass", m_hb_totmass);
       sc = hist_root->regHist(histDir+"massEEcomvrt", m_hb_totmassEE);
       sc = hist_root->regHist(histDir+"mass2trcomvrt0", m_hb_totmass2T0);
       sc = hist_root->regHist(histDir+"mass2trcomvrt1", m_hb_totmass2T1);
       sc = hist_root->regHist(histDir+"mass2trcomvrt2", m_hb_totmass2T2);
       sc = hist_root->regHist(histDir+"impact",    m_hb_impact);
       sc = hist_root->regHist(histDir+"impactR",   m_hb_impactR);
       sc = hist_root->regHist(histDir+"impactZ",   m_hb_impactZ);
       sc = hist_root->regHist(histDir+"impactRZ",  m_hb_impactRZ);
       sc = hist_root->regHist(histDir+"trkD0",     m_hb_trkD0);
       sc = hist_root->regHist(histDir+"r2interact",       m_hb_r2d);
       sc = hist_root->regHist(histDir+"r1interactCommon", m_hb_r1dc);
       sc = hist_root->regHist(histDir+"r2interactCommon", m_hb_r2dc);
       sc = hist_root->regHist(histDir+"r3interactCommon", m_hb_r3dc);
       sc = hist_root->regHist(histDir+"rNinteractCommon", m_hb_rNdc);
       sc = hist_root->regHist(histDir+"dstToMatL", m_hb_dstToMat);
       sc = hist_root->regHist(histDir+"impactV0",  m_hb_impV0);
       sc = hist_root->regHist(histDir+"ntrkjet",   m_hb_ntrkjet);
       sc = hist_root->regHist(histDir+"jetmom",    m_hb_jmom);
       sc = hist_root->regHist(histDir+"jetmomvrt", m_hb_mom);
       sc = hist_root->regHist(histDir+"signif3D",  m_hb_signif3D);
       sc = hist_root->regHist(histDir+"sig3dcommon", m_hb_sig3DTot);
       sc = hist_root->regHist(histDir+"sig3D1tr",  m_hb_sig3D1tr);
       sc = hist_root->regHist(histDir+"sig3D2tr",  m_hb_sig3D2tr);
       sc = hist_root->regHist(histDir+"sig3DNtr",  m_hb_sig3DNtr);
       sc = hist_root->regHist(histDir+"goodvrtN",  m_hb_goodvrtN);
       sc = hist_root->regHist(histDir+"distVV",    m_hb_distVV);
       sc = hist_root->regHist(histDir+"diffPS",    m_hb_diffPS);
       sc = hist_root->regHist(histDir+"trkPtMax",  m_hb_trkPtMax);
       sc = hist_root->regHist(histDir+"blshared",  m_hb_blshared);
       sc = hist_root->regHist(histDir+"pxshared",  m_hb_pxshared);
       sc = hist_root->regHist(histDir+"rawVrtN",   m_hb_rawVrtN);
       sc = hist_root->regHist(histDir+"lifetime",  m_hb_lifetime);
       sc = hist_root->regHist(histDir+"trkPErr",   m_hb_trkPErr);
       sc = hist_root->regHist(histDir+"deltaRSVPV",   m_hb_deltaRSVPV);
       sc = hist_root->regHist(histDir+"NSelTrkMean",  m_pr_NSelTrkMean);
       sc = hist_root->regHist(histDir+"effVrt2tr",    m_pr_effVrt2tr);
       sc = hist_root->regHist(histDir+"effVrt2trEta", m_pr_effVrt2trEta);
       sc = hist_root->regHist(histDir+"effVrt",       m_pr_effVrt);
       sc = hist_root->regHist(histDir+"effVrtEta",    m_pr_effVrtEta);
       if( sc.isFailure() ) {     // Check of StatusCode
         if(msgLvl(MSG::INFO))msg(MSG::INFO) << "BTagVrtSec Histogram registration failure!!!" << endmsg;
       }
       m_w_1 = 1.;
//-------------------------------------------------------
       m_curTup=new DevTuple();
       m_tuple = new TTree("Tracks","Tracks");
       std::string TreeDir;
       if(m_multiVertex) TreeDir="/file1/stat/MSVrtInJet"+m_instanceName+"/";
       else              TreeDir="/file1/stat/SVrtInJet"+m_instanceName+"/";
       sc = hist_root->regTree(TreeDir,m_tuple);
       if (sc.isSuccess()) {
          m_tuple->Branch("ptjet",       &m_curTup->ptjet,     "ptjet/F");
          m_tuple->Branch("etajet",      &m_curTup->etajet,    "etajet/F");
          m_tuple->Branch("phijet",      &m_curTup->phijet,    "phijet/F");
          m_tuple->Branch("ntrk",        &m_curTup->nTrkInJet, "ntrk/I");
          m_tuple->Branch("etatrk",      &m_curTup->etatrk,    "etatrk[ntrk]/F");
          m_tuple->Branch("prbS",        &m_curTup->s_prob,    "prbS[ntrk]/F");
          m_tuple->Branch("prbP",        &m_curTup->p_prob,    "prbP[ntrk]/F");
          m_tuple->Branch("wgtB",        &m_curTup->wgtB,      "wgtB[ntrk]/F");
          m_tuple->Branch("wgtL",        &m_curTup->wgtL,      "wgtL[ntrk]/F");
          m_tuple->Branch("wgtG",        &m_curTup->wgtG,      "wgtG[ntrk]/F");
          m_tuple->Branch("Sig3D",       &m_curTup->Sig3D,     "Sig3D[ntrk]/F");
          m_tuple->Branch("idMC",        &m_curTup->idMC,      "idMC[ntrk]/I");
          m_tuple->Branch("ibl",         &m_curTup->ibl,       "ibl[ntrk]/I");
          m_tuple->Branch("bl",          &m_curTup->bl,        "bl[ntrk]/I");
          m_tuple->Branch("SigR",        &m_curTup->SigR,      "SigR[ntrk]/F");
          m_tuple->Branch("SigZ",        &m_curTup->SigZ,      "SigZ[ntrk]/F");
          m_tuple->Branch("d0",          &m_curTup->d0,        "d0[ntrk]/F");
          m_tuple->Branch("Z0",          &m_curTup->Z0,        "Z0[ntrk]/F");
          m_tuple->Branch("pTvsJet",     &m_curTup->pTvsJet,   "pTvsJet[ntrk]/F");
          m_tuple->Branch("prodTJ",      &m_curTup->prodTJ,    "prodTJ[ntrk]/F");
          m_tuple->Branch("nVrtT",       &m_curTup->nVrtT,     "nVrtT[ntrk]/I");
          m_tuple->Branch("chg",         &m_curTup->chg,       "chg[ntrk]/I");
          //-----
          m_tuple->Branch("TotM",        &m_curTup->TotM,      "TotM/F");
          //-----
          m_tuple->Branch("nvrt",        &m_curTup->nVrt,      "nvrt/I");
          m_tuple->Branch("VrtDist2D",   &m_curTup->VrtDist2D, "VrtDist2D[nvrt]/F");
          m_tuple->Branch("VrtSig3D",    &m_curTup->VrtSig3D,  "VrtSig3D[nvrt]/F");
          m_tuple->Branch("VrtSig2D",    &m_curTup->VrtSig2D,  "VrtSig2D[nvrt]/F");
          m_tuple->Branch("VrtDR",       &m_curTup->VrtDR,     "VrtDR[nvrt]/F");
          m_tuple->Branch("itrk",        &m_curTup->itrk,      "itrk[nvrt]/I");
          m_tuple->Branch("jtrk",        &m_curTup->jtrk,      "jtrk[nvrt]/I");
          m_tuple->Branch("badV",        &m_curTup->badVrt,    "badV[nvrt]/I");
          m_tuple->Branch("mass",        &m_curTup->mass,      "mass[nvrt]/F");
          m_tuple->Branch("Chi2",        &m_curTup->Chi2,      "Chi2[nvrt]/F");
          //-----
          m_tuple->Branch("ntHF",        &m_curTup->NTHF,      "ntHF/I");
          m_tuple->Branch("itHF",        &m_curTup->itHF,      "itHF[ntHF]/I");
          //-----
          m_tuple->Branch("nNVrt",       &m_curTup->nNVrt,      "nNVrt/I");
          m_tuple->Branch("NVrtDist2D",  &m_curTup->NVrtDist2D, "NVrtDist2D[nNVrt]/F");
          m_tuple->Branch("NVrtNT",      &m_curTup->NVrtNT,     "NVrtNT[nNVrt]/I");
          m_tuple->Branch("NVrtTrkI",    &m_curTup->NVrtTrkI,   "NVrttrkI[nNVrt]/I");
          m_tuple->Branch("NVrtM",       &m_curTup->NVrtM,      "NVrtM[nNVrt]/F");
          m_tuple->Branch("NVrtChi2",    &m_curTup->NVrtChi2,   "NVrtChi2[nNVrt]/F");
          m_tuple->Branch("NVrtMaxW",    &m_curTup->NVrtMaxW,   "NVrtMaxW[nNVrt]/F");
          m_tuple->Branch("NVrtAveW",    &m_curTup->NVrtAveW,   "NVrtAveW[nNVrt]/F");
          m_tuple->Branch("NVrtDR",      &m_curTup->NVrtDR,     "NVrtDR[nNVrt]/F");
       }
     }

     if(!m_multiVertex)m_multiWithPrimary = false; 

     if(m_getNegativeTag){
        if(msgLvl(MSG::INFO))msg(MSG::INFO) << " Negative TAG is requested! " << endmsg;
        if(msgLvl(MSG::INFO))msg(MSG::INFO) << "Not compatible with negativeTAIL option, so getNegativeTail is set to FALSE." << endmsg;
        m_getNegativeTail=false;
     }

     //for(int ntv=2; ntv<=10; ntv++) m_chiScale[ntv]=chisin_(0.9,2*ntv-3)/ntv; m_chiScale[0]=m_chiScale[2];
     for(int ntv=2; ntv<=10; ntv++) m_chiScale[ntv]=TMath::ChisquareQuantile(0.9,2.*ntv-3.)/ntv;
     m_chiScale[0]=m_chiScale[2];
     for(int ntv=2; ntv<=10; ntv++) m_chiScale[ntv]/=m_chiScale[0];

     if(m_RobustFit>7)m_RobustFit=7;
     if(m_RobustFit<0)m_RobustFit=0;

     return StatusCode::SUCCESS;

   }




  StatusCode InDetVKalVxInJetTool::finalize()
  {
    //MsgStream log( msgSvc(), name() );
    if(m_timingProfile)m_timingProfile->chronoPrint("InDetVKalVxInJetTool");
    ATH_MSG_DEBUG("InDetVKalVxInJetTool finalize()");
    return StatusCode::SUCCESS; 
  }
  



   Trk::VxSecVertexInfo* InDetVKalVxInJetTool::findSecVertex(const xAOD::Vertex & PrimVrt,
							           const TLorentzVector & JetDir,
						 	           const std::vector<const xAOD::IParticle*> & IInpTrk)
    const  {
    if(m_timingProfile)m_timingProfile->chronoStart("InDetVKalVxInJetTool");
    std::vector<double>     Results;
    std::vector<const xAOD::TrackParticle*>            InpTrk;
    std::vector<const xAOD::TrackParticle*>            SelSecTrk;
    std::vector< std::vector<const xAOD::TrackParticle*> >  SelSecTrkPerVrt;
    std::vector<const xAOD::TrackParticle*>        xaodTrkFromV0;
    std::vector<xAOD::Vertex*> listVrtSec(0);
    double SecVtxMass =   0.;
    double RatioE     =   0.;
    double EnergyJet  =   0.;
    int N2trVertices  =   0 ;
    int NBigImpTrk    =   0 ;
    if(m_curTup){ m_curTup->nVrt=0; m_curTup->nTrkInJet=0; m_curTup->NTHF=0; m_curTup->nNVrt=0;}

    int pseudoVrt = 0;

    InpTrk.clear(); InpTrk.reserve(IInpTrk.size());
    std::vector<const xAOD::IParticle*>::const_iterator   i_itrk;
    for (i_itrk = IInpTrk.begin(); i_itrk < IInpTrk.end(); ++i_itrk) {
          const xAOD::TrackParticle * tmp=dynamic_cast<const xAOD::TrackParticle *> ((*i_itrk));
          if(tmp)InpTrk.push_back(tmp);
    }

    if(m_multiVertex){
      workVectorArrxAOD * tmpVectxAOD=new workVectorArrxAOD();
      tmpVectxAOD->InpTrk.resize(InpTrk.size());
      std::copy(InpTrk.begin(),InpTrk.end(), tmpVectxAOD->InpTrk.begin());
      //////listVrtSec = GetVrtSecMulti(InpTrk,PrimVrt,JetDir,Results,SelSecTrkPerVrt,TrkFromV0);
      listVrtSec = GetVrtSecMulti(tmpVectxAOD,PrimVrt,JetDir,Results);
      SelSecTrkPerVrt.swap(tmpVectxAOD->FoundSecondTracks);
      xaodTrkFromV0.swap(tmpVectxAOD->TrkFromV0);
      delete tmpVectxAOD;
    }else{
       xAOD::Vertex* secVrt = GetVrtSec( InpTrk,PrimVrt,JetDir,Results,SelSecTrk,xaodTrkFromV0);
       if(secVrt != 0) listVrtSec.push_back(secVrt);
       else if(m_fillHist){ m_pr_effVrt->Fill((float)m_NRefPVTrk,0.);
	                    m_pr_effVrtEta->Fill( JetDir.Eta(),0.);}
    }
    if(Results.size()<3) {
       listVrtSec.clear();
    }else{
       SecVtxMass =      Results[0];
       RatioE     =      Results[1];
       N2trVertices  = (int)Results[2];
       NBigImpTrk    = (int)Results[3];
       EnergyJet     =      Results[6];
       if( Results[2]==0 && Results[4]==0 ) pseudoVrt=1;
    }

    std::vector<const xAOD::IParticle*>  iparTrkFromV0(0); 
    for(int i=0; i<(int)xaodTrkFromV0.size(); i++)iparTrkFromV0.push_back(xaodTrkFromV0[i]);

    Trk::VxSecVKalVertexInfo* res;
    if(pseudoVrt){
      res =  new Trk::VxSecVKalVertexInfo(listVrtSec[0], SecVtxMass, RatioE, NBigImpTrk, iparTrkFromV0 );
    }else{
      res =  new Trk::VxSecVKalVertexInfo(listVrtSec, SecVtxMass, RatioE, N2trVertices, EnergyJet, iparTrkFromV0 );
      if(Results.size()>8)res->setDstToMatLay(Results[7]);
    }


    if(m_fillHist){  m_tuple->Fill(); };
    m_compatibilityGraph->clear();
    std::vector<int> zytmp(1000); m_WorkArray->m_Incomp.swap(zytmp);    // Deallocate memory
    std::vector<int> zwtmp(0);    m_WorkArray->m_Prmtrack.swap(zwtmp);  // 
    if(m_timingProfile)m_timingProfile->chronoStop("InDetVKalVxInJetTool");
    return res;
   }




}  // end InDet namespace
