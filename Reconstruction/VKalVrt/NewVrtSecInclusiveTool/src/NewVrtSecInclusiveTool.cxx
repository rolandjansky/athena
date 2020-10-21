/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///
///  @author  Vadim Kostyukhin <vadim.kostyukhin@cern.ch>
///


// Header include
#include "NewVrtSecInclusiveTool/NewVrtSecInclusiveTool.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "PathResolver/PathResolver.h"
 
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "MVAUtils/BDT.h" 
//


namespace Rec {

//
//Constructor-------------------------------------------------------------- 
NewVrtSecInclusiveTool::NewVrtSecInclusiveTool(const std::string& type,
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
    m_fastZSVCut(15.),
    m_fillHist(false),
    m_useVertexCleaning(true),
    m_multiWithOneTrkVrt(true),
    m_calibFileName("Fake2TrVertexReject.MVA.v01.root"),
    m_SV2T_BDT(nullptr),
    m_beamService("BeamCondSvc",name),
    m_fitSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this)
   {
//
// Declare additional interface
//
    declareInterface< IVrtInclusive >(this);
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
    declareProperty("CutChi2",       m_cutChi2,   "Track Chi2 selection cut" );
    declareProperty("TrkSigCut",     m_trkSigCut, "Track 3D impact significance w/r primary vertex. Should be >=AntiPileupSigRCut" );

    declareProperty("A0TrkErrorCut",  m_a0TrkErrorCut,  "Track A0 error cut" );
    declareProperty("ZTrkErrorCut",   m_zTrkErrorCut,   "Track Z impact error cut" );
    declareProperty("VrtMassLimit",   m_VrtMassLimit,   "Maximal allowed mass for found vertices" );
    declareProperty("Vrt2TrMassLimit",m_Vrt2TrMassLimit,"Maximal allowed mass for 2-track vertices" );
    declareProperty("Vrt2TrPtLimit",  m_Vrt2TrPtLimit,  "Maximal allowed Pt for 2-track vertices. Calibration limit" );

    declareProperty("Sel2VrtProbCut",    m_sel2VrtProbCut, "Cut on probability of 2-track vertex for initial selection"  );
    declareProperty("GlobVrtProbCut",    m_globVrtProbCut, "Cut on probability of any vertex for final selection"  );
    declareProperty("MaxSVRadiusCut",    m_maxSVRadiusCut, "Cut on maximal radius of SV (def = Pixel detector size)"  );
    declareProperty("SelVrtSigCut",      m_selVrtSigCut,  "Cut on significance of 3D distance between vertex and PV"  );
    declareProperty("AntiPileupSigRCut", m_antiPileupSigRCut,  "Upper cut on significance of 2D distance between beam and perigee"  );
    declareProperty("dRdZRatioCut",      m_dRdZRatioCut,  "Cut on dR/dZ ratio to remove pileup tracks"  );
    declareProperty("v2tIniBDTCut",      m_v2tIniBDTCut,  "Initial BDT cut for 2track vertices selection "  );
    declareProperty("v2tFinBDTCut",      m_v2tFinBDTCut,  "Final BDT cut for 2track vertices selection "  );
    declareProperty("FastZSVCut",        m_fastZSVCut,  "Cut to remove SV candidates based on fast SV estimation. To save full fit CPU."  );

    declareProperty("FillHist",   m_fillHist, "Fill technical histograms"  );


    declareProperty("useVertexCleaning",  m_useVertexCleaning,    "Clean vertices by requiring pixel hit presence according to vertex position" );

    declareProperty("MultiWithOneTrkVrt", m_multiWithOneTrkVrt,"Allow one-track-vertex addition to already found secondary vertices");

    declareProperty("VertexMergeCut",	  m_vertexMergeCut, "To allow vertex merging for MultiVertex Finder" );
    declareProperty("TrackDetachCut",	  m_trackDetachCut, "To allow track from vertex detachment for MultiVertex Finder" );

    declareProperty("beampipeR",	  m_beampipeR, "Radius of the beampipe material" );
    declareProperty("removeTrkMatSignif", m_removeTrkMatSignif, "Significance of Vertex-TrackingMaterial distance for removal. No removal if <=0." );

    declareProperty("calibFileName", m_calibFileName, " MVA calibration file for 2-track fake vertices removal" );

    declareProperty("BeamSpotSvc",         m_beamService, "Name of the BeamSpot service");
    declareProperty("VertexFitterTool",    m_fitSvc, "Name of the Vertex Fitter tool");
//
    m_massPi  =  Trk::ParticleMasses().mass[Trk::pion];
    m_massP   =  Trk::ParticleMasses().mass[Trk::proton];
    m_massE   =  Trk::ParticleMasses().mass[Trk::electron];
    m_massK0  =  Trk::ParticleMasses().mass[Trk::k0];
    m_massLam =  1115.683  ;
    m_compatibilityGraph = nullptr;
    m_instanceName=name;

   }

//Destructor---------------------------------------------------------------
    NewVrtSecInclusiveTool::~NewVrtSecInclusiveTool(){
     ATH_MSG_DEBUG("NewVrtSecInclusiveTool destructor called");
   }

//Initialize---------------------------------------------------------------
   StatusCode NewVrtSecInclusiveTool::initialize(){
     ATH_MSG_DEBUG( "Initialising NewVrtSecInclusiveTool- Package version: " << PACKAGE_VERSION ); 
     m_compatibilityGraph = new boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS>();

     ATH_CHECK( m_beamService.retrieve() );
     ATH_CHECK( m_extrapolator.retrieve() );

     ATH_CHECK( m_fitSvc.retrieve() );
     ATH_MSG_DEBUG("NewVrtSecInclusiveTool TrkVKalVrtFitter found");

//------------------------------------------       
//
     ITHistSvc*     hist_root=0;
     if(m_fillHist){

       StatusCode sc = service( "THistSvc", hist_root); 
       if( sc.isFailure() )  ATH_MSG_DEBUG("Could not find THistSvc service");
       else                  ATH_MSG_DEBUG("NewVrtSecInclusiveTool Histograms found");
 
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
//---
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
       //------
       m_curTup=new DevTuple();
       m_tuple = new TTree("Vertices","Vertices");
       std::string TreeDir;
       TreeDir="/file1/stat/MultiSVrt"+m_instanceName+"/";
       sc = hist_root->regTree(TreeDir,m_tuple);
       if (sc.isSuccess()) {
          m_tuple->Branch("ntrk",       &m_curTup->nTrk,    "ntrk/I");
          m_tuple->Branch("pttrk",      &m_curTup->pttrk,   "pttrk[ntrk]/F");
          m_tuple->Branch("d0trk",      &m_curTup->d0trk,   "d0trk[ntrk]/F");
          m_tuple->Branch("Sig3D",      &m_curTup->Sig3D,   "Sig3D[ntrk]/F");
          m_tuple->Branch("idHF",       &m_curTup->idHF,    "idHF[ntrk]/I");

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

//--------------------------------------------------------
     //std::string fileName="NewVrtSecInclusiveTool/Fake2TrVertexReject.MVA.v01.root";   ///For local calibration file
     //std::string rootFilePath = PathResolver::find_file(fileName, "DATAPATH");         ///
     std::string rootFilePath = PathResolver::find_calib_file("NewVrtSecInclusiveTool/"+m_calibFileName);
     TFile* rootFile = TFile::Open(rootFilePath.c_str(), "READ");    
     if (!rootFile) {
        ATH_MSG_FATAL("Could not retrieve root file: " << m_calibFileName);
        return StatusCode::FAILURE;
     }
     TTree * training = (TTree*)rootFile->Get("BDT");
     m_SV2T_BDT = std::make_unique<MVAUtils::BDT>(training);
//--------------------------------------------------------
     return StatusCode::SUCCESS;

   }




  StatusCode NewVrtSecInclusiveTool::finalize()
  {
    if(m_compatibilityGraph)delete m_compatibilityGraph;
    ATH_MSG_DEBUG("NewVrtSecInclusiveTool finalize()");
    return StatusCode::SUCCESS; 
  }
  



  std::unique_ptr<Trk::VxSecVertexInfo> NewVrtSecInclusiveTool::findAllVertices (
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


}  // end Rec namespace
