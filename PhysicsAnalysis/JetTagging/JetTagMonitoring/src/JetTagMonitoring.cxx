/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/JetContainer.h"   
#include "xAODJet/JetAttributes.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "JetTagMonitoring/JetTagMonitoring.h"
#include "JetTagTools/TrackSelector.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"    
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxSecVertexInfo.h"

#include "LArRecEvent/LArEventBitInfo.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TEfficiency.h"

#include <vector>
#include <string>
#include <sstream>

//** --------------------------------------------------------------------------------------------------------------- **//

// Private methods

namespace {

  template< class T >
  bool retrieveCollection(const ServiceHandle<StoreGateSvc>& sg, const DataHandle<T>& coll, std::string key ) {

    StatusCode sc;
    sc = sg->retrieve( coll, key );

    if (!sc.isSuccess())
      return false;

    return true;
  }

} // unnamed namespace


//** --------------------------------------------------------------------------------------------------------------- **//


JetTagMonitoring::JetTagMonitoring(const std::string & type, const std::string & name, const IInterface* parent) :
  ManagedMonitorToolBase(type, name, parent),
  m_storeGate( "StoreGateSvc", name ),
  m_trackSelectorTool("Analysis::TrackSelector"),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_histogramsCreated(false),
  m_switch_off(false),
  m_pixelCondSummarySvc("PixelConditionsSummarySvc",name)
{

  // Properties already defined in the jobOptions
  //START
  declareProperty("JetContainer",           m_jetName           = "AntiKt4EMTopoJets");
  declareProperty("SkipJetQuality",         m_skipJetQuality    = false);
  declareProperty("TrackParticleContainer", m_trackParticleName = "InDetTrackParticles");
  declareProperty("PrimaryVertexContainer", m_primaryVertexName = "PrimaryVertices");
  declareProperty("ElectronContainer",      m_electronName      = "Electrons");
  declareProperty("MuonContainer",          m_muonName          = "Muons" );

  declareProperty("DQcuts", m_do_cuts = true);

  declareProperty("PriVtxTrkMinCut", m_pri_vtx_trk_min_cut = 4 );
  declareProperty("D0_min_cuts", m_trk_d0_min_cut = 0);
  declareProperty("D0_max_cuts", m_trk_d0_max_cut = 1);
  declareProperty("JetPt_cuts", m_jet_pt_cut = 15.0); // in GeV
  declareProperty("JetEta_cuts", m_jet_eta_cut = 2.5);
  declareProperty("nTrk_cuts", m_trk_n = 1);

  declareProperty("MV_algorithmName", m_mv_algorithmName="MV2c10");
  declareProperty("MV_nBins", m_mv_nBins=100);
  declareProperty("MV_rangeStart", m_mv_rangeStart=-1.);
  declareProperty("MV_rangeStop", m_mv_rangeStop=1.);
  declareProperty("MV_cFraction", m_mv_cFrac=0.08);//DL1
  declareProperty("MV_60_cut", m_mv_60_weight_cut=0.94);
  declareProperty("MV_70_cut", m_mv_70_weight_cut=0.83);
  declareProperty("MV_77_cut", m_mv_77_weight_cut=0.64);
  declareProperty("MV_85_cut", m_mv_85_weight_cut=0.11);

  declareProperty("SV1IP3D_weight_cut", m_sv1ip3d_weight_cut=0.);
  //END

  declareProperty("UseTrackSelector", m_use_trackselector = true);
  declareProperty("UseTrigDecisionTool", m_use_trigdectool = true);
  declareProperty("doLumiBlock",     m_doLumiBlock=false);
  declareProperty("doStatus",        m_doStatus = false);

  declareProperty( "JetEtaCut", m_JetEtaCut = 2.5 );
  declareProperty( "JetPtCut", m_JetPtCut = 25.0 );
  declareProperty( "SoftMuonPtCut", m_softMuonPtCut = 5.0 );

  declareProperty( "ElectronPtCut", m_ElectronPtCut = 25.0 );
  declareProperty( "MuonPtCut", m_MuonPtCut = 25.0 );
  declareProperty( "ElectronEtaCut", m_ElectronEtaCut = 2.47 );
  declareProperty( "MuonEtaCut", m_MuonEtaCut = 2.5 );
  declareProperty( "ElectronEtaCrackLowCut", m_ElectronEtaCrackLowCut = 1.37 );
  declareProperty( "ElectronEtaCrackHighCut", m_ElectronEtaCrackHighCut = 1.52 );

  declareProperty( "ElectronTopoEtCone20Cut", m_ElectronTopoEtCone20Cut = 0.06 );// corresponds to the FixedTightCut working point
  declareProperty( "MuonTopoEtCone20Cut", m_MuonTopoEtCone20Cut = 0.06 );// corresponds to the FixedTightCut working point
  declareProperty( "ElectronPtVarCone20Cut", m_ElectronPtVarCone20Cut = 0.06 );// corresponds to the FixedTightCut working point
  declareProperty( "MuonPtVarCone30Cut", m_MuonPtVarCone30Cut = 0.06 );// corresponds to the FixedTightCut working point  

  declareProperty( "ElectronTrigger_201X", m_ElectronTrigger_201X = "HLT_e[2-9][0-9]_.*" ); // e20-e99 triggers
  declareProperty( "MuonTrigger_201X", m_MuonTrigger_201X = "HLT_mu[2-9][0-9].*" ); // mu20-mu99 triggers 
  //declareProperty( "MuonTrigger_201X", m_MuonTrigger_201X = "HLT_mu.*" ); // all mu triggers (including mu4 for Special Runs)
  //declareProperty( "JetTrigger_201X", m_JetTrigger_201X = "HLT_j15" ); //extra jet triggers
  
  m_isNewLumiBlock = false;
}

//** --------------------------------------------------------------------------------------------------------------- **//

JetTagMonitoring::~JetTagMonitoring() {}

//** --------------------------------------------------------------------------------------------------------------- **//

StatusCode JetTagMonitoring::registerHist(MonGroup& theGroup, TH1* h1) {

  ATH_MSG_VERBOSE("in JetTagMonitoring::registerHist " << h1->GetName());

  StatusCode sc = theGroup.regHist(h1);
  if (! sc.isSuccess())
    ATH_MSG_WARNING("Could not register histogram ");

  return sc;
}

//** --------------------------------------------------------------------------------------------------------------- **//

StatusCode JetTagMonitoring::registerHist(MonGroup& theGroup, LWHist* h1) {

  ATH_MSG_VERBOSE("in JetTagMonitoring::registerHist " << h1->GetName());

  StatusCode sc = theGroup.regHist(h1);
  if (!sc.isSuccess())
    ATH_MSG_WARNING("Could not register histogram ");

  return sc;
}

//** --------------------------------------------------------------------------------------------------------------- **//

StatusCode JetTagMonitoring::initialize() {

  ATH_MSG_INFO("in JetTagMonitoring::initialize");

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (! sc.isSuccess()) {
    ATH_MSG_WARNING("Unable to initialize ManagedMonitorToolBase");
    return sc;
  }

  sc = m_storeGate.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Unable to retrieve StoreGateSvc handle");
    m_switch_off = true;
  }

  if ( m_use_trackselector ) {
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_WARNING("Failed to retrieve tool " << m_trackSelectorTool);
      m_switch_off = true;
    }
  } else {
    ATH_MSG_INFO("Analysis::TrackSelector not used");
  }

  if ( m_switch_off )
    ATH_MSG_WARNING("Switching off JetTagMonitoring::fillHistograms()");

  m_lumiBlockNum = 0;

  return StatusCode::SUCCESS;
}

//** --------------------------------------------------------------------------------------------------------------- **//

StatusCode JetTagMonitoring::bookHistograms() {

  ATH_MSG_DEBUG("bookHistograms()");

  m_isNewLumiBlock = newLumiBlockFlag();
  if ( !newRunFlag() ) {
    // Only book run-length histogram
    return StatusCode::SUCCESS;
  }


  if ( (m_environment != AthenaMonManager::tier0) &&
       (m_environment != AthenaMonManager::tier0ESD) &&
       (m_environment != AthenaMonManager::online) ) {
    // Only produce histograms from ESD, or from RAW if ESD-only monitoring is not enabled
    ATH_MSG_INFO("Monitoring environment is \'" << m_environment << "\', not booking histograms");
    //   return StatusCode::SUCCESS;
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    ATH_MSG_INFO("Cosmic-ray data, not booking histograms");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_INFO("Monitoring environment is \'" << m_environment << "\', data type is \'" << m_dataType << "\', booking histograms now.");


  m_monGr_shift = new MonGroup( this, "JetTagging", run );
  m_monGr_LowStat = new MonGroup( this, "JetTagging", lowStat );

  ////////////////////////
  // General histograms //
  ////////////////////////

  registerHist(*m_monGr_shift, m_jet_n = TH1F_LW::create("jet_n","Number of Jets; Number of jets",50,0.,50.));
  registerHist(*m_monGr_shift, m_jet_et  = TH1F_LW::create("jet_et","Jet pT; pT [GeV]",100,0.,500.)); // this is actually filled with pT although called et...
  registerHist(*m_monGr_shift, m_jet_eta = TH1F_LW::create("jet_eta","Jet #eta; #eta",100,-5.,5.));
  registerHist(*m_monGr_shift, m_jet_phi = TH1F_LW::create("jet_phi","Jet #phi; #phi",100,-3.15,3.15));

  registerHist(*m_monGr_shift, m_global_nPrimVtx = TH1F_LW::create("global_nPrimVtx","Number of Primary Vertices; Number of PVs",50,0.,50.));
  registerHist(*m_monGr_shift, m_global_xPrimVtx = TH1F_LW::create("global_xPrimVtx","Primary Vertex x Position; PV x [mm]",40,-2.0,2.0));
  registerHist(*m_monGr_shift, m_global_yPrimVtx = TH1F_LW::create("global_yPrimVtx","Primary Vertex y Position; PV y [mm]",40,-2.0,2.0));
  registerHist(*m_monGr_shift, m_global_zPrimVtx = TH1F_LW::create("global_zPrimVtx","Primary Vetex z Position; PV z [mm]",100,-250.,250.));
  registerHist(*m_monGr_shift, m_priVtx_trks = TH1F_LW::create("priVtx_trks","Number of Tracks in PV; Number of tracks",150,-0.5,149.5));

  registerHist(*m_monGr_shift, m_trackParticle_n = TH1F_LW::create("NTrackParticle","Number of TrackParticles; Number of tracks",100,0.,1000.));
  registerHist(*m_monGr_shift, m_global_BLayerHits = TH1F_LW::create("global_BLayerHits","Number of IBL Hits on TrackParticle; Number of IBL hits",5,0.,5.));
  registerHist(*m_monGr_shift, m_global_PixelHits = TH1F_LW::create("global_PixelHits","Number of PIX Hits on TrackParticle; Number of PIX hits",10,0.,10.));
  registerHist(*m_monGr_shift, m_global_SCTHits = TH1F_LW::create("global_SCTHits","Number of SCT Hits on TrackParticle; Number of SCT hits",15,0.,15.));
  registerHist(*m_monGr_shift, m_global_SiHits = TH1F_LW::create("global_SiHits","Number of PIX+SCT Hits on TrackParticle; Number of PIX+SCT hits",25,0.,25.));
  registerHist(*m_monGr_shift, m_global_TRTHits = TH1F_LW::create("global_TRTHits","Number of TRT Hits on TrackParticle; Number of TRT hits",100,0.,100.));

  registerHist(*m_monGr_shift, m_jet_tracks_n = TH1F_LW::create("jet_tracks_n","Number of Tracks per Jet; Number of tracks",50,0.,50.));
  registerHist(*m_monGr_shift, m_jet_tracks_pt = TH1F_LW::create("jet_tracks_pt","pT of Tracks in Jets; pT [GeV]",100,0.,100.));
  registerHist(*m_monGr_shift, m_jet_tracks_eta = TH1F_LW::create("jet_tracks_eta","#eta of Tracks in Jets; #eta",100,-2.5,2.5));
  registerHist(*m_monGr_shift, m_jet_tracks_phi = TH1F_LW::create("jet_tracks_phi","#phi of Tracks in Jets; #phi",100,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_tracks_d0 = TH1F_LW::create("jet_tracks_d0","d0 of Tracks in Jets; d0 [mm]",100,-5.,5.));
  registerHist(*m_monGr_shift, m_jet_tracks_z0 = TH1F_LW::create("jet_tracks_z0","z0 of Tracks in Jets; z0 [mm]",100,-300.,300.));
  registerHist(*m_monGr_shift, m_jet_tracks_BLayerHits = TH1F_LW::create("jet_tracks_hits_BLayer","Number of IBL Hits on Tracks in Jets; Number of IBL hits",5,0.,5.));
  registerHist(*m_monGr_shift, m_jet_tracks_PixelHits = TH1F_LW::create("jet_tracks_hits_Pixel","Number of PIX Hits on Tracks in Jets; Number of PIX hits",10,0.,10.));
  registerHist(*m_monGr_shift, m_jet_tracks_SCTHits = TH1F_LW::create("jet_tracks_hits_SCT","Number of SCT Hits on Tracks in Jets; Number of SCT hits",15,0.,15.));

  ////////////////////////
  // Trigger histograms //
  ////////////////////////

  registerHist(*m_monGr_shift, m_trigPassed = TH1F_LW::create("trigPassed","Number of events passing trigger chains",5,-0.5,4.5));
  
  m_trigPassed->GetXaxis()->SetBinLabel(1,"All") ;

  // 201X menu triggers
  m_trigPassed->GetXaxis()->SetBinLabel(2,m_ElectronTrigger_201X.c_str());
  m_trigPassed->GetXaxis()->SetBinLabel(3,m_MuonTrigger_201X.c_str());
  m_trigPassed->GetXaxis()->SetBinLabel(4,(m_ElectronTrigger_201X+"_OR_"+m_MuonTrigger_201X).c_str());
  m_trigPassed->GetXaxis()->SetBinLabel(5,(m_ElectronTrigger_201X+"_AND_"+m_MuonTrigger_201X).c_str());
  //m_trigPassed->GetXaxis()->SetBinLabel(6,m_JetTrigger_201X.c_str());

  ///////////////////////
  // Cutflow hitograms //
  ///////////////////////

  registerHist(*m_monGr_shift, m_cutflow = TH1F_LW::create("DQ_Cutflow","Number of events passing cuts",10,-0.5,9.5));

  m_cutflow->GetXaxis()->SetBinLabel(1,"All");
  m_cutflow->GetXaxis()->SetBinLabel(2,"Good LAr");
  m_cutflow->GetXaxis()->SetBinLabel(3,"PV Present");
  m_cutflow->GetXaxis()->SetBinLabel(4,"PV Ntrk");
  m_cutflow->GetXaxis()->SetBinLabel(5,"Trigger");

  registerHist(*m_monGr_shift, m_cutflow_jet = TH1F_LW::create("Jet_Cutflow","Number of jets passing cuts",10,-0.5,9.5));

  m_cutflow_jet->GetXaxis()->SetBinLabel(1,"All");
  m_cutflow_jet->GetXaxis()->SetBinLabel(2,"Quality Cuts");
  m_cutflow_jet->GetXaxis()->SetBinLabel(3,"Kinematic Cuts");
  m_cutflow_jet->GetXaxis()->SetBinLabel(4,"JVT Cut");
  m_cutflow_jet->GetXaxis()->SetBinLabel(5,"Muon Overlap");
  m_cutflow_jet->GetXaxis()->SetBinLabel(6,"Taggable");
  m_cutflow_jet->GetXaxis()->SetBinLabel(7,"Taggable Good");
  m_cutflow_jet->GetXaxis()->SetBinLabel(8,"Taggable Suspect");

  ////////////////////////////
  // Taggability histograms //
  ////////////////////////////

  registerHist(*m_monGr_shift, m_track_selector_eff = TH2F_LW::create("track_selector_eff", "Fraction of Taggable Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_track_selector_all = TH2F_LW::create("track_selector_all", "Number of Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_track_selector_suspect = TH2F_LW::create("track_selector_suspect", "Number of Taggable Suspect Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  //////////////////////
  //  JVT histograms  //
  //////////////////////

  registerHist(*m_monGr_shift, m_jvt =  TH1F_LW::create("jvt_jet","JVT distribution for Jets; JVT",100,-1.,1.));
  registerHist(*m_monGr_shift, m_mv_pre_jvt_cut =  TH1F_LW::create("jet_MV_pre_jvt_cut",(m_mv_algorithmName+" Tag Weight (Jets before JVT); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  registerHist(*m_monGr_shift, m_mv_post_jvt_cut =  TH1F_LW::create("jet_MV_post_jvt_cut",(m_mv_algorithmName+" Tag Weight (Jets after JVT); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  
  ////////////////////
  // SMT histograms //
  ////////////////////
  
  registerHist(*m_monGr_shift, m_n_soft_mu =  TH1F_LW::create("n_soft_mu","Number of Soft Muons per Jet; Number of muons",3,-0.5,2.5));
  registerHist(*m_monGr_shift, m_n_smt_jet =  TH1F_LW::create("n_smt_jet","Number of Jets with 1 Soft Muon per event; Number of jets",3,-0.5,2.5));

  registerHist(*m_monGr_shift, m_smt_dr =  TH1F_LW::create("smt_dr","DeltaR between Jet and Soft Muon; DR",40,0.,0.4));
  registerHist(*m_monGr_shift, m_smt_mu_pt =  TH1F_LW::create("smt_mu_pt","pT of Soft Muons; pT [GeV]",50,0.,100.));
  registerHist(*m_monGr_shift, m_smt_jet_pt =  TH1F_LW::create("smt_jet_pt","pT of SMT Jets (with 1 Soft Muon); pT [GeV]",50,0.,150.));
  registerHist(*m_monGr_shift, m_smt_jet_pt_ratio =  TH1F_LW::create("smt_jet_pt_ratio","pT ratio of SMT / Jet; pT ratio",50,0.,1.));
  registerHist(*m_monGr_shift, m_smt_jet_pt_rel =  TH1F_LW::create("smt_jet_pt_rel","pT rel of SMT vs Jet; pT rel",50,0.,5.));
  registerHist(*m_monGr_shift, m_smt_jet_mv_w =  TH1F_LW::create("smt_jet_MV_w",(m_mv_algorithmName+" Tag Weight (SMT Jets); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));

  ///////////////////////
  // Pileup histograms //
  ///////////////////////

  registerHist(*m_monGr_shift, m_n_mu =  TH1F_LW::create("n_mu","Number of pile up interactions; <#mu>",100,-0.5,99.5));

  registerHist(*m_monGr_shift, m_tag_mv_w_mu0_30   = TH1F_LW::create("tag_MV_w_mu0_30"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, mu = [0,30]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  registerHist(*m_monGr_shift, m_tag_mv_w_mu30_50   = TH1F_LW::create("tag_MV_w_mu30_50"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, mu = [30,50]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  registerHist(*m_monGr_shift, m_tag_mv_w_mu50_100   = TH1F_LW::create("tag_MV_w_mu50_100"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, mu = [50,100]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
	       
  registerHist(*m_monGr_shift, m_tag_mv_w_tracks0_10   = TH1F_LW::create("tag_MV_w_tracks0_10"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, tracks = [0,10]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  registerHist(*m_monGr_shift, m_tag_mv_w_tracks10_20   = TH1F_LW::create("tag_MV_w_tracks10_20"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, tracks = [10,20]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));
  registerHist(*m_monGr_shift, m_tag_mv_w_tracks20_50   = TH1F_LW::create("tag_MV_w_tracks20_50"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, tracks = [20,50]); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));

  registerHist(*m_monGr_shift, m_mu_vs_n_tracks   = TH2F_LW::create("m_mu_vs_n_tracks"  ,"2D mu vs jet n tracks; Number of tracks; <#mu>",50,-0.5,49.5,100,-0.5,99.5));
  registerHist(*m_monGr_shift, m_prof_n_tracks_vs_mu   = TProfile_LW::create("m_prof_n_tracks_vs_mu"  ,"Profile n tracks vs mu; <#mu>; Number of tracks",100,-0.5,99.5,-0.5,49.5));

  ////////////////////
  // Top histograms //
  ////////////////////

  registerHist(*m_monGr_shift, m_n_iso_el =  TH1F_LW::create("n_iso_el","Number of Isolated Electrons; Number of electrons",3,-0.5,2.5));
  registerHist(*m_monGr_shift, m_n_iso_mu =  TH1F_LW::create("n_iso_mu","Number of Isolated Muons; Number of muons",3,-0.5,2.5));

  registerHist(*m_monGr_shift, m_jet_top = TH1F_LW::create("jet_top","Number of Jets in Top Events; Number of jets",1,-0.5,0.5));
  registerHist(*m_monGr_shift, m_jet_pt_top = TH1F_LW::create("jet_pt_top","pT of Jets in Top Events, pT [GeV]; pT",10,0.,200.));
  registerHist(*m_monGr_shift, m_jet_mv_w_top = TH1F_LW::create("jet_MV_top",(m_mv_algorithmName+" Tag Weight of Jets in Top Events; "+m_mv_algorithmName+" tag weight").c_str(),int(m_mv_nBins/2),m_mv_rangeStart,m_mv_rangeStop));

  registerHist(*m_monGr_shift, m_jet_top_tagged = TH1F_LW::create("jet_top_tagged","Number of b-tagged Jets in Top Events",1,-0.5,0.5));
  registerHist(*m_monGr_shift, m_jet_pt_top_tagged = TH1F_LW::create("jet_pt_top_tagged","pT of b-tagged Jets in Top Events; pT [GeV]",10,0.,200.));

  registerHist(*m_monGr_shift, m_jet_top_eff = TH1F_LW::create("jet_top_eff","Fraction of b-tagged Jets in Top Events",1,-0.5,0.5));
  registerHist(*m_monGr_shift, m_jet_pt_top_eff = TH1F_LW::create("jet_pt_top_eff","Fraction of b-tagged Jets in Top Events vs pT; pT [GeV]",10,0.,200.));

  //////////////////////
  // 2D jet hisograms //
  //////////////////////

  registerHist(*m_monGr_shift, m_jet_2D_all = TH2F_LW::create("jet_2D_all", "Number of Jets (No Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_good = TH2F_LW::create("jet_2D_good", "Number of Jets (Quality Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_kinematic = TH2F_LW::create("jet_2D_kinematic", "Number of Jets (Kinematic Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_mjvt = TH2F_LW::create("jet_2D_mjvt", "Number of Jets (JVT Cut);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_overlap = TH2F_LW::create("jet_2D_overlap", "Number of Jets (Muon Overlap Cut);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_quality = TH2F_LW::create("jet_2D_quality", "Number of Jets (Taggable Good);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_suspect = TH2F_LW::create("jet_2D_suspect", "Number of Jets (Taggable Suspect);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_tbad = TH2F_LW::create("jet_2D_tbad", "Number of Jets (Non-Taggable);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_jet_2D_tsmt = TH2F_LW::create("jet_2D_tsmt", "Number of SMT Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  ///////////////////////////
  // Tag weight histograms //
  ///////////////////////////

  // High level tagger: MV2/DL1

  registerHist(*m_monGr_shift, m_tag_mv_w  = TH1F_LW::create("tag_MV_w",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  
  registerHist(*m_monGr_shift, m_tag_mv_w_pT10_20   = TH1F_LW::create("tag_MV_w_pT10_20"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [10,20] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_pT20_50   = TH1F_LW::create("tag_MV_w_pT20_50"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [20,50] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_pT50_100  = TH1F_LW::create("tag_MV_w_pT50_100" ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [50,100] GeV); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_pT100_200 = TH1F_LW::create("tag_MV_w_pT100_200",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [100,200] GeV); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_pT200     = TH1F_LW::create("tag_MV_w_pT200"    ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT > 200 GeV); "+m_mv_algorithmName+" tag weight").c_str()      ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
	       
  registerHist(*m_monGr_shift, m_tag_mv_w_eta0_05  = TH1F_LW::create("tag_MV_w_eta0_05" ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#eta| = [0.0,0.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_eta05_10 = TH1F_LW::create("tag_MV_w_eta05_10",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#eta| = [0.5,1.0]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_eta10_15 = TH1F_LW::create("tag_MV_w_eta10_15",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#eta| = [1.0,1.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_eta15_20 = TH1F_LW::create("tag_MV_w_eta15_20",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#eta| = [1.5,2.0]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_eta20_25 = TH1F_LW::create("tag_MV_w_eta20_25",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#eta| = [2.0,2.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  
  registerHist(*m_monGr_shift, m_tag_mv_w_phi0_07  = TH1F_LW::create("tag_MV_w_phi0_07" ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#phi| = [0.0,0.7]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_phi07_14 = TH1F_LW::create("tag_MV_w_phi07_14",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#phi| = [0.7,1.4]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_phi14_21 = TH1F_LW::create("tag_MV_w_phi14_21",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#phi| = [1.4,2.1]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_phi21_28 = TH1F_LW::create("tag_MV_w_phi21_28",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#phi| = [2.1,2.8]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_phi28    = TH1F_LW::create("tag_MV_w_phi28",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, |#phi| > 2.8); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));

  registerHist(*m_monGr_shift, m_mv_tag_60_2D = TH2F_LW::create("mv_tag_60_rate_2D", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 60% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_70_2D = TH2F_LW::create("mv_tag_70_rate_2D", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 70% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_77_2D = TH2F_LW::create("mv_tag_77_rate_2D", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 77% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_85_2D = TH2F_LW::create("mv_tag_85_rate_2D", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 85% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  registerHist(*m_monGr_shift, m_tag_mv_w_phi_sum85OP = TH1F_LW::create("tag_MV_w_phi_sum85OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 85% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_sum77OP = TH1F_LW::create("tag_MV_w_phi_sum77OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 77% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_sum70OP = TH1F_LW::create("tag_MV_w_phi_sum70OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 70% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_sum60OP = TH1F_LW::create("tag_MV_w_phi_sum60OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 60% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_sumAll  = TH1F_LW::create("tag_MV_w_phi_sumAll" ,"Number of Jets vs #phi (Taggable Good Jets); #phi" ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_frac85OP = TH1F_LW::create("tag_MV_w_phi_frac85OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 85% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_frac77OP = TH1F_LW::create("tag_MV_w_phi_frac77OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 77% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_frac70OP = TH1F_LW::create("tag_MV_w_phi_frac70OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 70% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_phi_frac60OP = TH1F_LW::create("tag_MV_w_phi_frac60OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 60% WP, Taggable Good Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));

  registerHist(*m_monGr_shift, m_tag_mv_w_eta_sum85OP = TH1F_LW::create("tag_MV_w_eta_sum85OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 85% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_sum77OP = TH1F_LW::create("tag_MV_w_eta_sum77OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 77% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_sum70OP = TH1F_LW::create("tag_MV_w_eta_sum70OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 70% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_sum60OP = TH1F_LW::create("tag_MV_w_eta_sum60OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 60% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_sumAll  = TH1F_LW::create("tag_MV_w_eta_sumAll" ,"Number of Jets vs #eta (Taggable Good Jets); #eta"      ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_frac85OP = TH1F_LW::create("tag_MV_w_eta_frac85OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 85% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_frac77OP = TH1F_LW::create("tag_MV_w_eta_frac77OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 77% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_frac70OP = TH1F_LW::create("tag_MV_w_eta_frac70OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 70% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_eta_frac60OP = TH1F_LW::create("tag_MV_w_eta_frac60OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 60% WP, Taggable Good Jets); #eta").c_str() ,20,-2.5,2.5));
  
  registerHist(*m_monGr_shift, m_tag_mv_w_sj  = TH1F_LW::create("tag_MV_w_sj",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));   
  
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_pT10_20   = TH1F_LW::create("tag_MV_w_sj_pT10_20"  ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, pT = [10,20] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_pT20_50   = TH1F_LW::create("tag_MV_w_sj_pT20_50"  ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, pT = [20,50] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_pT50_100  = TH1F_LW::create("tag_MV_w_sj_pT50_100" ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, pT = [50,100] GeV); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_pT100_200 = TH1F_LW::create("tag_MV_w_sj_pT100_200",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, pT = [100,200] GeV); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_pT200     = TH1F_LW::create("tag_MV_w_sj_pT200"    ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, pT > 200 GeV); "+m_mv_algorithmName+" tag weight").c_str()      ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta0_05  = TH1F_LW::create("tag_MV_w_sj_eta0_05" ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#eta| = [0.0,0.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta05_10 = TH1F_LW::create("tag_MV_w_sj_eta05_10",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#eta| = [0.5,1.0]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta10_15 = TH1F_LW::create("tag_MV_w_sj_eta10_15",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#eta| = [1.0,1.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta15_20 = TH1F_LW::create("tag_MV_w_sj_eta15_20",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#eta| = [1.5,2.0]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta20_25 = TH1F_LW::create("tag_MV_w_sj_eta20_25",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#eta| = [2.0,2.5]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));  
  
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi0_07  = TH1F_LW::create("tag_MV_w_sj_phi0_07" ,(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#phi| = [0.0,0.7]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi07_14 = TH1F_LW::create("tag_MV_w_sj_phi07_14",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#phi| = [0.7,1.4]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi14_21 = TH1F_LW::create("tag_MV_w_sj_phi14_21",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#phi| = [1.4,2.1]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi21_28 = TH1F_LW::create("tag_MV_w_sj_phi21_28",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#phi| = [2.1,2.8]); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi28    = TH1F_LW::create("tag_MV_w_sj_phi28",(m_mv_algorithmName+" Tag Weight (Taggable Suspect Jets, |#phi| > 2.8); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));  
    
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_sum85OP = TH1F_LW::create("tag_MV_w_sj_phi_sum85OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 85% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_sum77OP = TH1F_LW::create("tag_MV_w_sj_phi_sum77OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 77% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_sum70OP = TH1F_LW::create("tag_MV_w_sj_phi_sum70OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 70% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_sum60OP = TH1F_LW::create("tag_MV_w_sj_phi_sum60OP",("Number of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 60% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_sumAll  = TH1F_LW::create("tag_MV_w_sj_phi_sumAll" ,"Number of Jets vs #phi (Taggable Suspect Jets); #phi"      ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_frac85OP = TH1F_LW::create("tag_MV_w_sj_phi_frac85OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 85% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_frac77OP = TH1F_LW::create("tag_MV_w_sj_phi_frac77OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 77% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_frac70OP = TH1F_LW::create("tag_MV_w_sj_phi_frac70OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 70% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_phi_frac60OP = TH1F_LW::create("tag_MV_w_sj_phi_frac60OP",("Fraction of Tagged Jets vs #phi ("+m_mv_algorithmName+" @ 60% WP, Taggable Suspect Jets); #phi").c_str() ,14,-TMath::Pi(),TMath::Pi()));

  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_sum85OP = TH1F_LW::create("tag_MV_w_sj_eta_sum85OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 85% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_sum77OP = TH1F_LW::create("tag_MV_w_sj_eta_sum77OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 77% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_sum70OP = TH1F_LW::create("tag_MV_w_sj_eta_sum70OP",("Number of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 70% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_sum60OP = TH1F_LW::create("tag_MV_w_sj_eta_sum60OP",("Number of Tagged Jets vs #ets ("+m_mv_algorithmName+" @ 60% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_sumAll  = TH1F_LW::create("tag_MV_w_sj_eta_sumAll" ,"Number of Jets vs #eta (Taggable Suspect Jets); #eta"      ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_frac85OP = TH1F_LW::create("tag_MV_w_sj_eta_frac85OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 85% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_frac77OP = TH1F_LW::create("tag_MV_w_sj_eta_frac77OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 77% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_frac70OP = TH1F_LW::create("tag_MV_w_sj_eta_frac70OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 70% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));
  registerHist(*m_monGr_shift, m_tag_mv_w_sj_eta_frac60OP = TH1F_LW::create("tag_MV_w_sj_eta_frac60OP",("Fraction of Tagged Jets vs #eta ("+m_mv_algorithmName+" @ 60% WP, Taggable Suspect Jets); #eta").c_str() ,20,-2.5,2.5));

  // Low level taggers: SV1+IP3D, IP2D and IP3D Taggers

  registerHist(*m_monGr_shift, m_tag_sv1ip3d_w = TH1F_LW::create("tag_SV1IP3D_w","SV1+IP3D Tag Weight (Taggable Good Jets); SV1+IP3D tag weight",100,-10.,10.));
  registerHist(*m_monGr_shift, m_sv1ip3d_tag_pos_rate_2D = TH2F_LW::create("sv1ip3d_tag_pos_rate_2D", "SV1+IP3D Positive Tag Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_sv1ip3d_tag_neg_rate_2D = TH2F_LW::create("sv1ip3d_tag_neg_rate_2D", "SV1+IP3D Negative Tag Rate;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  registerHist(*m_monGr_shift, m_tag_sv1ip3d_w_sj = TH1F_LW::create("tag_SV1IP3D_w_sj","SV1+IP3D Tag Weight (Taggable Suspect Jets); SV1+IP3D tag weight",100,-10.,10.));

  registerHist(*m_monGr_shift, m_tag_ip2d_n    = TH1F_LW::create("tag_IP2D_n","Number of IP2D Tracks (Taggable Good Jets); Number of tracks",20,0.,20.));
  registerHist(*m_monGr_shift, m_tag_ip2d_b    = TH1F_LW::create("tag_IP2D_b","IP2D Pb (Taggable Good Jets); IP2D Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_u    = TH1F_LW::create("tag_IP2D_u","IP2D Pu (Taggable Good Jets); IP2D Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_c    = TH1F_LW::create("tag_IP2D_c","IP2D Pc (Taggable Good Jets); IP2D Pc",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_llr  = TH1F_LW::create("tag_IP2D_llr","IP2D llr (Taggable Good Jets); IP2D llr",160,-40.,40.));

  registerHist(*m_monGr_shift, m_tag_ip2d_n_sj    = TH1F_LW::create("tag_IP2D_n_sj","Number of IP2D Tracks (Taggable Suspect Jets); Number of tracks",20,0.,20.));
  registerHist(*m_monGr_shift, m_tag_ip2d_b_sj    = TH1F_LW::create("tag_IP2D_b_sj","IP2D Pb (Taggable Suspect Jets); IP2D Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_u_sj    = TH1F_LW::create("tag_IP2D_u_sj","IP2D Pu (Taggable Suspect Jets); IP2D Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_c_sj    = TH1F_LW::create("tag_IP2D_c_sj","IP2D Pc (Taggable Suspect Jets); IP2D Pc",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip2d_llr_sj  = TH1F_LW::create("tag_IP2D_llr_sj","IP2D llr (Taggable Suspect Jets); IP2D llr",160,-40.,40.));
    
  registerHist(*m_monGr_shift, m_tag_ip3d_n    = TH1F_LW::create("tag_IP3D_n","Number of IP3D Tracks (Taggable Good Jets); Number of tracks",20,0.,20.));
  registerHist(*m_monGr_shift, m_tag_ip3d_b    = TH1F_LW::create("tag_IP3D_b","IP3D Pb (Taggable Good Jets); IP3D Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip3d_u    = TH1F_LW::create("tag_IP3D_u","IP3D Pu (Taggable Good Jets); IP3D Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip3d_c    = TH1F_LW::create("tag_IP3D_c","IP3D Pc (Taggable Good Jets); IP3D Pc",100,0.,1.)); 
  registerHist(*m_monGr_shift, m_tag_ip3d_llr  = TH1F_LW::create("tag_IP3D_llr","IP3D llr (Taggable Good Jets); IP3D llr",160,-40.,40.));  

  registerHist(*m_monGr_shift, m_tag_ip3d_n_sj    = TH1F_LW::create("tag_IP3D_n_sj","Number of IP3D Tracks (Taggable Suspect Jets); Number of tracks",20,0.,20.));
  registerHist(*m_monGr_shift, m_tag_ip3d_b_sj    = TH1F_LW::create("tag_IP3D_b_sj","IP3D Pb (Taggable Suspect Jets); IP3D Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip3d_u_sj    = TH1F_LW::create("tag_IP3D_u_sj","IP3D Pu (Taggable Suspect Jets); IP3D Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_ip3d_c_sj    = TH1F_LW::create("tag_IP3D_c_sj","IP3D Pc (Taggable Suspect Jets); IP3D Pc",100,0.,1.)); 
  registerHist(*m_monGr_shift, m_tag_ip3d_llr_sj  = TH1F_LW::create("tag_IP3D_llr_sj","IP3D llr (Taggable Suspect Jets); IP3D llr",160,-40.,40.));  
    
  // Low level taggers: SV0 and SV1 Taggers

  registerHist(*m_monGr_shift, m_tag_sv0_sig3d  = TH1F_LW::create("tag_SV0_sig3d","SV1 sig3d; SV1 sig3d",100, 0.,100.));     
     
  registerHist(*m_monGr_shift, m_tag_sv1_b    = TH1F_LW::create("tag_SV1_b","SV1 Pb (Taggable Good Jets); SV1 Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_u    = TH1F_LW::create("tag_SV1_u","SV1 Pu (Taggable Good Jets); SV1 Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_c    = TH1F_LW::create("tag_SV1_c","SV1 Pc (Taggable Good Jets); SV1 Pc",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_llr  = TH1F_LW::create("tag_SV1_llr","SV1 llr (Taggable Good Jets); SV1 llr",160,-40.,40.));

  registerHist(*m_monGr_shift, m_tag_sv1_b_sj    = TH1F_LW::create("tag_SV1_b_sj","SV1 Pb (Taggable Suspect Jets); SV1 Pb",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_u_sj    = TH1F_LW::create("tag_SV1_u_sj","SV1 Pu (Taggable Suspect Jets); SV1 Pu",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_c_sj    = TH1F_LW::create("tag_SV1_c_sj","SV1 Pc (Taggable Suspect Jets); SV1 Pc",100,0.,1.));
  registerHist(*m_monGr_shift, m_tag_sv1_llr_sj  = TH1F_LW::create("tag_SV1_llr_sj","SV1 llr (Taggable Suspect Jets); SV1 llr",160,-40.,40.));
    
  // Low level taggers:  JetFitter Taggers

  registerHist(*m_monGr_shift, m_tag_jetfitter_llr = TH1F_LW::create("tag_JetFitter_llr","JetFitter llr (Taggable Good Jets); JetFitter llr",160,-40.,40.));
  registerHist(*m_monGr_shift, m_tag_jfcnn_llr   = TH1F_LW::create("tag_JFCNN_llr","JetFitterCOMBNN Tag Weight (Taggable Good Jets); JetFitterCOMBNN tag weight",160,-40.,40.));

  registerHist(*m_monGr_shift, m_tag_jetfitter_llr_sj = TH1F_LW::create("tag_JetFitter_llr_sj","JetFitter llr (Taggable Suspect Jets); JetFitter llr",160,-40.,40.));
  registerHist(*m_monGr_shift, m_tag_jfcnn_llr_sj   = TH1F_LW::create("tag_JFCNN_llr_sj","JetFitterCOMBNN Tag Weight (Taggable Suspect Jets); JetFitterCOMBNN tag weight",160,-40.,40.));
    
  //////////////////////////
  // Track cut histograms //
  //////////////////////////
 
  registerHist(*m_monGr_shift, m_tracks_all_2D = TH2F_LW::create("tracks_all_2D", "Number of Tracks (No Cuts); #eta; #phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_pTMin_2D = TH2F_LW::create("tracks_pTMin_2D", "Number of Tracks Failing pTMin Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_d0Max_2D = TH2F_LW::create("tracks_d0Max_2D", "Number of Tracks Failing d0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_z0Max_2D = TH2F_LW::create("tracks_z0Max_2D", "Number of Tracks Failing z0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_sigd0Max_2D = TH2F_LW::create("tracks_sigd0Max_2D", "Number of Tracks Failing sigd0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_sigz0Max_2D = TH2F_LW::create("tracks_sigz0Max_2D", "Number of Tracks Failing sigz0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_etaMax_2D = TH2F_LW::create("tracks_etaMax_2D", "Number of Tracks Failing etaMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitBLayer_2D = TH2F_LW::create("tracks_nHitBLayer_2D", "Number of Tracks Failing IBL Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_deadBLayer_2D = TH2F_LW::create("tracks_deadBLayer_2D", "Number of Tracks Failing Dead IBL Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitPix_2D = TH2F_LW::create("tracks_nHitPix_2D", "Number of Tracks Failing PIX Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitSct_2D = TH2F_LW::create("tracks_nHitSct_2D", "Number of Tracks Failing SCT Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitSi_2D = TH2F_LW::create("tracks_nHitSi_2D", "Number of Tracks Failing Silicon Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitTrt_2D = TH2F_LW::create("tracks_nHitTrt_2D", "Number of Tracks Failing TRT Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_nHitTrtHighE_2D = TH2F_LW::create("tracks_nHitTrtHighE_2D", "Number of Tracks Failing TRT High E Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_fitChi2_2D = TH2F_LW::create("tracks_fitChi2_2D", "Number of Tracks Failing fitChi2 Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_fitProb_2D = TH2F_LW::create("tracks_fitProb_2D", "Number of Tracks Failing fitProb Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_fitChi2OnNdfMax_2D = TH2F_LW::create("tracks_fitChi2OnNdfMax_2D", "Number of Tracks Failing fitChi2OnNdfMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_tracks_passedCuts_2D = TH2F_LW::create("tracks_passedCuts_2D", "Number of Tracks Passing All Cuts;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  ////////////////////////
  // LowStat histograms //
  ////////////////////////

  if ( newRunFlag() || newLowStatIntervalFlag() ) {
    registerHist(*m_monGr_LowStat, m_tracks_all_2D_LS = TH2F_LW::create("tracks_all_2D_LS", "Number of Tracks (No Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_pTMin_2D_LS = TH2F_LW::create("tracks_pTMin_2D_LS", "Number of Tracks Failing pTMin Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_d0Max_2D_LS = TH2F_LW::create("tracks_d0Max_2D_LS", "Number of Tracks Failing d0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_z0Max_2D_LS = TH2F_LW::create("tracks_z0Max_2D_LS", "Number of Tracks Failing z0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_sigd0Max_2D_LS = TH2F_LW::create("tracks_sigd0Max_2D_LS", "Number of Tracks Failing sigd0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_sigz0Max_2D_LS = TH2F_LW::create("tracks_sigz0Max_2D_LS", "Number of Tracks Failing sigz0Max Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_etaMax_2D_LS = TH2F_LW::create("tracks_etaMax_2D_LS", "Number of Tracks Failing etaMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitBLayer_2D_LS = TH2F_LW::create("tracks_nHitBLayer_2D_LS", "Number of Tracks Failing IBL Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_deadBLayer_2D_LS = TH2F_LW::create("tracks_deadBLayer_2D_LS", "Number of Tracks Failing Dead IBL Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitPix_2D_LS = TH2F_LW::create("tracks_nHitPix_2D_LS", "Number of Tracks Failing PIX Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitSct_2D_LS = TH2F_LW::create("tracks_nHitSct_2D_LS", "Number of Tracks Failing SCT Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitSi_2D_LS = TH2F_LW::create("tracks_nHitSi_2D_LS", "Number of Tracks Failing Silicon Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitTrt_2D_LS = TH2F_LW::create("tracks_nHitTrt_2D_LS", "Number of Tracks Failing TRT Hits Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_nHitTrtHighE_2D_LS = TH2F_LW::create("tracks_nHitTrtHighE_2D_LS", "Number of Tracks Failing TRT High E Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_fitChi2_2D_LS = TH2F_LW::create("tracks_fitChi2_2D_LS", "Number of Tracks Failing fitChi2 Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_fitProb_2D_LS = TH2F_LW::create("tracks_fitProb_2D_LS", "Numbr of Tracks Failing fitProb Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_tracks_fitChi2OnNdfMax_2D_LS = TH2F_LW::create("tracks_fitChi2OnNdfMax_2D_LS", "Number of Tracks Failing fitChi2OnNdfMax Cut;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_LowStat, m_jet_tracks_d0_LS = TH1F_LW::create("jet_tracks_d0_LS","d0 of Tracks in Jets; d0 [mm]",100,-5.,5.));
    registerHist(*m_monGr_LowStat, m_jet_tracks_z0_LS = TH1F_LW::create("jet_tracks_z0_LS","z0 of Tracks in Jets; z0 [mm]",100,-300.,300.));

    registerHist(*m_monGr_LowStat, m_tag_mv_w_LS  = TH1F_LW::create("tag_MV_w_LS",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));  
    registerHist(*m_monGr_LowStat, m_tag_mv_w_pT10_20_LS   = TH1F_LW::create("tag_MV_w_pT10_20_LS"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [10,20] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
    registerHist(*m_monGr_LowStat, m_tag_mv_w_pT20_50_LS   = TH1F_LW::create("tag_MV_w_pT20_50_LS"  ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [20,50] GeV); "+m_mv_algorithmName+" tag weight").c_str()  ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
    registerHist(*m_monGr_LowStat, m_tag_mv_w_pT50_100_LS  = TH1F_LW::create("tag_MV_w_pT50_100_LS" ,(m_mv_algorithmName+" Tag Weight Taggable Good Jets, pT = [50,100] GeV); "+m_mv_algorithmName+" tag weight").c_str() ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
    registerHist(*m_monGr_LowStat, m_tag_mv_w_pT100_200_LS = TH1F_LW::create("tag_MV_w_pT100_200_LS",(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT = [100,200] GeV); "+m_mv_algorithmName+" tag weight").c_str(),m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));    
    registerHist(*m_monGr_LowStat, m_tag_mv_w_pT200_LS     = TH1F_LW::create("tag_MV_w_pT200_LS"    ,(m_mv_algorithmName+" Tag Weight (Taggable Good Jets, pT > 200 GeV); "+m_mv_algorithmName+" tag weight").c_str()      ,m_mv_nBins,m_mv_rangeStart,m_mv_rangeStop));   
    
    registerHist(*m_monGr_LowStat, m_track_selector_eff_LS   = TH2F_LW::create("track_selector_eff_LS", "Fraction of Taggable Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_track_selector_all_LS   = TH2F_LW::create("track_selector_all_LS", "Number of Jets;#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

    registerHist(*m_monGr_LowStat, m_jet_2D_quality_LS     = TH2F_LW::create("jet_2D_quality_LS", "Number of Jets (Taggable Good);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
    registerHist(*m_monGr_LowStat, m_jet_2D_kinematic_LS     = TH2F_LW::create("jet_2D_kinematic_LS", "Number of Jets (Kinematic Cuts);#eta;#phi", 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  registerHist(*m_monGr_shift, m_mv_tag_60_2D_LS = TH2F_LW::create("mv_tag_60_rate_2D_LS", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 60% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_70_2D_LS = TH2F_LW::create("mv_tag_70_rate_2D_LS", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 70% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_77_2D_LS = TH2F_LW::create("mv_tag_77_rate_2D_LS", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 77% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));
  registerHist(*m_monGr_shift, m_mv_tag_85_2D_LS = TH2F_LW::create("mv_tag_85_rate_2D_LS", ("Number of Tagged Jets vs #eta and #phi ("+m_mv_algorithmName+" @ 85% WP);#eta;#phi").c_str(), 25, -2.5, 2.5, 25, -TMath::Pi(), TMath::Pi()));

  }

  /*
    Ended registering LowStat histograms
  */

  m_histogramsCreated = true;

  return StatusCode::SUCCESS;
}

//** --------------------------------------------------------------------------------------------------------------- **//

StatusCode JetTagMonitoring::fillHistograms() {

  ATH_MSG_DEBUG("fillHistograms()");

  if (!m_histogramsCreated)
    return StatusCode::SUCCESS;

  if (m_switch_off)
    return StatusCode::SUCCESS;

  m_cutflow->Fill(0.);

  const xAOD::EventInfo* thisEventInfo;
  if (evtStore()->retrieve(thisEventInfo).isFailure())
    ATH_MSG_WARNING("Cannot retrieve EventInfo");

  m_lumiBlockNum = thisEventInfo->lumiBlock();

  ATH_MSG_DEBUG("Lumiblock ID: " << m_lumiBlockNum);

  ////////////////////////////////
  //* Event cleaning           *//
  ////////////////////////////////

  if ( thisEventInfo->errorState(xAOD::EventInfo::EventFlagSubDet::Tile) == xAOD::EventInfo::Error || 
       thisEventInfo->errorState(xAOD::EventInfo::EventFlagSubDet::LAr)  == xAOD::EventInfo::Error ||
       thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) { 
    ATH_MSG_DEBUG("Event cleaning : skip event");
    return StatusCode::SUCCESS;    
  }
    
  m_cutflow->Fill(1.);

  //////////////////////
  //* Get Pileup     *//
  //////////////////////

  m_mu = thisEventInfo->actualInteractionsPerCrossing();
  m_n_mu->Fill(m_mu); 

  //////////////////////
  //* Primary vertex *//
  //////////////////////

  unsigned int npv = 0, npv_trk = 0;
  double xpv = 0., ypv = 0., zpv = 0.;

  const xAOD::VertexContainer* vxContainer(0);
  bool foundPrimaryVtx = evtStore()->retrieve(vxContainer, m_primaryVertexName);

  if (!foundPrimaryVtx) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_primaryVertexName << "\" from StoreGate");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("VxContainer \"" << m_primaryVertexName << "\" found with " << vxContainer->size() << " entries");

  npv = vxContainer->size();
  m_global_nPrimVtx->Fill((float)npv);

  if (vxContainer->size() < 2) {
    ATH_MSG_WARNING("No primary vertices reconstructed");
    return StatusCode::SUCCESS;
  }

  m_cutflow->Fill(2.);

  bool foundVxTypePriVtx = false;
  m_priVtx = nullptr;
  for (xAOD::VertexContainer::const_iterator vtx = vxContainer->begin(); vtx != vxContainer->end(); ++vtx) {
    if ((*vtx)->vertexType() == xAOD::VxType::PriVtx && (*vtx)->numberDoF() > 0 ) {
      m_priVtx = *vtx;
      foundVxTypePriVtx = true;
      break;
    }
  }
  if ( ! foundVxTypePriVtx ) {
    ATH_MSG_WARNING("xAOD::Vertex of type xAOD::VxType::PriVtx was not found in vertex container.");
    return StatusCode::SUCCESS;
  }

  // if (m_priVtx->vxTrackAtVertexAvailable()) {
  if (m_priVtx->nTrackParticles()>0) {
    //npv_trk = m_priVtx->vxTrackAtVertex().size()
    npv_trk = m_priVtx->nTrackParticles();
    ATH_MSG_DEBUG("PV tracks: " << npv_trk);
  }

  xpv = m_priVtx->x();
  ypv = m_priVtx->y();
  zpv = m_priVtx->z();

  ATH_MSG_DEBUG("primary vertex: x = " << xpv << ", y = " << ypv << ", z = " << zpv);

  if (xpv == 0 && ypv == 0 && zpv == 0)
    ATH_MSG_WARNING("Primary Vertex is (0,0,0)");
    
  m_global_xPrimVtx->Fill(xpv);
  m_global_yPrimVtx->Fill(ypv);
  m_global_zPrimVtx->Fill(zpv);
  m_priVtx_trks->Fill(npv_trk);

  // check first PV > 4 tracks, if not, fail
  if ( npv_trk <  m_pri_vtx_trk_min_cut )
    return StatusCode::SUCCESS;

  m_cutflow->Fill(3.);

  //     double vtxChiSq = primaryRecVertex.fitQuality().chiSquared();
  //     int    vtxNDoF  = primaryRecVertex.fitQuality().numberDoF();
  //     double vtxProb  = vtxChiSq / vtxNDoF;


  ///////////////////////////////
  //* Trigger container       *//
  ///////////////////////////////

  if (m_use_trigdectool && m_trigDecTool != 0) { // only require trigger if m_use_trigdectool is true (false for express stream) and trigDecTool is ok

    ATH_MSG_DEBUG("TrigDecTool: " << m_trigDecTool);
    ATH_MSG_DEBUG("m_use_trigdectool: " << m_use_trigdectool << ", (m_trigDecTool->isPassed(" << m_ElectronTrigger_201X << "): " << m_trigDecTool->isPassed(m_ElectronTrigger_201X));
    ATH_MSG_DEBUG("m_use_trigdectool: " << m_use_trigdectool << ", (m_trigDecTool->isPassed(" << m_MuonTrigger_201X << "): " << m_trigDecTool->isPassed(m_MuonTrigger_201X));
    //ATH_MSG_DEBUG("m_use_trigdectool: " << m_use_trigdectool << ", (m_trigDecTool->isPassed(" << m_JetTrigger_201X << "): " << m_trigDecTool->isPassed(m_JetTrigger_201X));

    auto chainGroup = m_trigDecTool->getChainGroup(".*");
    for (auto & trig : chainGroup->getListOfTriggers()) {
      ATH_MSG_DEBUG("Found trigger " << trig);
    }
    
    m_trigPassed->Fill(0.);

    // 201X menu triggers
    if (m_trigDecTool->isPassed(m_ElectronTrigger_201X)) m_trigPassed->Fill(1.);
    if (m_trigDecTool->isPassed(m_MuonTrigger_201X)) m_trigPassed->Fill(2.);
    if (m_trigDecTool->isPassed(m_ElectronTrigger_201X) || m_trigDecTool->isPassed(m_MuonTrigger_201X)) m_trigPassed->Fill(3.);
    if (m_trigDecTool->isPassed(m_ElectronTrigger_201X) && m_trigDecTool->isPassed(m_MuonTrigger_201X)) m_trigPassed->Fill(4.);
    // if (m_trigDecTool->isPassed(m_JetTrigger_201X)) m_trigPassed->Fill(5.);

    // Require e/mu trigger to have unbiased sample of jets (and larger fraction of b-jets since many of these are ttbar events)

    if (!m_trigDecTool->isPassed(m_ElectronTrigger_201X) && !m_trigDecTool->isPassed(m_MuonTrigger_201X)) // 201X menu triggers
      return StatusCode::SUCCESS;
  }
  
  m_cutflow->Fill(4.);
  
  /* Fill Jet-based histograms */
  fillJetHistograms();

  ///////////////////////////////
  //* TrackParticle container *//
  ///////////////////////////////
        
  const DataHandle<xAOD::TrackParticleContainer> trackParticles;
  bool foundTrackPartColl = retrieveCollection(m_storeGate, trackParticles, m_trackParticleName);

  if (!foundTrackPartColl)  {

    ATH_MSG_WARNING("Unable to retrieve \"" << m_trackParticleName << "\" from StoreGate");

    return StatusCode::SUCCESS;

  }

  ATH_MSG_DEBUG("TrackParticleContainer \"" << m_trackParticleName << "\" found with " << trackParticles->size() << " entries");

  xAOD::TrackParticleContainer::const_iterator trackParticleItr = trackParticles->begin();
  xAOD::TrackParticleContainer::const_iterator trackParticleEnd = trackParticles->end();

  uint8_t nPixHits    = 0;
  uint8_t nSCTHits    = 0;
  uint8_t nSiHits     = 0;
  uint8_t nBLayerHits = 0;
  uint8_t nTRTHits    = 0;

  for ( ; trackParticleItr != trackParticleEnd; trackParticleItr++) { 
      
    if ((*trackParticleItr)->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits)) { m_global_BLayerHits->Fill((float) nBLayerHits); }
    if ((*trackParticleItr)->summaryValue(nPixHits, xAOD::numberOfPixelHits))     { m_global_PixelHits->Fill((float) nPixHits); }
    if ((*trackParticleItr)->summaryValue(nSCTHits, xAOD::numberOfSCTHits))       { m_global_SCTHits->Fill((float) nSCTHits); }
    if ((*trackParticleItr)->summaryValue(nTRTHits, xAOD::numberOfTRTHits))       { m_global_TRTHits->Fill((float) nTRTHits); }
      
    nSiHits = nPixHits + nSCTHits;
    m_global_SiHits->Fill((float)nSiHits);
    nSiHits = 0;
  }

  m_trackParticle_n->Fill((*trackParticles).size());	    

  ATH_MSG_DEBUG("end of fillHistograms()");

  return StatusCode::SUCCESS;
}

bool JetTagMonitoring::passJetQualityCuts(const xAOD::Jet *jet){

  bool pass_cuts = false;
  	
  float jetQuality      = jet->getAttribute<float>(xAOD::JetAttribute::LArQuality);
  float jetTime         = jet->getAttribute<float>(xAOD::JetAttribute::Timing);
  float hecq            = jet->getAttribute<float>(xAOD::JetAttribute::HECQuality);
  float negE            = jet->getAttribute<float>(xAOD::JetAttribute::NegativeE);
  std::vector<float> SumPtTrkPt1000;
  jet->getAttribute(xAOD::JetAttribute::SumPtTrkPt1000,SumPtTrkPt1000);
  float chf             = SumPtTrkPt1000.size() > 0 ? SumPtTrkPt1000.at(0)/jet->pt() : -1;
  float emf             = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
  float hecf            = jet->getAttribute<float>(xAOD::JetAttribute::HECFrac); 
  float fracSamplingMax = jet->getAttribute<float>(xAOD::JetAttribute::FracSamplingMax); 
 	
  if ( 
      !(
	(hecf>0.5 && fabs(hecq)>0.5) || (fabs(negE) > 60*CLHEP::GeV) ||
	(emf>0.95 && fabs(jetQuality)>0.8 && fabs(jet->eta()) < 2.8) || 
	(fabs(jetTime)>25) ||
	(emf<0.05 && chf<0.05 && fabs(jet->eta())<2) ||
	(emf<0.05 && fabs(jet->eta())>= 2) ||
	(fracSamplingMax>0.99 && fabs(jet->eta())<2)
	) 
       ){
    pass_cuts = true; 
  }
	
  if (m_skipJetQuality) //In case of Ion-Ion or Ion-proton collision
  pass_cuts = true; //Skip the Jet Quality cuts (thresholds tuned on pp event jets)

  return pass_cuts;
}

bool JetTagMonitoring::passKinematicCuts(const xAOD::Jet *jet) {
  
  ATH_MSG_DEBUG("passKinematicCuts()");
  
  //* get jet basic kinematics *//
  m_jet_eta->Fill(jet->eta());
  m_jet_phi->Fill(jet->phi());
  m_jet_et->Fill(jet->pt() / CLHEP::GeV);

  ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt() / CLHEP::GeV);

  // kinematic cuts defined as properties
  if ( jet->pt()  / CLHEP::GeV < m_jet_pt_cut || fabs(jet->eta()) > m_jet_eta_cut )
    return false;

  return true;
}


bool JetTagMonitoring::passJVTCuts(const xAOD::Jet *jet) {

  ATH_MSG_DEBUG("passJVTCuts()");
  
  static SG::AuxElement::Accessor<float> JVT( "Jvt" ); //JVT > 0.59 (60 GeV)
  double jvt = JVT(*jet);

  if( !( ((jet->pt()<60000)&&(abs(jet->eta())<2.4)&&(jvt > 0.59))||((jet->pt()<60000)&&(abs(jet->eta())>2.4))||(jet->pt()>60000) ) ) return false;

  return true;
}

bool JetTagMonitoring::passMuonOverlap(const xAOD::Jet *jet) {

  ATH_MSG_DEBUG("passMuonOverlap()");

  const DataHandle<xAOD::MuonContainer> muons;
  bool foundMuonColl = retrieveCollection(m_storeGate, muons, m_muonName);
  
  if (!foundMuonColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_muonName << "\" from StoreGate");
    return false;
  }

  xAOD::MuonContainer::const_iterator muonItr = muons->begin();
  xAOD::MuonContainer::const_iterator muonEnd = muons->end();
  xAOD::MuonContainer::const_iterator isoMuonItr = muons->end();

  // loop over muon container  
  int n_isoMuons = 0, n_ptMuons = 0;
  for ( ; muonItr != muonEnd; muonItr++) {
    //select muons which passed pT cut
    if ((*muonItr) -> pt() / CLHEP::GeV < m_MuonPtCut) continue;
    bool inAcceptance = TMath::Abs((*muonItr) -> eta()) < m_MuonEtaCut;
    if (!inAcceptance) continue;
    // medium muons
    //if ((*muonItr)->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)

    if ( ((*muonItr)->pt() > (0.9*jet->pt())) && (jet->p4().DeltaR((*muonItr)->p4())<0.4) ) ++n_ptMuons;

    float topoetcone20_value = -999.;
    float ptvarcone30_value = -999.;
    (*muonItr)-> isolation(topoetcone20_value, xAOD::Iso::topoetcone20);
    (*muonItr)-> isolation(ptvarcone30_value, xAOD::Iso::ptvarcone30);
    if (topoetcone20_value/(*muonItr)->pt() > m_MuonTopoEtCone20Cut) continue;
    if (ptvarcone30_value/(*muonItr)->pt() > m_MuonPtVarCone30Cut) continue;
    if (jet->p4().DeltaR((*muonItr)->p4())<0.4) ++n_isoMuons;
  }
  
  ATH_MSG_DEBUG("Number of isolated muons within this jet: \"" << n_isoMuons);
  ATH_MSG_DEBUG("Number of muons with 90% pT within this jet: \"" << n_ptMuons);

  if(n_isoMuons>0) return false;
  if(n_ptMuons>0) return false;

  return true;
}
  
double JetTagMonitoring::getMVweight(const xAOD::Jet *jet) {

  ATH_MSG_DEBUG("retrieving MV2c10/DL1* weight");
  
  const xAOD::BTagging* btag = jet->btagging();

  if (!btag) return 0;

  double mv = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  
  if(m_mv_algorithmName=="MV2c10" || m_mv_algorithmName=="MV2c10mu" || m_mv_algorithmName=="MV2c10rnn" || m_mv_algorithmName=="MV2c10r" || m_mv_algorithmName=="MV2c10rmu"){
    btag->MVx_discriminant(m_mv_algorithmName,mv);
  }
  else if (m_mv_algorithmName=="DL1" || m_mv_algorithmName=="DL1mu" || m_mv_algorithmName=="DL1rnn" || m_mv_algorithmName=="DL1r" || m_mv_algorithmName=="DL1rmu"){
    btag->pu(m_mv_algorithmName,mv_pu); 
    btag->pb(m_mv_algorithmName,mv_pb); 
    btag->pc(m_mv_algorithmName,mv_pc); 
    mv = log(mv_pb/(mv_pu*(1-m_mv_cFrac)+m_mv_cFrac*mv_pc));
  } //DL1* formula      

  return mv;
}

JetTagMonitoring::Jet_t JetTagMonitoring::getTaggabilityLabel(const xAOD::Jet *jet) {
  
  // goodJet    = jet with at least one associated track passing track selection
  // suspectJet = as goodJet but, at least one ass. track does not pass deadBLayer, nHitBLayer or nSiHit requirement
  // badJet     = not goodJet (i.e. no associated track passing track selection)

  ATH_MSG_DEBUG("in applyTaggabilityCuts()");

  /* if TrackSelector not used, return true */
  if ( ! m_use_trackselector ) {
    ATH_MSG_DEBUG("Not running track selection");
    return goodJet;
  }

  ATH_MSG_DEBUG("Running track selection");
  
  unsigned int nTrk = 0;

  m_track_selector_all->Fill(jet->eta(), jet->phi(), 1.);
  m_track_selector_all_LS->Fill(jet->eta(), jet->phi(), 1.);

  m_trackSelectorTool->primaryVertex(m_priVtx->position());
  m_trackSelectorTool->prepare();

  std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack); 

  bool isSuspectJet = false;

  std::vector<const xAOD::IParticle*>::const_iterator trkItr;

  for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

    const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);
    if ( ! trackPart ) continue;

    double trk_eta = trackPart->eta();
    double trk_phi = trackPart->phi();

    m_tracks_all_2D->Fill(trk_eta, trk_phi, 1.);
    m_tracks_all_2D_LS->Fill(trk_eta, trk_phi, 1.);

    if ( m_trackSelectorTool->selectTrack(trackPart) ) {

      m_tracks_passedCuts_2D->Fill(trk_eta, trk_phi);
      nTrk++;

    } else {
      const std::bitset<17> passedCuts = m_trackSelectorTool->currentTrackpassedCuts();
      fillBadTrackBits(passedCuts, trk_eta, trk_phi);

      if ( !passedCuts[deadBLayer] || !passedCuts[nHitBLayer] || !passedCuts[nHitSi] ) // if at least one associated track does not pass the deadBLayer, nHitBLayer or nSiHit requirement the jet will be labeled suspectJet
	isSuspectJet = true;
    }

  }
  ATH_MSG_DEBUG("Good tracks: " << nTrk);

  if ( nTrk < m_trk_n ) return badJet;

  m_track_selector_eff->Fill(jet->eta(), jet->phi(), 1.);
  m_track_selector_eff_LS->Fill(jet->eta(), jet->phi(), 1.);

  if ( isSuspectJet ) return suspectJet;
  
  return goodJet;
}
  
bool JetTagMonitoring::isTopEvent() {
  
  // DO TOP-PAIRS SELECTION
  // == 1 isolated electron with pT > m_ElectronPtCut (normally 25 GeV)
  // == 1 isolated muon with pT > m_MuonPtCut (normally 25 GeV)
  // electron and muon of opposite charge
  
  const DataHandle<xAOD::ElectronContainer> electrons;
  bool foundElectronColl = retrieveCollection(m_storeGate, electrons, m_electronName);

  if (!foundElectronColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_electronName << "\" from StoreGate");
    return false;
  }
  
  ATH_MSG_DEBUG("ElectronContainer \"" << m_electronName << "\" found with " << electrons->size() << " entries");
  
  xAOD::ElectronContainer::const_iterator electronItr = electrons->begin();
  xAOD::ElectronContainer::const_iterator electronEnd = electrons->end();
  xAOD::ElectronContainer::const_iterator isoElectronItr = electrons->end();
  
  const DataHandle<xAOD::MuonContainer> muons;
  bool foundMuonColl = retrieveCollection(m_storeGate, muons, m_muonName);
  
  if (!foundMuonColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_muonName << "\" from StoreGate");
    return false;
  }
  
  ATH_MSG_DEBUG("MuonContainer \"" << m_muonName << "\" found with " << muons->size() << " entries");
  
  xAOD::MuonContainer::const_iterator muonItr = muons->begin();
  xAOD::MuonContainer::const_iterator muonEnd = muons->end();
  xAOD::MuonContainer::const_iterator isoMuonItr = muons->end();

  // loop over electron container
  int n_isoElectrons = 0;
  for ( ; electronItr != electronEnd; electronItr++) {
    //select electrons which passed author and pT cut
    if (!(*electronItr)->author(xAOD::EgammaParameters::AuthorElectron)) continue; 
    if ((*electronItr) -> pt() / CLHEP::GeV < m_ElectronPtCut) continue;
    bool inAcceptance = (TMath::Abs((*electronItr) -> eta()) > m_ElectronEtaCrackHighCut || TMath::Abs((*electronItr) -> eta()) < m_ElectronEtaCrackLowCut) 
      && TMath::Abs((*electronItr) -> eta()) < m_ElectronEtaCut;
    if (!inAcceptance) continue;
    // select mediumLH electron
    bool lhmedium = false;
    (*electronItr)->passSelection(lhmedium, "LHMedium");
    if (!lhmedium) continue;
    float topoetcone20_value = -999.;
    float ptvarcone20_value = -999.;
    (*electronItr)-> isolationValue(topoetcone20_value, xAOD::Iso::topoetcone20);
    (*electronItr)-> isolationValue(ptvarcone20_value, xAOD::Iso::ptvarcone20);
    if (topoetcone20_value/(*electronItr)->pt() > m_ElectronTopoEtCone20Cut) continue;
    if (ptvarcone20_value/(*electronItr)->pt() > m_ElectronPtVarCone20Cut) continue;
    isoElectronItr = electronItr;
    ++n_isoElectrons;
  }
  
  ATH_MSG_DEBUG("Number of isolated electrons is \"" << n_isoElectrons);
  m_n_iso_el->Fill(n_isoElectrons);

  // loop over muon container  
  int n_isoMuons = 0;
  for ( ; muonItr != muonEnd; muonItr++) {
    //select muons which passed pT cut
    if ((*muonItr) -> pt() / CLHEP::GeV < m_MuonPtCut) continue;
    bool inAcceptance = TMath::Abs((*muonItr) -> eta()) < m_MuonEtaCut;
    if (!inAcceptance) continue;
    // medium muons
    if ((*muonItr)->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)
    float topoetcone20_value = -999.;
    float ptvarcone30_value = -999.;
    (*muonItr)-> isolation(topoetcone20_value, xAOD::Iso::topoetcone20);
    (*muonItr)-> isolation(ptvarcone30_value, xAOD::Iso::ptvarcone30);
    if (topoetcone20_value/(*muonItr)->pt() > m_MuonTopoEtCone20Cut) continue;
    if (ptvarcone30_value/(*muonItr)->pt() > m_MuonPtVarCone30Cut) continue;
    isoMuonItr = muonItr;
    ++n_isoMuons;
  }
  
  ATH_MSG_DEBUG("Number of isolated muons is \"" << n_isoMuons);
  m_n_iso_mu->Fill(n_isoMuons);
  
  // require == 1 electron, == 1 muon and opposite charge
  if (n_isoElectrons != 1) return false;
  if (n_isoMuons != 1) return false;
  if ((*isoElectronItr)->charge()*(*isoMuonItr)->charge() != -1.) return false;

  return true;
}


bool JetTagMonitoring::isSoftMuonEvent() {
  
  // DO SOFT MUON SELECTION
  // >= 1 jet with eta & pT & quality cuts (20 GeV) or kinematic cuts (15 GeV)
  // == 1 soft muon (per jet) with pT > m_softMuonPtCut (4 GeV) or dynamic pT cut (5-6-9 GeV)
  // tight muons
  // DR(jet,muon) < 0.4  
  // Plot Muon PT (1-100 GeV)
  // Plot DR (0-0.4)
    
  const DataHandle<xAOD::JetContainer> jets;
  bool foundJetColl = retrieveCollection(m_storeGate, jets, m_jetName);

  if (!foundJetColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_jetName << "\" from StoreGate");
    return false;
  }
  
  ATH_MSG_DEBUG("JetContainer \"" << m_jetName << "\" found with " << jets->size() << " entries");
  
  xAOD::JetContainer::const_iterator jetItr = jets->begin();
  xAOD::JetContainer::const_iterator jetEnd = jets->end();
  xAOD::JetContainer::const_iterator ismtJetItr = jets->end();
  
  const DataHandle<xAOD::MuonContainer> muons;
  bool foundMuonColl = retrieveCollection(m_storeGate, muons, m_muonName);
  
  if (!foundMuonColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_muonName << "\" from StoreGate");
    return false;
  }
  
  ATH_MSG_DEBUG("MuonContainer \"" << m_muonName << "\" found with " << muons->size() << " entries");
  
  xAOD::MuonContainer::const_iterator softMuonItr = muons->begin();
  xAOD::MuonContainer::const_iterator softMuonEnd = muons->end();
  xAOD::MuonContainer::const_iterator isoftMuonItr = muons->end();

  // loop over jet container
  int n_goodJets = 0;
  int n_softMuons = 0;
  int n_smtJets = 0;
  float dr_smt = 0;
  float pt_smt_mu = 0;
  float pt_smt_jet = 0;
  //float pt_smt_mu_dynCut = 0;
  TLorentzVector smt_jet, smt_mu;
  double mv = 0;
  for ( ; jetItr != jetEnd; jetItr++) {

    // Jets quality cuts
    if( !passJetQualityCuts(*jetItr) ) continue;
    if( !passKinematicCuts(*jetItr) ) continue;  //15 GeV, eta < 2.5
    if( !passJVTCuts(*jetItr) )  continue; // Jets passing JVT cuts
    Jet_t taggabilityLabel = getTaggabilityLabel(*jetItr); // check if jet is taggable (defined as goodJet or suspectJet)
    if ( taggabilityLabel != goodJet && taggabilityLabel != suspectJet ) continue;

    if ((*jetItr) -> pt() / CLHEP::GeV < m_JetPtCut) continue; // stronger Jet pT cut (25 GeV, eta < 2.5)
    bool inAcceptance = TMath::Abs((*jetItr) -> eta()) < m_JetEtaCut; // usual eta cut
    if (!inAcceptance) continue;

    //Dynamic cut of muon pT threshold, based on jet pT
    //pt_smt_mu_dynCut = 5; //GeV of mu pT  (jet pT < 40 GeV)
    //if((*jetItr) -> pt() / CLHEP::GeV > 40) pt_smt_mu_dynCut = 6; //GeV of pT
    //if((*jetItr) -> pt() / CLHEP::GeV > 90) pt_smt_mu_dynCut = 8; //GeV of pT

    ++n_goodJets;

    n_softMuons=0;
    // loop over muon container  
    for ( ; softMuonItr != softMuonEnd; softMuonItr++) {
      //select soft muons which pass eta and pT cut
      //eta cut
      bool inAcceptance = TMath::Abs((*softMuonItr) -> eta()) < m_MuonEtaCut;
      if (!inAcceptance) continue;
      if ((*softMuonItr) -> pt() / CLHEP::GeV < m_softMuonPtCut) continue; //fixed pT cut 5 GeV
      //if((*softMuonItr) -> pt() / CLHEP::GeV < pt_smt_mu_dynCut) continue;  //dynamic pT cut
      // NO QUALITY (previously tight muons)
      //if ((*softMuonItr)->quality() > 0) continue; // 0 tight, 1 medium, 1 & 0 tight & medium
      const ElementLink< xAOD::TrackParticleContainer >& pMuIDTrack=(*softMuonItr)->inDetTrackParticleLink();
      const ElementLink< xAOD::TrackParticleContainer >& pMuMSTrack=(*softMuonItr)->muonSpectrometerTrackParticleLink();
      if ( !pMuIDTrack.isValid() || !pMuMSTrack.isValid()) continue;
      if ((*softMuonItr)->muonType()!= xAOD::Muon::Combined) continue;//combined
      if ((*softMuonItr)->primaryTrackParticle()->d0() > 0.4) continue; // d0 cut < 0.4 mm
      dr_smt = ((*jetItr)->p4()).DeltaR((*softMuonItr)->p4());
      if ( dr_smt >= 0.4) continue; //jet muon DR < 0.4
      pt_smt_mu = (*softMuonItr) -> pt() / CLHEP::GeV;
      smt_mu.SetPtEtaPhiE((*softMuonItr)->pt(),(*softMuonItr)->eta(),(*softMuonItr)->phi(),(*softMuonItr)->e());
      ++n_softMuons;
    }//end loop on muons

    ATH_MSG_DEBUG("Number of soft muons per jet is \"" << n_softMuons);
    m_n_soft_mu->Fill(n_softMuons);

    if( n_softMuons != 1) continue;

    pt_smt_jet = (*jetItr) -> pt() / CLHEP::GeV;
    smt_jet.SetPtEtaPhiE((*jetItr)->pt(),(*jetItr)->eta(),(*jetItr)->phi(),(*jetItr)->e());
    ++n_smtJets;

    mv = getMVweight(*jetItr); // get MV weight (MV2c10/DL1*)
    
  }//end loop on jets
  
  ATH_MSG_DEBUG("Number of jets with == 1 soft muon per event is \"" << n_smtJets);
  m_n_smt_jet->Fill(n_smtJets);

  if (n_smtJets != 1) return false;

  if(n_smtJets == 1){ //Fill SMT plots 
    m_smt_dr->Fill(dr_smt);
    m_smt_mu_pt->Fill(pt_smt_mu);
    m_smt_jet_pt->Fill(pt_smt_jet);
    m_smt_jet_pt_ratio->Fill(pt_smt_mu/pt_smt_jet);
    m_smt_jet_pt_rel->Fill(smt_mu.Perp((smt_jet+smt_mu).Vect()) / CLHEP::GeV);
    m_smt_jet_mv_w->Fill(mv);   
    m_jet_2D_tsmt->Fill(smt_jet.Eta(), smt_jet.Phi(), 1.);
  }

  return true;
}

void JetTagMonitoring::fillEffHist(TH1 * h_passed, TH1 * h_all, TH1F_LW * effHist) {
  
  if ( TEfficiency::CheckConsistency(*h_passed, *h_all) ) {

    TEfficiency * eff = new TEfficiency(*h_passed, *h_all);
    eff->SetStatisticOption(TEfficiency::kBJeffrey);

    for (int i = 1; i <= h_all->GetNbinsX(); ++i) {
      effHist->SetBinContent( i , eff->GetEfficiency(i) );
      effHist->SetBinError( i , (eff->GetEfficiencyErrorUp(i) + eff->GetEfficiencyErrorLow(i))/2. );
    }

    delete eff;

  }
  
}

StatusCode JetTagMonitoring::procHistograms() {
  
  ATH_MSG_DEBUG("in procHistograms()");

  /* Finalize the histograms */
  if ( m_histogramsCreated && (endOfRunFlag() || AthenaMonManager::environment() == AthenaMonManager::online) ) {
      
    m_track_selector_eff->getROOTHist()->Divide(m_track_selector_all->getROOTHist());
    m_track_selector_eff_LS->getROOTHist()->Divide(m_track_selector_all_LS->getROOTHist());

    m_jet_pt_top_eff->getROOTHist()->Divide(m_jet_pt_top_tagged->getROOTHist(),m_jet_pt_top->getROOTHist());
    m_jet_top_eff->getROOTHist()->Divide(m_jet_top_tagged->getROOTHist(),m_jet_top->getROOTHist());
     
    {
      TH1 * qualityJets_sum85OP = m_tag_mv_w_phi_sum85OP->getROOTHistBase();     
      TH1 * qualityJets_sum77OP = m_tag_mv_w_phi_sum77OP->getROOTHistBase();     
      TH1 * qualityJets_sum70OP = m_tag_mv_w_phi_sum70OP->getROOTHistBase();     
      TH1 * qualityJets_sum60OP = m_tag_mv_w_phi_sum60OP->getROOTHistBase();     
      TH1 * qualityJets_sumAll  = m_tag_mv_w_phi_sumAll->getROOTHistBase();     

      fillEffHist(qualityJets_sum85OP, qualityJets_sumAll, m_tag_mv_w_phi_frac85OP);
      fillEffHist(qualityJets_sum77OP, qualityJets_sumAll, m_tag_mv_w_phi_frac77OP);
      fillEffHist(qualityJets_sum70OP, qualityJets_sumAll, m_tag_mv_w_phi_frac70OP);
      fillEffHist(qualityJets_sum60OP, qualityJets_sumAll, m_tag_mv_w_phi_frac60OP);
    }
      
    {
      TH1 * qualityJets_sum85OP = m_tag_mv_w_eta_sum85OP->getROOTHistBase();     
      TH1 * qualityJets_sum77OP = m_tag_mv_w_eta_sum77OP->getROOTHistBase();     
      TH1 * qualityJets_sum70OP = m_tag_mv_w_eta_sum70OP->getROOTHistBase();     
      TH1 * qualityJets_sum60OP = m_tag_mv_w_eta_sum60OP->getROOTHistBase();     
      TH1 * qualityJets_sumAll  = m_tag_mv_w_eta_sumAll->getROOTHistBase();     
	
      fillEffHist(qualityJets_sum85OP, qualityJets_sumAll, m_tag_mv_w_eta_frac85OP);
      fillEffHist(qualityJets_sum77OP, qualityJets_sumAll, m_tag_mv_w_eta_frac77OP);
      fillEffHist(qualityJets_sum70OP, qualityJets_sumAll, m_tag_mv_w_eta_frac70OP);
      fillEffHist(qualityJets_sum60OP, qualityJets_sumAll, m_tag_mv_w_eta_frac60OP);
    }

    {
      TH1 * suspectJets_sum85OP = m_tag_mv_w_sj_phi_sum85OP->getROOTHistBase();     
      TH1 * suspectJets_sum77OP = m_tag_mv_w_sj_phi_sum77OP->getROOTHistBase();     
      TH1 * suspectJets_sum70OP = m_tag_mv_w_sj_phi_sum70OP->getROOTHistBase();     
      TH1 * suspectJets_sum60OP = m_tag_mv_w_sj_phi_sum60OP->getROOTHistBase();     
      TH1 * suspectJets_sumAll  = m_tag_mv_w_sj_phi_sumAll->getROOTHistBase();     
	
      fillEffHist(suspectJets_sum85OP, suspectJets_sumAll, m_tag_mv_w_sj_phi_frac85OP);
      fillEffHist(suspectJets_sum77OP, suspectJets_sumAll, m_tag_mv_w_sj_phi_frac77OP);
      fillEffHist(suspectJets_sum70OP, suspectJets_sumAll, m_tag_mv_w_sj_phi_frac70OP);
      fillEffHist(suspectJets_sum60OP, suspectJets_sumAll, m_tag_mv_w_sj_phi_frac60OP);
    }

    {
      TH1 * suspectJets_sum85OP = m_tag_mv_w_sj_eta_sum85OP->getROOTHistBase();     
      TH1 * suspectJets_sum77OP = m_tag_mv_w_sj_eta_sum77OP->getROOTHistBase();     
      TH1 * suspectJets_sum70OP = m_tag_mv_w_sj_eta_sum70OP->getROOTHistBase();     
      TH1 * suspectJets_sum60OP = m_tag_mv_w_sj_eta_sum60OP->getROOTHistBase();     
      TH1 * suspectJets_sumAll  = m_tag_mv_w_sj_eta_sumAll->getROOTHistBase();     
	
      fillEffHist(suspectJets_sum85OP, suspectJets_sumAll, m_tag_mv_w_sj_eta_frac85OP);
      fillEffHist(suspectJets_sum77OP, suspectJets_sumAll, m_tag_mv_w_sj_eta_frac77OP);
      fillEffHist(suspectJets_sum70OP, suspectJets_sumAll, m_tag_mv_w_sj_eta_frac70OP);
      fillEffHist(suspectJets_sum60OP, suspectJets_sumAll, m_tag_mv_w_sj_eta_frac60OP);
    }

  }
  ATH_MSG_DEBUG("Exiting from procHistograms()");
  return StatusCode::SUCCESS;
}


void JetTagMonitoring::fillJetHistograms() {
  
  ATH_MSG_DEBUG("in fillJetHistograms()");
  
  /////////////////////
  //* Jet container *//
  /////////////////////
  
  const DataHandle<xAOD::JetContainer> jets;
  bool foundJetColl = retrieveCollection(m_storeGate, jets, m_jetName);
  
  if (!foundJetColl) {
    ATH_MSG_WARNING("Unable to retrieve \"" << m_jetName << "\" from StoreGate");
    return;
  }
  
  ATH_MSG_DEBUG("JetContainer \"" << m_jetName << "\" found with " << jets->size() << " entries");
  
  xAOD::JetContainer::const_iterator jetItr = jets->begin();
  xAOD::JetContainer::const_iterator jetEnd = jets->end();
  
  // Do top-pairs selection
  bool is_top_event = isTopEvent();
  
  // Do SMT selection and fill SMT plots
  bool is_smt_event = isSoftMuonEvent();
  if(is_smt_event) ATH_MSG_DEBUG("found SMT jet event");
  
  m_jet_n->Fill((*jets).size());
    
  int n_jets_kinematic = 0;
  xAOD::JetContainer::const_iterator firstKinematicjetItr = jets->end();
  
  // Loop over jets
  for ( ; jetItr != jetEnd; jetItr++) {
    
    // All jets
    
    m_cutflow_jet->Fill(0.); // all jets
    
    m_jet_2D_all->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
    
    if( !passJetQualityCuts(*jetItr) ) continue;
    
    // Jets passing quality cuts
    
    m_cutflow_jet->Fill(1.); // good jets (standard jet cleaning cuts)
    
    m_jet_2D_good->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

    if ( !passKinematicCuts(*jetItr) ) continue; 
    
    // Jets passing kinemtic cuts

    m_cutflow_jet->Fill(2.); // jets passing kinematic cuts

    m_jet_2D_kinematic->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
    m_jet_2D_kinematic_LS->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

    fillTrackInJetHistograms(*jetItr); // fill histograms with properties of associated tracks

    double mv = getMVweight(*jetItr); // get MV weight (MV2c10/DL1*)

    // Plots with and without JVT
    static SG::AuxElement::Accessor<float> JVT( "Jvt" ); //JVT > 0.59 (60 GeV)
    double jvt = JVT(*(*jetItr));
    m_jvt->Fill(jvt);
    m_mv_pre_jvt_cut->Fill(mv);
    
    if ( !passJVTCuts(*jetItr) ) continue; 

    // Jets passing JVT cuts
    m_mv_post_jvt_cut->Fill(mv);

    m_cutflow_jet->Fill(3.); // passing JVT cuts (Jet Vertex Tagger, against pile-up jets)

    m_jet_2D_mjvt->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

    if ( !passMuonOverlap(*jetItr) ) continue; 

    m_cutflow_jet->Fill(4.); // jet passing basic isolated-muon overlap

    m_jet_2D_overlap->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);

    if (n_jets_kinematic == 0) firstKinematicjetItr = jetItr; //1st jet iterator needed to fill top histograms (if a 2nd kinematic jet is found later)
    ++n_jets_kinematic; //actually the cut now is after JVT selection, but called kinematic jet...

    Jet_t taggabilityLabel = getTaggabilityLabel(*jetItr); // check if jet is taggable (defined as goodJet or suspectJet)
    
    if ( taggabilityLabel == goodJet || taggabilityLabel == suspectJet ) {
      m_cutflow_jet->Fill(5.);
      fillDetailedHistograms(*jetItr, taggabilityLabel);
    }
    
    if ( taggabilityLabel == goodJet ) {
      m_cutflow_jet->Fill(6.);
      m_jet_2D_quality->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
      m_jet_2D_quality_LS->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
      fillGoodJetHistos(*jetItr);

      fillBtagHistograms(*jetItr, false); // fill non-top histograms with b-tagging information
    
      // Looking for ttbar events
      if (is_top_event && n_jets_kinematic >= 2) { //at least 2 kinematic jets --> fill top histograms
	m_jet_top->Fill(0., 1.);
	m_jet_pt_top->Fill((*jetItr)->pt() / CLHEP::GeV, 1.);
	fillBtagHistograms(*jetItr, true); // fill histograms with b-tagging information
	if (n_jets_kinematic == 2) { //(at least) two kinematic jets --> fill also for 1st kinematic jet
	  m_jet_top->Fill(0., 1.);
	  m_jet_pt_top->Fill((*firstKinematicjetItr)->pt() / CLHEP::GeV, 1.);
	  fillBtagHistograms(*firstKinematicjetItr, true); // fill histograms with b-tagging information
	}
      }
      
    }
    else if ( taggabilityLabel == suspectJet ) {
      m_cutflow_jet->Fill(7.);
      m_jet_2D_suspect->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
      fillSuspectJetHistos(*jetItr);
    }
    else if ( taggabilityLabel == badJet ) {
      m_jet_2D_tbad->Fill((*jetItr)->eta(), (*jetItr)->phi(), 1.);
      //fillBadJetHistos(*jetItr); //nothing to fill
    }
    
  }
  
  ATH_MSG_DEBUG("... done fillJetHistograms()");
  return;
}

void JetTagMonitoring::fillTrackInJetHistograms(const xAOD::Jet *jet) {
  
  ATH_MSG_DEBUG("in fillInJetTrackHistograms()");
  
  //* get information about Track Constituents *//
  int nTrkInJet = 0;

  std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);

  std::vector<const xAOD::IParticle*>::const_iterator trkItr;

  nTrkInJet = trackVector.size();
  ATH_MSG_DEBUG("examining " << nTrkInJet << " tracks.");

  // loop over tracks in a jet // 
  for ( trkItr = trackVector.begin(); trkItr != trackVector.end() ; trkItr++ ) {

    const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*trkItr);
    if ( ! trackPart ) continue;

    uint8_t nBLayerHits = 0;
    uint8_t nPixHits    = 0;
    uint8_t nSCTHits    = 0;

    m_jet_tracks_pt->Fill(trackPart->pt() / CLHEP::GeV);
    m_jet_tracks_eta->Fill(trackPart->eta());
    m_jet_tracks_phi->Fill(trackPart->phi());
    m_jet_tracks_d0->Fill(trackPart->d0());
    m_jet_tracks_z0->Fill(trackPart->z0());
    m_jet_tracks_d0_LS->Fill(trackPart->d0());
    m_jet_tracks_z0_LS->Fill(trackPart->z0());

    if (trackPart->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits)) { m_jet_tracks_BLayerHits->Fill((float) nBLayerHits); }
    if (trackPart->summaryValue(nPixHits, xAOD::numberOfPixelHits))     { m_jet_tracks_PixelHits->Fill((float)  nPixHits); }
    if (trackPart->summaryValue(nSCTHits, xAOD::numberOfSCTHits))       { m_jet_tracks_SCTHits->Fill((float)    nSCTHits); }
    
  }

  m_jet_tracks_n->Fill((float)nTrkInJet);

  return;
}

void JetTagMonitoring::fillBtagHistograms(const xAOD::Jet *jet, bool fill_top_histos) {
  
  const xAOD::BTagging* btag = jet->btagging();
  
  if (!btag) return;
  
  double sv1ip3d = btag->SV1plusIP3D_discriminant();
  double mv = getMVweight(jet); // get MV weight (MV2c10/DL1*)

  ATH_MSG_DEBUG("btagging weights: " << "SV1IP3D: " << sv1ip3d << ", MV: "  << mv);
  
  if (fill_top_histos) { //fill top histograms
    m_jet_mv_w_top->Fill(mv);
    if (mv > m_mv_70_weight_cut) {
      m_jet_top_tagged->Fill(0., 1.);
      m_jet_pt_top_tagged->Fill(jet->pt() / CLHEP::GeV, 1.);
    }
  }
  else { //fill non-top histograms
    if ( sv1ip3d > m_sv1ip3d_weight_cut ) {
      m_sv1ip3d_tag_pos_rate_2D->Fill(jet->eta(), jet->phi(), 1.);
    } 
    else {
      m_sv1ip3d_tag_neg_rate_2D->Fill(jet->eta(), jet->phi(), 1.);
    }
    
    if ( mv > m_mv_60_weight_cut ) m_mv_tag_60_2D->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_70_weight_cut ) m_mv_tag_70_2D->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_77_weight_cut ) m_mv_tag_77_2D->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_85_weight_cut ) m_mv_tag_85_2D->Fill(jet->eta(), jet->phi(), 1.);

    if ( mv > m_mv_60_weight_cut ) m_mv_tag_60_2D_LS->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_70_weight_cut ) m_mv_tag_70_2D_LS->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_77_weight_cut ) m_mv_tag_77_2D_LS->Fill(jet->eta(), jet->phi(), 1.);
    if ( mv > m_mv_85_weight_cut ) m_mv_tag_85_2D_LS->Fill(jet->eta(), jet->phi(), 1.);
  }
 
  return;
  
}

void JetTagMonitoring::fillDetailedHistograms(const xAOD::Jet *jet, Jet_t taggabilityLabel) {

  ATH_MSG_DEBUG("in fillDetailedHistograms()");

  //* get detailed information *//
  const xAOD::BTagging* btag = jet->btagging(); 

  if (!btag) return;

  //* detailed information for impact parameter-based informations *//
  unsigned int ntrk  = btag->nIP3D_TrackParticles();                  // number of tracks used for tagging in the jet
  double pb          = btag->IP3D_pb();                               // b likelihood
  double pu          = btag->IP3D_pu();                               // u likelihood
  double pc          = btag->IP3D_pc();                               // c likelihood
  double llr         = btag->IP3D_loglikelihoodratio();               // log likelihood ratio 
        
  if ( taggabilityLabel == goodJet ) {
    m_tag_ip3d_n->Fill((float)ntrk); 
    m_tag_ip3d_b->Fill(pb);
    m_tag_ip3d_u->Fill(pu);
    m_tag_ip3d_c->Fill(pc);
    m_tag_ip3d_llr->Fill(llr);    
  }
  else if ( taggabilityLabel == suspectJet ) {
    m_tag_ip3d_n_sj->Fill((float)ntrk); 
    m_tag_ip3d_b_sj->Fill(pb);
    m_tag_ip3d_u_sj->Fill(pu);
    m_tag_ip3d_c_sj->Fill(pc);
    m_tag_ip3d_llr_sj->Fill(llr);    
  }

  ATH_MSG_VERBOSE("InfoBase for IP3D: #tracks = " << ntrk << ", Pb = " << pb 
		  << ", Pu = " << pu<< ", Pc = " << pc << ", llr = " << llr);

  ntrk  = btag->nIP2D_TrackParticles();  // number of tracks used for tagging in the jet
  pb          = btag->IP2D_pb();        // b likelihood
  pu          = btag->IP2D_pu();        // u likelihood
  pc          = btag->IP2D_pc();        // c likelihood
  llr         = btag->IP2D_loglikelihoodratio(); // log likelihood ratio 
    
  if ( taggabilityLabel == goodJet ) {
    m_tag_ip2d_n->Fill((float)ntrk); 
    m_tag_ip2d_b->Fill(pb);
    m_tag_ip2d_u->Fill(pu);
    m_tag_ip2d_c->Fill(pc);
    m_tag_ip2d_llr->Fill(llr);  
  }
  else if ( taggabilityLabel == suspectJet ) {
    m_tag_ip2d_n_sj->Fill((float)ntrk); 
    m_tag_ip2d_b_sj->Fill(pb);
    m_tag_ip2d_u_sj->Fill(pu);
    m_tag_ip2d_c_sj->Fill(pc);
    m_tag_ip2d_llr_sj->Fill(llr);  
  }
     
  ATH_MSG_VERBOSE("InfoBase for IP2D: #tracks = " << ntrk << ", Pb = " << pb 
		  << ", Pu = " << pu<< ", Pc = " << pc << ", llr = " << llr);

  float sig3d = 0;
  btag->variable<float>("SV1", "significance3d" , sig3d);
  m_tag_sv0_sig3d->Fill(sig3d);
    
  pb          = btag->SV1_pb();        // b likelihood
  pu          = btag->SV1_pu();        // u likelihood
  pc          = btag->SV1_pc();        // c likelihood
  llr         = btag->SV1_loglikelihoodratio();// log likelihood ratio 
    
  if ( taggabilityLabel == goodJet ) {
    m_tag_sv1_b->Fill(pb);
    m_tag_sv1_u->Fill(pu);
    m_tag_sv1_c->Fill(pc);
    m_tag_sv1_llr->Fill(llr);
  }
  else if ( taggabilityLabel == suspectJet ) {
    m_tag_sv1_b_sj->Fill(pb);
    m_tag_sv1_u_sj->Fill(pu);
    m_tag_sv1_c_sj->Fill(pc);
    m_tag_sv1_llr_sj->Fill(llr);
  }

  ATH_MSG_VERBOSE("InfoBase for SV1: Pb = " << pb << ", Pu = " << pu<< ", Pc = " << pc<< ", llr = " << llr);
    
  llr = btag->JetFitter_loglikelihoodratio(); // log likelihood ratio 
  if ( taggabilityLabel == goodJet ) {
    m_tag_jetfitter_llr->Fill(llr);
  }
  else if ( taggabilityLabel == suspectJet ) {
    m_tag_jetfitter_llr_sj->Fill(llr);
  }
 
  llr = btag->JetFitter_loglikelihoodratio();  // log likelihood ratio
 
  if ( taggabilityLabel == goodJet ) {
    m_tag_jfcnn_llr->Fill(llr);
  }
  else if ( taggabilityLabel == suspectJet ) {
    m_tag_jfcnn_llr_sj->Fill(llr);
  }  
  
  return;
}


void JetTagMonitoring::fillBadTrackBits(const std::bitset<17> failedCuts, double eta, double phi) {

  if ( failedCuts[pTMin]           == 0 ) m_tracks_pTMin_2D->Fill(eta, phi, 1.);
  if ( failedCuts[d0Max]           == 0 ) m_tracks_d0Max_2D->Fill(eta, phi, 1.);
  if ( failedCuts[z0Max]           == 0 ) m_tracks_z0Max_2D->Fill(eta, phi, 1.);
  if ( failedCuts[sigd0Max]        == 0 ) m_tracks_sigd0Max_2D->Fill(eta, phi, 1.);
  if ( failedCuts[sigz0Max]        == 0 ) m_tracks_sigz0Max_2D->Fill(eta, phi, 1.);
  if ( failedCuts[etaMax]          == 0 ) m_tracks_etaMax_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitBLayer]      == 0 ) m_tracks_nHitBLayer_2D->Fill(eta, phi, 1.);
  if ( failedCuts[deadBLayer]      == 0 ) m_tracks_deadBLayer_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitPix]         == 0 ) m_tracks_nHitPix_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitSct]         == 0 ) m_tracks_nHitSct_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitSi]          == 0 ) m_tracks_nHitSi_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitTrt]         == 0 ) m_tracks_nHitTrt_2D->Fill(eta, phi, 1.);
  if ( failedCuts[nHitTrtHighE]    == 0 ) m_tracks_nHitTrtHighE_2D->Fill(eta, phi, 1.);
  if ( failedCuts[fitChi2]         == 0 ) m_tracks_fitChi2_2D->Fill(eta, phi, 1.);
  if ( failedCuts[fitProb]         == 0 ) m_tracks_fitProb_2D->Fill(eta, phi, 1.);
  if ( failedCuts[fitChi2OnNdfMax] == 0 ) m_tracks_fitChi2OnNdfMax_2D->Fill(eta, phi, 1.);

  if ( failedCuts[pTMin]           == 0 ) m_tracks_pTMin_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[d0Max]           == 0 ) m_tracks_d0Max_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[z0Max]           == 0 ) m_tracks_z0Max_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[sigd0Max]        == 0 ) m_tracks_sigd0Max_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[sigz0Max]        == 0 ) m_tracks_sigz0Max_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[etaMax]          == 0 ) m_tracks_etaMax_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitBLayer]      == 0 ) m_tracks_nHitBLayer_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[deadBLayer]      == 0 ) m_tracks_deadBLayer_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitPix]         == 0 ) m_tracks_nHitPix_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitSct]         == 0 ) m_tracks_nHitSct_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitSi]          == 0 ) m_tracks_nHitSi_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitTrt]         == 0 ) m_tracks_nHitTrt_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[nHitTrtHighE]    == 0 ) m_tracks_nHitTrtHighE_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[fitChi2]         == 0 ) m_tracks_fitChi2_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[fitProb]         == 0 ) m_tracks_fitProb_2D_LS->Fill(eta, phi, 1.);
  if ( failedCuts[fitChi2OnNdfMax] == 0 ) m_tracks_fitChi2OnNdfMax_2D_LS->Fill(eta, phi, 1.);

  return;

}



void JetTagMonitoring::fillGoodJetHistos(const xAOD::Jet *jet) {

  const xAOD::BTagging* btag = jet->btagging();
  
  if (!btag) return;
  
  double sv1ip3d = btag->SV1plusIP3D_discriminant();
  double mv = getMVweight(jet); // get MV weight (MV2c10/DL1*)

  m_tag_sv1ip3d_w->Fill(sv1ip3d);
  m_tag_mv_w->Fill(mv);   
  m_tag_mv_w_LS->Fill(mv);   

  // Plots Tagger in pile up bins
  double mu = m_mu; 

  if ( mu > 0. && mu < 30. ) m_tag_mv_w_mu0_30->Fill(mv);
  else if ( mu > 30. && mu < 50. ) m_tag_mv_w_mu30_50->Fill(mv);
  else if ( mu > 50. && mu < 100. ) m_tag_mv_w_mu50_100->Fill(mv);

  // Plots Tagger in n_tracks in jet bins
  int nTrkInJet = 0;
  std::vector<const xAOD::IParticle*> trackVector = jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);
  nTrkInJet = trackVector.size();

  if ( nTrkInJet > 0 && nTrkInJet < 10 ) m_tag_mv_w_tracks0_10->Fill(mv); 
  else if ( nTrkInJet > 10 && nTrkInJet < 20 ) m_tag_mv_w_tracks10_20->Fill(mv);
  else if ( nTrkInJet > 20 && nTrkInJet < 50 ) m_tag_mv_w_tracks20_50->Fill(mv);

  m_mu_vs_n_tracks->Fill(nTrkInJet,mu);
  m_prof_n_tracks_vs_mu->Fill(mu,nTrkInJet,1);

  // Fill general jet kinematic plots vs High level tagger

  if      ( jet->pt() / CLHEP::GeV > 200. ) m_tag_mv_w_pT200->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV > 100. ) m_tag_mv_w_pT100_200->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  50. ) m_tag_mv_w_pT50_100->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  20. ) m_tag_mv_w_pT20_50->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  10. ) m_tag_mv_w_pT10_20->Fill(mv);   

  if      ( jet->pt() / CLHEP::GeV > 200. ) m_tag_mv_w_pT200_LS->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV > 100. ) m_tag_mv_w_pT100_200_LS->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  50. ) m_tag_mv_w_pT50_100_LS->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  20. ) m_tag_mv_w_pT20_50_LS->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  10. ) m_tag_mv_w_pT10_20_LS->Fill(mv);   


  if      ( fabs(jet->eta()) > 2.0 ) m_tag_mv_w_eta20_25->Fill(mv);   
  else if ( fabs(jet->eta()) > 1.5 ) m_tag_mv_w_eta15_20->Fill(mv);   
  else if ( fabs(jet->eta()) > 1.0 ) m_tag_mv_w_eta10_15->Fill(mv);   
  else if ( fabs(jet->eta()) > 0.5 ) m_tag_mv_w_eta05_10->Fill(mv);   
  else                               m_tag_mv_w_eta0_05->Fill(mv);   

  if      ( fabs(jet->phi()) > 2.8 ) m_tag_mv_w_phi28->Fill(mv);
  else if ( fabs(jet->phi()) > 2.1 ) m_tag_mv_w_phi21_28->Fill(mv);
  else if ( fabs(jet->phi()) > 1.4 ) m_tag_mv_w_phi14_21->Fill(mv);   
  else if ( fabs(jet->phi()) > 0.7 ) m_tag_mv_w_phi07_14->Fill(mv);   
  else                               m_tag_mv_w_phi0_07->Fill(mv);   

  if ( mv > m_mv_85_weight_cut ) {
    m_tag_mv_w_phi_sum85OP->Fill(jet->phi());
    m_tag_mv_w_eta_sum85OP->Fill(jet->eta());
  }
  if ( mv > m_mv_77_weight_cut ) {
    m_tag_mv_w_phi_sum77OP->Fill(jet->phi());
    m_tag_mv_w_eta_sum77OP->Fill(jet->eta());
  }
  if ( mv > m_mv_70_weight_cut ) {
    m_tag_mv_w_phi_sum70OP->Fill(jet->phi());
    m_tag_mv_w_eta_sum70OP->Fill(jet->eta());
  }
  if ( mv >  m_mv_60_weight_cut ) {
    m_tag_mv_w_phi_sum60OP->Fill(jet->phi());
    m_tag_mv_w_eta_sum60OP->Fill(jet->eta());
  }
  m_tag_mv_w_phi_sumAll->Fill(jet->phi());
  m_tag_mv_w_eta_sumAll->Fill(jet->eta());

  return;

}

void JetTagMonitoring::fillSuspectJetHistos(const xAOD::Jet *jet) {

  m_track_selector_suspect->Fill(jet->eta(), jet->phi(), 1.);
  
  const xAOD::BTagging* btag = jet->btagging();
  
  if (!btag) return;
  
  double sv1ip3d = btag->SV1plusIP3D_discriminant();
  double mv = getMVweight(jet); // get MV weight (MV2c10/DL1*)

  m_tag_sv1ip3d_w_sj->Fill(sv1ip3d);
  m_tag_mv_w_sj->Fill(mv);   

  if      ( jet->pt() / CLHEP::GeV > 200. ) m_tag_mv_w_sj_pT200->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV > 100. ) m_tag_mv_w_sj_pT100_200->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  50. ) m_tag_mv_w_sj_pT50_100->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  20. ) m_tag_mv_w_sj_pT20_50->Fill(mv);   
  else if ( jet->pt() / CLHEP::GeV >  10. ) m_tag_mv_w_sj_pT10_20->Fill(mv);     

  if      ( fabs(jet->eta()) > 2.0 ) m_tag_mv_w_sj_eta20_25->Fill(mv);   
  else if ( fabs(jet->eta()) > 1.5 ) m_tag_mv_w_sj_eta15_20->Fill(mv);   
  else if ( fabs(jet->eta()) > 1.0 ) m_tag_mv_w_sj_eta10_15->Fill(mv);   
  else if ( fabs(jet->eta()) > 0.5 ) m_tag_mv_w_sj_eta05_10->Fill(mv);   
  else                               m_tag_mv_w_sj_eta0_05->Fill(mv);   

  if      ( fabs(jet->phi()) > 2.8 ) m_tag_mv_w_sj_phi28->Fill(mv);
  else if ( fabs(jet->phi()) > 2.1 ) m_tag_mv_w_sj_phi21_28->Fill(mv);
  else if ( fabs(jet->phi()) > 1.4 ) m_tag_mv_w_sj_phi14_21->Fill(mv);   
  else if ( fabs(jet->phi()) > 0.7 ) m_tag_mv_w_sj_phi07_14->Fill(mv);   
  else                               m_tag_mv_w_sj_phi0_07->Fill(mv); 

  if ( mv > m_mv_85_weight_cut ) {
    m_tag_mv_w_sj_phi_sum85OP->Fill(jet->phi());
    m_tag_mv_w_sj_eta_sum85OP->Fill(jet->eta());
  }
  if ( mv > m_mv_77_weight_cut ) {
    m_tag_mv_w_sj_phi_sum77OP->Fill(jet->phi());
    m_tag_mv_w_sj_eta_sum77OP->Fill(jet->eta());
  }
  if ( mv > m_mv_70_weight_cut ) {
    m_tag_mv_w_sj_phi_sum70OP->Fill(jet->phi());
    m_tag_mv_w_sj_eta_sum70OP->Fill(jet->eta());
  }
  if ( mv >  m_mv_60_weight_cut ) {
    m_tag_mv_w_sj_phi_sum60OP->Fill(jet->phi());
    m_tag_mv_w_sj_eta_sum60OP->Fill(jet->eta());
  }
  m_tag_mv_w_sj_phi_sumAll->Fill(jet->phi());
  m_tag_mv_w_sj_eta_sumAll->Fill(jet->eta());

  return;
}
