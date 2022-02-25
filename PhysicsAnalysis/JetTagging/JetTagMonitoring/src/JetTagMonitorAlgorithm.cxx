/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMonitorAlgorithm.h"

JetTagMonitorAlgorithm::JetTagMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("JetsCollection",m_JetContainerKey);
  declareProperty("MuonsCollection",m_MuonContainerKey);
  declareProperty("ElectronsCollection",m_ElectronContainerKey);

  declareProperty("SkipJetFilter", m_SkipJetFilter);
  declareProperty("DoExtraTaggerHistos", m_DoExtraTaggerHistos);
 
  declareProperty("JetEtaCut", m_JetEtaCut);
  declareProperty("JetPtCut", m_JetPtCut);
  declareProperty("SoftMuonPtMin", m_SoftMuonPtMin);
  declareProperty("SoftMuonPtMax", m_SoftMuonPtMax);

  declareProperty("TrackSelectionTool",m_TrackSelectionTool);
  declareProperty("MinGoodTrackCut", m_MinGoodTrackCut);
  declareProperty("TrackPtCut", m_TrackPtCut);
  declareProperty("Trackd0Cut", m_Trackd0Cut);
  declareProperty("Trackz0sinCut", m_Trackz0sinCut);
  declareProperty("TrackHitIBLCut", m_TrackHitIBLCut);

  declareProperty("ElectronPtCut", m_ElectronPtCut);
  declareProperty("MuonPtCut", m_MuonPtCut);
  declareProperty("ElectronEtaCut", m_ElectronEtaCut);
  declareProperty("MuonEtaCut", m_MuonEtaCut);
  declareProperty("ElectronEtaCrackLowCut", m_ElectronEtaCrackLowCut);
  declareProperty("ElectronEtaCrackHighCut", m_ElectronEtaCrackHighCut);

  declareProperty("ElectronTopoEtCone20Cut", m_ElectronTopoEtCone20Cut);
  declareProperty("MuonTopoEtCone20Cut", m_MuonTopoEtCone20Cut);
  declareProperty("ElectronPtVarCone20Cut", m_ElectronPtVarCone20Cut);
  declareProperty("MuonPtVarCone30Cut", m_MuonPtVarCone30Cut);

  declareProperty("JVTCut", m_JVTCut);
  declareProperty("JVTpTCut", m_JVTpTCut);
  declareProperty("JVTetaCut", m_JVTetaCut);

  declareProperty("TaggerName", m_TaggerName);
  declareProperty("cFraction", m_cFraction);
  declareProperty("WP60Cut", m_WP60Cut);
  declareProperty("WP70Cut", m_WP70Cut);
  declareProperty("WP77Cut", m_WP77Cut);
  declareProperty("WP85Cut", m_WP85Cut);
}

JetTagMonitorAlgorithm::~JetTagMonitorAlgorithm() {}

StatusCode JetTagMonitorAlgorithm::initialize() {

  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_JetContainerKey.initialize());
  ATH_CHECK(m_MuonContainerKey.initialize());
  ATH_CHECK(m_ElectronContainerKey.initialize());

  ATH_CHECK(m_VertContainerKey.initialize());
  ATH_CHECK(m_TrackContainerKey.initialize());

  if (m_btagLinkKey.empty()) {
    m_btagLinkKey = m_JetContainerKey.key() + ".btaggingLink";
  }
  ATH_CHECK(m_btagLinkKey.initialize());

  if (m_btagResultKey.empty()) {
    if (m_TaggerName=="DL1dv00" || m_TaggerName=="DL1r") {
      std::string rawJetContainerName = m_JetContainerKey.key();
      const size_t jetStringItr = rawJetContainerName.find("Jets");
      if (jetStringItr != std::string::npos) {
        rawJetContainerName = rawJetContainerName.replace(jetStringItr, std::string::npos, "");
      }
      m_btagResultKey = "BTagging_" + rawJetContainerName + "." + m_TaggerName + "_pb";
    }
  }
  ATH_CHECK(m_btagResultKey.initialize(SG::AllowEmpty));

  ATH_CHECK(m_MuonEtIsoDecorKey.initialize());
  ATH_CHECK(m_MuonPtIsoDecorKey.initialize());
  ATH_CHECK(m_EleEtIsoDecorKey.initialize());
  ATH_CHECK(m_ElePtIsoDecorKey.initialize());

  return StatusCode::SUCCESS;
}


StatusCode JetTagMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using namespace Monitored;

  auto Run_event = Monitored::Scalar<int>("Run_event",0);
  auto Run_lb = Monitored::Scalar<int>("Run_lb",0);
  auto Run_mu = Monitored::Scalar<float>("Run_mu",0);

  Run_event = GetEventInfo(ctx)->eventNumber();
  Run_lb = GetEventInfo(ctx)->lumiBlock();
  Run_mu = lbInteractionsPerCrossing(ctx);

  auto tool = getGroup("JetTagMonitor");

  auto Cutflow_Event = Monitored::Scalar<int>("Cutflow_Event",0);
  Cutflow_Event = 0;
  fill(tool,Run_lb,Run_mu,Cutflow_Event);

  if ( GetEventInfo(ctx)->errorState(xAOD::EventInfo::EventFlagSubDet::Tile) == xAOD::EventInfo::Error || 
       GetEventInfo(ctx)->errorState(xAOD::EventInfo::EventFlagSubDet::LAr)  == xAOD::EventInfo::Error ||
       GetEventInfo(ctx)->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) { 
    ATH_MSG_DEBUG("Event cleaning : skip event");
    return StatusCode::SUCCESS;    
  }

  Cutflow_Event = 1;
  fill(tool,Cutflow_Event);

  //////////////////////
  //* Primary vertex *//
  //////////////////////
 
  auto PV_n = Monitored::Scalar<int>("PV_n",0);
  auto PV_x = Monitored::Scalar<float>("PV_x",0);
  auto PV_y = Monitored::Scalar<float>("PV_y",0);
  auto PV_z = Monitored::Scalar<float>("PV_z",0);
  auto PV_tracks_n = Monitored::Scalar<int>("PV_tracks_n",0);

  float PVZ=0;
  
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_VertContainerKey, ctx);
  if (!vertices.isValid()) {
    ATH_MSG_WARNING("Could not find vertex AOD container with name " << m_VertContainerKey);
    return StatusCode::SUCCESS;
  }
  
  PV_n = vertices->size();
  fill(tool,PV_n);
  
  if (vertices->size() < 2) {
    ATH_MSG_DEBUG("No vertices reconstructed");
    return StatusCode::SUCCESS;
  }
  
  Cutflow_Event = 2;
  fill(tool,Cutflow_Event);

  bool foundVxTypePriVtx = false;

  for (const auto vertItr : *vertices) {
    ATH_MSG_DEBUG("Simple vertex: x = " << PV_x << ", y = " << PV_y << ", z = " << PV_z);
   
    if (vertItr->vertexType() == xAOD::VxType::PriVtx && vertItr->numberDoF() > 0 ) { 
      foundVxTypePriVtx = true;

      PV_x = vertItr->x();
      PV_y = vertItr->y();
      PV_z = vertItr->z();
      PVZ=PV_z;
      ATH_MSG_DEBUG("Primary vertex: x = " << PV_x << ", y = " << PV_y << ", z = " << PV_z);
      
      if (PV_x == 0 && PV_y == 0 && PV_z == 0)
	ATH_MSG_DEBUG("Primary Vertex is (0,0,0)");
      
      if (vertItr->nTrackParticles()>0) {
	PV_tracks_n = vertItr->nTrackParticles();
	ATH_MSG_DEBUG("PV has "<< PV_tracks_n <<" tracks");
      }

      fill(tool,PV_x,PV_y,PV_z,PV_tracks_n);

    }
  }
  
  if ( ! foundVxTypePriVtx ) {
    ATH_MSG_DEBUG("xAOD::Vertex of type xAOD::VxType::PriVtx was not found in vertex container.");
    return StatusCode::SUCCESS;
  }

  if ( PV_tracks_n <  4 )
    return StatusCode::SUCCESS;

  Cutflow_Event = 3;
  fill(tool,Cutflow_Event);

  //----------------------Start Muon & Electron Part for pre-selection and ttbar events ------------------------

  //////////////////////
  //* Muon container *//
  //////////////////////

  auto Muon_pT = Monitored::Scalar<float>("Muon_pT",0.0);
  auto IsolatedMuons_n = Monitored::Scalar<int>("IsolatedMuons_n",0.0);
  int Muon_charge = 0;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_WARNING("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::SUCCESS;
  }

  for (const auto muonItr : *muons) {
    ATH_MSG_DEBUG("Muon kinematics: eta = " << muonItr->eta() << ", phi= " << muonItr->phi() << ", pT= " << muonItr->pt() / Gaudi::Units::GeV);
    Muon_pT = muonItr->pt();
    fill(tool,Muon_pT);

    //Look for isolated muons (for pre-selection and ttbar events selection)
    //Select muons which passed pT cut
    if (muonItr->pt() / Gaudi::Units::GeV < m_MuonPtCut) continue;
    bool inAcceptance = TMath::Abs(muonItr->eta()) < m_MuonEtaCut;
    if (!inAcceptance) continue;
    //Select medium muons
    if (muonItr->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)
    //Select isolated muons
    float topoetcone20_value = -999.;
    float ptvarcone30_value = -999.;
    muonItr-> isolation(topoetcone20_value, xAOD::Iso::topoetcone20);
    muonItr-> isolation(ptvarcone30_value, xAOD::Iso::ptvarcone30);
    if (topoetcone20_value/muonItr->pt() > m_MuonTopoEtCone20Cut) continue;
    if (ptvarcone30_value/muonItr->pt() > m_MuonPtVarCone30Cut) continue;
    Muon_charge = muonItr->charge();
    ++IsolatedMuons_n;
  }

  if(IsolatedMuons_n>0)
    ATH_MSG_DEBUG(IsolatedMuons_n << " Muon(s) isolated in event "<< Run_event);
    
  //////////////////////////
  //* Electron container *//
  //////////////////////////

  auto Electron_pT = Monitored::Scalar<float>("Electron_pT",0.0);
  auto IsolatedElectrons_n = Monitored::Scalar<int>("IsolatedElectrons_n",0.0);
  int Electron_charge = 0;

  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_ElectronContainerKey, ctx);
  if (! electrons.isValid() ) {
    ATH_MSG_WARNING("evtStore() does not contain electron Collection with name "<< m_ElectronContainerKey);
    return StatusCode::SUCCESS;
  }

  for (const auto electronItr : *electrons) {
    ATH_MSG_DEBUG("Electron kinematics: eta = " << electronItr->eta() << ", phi= " << electronItr->phi() << ", pT= " << electronItr->pt() / Gaudi::Units::GeV);
    Electron_pT = electronItr->pt();
    fill(tool,Electron_pT);

    //Look for isolated electrons (for pre-selection and ttbar events selection)
    //Select electrons which passed author and pT cut
    if (!electronItr->author(xAOD::EgammaParameters::AuthorElectron)) continue; 
    if (electronItr->pt() / Gaudi::Units::GeV < m_ElectronPtCut) continue;
    bool inAcceptance = (TMath::Abs(electronItr->eta()) > m_ElectronEtaCrackHighCut || TMath::Abs(electronItr->eta()) < m_ElectronEtaCrackLowCut) 
      && TMath::Abs(electronItr->eta()) < m_ElectronEtaCut;
    if (!inAcceptance) continue;
    //Select mediumLH electron
    bool lhmedium = false;
    electronItr->passSelection(lhmedium, "LHMedium");
    if (!lhmedium) continue;
    //Select isolated electrons
    float topoetcone20_value = -999.;
    float ptvarcone20_value = -999.;
    electronItr-> isolationValue(topoetcone20_value, xAOD::Iso::topoetcone20);
    electronItr-> isolationValue(ptvarcone20_value, xAOD::Iso::ptvarcone20);
    if (topoetcone20_value/electronItr->pt() > m_ElectronTopoEtCone20Cut) continue;
    if (ptvarcone20_value/electronItr->pt() > m_ElectronPtVarCone20Cut) continue;
    Electron_charge = electronItr->charge();
    ++IsolatedElectrons_n;
  }

  if(IsolatedElectrons_n>0)
    ATH_MSG_DEBUG(IsolatedElectrons_n << " Electron(s) isolated in event "<< Run_event);

  fill(tool,IsolatedMuons_n,IsolatedElectrons_n);

  // Require eventually opposite sign electron + muon to identify ttbar candidate events (used to fill few plots with ttbar jets)
  bool isTTbarEvent = false;
  if (IsolatedElectrons_n == 1 && IsolatedMuons_n == 1 && Electron_charge*Muon_charge == -1) isTTbarEvent = true;

  if(isTTbarEvent)
    ATH_MSG_DEBUG("This is a candidate ttbar event "<< Run_event);

  // Require isolated electron or muon to reduce pile-up dependency (replace trigger pre-selection)
  if (IsolatedElectrons_n>0 || IsolatedMuons_n>0){
    Cutflow_Event = 4;
    fill(tool,Cutflow_Event);

    if (IsolatedElectrons_n>0){ //at least one electron
      Cutflow_Event = 5;
      fill(tool,Cutflow_Event);
    }
    else if(IsolatedMuons_n>0){  //at least one muon
      Cutflow_Event = 6;
      fill(tool,Cutflow_Event);
    }

  }
  else { //no single lepton
    return StatusCode::SUCCESS;
  }
  
  //Events are selected
  //Fill track plots with ID tracks
  //Fill jet histograms (after Jet Selection)  
  
  ///////////////////////////////
  //* TrackParticle container *//
  ///////////////////////////////
        
  auto Tracks_n = Monitored::Scalar<int>("Tracks_n",0);

  uint8_t nIBLHits   = 0;
  uint8_t nPixelHits = 0;
  uint8_t nSCTHits   = 0;
  uint8_t nTRTHits   = 0;

  auto Hits_IBL = Monitored::Scalar<int>("Hits_IBL",0);
  auto Hits_Pixel = Monitored::Scalar<int>("Hits_Pixel",0);
  auto Hits_SCT = Monitored::Scalar<int>("Hits_SCT",0);
  auto Hits_Si = Monitored::Scalar<int>("Hits_Si",0);
  auto Hits_TRT = Monitored::Scalar<int>("Hits_TRT",0);
  auto Hits_ID = Monitored::Scalar<int>("Hits_ID",0);

  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrackContainerKey, ctx);
  if (!tracks.isValid()) {
    ATH_MSG_WARNING("Could not find track AOD container with name " << m_TrackContainerKey);
    return StatusCode::SUCCESS;
  }
  
  Tracks_n = tracks->size();
  fill(tool,Tracks_n);

  for (const auto trackItr : *tracks) {
    trackItr->summaryValue(nIBLHits, xAOD::numberOfInnermostPixelLayerHits);
    Hits_IBL = (int)nIBLHits;
    trackItr->summaryValue(nPixelHits, xAOD::numberOfPixelHits);
    Hits_Pixel=(int)nPixelHits;
    trackItr->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    Hits_SCT=(int)nSCTHits;
    trackItr->summaryValue(nTRTHits, xAOD::numberOfTRTHits);
    Hits_TRT=(int)nTRTHits;
    Hits_Si=(int)nPixelHits+(int)nSCTHits;
    Hits_ID=(int)nPixelHits+(int)nSCTHits+(int)nTRTHits;
    fill(tool,Hits_IBL,Hits_Pixel,Hits_SCT, Hits_TRT,Hits_Si,Hits_ID);
        
    ATH_MSG_DEBUG("Track hits: IBL = " << Hits_IBL << ", PIX = " << Hits_Pixel);
    ATH_MSG_DEBUG("Track hits: SCT = " << Hits_SCT << ", Si = " << Hits_Si);
    ATH_MSG_DEBUG("Track hits: TRT = " << Hits_TRT << ", ID = " << Hits_ID);
    
  }

  /////////////////////
  //* Jet container *//
  /////////////////////

  ATH_MSG_DEBUG("Start jet part");

  //----------------------Start Jet Part---------------------------

  SG::ReadHandle<xAOD::JetContainer> jets(m_JetContainerKey, ctx);
  if (!jets.isValid()) {
    ATH_MSG_WARNING("Could not find jet AOD container with name " << m_JetContainerKey);
    return StatusCode::SUCCESS;
  }

  auto Cutflow_Jet = Monitored::Scalar<int>("Cutflow_Jet",0);
 
  auto jet_n_all = Monitored::Scalar<int>("jet_n_all",0.0);

  auto jet_pT_all = Monitored::Scalar<float>("jet_pT_all",0.0);
  auto jet_pT_good = Monitored::Scalar<float>("jet_pT_good",0.0);

  auto jet_eta_all = Monitored::Scalar<float>("jet_eta_all",0.0);
  auto jet_phi_all = Monitored::Scalar<float>("jet_phi_all",0.0);
  auto jet_eta_filter = Monitored::Scalar<float>("jet_eta_filter",0.0);
  auto jet_phi_filter = Monitored::Scalar<float>("jet_phi_filter",0.0);
  auto jet_eta_kin = Monitored::Scalar<float>("jet_eta_kin",0.0);
  auto jet_phi_kin = Monitored::Scalar<float>("jet_phi_kin",0.0);
  auto jet_eta_jvt = Monitored::Scalar<float>("jet_eta_jvt",0.0);
  auto jet_phi_jvt = Monitored::Scalar<float>("jet_phi_jvt",0.0);
  auto jet_eta_overlap = Monitored::Scalar<float>("jet_eta_overlap",0.0);
  auto jet_phi_overlap = Monitored::Scalar<float>("jet_phi_overlap",0.0);
  auto jet_eta_good = Monitored::Scalar<float>("jet_eta_good",0.0);
  auto jet_phi_good = Monitored::Scalar<float>("jet_phi_good",0.0);
  auto jet_eta_suspect = Monitored::Scalar<float>("jet_eta_suspect",0.0);
  auto jet_phi_suspect = Monitored::Scalar<float>("jet_phi_suspect",0.0);
  auto jet_eta_bad = Monitored::Scalar<float>("jet_eta_bad",0.0);
  auto jet_phi_bad = Monitored::Scalar<float>("jet_phi_bad",0.0);
  auto jet_eta_smt = Monitored::Scalar<float>("jet_eta_smt",0.0);
  auto jet_phi_smt = Monitored::Scalar<float>("jet_phi_smt",0.0);

  auto jet_MV_all = Monitored::Scalar<float>("jet_MV_all",0.0);
  auto jet_MV_2_beforeJVTCut = Monitored::Scalar<float>("jet_MV_2_beforeJVTCut",0.0);
  auto jet_MV_3_afterJVTCut = Monitored::Scalar<float>("jet_MV_3_afterJVTCut",0.0);

  //Variables for TTbar events
  const xAOD::Jet* firstTTbarJet = nullptr;
  auto nTTbarGoodJets = Monitored::Scalar<int>("nTTbarGoodJets",0.0);

  // soft muon observables
  auto SoftMuons_n = Monitored::Scalar<int>("SoftMuons_n",0.0);
  auto SMTJets_n = Monitored::Scalar<int>("SMTJets_n",0.0);
  auto SMTJets_dr = Monitored::Scalar<float>("SMTJets_dr",0.0);
  auto SoftMuons_pT = Monitored::Scalar<float>("SoftMuons_pT",0.0);
  auto SMTJets_pT = Monitored::Scalar<float>("SMTJets_pT",0.0);
  auto SMTJets_pTratio = Monitored::Scalar<float>("SMTJets_pTratio",0.0);
  auto SMTJets_MV = Monitored::Scalar<float>("SMTJets_MV",0.0);
  TLorentzVector smt_jet, smt_mu;
  auto SMTJets_pTrel = Monitored::Scalar<float>("SMTJets_pTrel",0.0);

  jet_n_all = jets->size();
  fill(tool,jet_n_all);

  for (const auto jetItr : *jets) {
    
    jet_pT_all = jetItr->pt() / Gaudi::Units::GeV;
    jet_eta_all = jetItr->eta();
    jet_phi_all = jetItr->phi();
    ATH_MSG_DEBUG("Jet kinematics: eta = " << jet_eta_all << ", phi= " << jet_phi_all << ", pT= " << jet_pT_all);
    
    double mv = 0; 
    mv = getTaggerWeight(jetItr);
    jet_MV_all = mv;

    // All jets
    Cutflow_Jet = 0;
    jet_eta_all = jetItr->eta();
    jet_phi_all = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_pT_all,jet_MV_all,jet_eta_all,jet_phi_all);

    if( !passJetFilterCut(jetItr) ) continue;
    
    // Jets passing filter cuts --> filtered jets (standard jet cleaning cuts)
    Cutflow_Jet = 1;
    jet_eta_filter = jetItr->eta();
    jet_phi_filter = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_filter,jet_phi_filter);
    
    if ( !passKinematicCut(jetItr) ) continue; 
    
    // Jets passing kinematic cuts
    Cutflow_Jet = 2;
    jet_eta_kin = jetItr->eta();
    jet_phi_kin = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_kin,jet_phi_kin);
   
    jet_MV_2_beforeJVTCut = mv;
    fill(tool,jet_MV_2_beforeJVTCut);

    if ( !passJVTCut(jetItr) ) continue; 

    jet_MV_3_afterJVTCut = mv;
    fill(tool,jet_MV_3_afterJVTCut);

    // Jets passing JVT cuts
    Cutflow_Jet = 3;
    jet_eta_jvt = jetItr->eta();
    jet_phi_jvt = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_jvt,jet_phi_jvt);
 
    //Loop over muon container  
    int n_isoMuons = 0, n_ptMuons = 0;
    for (const auto muonItr : *muons) {
      //Select muons which passed pT cut
      if (muonItr->pt() / Gaudi::Units::GeV < m_MuonPtCut) continue;
      bool inAcceptance = TMath::Abs(muonItr->eta()) < m_MuonEtaCut;
      if (!inAcceptance) continue;
      //Select medium muons?
      //if ((*muonItr)->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)

      //Look for isolated muons, then check for DR < 0.4
      float topoetcone20_value = -999.;
      float ptvarcone30_value = -999.;
      muonItr-> isolation(topoetcone20_value, xAOD::Iso::topoetcone20);
      muonItr-> isolation(ptvarcone30_value, xAOD::Iso::ptvarcone30);
      if (topoetcone20_value/muonItr->pt() > m_MuonTopoEtCone20Cut) continue;
      if (ptvarcone30_value/muonItr->pt() > m_MuonPtVarCone30Cut) continue;
      if (jetItr->p4().DeltaR(muonItr->p4())<0.4) ++n_isoMuons;

      //Look for muons with pT > 90% of jet pT, then check for DR < 0.4 
      if ( (muonItr->pt() > (0.9*jetItr->pt())) && (jetItr->p4().DeltaR(muonItr->p4())<0.4) ) ++n_ptMuons;
    }
  
    ATH_MSG_DEBUG("Number of isolated muons within this jet: \"" << n_isoMuons);
    ATH_MSG_DEBUG("Number of muons with 90% pT within this jet: \"" << n_ptMuons);

    if(n_isoMuons>0) continue;
    if(n_ptMuons>0) continue;
  
    // Jets passing basic isolated-muon overlap
    Cutflow_Jet = 4;
    jet_eta_overlap = jetItr->eta();
    jet_phi_overlap = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_overlap,jet_phi_overlap);

    // Fill histograms with properties of jet associated tracks
    // Check if jet is taggable (defined as goodJet or suspectJet or badJet)
    Jet_t qualityLabel = getQualityLabel(jetItr, PVZ); 

    if ( qualityLabel == goodJet ) {
      Cutflow_Jet = 5;

      jet_pT_good = jetItr->pt() / Gaudi::Units::GeV;
      jet_eta_good = jetItr->eta();
      jet_phi_good = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_good,jet_phi_good,jet_pT_good);
      
      //fill good jet histograms, also with main High Level Tagger information
      fillGoodJetHistos(jetItr);

      //fill good jet histograms, with Extra Taggers information, only if needed
      if(m_DoExtraTaggerHistos) fillExtraTaggerHistos(jetItr);

      //Fill MV plots vs <mu>
      auto jet_MV_mu_0_30 = Monitored::Scalar<float>("jet_MV_mu_0_30",0);
      auto jet_MV_mu_30_50 = Monitored::Scalar<float>("jet_MV_mu_30_50",0);
      auto jet_MV_mu_50_100 = Monitored::Scalar<float>("jet_MV_mu_50_100",0);
      
      if ( Run_mu > 0. && Run_mu < 30. ) {
	jet_MV_mu_0_30 = mv;
	fill(tool,jet_MV_mu_0_30);}
      else if ( Run_mu > 30. && Run_mu < 50. ) {
	jet_MV_mu_30_50 = mv;
	fill(tool,jet_MV_mu_30_50);}
      else if ( Run_mu > 50. && Run_mu < 100. ) {
	jet_MV_mu_50_100 = mv;
	fill(tool,jet_MV_mu_50_100);}

      //Fill ttbar plots
      if (isTTbarEvent){// Looking for ttbar events
	++nTTbarGoodJets; //good jet counter, to check if jets are more than 2
	if (nTTbarGoodJets == 1) firstTTbarJet = jetItr; //1st jet needed to fill ttbar histograms (if a 2nd jet is found later)
	if(nTTbarGoodJets >= 2) { //at least 2 good jets --> fill ttbar histograms
	  fillTTbarEventJetHistos(jetItr); // fill histograms with b-tagging information
	  if (nTTbarGoodJets == 2) { //(at least) two good jets --> fill also for 1st jet
	    fillTTbarEventJetHistos(firstTTbarJet); // fill histograms with b-tagging information
	  }
	}
      }

      //Fill SMT jet plots (loop over muons looking for soft muons)
      if (jetItr-> pt() / Gaudi::Units::GeV < m_JetPtCut) continue; // stronger Jet pT cut (25 GeV, eta < 2.5)
      bool inAcceptance = TMath::Abs(jetItr-> eta()) < m_JetEtaCut; // usual eta cut
      if (!inAcceptance) continue;

      SoftMuons_n = 0; //restarting counter;

      for (const auto softMuonItr : *muons) {
	//Look for soft muons (for SMT jets selection)
	//select soft muons which pass eta and pT cut
	bool inAcceptance = TMath::Abs(softMuonItr->eta()) < m_MuonEtaCut;
	if (!inAcceptance) continue;
	//fixed pT cut > 5 GeV and < 25 GeV (accepted in pre-selection)
	if (softMuonItr->pt() / Gaudi::Units::GeV < m_SoftMuonPtMin) continue;
	if (softMuonItr->pt() / Gaudi::Units::GeV > m_SoftMuonPtMax) continue;
	// NO QUALITY (previously tight muons)
	//if (softMuonItr->quality() > 0) continue; // 0 tight, 1 medium, 1 & 0 tight & medium
	const ElementLink< xAOD::TrackParticleContainer >& pMuIDTrack=softMuonItr->inDetTrackParticleLink();
	const ElementLink< xAOD::TrackParticleContainer >& pMuMSTrack=softMuonItr->muonSpectrometerTrackParticleLink();
	if ( !pMuIDTrack.isValid() || !pMuMSTrack.isValid()) continue;
	if (softMuonItr->muonType()!= xAOD::Muon::Combined) continue; //combined
	if (softMuonItr->primaryTrackParticle()->d0() > 0.4) continue; // d0 cut < 0.4 mm
	//this need to be done at jet level
	SMTJets_dr = (jetItr->p4()).DeltaR(softMuonItr->p4());
	if ( SMTJets_dr >= 0.4) continue; //jet muon DR < 0.4
	SoftMuons_pT = softMuonItr->pt() / Gaudi::Units::GeV;
	smt_mu.SetPtEtaPhiE(softMuonItr->pt(),softMuonItr->eta(),softMuonItr->phi(),softMuonItr->e());
	++SoftMuons_n;
      }//end loop on muons
      
      ATH_MSG_DEBUG("Number of soft muons per jet is " << SoftMuons_n);
      fill(tool,SoftMuons_n);
      
      if( SoftMuons_n == 1){
	SMTJets_pT = jetItr->pt() / Gaudi::Units::GeV;
	jet_eta_smt = jetItr->eta();
	jet_phi_smt = jetItr->phi();
	smt_jet.SetPtEtaPhiE(jetItr->pt(),jetItr->eta(),jetItr->phi(),jetItr->e());
	SMTJets_MV = mv;
	++SMTJets_n;
      }

    }
    else if ( qualityLabel == suspectJet ) {
      Cutflow_Jet = 6;
      jet_eta_suspect = jetItr->eta();
      jet_phi_suspect = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_suspect,jet_phi_suspect);
      fillSuspectJetHistos(jetItr); //other histograms to fill
    }
    else if ( qualityLabel == badJet ) {
      Cutflow_Jet = 7;
      jet_eta_bad = jetItr->eta();
      jet_phi_bad = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_bad,jet_phi_bad);
    }
    
  }
  //end of jetItr loop

  if(isTTbarEvent){
    ATH_MSG_DEBUG("Number of jets in ttbar events " << nTTbarGoodJets);
    fill(tool,nTTbarGoodJets);
  }

  ATH_MSG_DEBUG("Number of jets with == 1 soft muon per event is " << SMTJets_n);
  fill(tool,SMTJets_n);

  if (SMTJets_n == 1){
    SMTJets_pTratio = SoftMuons_pT/SMTJets_pT;
    SMTJets_pTrel = smt_mu.Perp((smt_jet+smt_mu).Vect()) / Gaudi::Units::GeV;
    fill(tool,SoftMuons_pT,SMTJets_pT,SMTJets_pTratio,SMTJets_pTrel,SMTJets_dr,SMTJets_MV,jet_eta_smt,jet_phi_smt);
  }

  return StatusCode::SUCCESS;
}

bool JetTagMonitorAlgorithm::passJetFilterCut(const xAOD::Jet *jet) const {

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
	(hecf>0.5 && std::abs(hecq)>0.5) || (std::abs(negE) > 60*Gaudi::Units::GeV) ||
	(emf>0.95 && std::abs(jetQuality)>0.8 && std::abs(jet->eta()) < 2.8) || 
	(std::abs(jetTime)>25) ||
	(emf<0.05 && chf<0.05 && std::abs(jet->eta())<2) ||
	(emf<0.05 && std::abs(jet->eta())>= 2) ||
	(fracSamplingMax>0.99 && std::abs(jet->eta())<2)
	) 
       ){
    pass_cuts = true; 
  }
  
  if (m_SkipJetFilter) //In case of Ion-Ion or Ion-proton collision
    pass_cuts = true; //Skip the Jet Filter cuts (thresholds tuned on pp event jets)

  return pass_cuts;
}

bool JetTagMonitorAlgorithm::passKinematicCut(const xAOD::Jet *jet) const {
  
  ATH_MSG_DEBUG("passKinematicCut()");
  ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt() / Gaudi::Units::GeV);

  if ( jet->pt() / Gaudi::Units::GeV < m_JetPtCut || std::abs(jet->eta()) > m_JetEtaCut )
    return false;

  return true;
}


bool JetTagMonitorAlgorithm::passJVTCut(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("passJVTCut()");
  
  static SG::AuxElement::Accessor<float> JVT( "Jvt" );
  float jvt = JVT(*jet);

  if( (jet->pt()/Gaudi::Units::GeV < m_JVTpTCut) && (std::abs(jet->eta())<m_JVTetaCut) && (jvt < m_JVTCut) )
    return false;

  return true;
}

double JetTagMonitorAlgorithm::getTaggerWeight(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("retrieving DL1* weight");

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_WARNING( "Could not retrieve b-tagging object from selected jet." );
    return 0;
  }

  double mv = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if (m_TaggerName=="DL1dv00" || m_TaggerName=="DL1r"){
    bTaggingObject->pu(m_TaggerName,mv_pu);
    bTaggingObject->pc(m_TaggerName,mv_pc);
    bTaggingObject->pb(m_TaggerName,mv_pb);
    //DL1* formula (standard)
    if ( mv_pb != 0 && (mv_pu != 0 || mv_pc != 0)) {
      mv = log( mv_pb / ( mv_pu * ( 1 - m_cFraction ) + mv_pc * m_cFraction ) );
    }
  }
  return mv;
}

void JetTagMonitorAlgorithm::fillTTbarEventJetHistos(const xAOD::Jet *jet) const {
  
  double mv = getTaggerWeight(jet);

  auto tool = getGroup("JetTagMonitor");

  auto TTbarJets_MV = Monitored::Scalar<float>("TTbarJets_MV",0);

  TTbarJets_MV=mv;
  fill(tool,TTbarJets_MV);

  auto TTbarJets_n_60tag = Monitored::Scalar<int>("TTbarJets_n_60tag",0);
  auto TTbarJets_n_70tag = Monitored::Scalar<int>("TTbarJets_n_70tag",0);
  auto TTbarJets_n_77tag = Monitored::Scalar<int>("TTbarJets_n_77tag",0);
  auto TTbarJets_n_85tag = Monitored::Scalar<int>("TTbarJets_n_85tag",0);

  auto TTbarJets_pT_60tag = Monitored::Scalar<float>("TTbarJets_pT_60tag",0);
  auto TTbarJets_pT_70tag = Monitored::Scalar<float>("TTbarJets_pT_70tag",0);
  auto TTbarJets_pT_77tag = Monitored::Scalar<float>("TTbarJets_pT_77tag",0);
  auto TTbarJets_pT_85tag = Monitored::Scalar<float>("TTbarJets_pT_85tag",0);

  if (mv > m_WP85Cut) {
    fill(tool,TTbarJets_n_85tag);
    TTbarJets_pT_85tag=jet->pt() / Gaudi::Units::GeV;
    fill(tool,TTbarJets_pT_85tag);
    if (mv > m_WP77Cut) {
      fill(tool,TTbarJets_n_77tag);
      TTbarJets_pT_77tag=jet->pt() / Gaudi::Units::GeV;
      fill(tool,TTbarJets_pT_77tag);
      if (mv > m_WP70Cut) {
	fill(tool,TTbarJets_n_70tag);
	TTbarJets_pT_70tag=jet->pt() / Gaudi::Units::GeV;
	fill(tool,TTbarJets_pT_70tag);
	if (mv > m_WP60Cut) {
	  fill(tool,TTbarJets_n_60tag);
	  TTbarJets_pT_60tag=jet->pt() / Gaudi::Units::GeV;
	  fill(tool,TTbarJets_pT_60tag);
	}
      }
    }
  }

  auto TTbarJets_n = Monitored::Scalar<int>("TTbarJets_n",0);
  auto pass85n = Monitored::Scalar<bool>("pass85n",false);
  auto pass77n = Monitored::Scalar<bool>("pass77n",false);
  auto pass70n = Monitored::Scalar<bool>("pass70n",false);
  auto pass60n = Monitored::Scalar<bool>("pass60n",false);
  pass85n = mv > m_WP85Cut;
  pass77n = mv > m_WP77Cut;
  pass70n = mv > m_WP70Cut;
  pass60n = mv > m_WP60Cut;
  
  auto TTbarJets_pT = Monitored::Scalar<float>("TTbarJets_pT",0);
  TTbarJets_pT=jet->pt() / Gaudi::Units::GeV;
  auto pass85p = Monitored::Scalar<bool>("pass85p",false);
  auto pass77p = Monitored::Scalar<bool>("pass77p",false);
  auto pass70p = Monitored::Scalar<bool>("pass70p",false);
  auto pass60p = Monitored::Scalar<bool>("pass60p",false);
  pass85p = mv > m_WP85Cut;
  pass77p = mv > m_WP77Cut;
  pass70p = mv > m_WP70Cut;
  pass60p = mv > m_WP60Cut;
  fill(tool,TTbarJets_n,pass85n,pass77n,pass70n,pass60n,TTbarJets_pT,pass85p,pass77p,pass70p,pass60p);

  return;
}


void JetTagMonitorAlgorithm::fillGoodJetHistos(const xAOD::Jet *jet) const {

  double mv = getTaggerWeight(jet);

  auto tool = getGroup("JetTagMonitor");

  auto jet_MV_good = Monitored::Scalar<float>("jet_MV_good",0);

  jet_MV_good = mv;
  fill(tool,jet_MV_good);

  auto jet_MV_pt_0_20 = Monitored::Scalar<float>("jet_MV_pt_0_20",0);
  auto jet_MV_pt_20_40 = Monitored::Scalar<float>("jet_MV_pt_20_40",0);
  auto jet_MV_pt_40_70 = Monitored::Scalar<float>("jet_MV_pt_40_70",0);
  auto jet_MV_pt_70_100 = Monitored::Scalar<float>("jet_MV_pt_70_100",0);
  auto jet_MV_pt_100_150 = Monitored::Scalar<float>("jet_MV_pt_100_150",0);
  auto jet_MV_pt_150_200 = Monitored::Scalar<float>("jet_MV_pt_150_200",0);
  auto jet_MV_pt_200_1000 = Monitored::Scalar<float>("jet_MV_pt_200_1000",0);

  if      ( jet->pt() / Gaudi::Units::GeV < 1000. ) {
    if      ( jet->pt() / Gaudi::Units::GeV > 200. ) {
      jet_MV_pt_200_1000=mv;
      fill(tool,jet_MV_pt_200_1000);}
    else if ( jet->pt() / Gaudi::Units::GeV > 150. ) {
      jet_MV_pt_150_200=mv;
      fill(tool,jet_MV_pt_150_200);}
    else if ( jet->pt() / Gaudi::Units::GeV > 100. ) {
      jet_MV_pt_100_150=mv;
      fill(tool,jet_MV_pt_100_150);}
    else if ( jet->pt() / Gaudi::Units::GeV > 70. ) {
      jet_MV_pt_70_100=mv;
      fill(tool,jet_MV_pt_70_100);}
    else if ( jet->pt() / Gaudi::Units::GeV > 40. ) {
      jet_MV_pt_40_70=mv;
      fill(tool,jet_MV_pt_40_70);}
    else if ( jet->pt() / Gaudi::Units::GeV > 20. ) {
      jet_MV_pt_20_40=mv;
      fill(tool,jet_MV_pt_20_40);}
    else if ( jet->pt() / Gaudi::Units::GeV > 0. ) {
      jet_MV_pt_0_20=mv;
      fill(tool,jet_MV_pt_0_20);}

  }

  auto jet_MV_eta_00_05 = Monitored::Scalar<float>("jet_MV_eta_00_05",0);
  auto jet_MV_eta_05_10 = Monitored::Scalar<float>("jet_MV_eta_05_10",0);
  auto jet_MV_eta_10_15 = Monitored::Scalar<float>("jet_MV_eta_10_15",0);
  auto jet_MV_eta_15_20 = Monitored::Scalar<float>("jet_MV_eta_15_20",0);
  auto jet_MV_eta_20_25 = Monitored::Scalar<float>("jet_MV_eta_20_25",0);

  auto jet_MV_phi_00_05 = Monitored::Scalar<float>("jet_MV_phi_00_05",0);
  auto jet_MV_phi_05_10 = Monitored::Scalar<float>("jet_MV_phi_05_10",0);
  auto jet_MV_phi_10_15 = Monitored::Scalar<float>("jet_MV_phi_10_15",0);
  auto jet_MV_phi_15_20 = Monitored::Scalar<float>("jet_MV_phi_15_20",0);
  auto jet_MV_phi_20_25 = Monitored::Scalar<float>("jet_MV_phi_20_25",0);
  auto jet_MV_phi_25_31 = Monitored::Scalar<float>("jet_MV_phi_25_31",0);

  if      ( std::abs(jet->eta()) > 2.0 ) {
    jet_MV_eta_20_25=mv;
    fill(tool,jet_MV_eta_20_25);}
  else if ( std::abs(jet->eta()) > 1.5 ) {
    jet_MV_eta_15_20=mv;
    fill(tool,jet_MV_eta_15_20);}
  else if ( std::abs(jet->eta()) > 1.0 ) {
    jet_MV_eta_10_15=mv;
    fill(tool,jet_MV_eta_10_15);}
  else if ( std::abs(jet->eta()) > 0.5 ) {
    jet_MV_eta_05_10=mv;
    fill(tool,jet_MV_eta_05_10);}
  else if ( std::abs(jet->eta()) > 0.0 ) {
    jet_MV_eta_00_05=mv;
    fill(tool,jet_MV_eta_00_05);}

  if      ( std::abs(jet->phi()) > 2.5 ) {
    jet_MV_phi_25_31=mv;
    fill(tool,jet_MV_phi_25_31);}
  else if ( std::abs(jet->phi()) > 2.0 ) {
    jet_MV_phi_20_25=mv;
    fill(tool,jet_MV_phi_20_25);}
  else if ( std::abs(jet->phi()) > 1.5 ) {
    jet_MV_phi_15_20=mv;
    fill(tool,jet_MV_phi_15_20);}
  else if ( std::abs(jet->phi()) > 1.0 ) {
    jet_MV_phi_10_15=mv;
    fill(tool,jet_MV_phi_10_15);}
  else if ( std::abs(jet->phi()) > 0.5 ) {
    jet_MV_phi_05_10=mv;
    fill(tool,jet_MV_phi_05_10);}
  else if ( std::abs(jet->phi()) > 0.0 ) {
    jet_MV_phi_00_05=mv;
    fill(tool,jet_MV_phi_00_05);}

  auto jet_eta_60tag = Monitored::Scalar<float>("jet_eta_60tag",0);
  auto jet_eta_70tag = Monitored::Scalar<float>("jet_eta_70tag",0);
  auto jet_eta_77tag = Monitored::Scalar<float>("jet_eta_77tag",0);
  auto jet_eta_85tag = Monitored::Scalar<float>("jet_eta_85tag",0);

  auto jet_phi_60tag = Monitored::Scalar<float>("jet_phi_60tag",0);
  auto jet_phi_70tag = Monitored::Scalar<float>("jet_phi_70tag",0);
  auto jet_phi_77tag = Monitored::Scalar<float>("jet_phi_77tag",0);
  auto jet_phi_85tag = Monitored::Scalar<float>("jet_phi_85tag",0);
  
  if ( mv > m_WP85Cut ) {
    jet_eta_85tag = jet->eta();
    jet_phi_85tag = jet->phi();   
    fill(tool,jet_eta_85tag,jet_phi_85tag);
    if ( mv > m_WP77Cut ) {
      jet_eta_77tag = jet->eta();
      jet_phi_77tag = jet->phi();
      fill(tool,jet_eta_77tag,jet_phi_77tag);
      if ( mv > m_WP70Cut ) {
	jet_eta_70tag = jet->eta();
	jet_phi_70tag = jet->phi();
	fill(tool,jet_eta_70tag,jet_phi_70tag);
	if ( mv >  m_WP60Cut ) {
	  jet_eta_60tag = jet->eta();
	  jet_phi_60tag = jet->phi();
	  fill(tool,jet_eta_60tag,jet_phi_60tag);
	}
      }
    }
  }
  
  auto jet_eta = Monitored::Scalar<float>("jet_eta",0);
  jet_eta = jet->eta();
  auto pass85e = Monitored::Scalar<bool>("pass85e",false);
  auto pass77e = Monitored::Scalar<bool>("pass77e",false);
  auto pass70e = Monitored::Scalar<bool>("pass70e",false);
  auto pass60e = Monitored::Scalar<bool>("pass60e",false);
  pass85e = mv > m_WP85Cut;
  pass77e = mv > m_WP77Cut;
  pass70e = mv > m_WP70Cut;
  pass60e = mv > m_WP60Cut;

  auto jet_phi = Monitored::Scalar<float>("jet_phi",0);
  auto pass85f = Monitored::Scalar<bool>("pass85f",false);
  auto pass77f = Monitored::Scalar<bool>("pass77f",false);
  auto pass70f = Monitored::Scalar<bool>("pass70f",false);
  auto pass60f = Monitored::Scalar<bool>("pass60f",false);
  jet_phi = jet->phi();
  pass85f = mv > m_WP85Cut;
  pass77f = mv > m_WP77Cut;
  pass70f = mv > m_WP70Cut;
  pass60f = mv > m_WP60Cut;
  fill(tool,jet_eta,pass85e,pass77e,pass70e,pass60e,jet_phi,pass85f,pass77f,pass70f,pass60f);
  
  return;
}

void JetTagMonitorAlgorithm::fillSuspectJetHistos(const xAOD::Jet *jet) const {

  double mv = getTaggerWeight(jet);

  auto tool = getGroup("JetTagMonitor");

  auto jet_MV_suspect = Monitored::Scalar<float>("jet_MV_suspect",0);
  auto jet_pT_suspect = Monitored::Scalar<float>("jet_pT_suspect",0);

  jet_MV_suspect = mv;
  jet_pT_suspect = jet->pt() / Gaudi::Units::GeV;
  fill(tool,jet_MV_suspect,jet_pT_suspect);

  auto sus_jet_MV_pt_0_20 = Monitored::Scalar<float>("sus_jet_MV_pt_0_20",0);
  auto sus_jet_MV_pt_20_40 = Monitored::Scalar<float>("sus_jet_MV_pt_20_40",0);
  auto sus_jet_MV_pt_40_70 = Monitored::Scalar<float>("sus_jet_MV_pt_40_70",0);
  auto sus_jet_MV_pt_70_100 = Monitored::Scalar<float>("sus_jet_MV_pt_70_100",0);
  auto sus_jet_MV_pt_100_150 = Monitored::Scalar<float>("sus_jet_MV_pt_100_150",0);
  auto sus_jet_MV_pt_150_200 = Monitored::Scalar<float>("sus_jet_MV_pt_150_200",0);
  auto sus_jet_MV_pt_200_1000 = Monitored::Scalar<float>("sus_jet_MV_pt_200_1000",0);

  if      ( jet->pt() / Gaudi::Units::GeV < 1000. ) {
    if      ( jet->pt() / Gaudi::Units::GeV > 200. ) {
      sus_jet_MV_pt_200_1000=mv;
      fill(tool,sus_jet_MV_pt_200_1000);}
    else if ( jet->pt() / Gaudi::Units::GeV > 150. ) {
      sus_jet_MV_pt_150_200=mv;
      fill(tool,sus_jet_MV_pt_150_200);}
    else if ( jet->pt() / Gaudi::Units::GeV > 100. ) {
      sus_jet_MV_pt_100_150=mv;
      fill(tool,sus_jet_MV_pt_100_150);}
    else if ( jet->pt() / Gaudi::Units::GeV > 70. ) {
      sus_jet_MV_pt_70_100=mv;
      fill(tool,sus_jet_MV_pt_70_100);}
    else if ( jet->pt() / Gaudi::Units::GeV > 40. ) {
      sus_jet_MV_pt_40_70=mv;
      fill(tool,sus_jet_MV_pt_40_70);}
    else if ( jet->pt() / Gaudi::Units::GeV > 20. ) {
      sus_jet_MV_pt_20_40=mv;
      fill(tool,sus_jet_MV_pt_20_40);}
    else if ( jet->pt() / Gaudi::Units::GeV > 0. ) {
      sus_jet_MV_pt_0_20=mv;
      fill(tool,sus_jet_MV_pt_0_20);}

  }

  auto sus_jet_MV_eta_00_05 = Monitored::Scalar<float>("sus_jet_MV_eta_00_05",0);
  auto sus_jet_MV_eta_05_10 = Monitored::Scalar<float>("sus_jet_MV_eta_05_10",0);
  auto sus_jet_MV_eta_10_15 = Monitored::Scalar<float>("sus_jet_MV_eta_10_15",0);
  auto sus_jet_MV_eta_15_20 = Monitored::Scalar<float>("sus_jet_MV_eta_15_20",0);
  auto sus_jet_MV_eta_20_25 = Monitored::Scalar<float>("sus_jet_MV_eta_20_25",0);

  auto sus_jet_MV_phi_00_05 = Monitored::Scalar<float>("sus_jet_MV_phi_00_05",0);
  auto sus_jet_MV_phi_05_10 = Monitored::Scalar<float>("sus_jet_MV_phi_05_10",0);
  auto sus_jet_MV_phi_10_15 = Monitored::Scalar<float>("sus_jet_MV_phi_10_15",0);
  auto sus_jet_MV_phi_15_20 = Monitored::Scalar<float>("sus_jet_MV_phi_15_20",0);
  auto sus_jet_MV_phi_20_25 = Monitored::Scalar<float>("sus_jet_MV_phi_20_25",0);
  auto sus_jet_MV_phi_25_31 = Monitored::Scalar<float>("sus_jet_MV_phi_25_31",0);

  if ( std::abs(jet->eta()) > 2.0 ) {
    sus_jet_MV_eta_20_25=mv;
    fill(tool,sus_jet_MV_eta_20_25);}
  else if ( std::abs(jet->eta()) > 1.5 ) {
    sus_jet_MV_eta_15_20=mv;
    fill(tool,sus_jet_MV_eta_15_20);}
  else if ( std::abs(jet->eta()) > 1.0 ) {
    sus_jet_MV_eta_10_15=mv;
    fill(tool,sus_jet_MV_eta_10_15);}
  else if ( std::abs(jet->eta()) > 0.5 ) {
    sus_jet_MV_eta_05_10=mv;
    fill(tool,sus_jet_MV_eta_05_10);}
  else if ( std::abs(jet->eta()) > 0.0 ) {
    sus_jet_MV_eta_00_05=mv;
    fill(tool,sus_jet_MV_eta_00_05);}

  if ( std::abs(jet->phi()) > 2.5 ) {
    sus_jet_MV_phi_25_31=mv;
    fill(tool,sus_jet_MV_phi_25_31);}
  else if ( std::abs(jet->phi()) > 2.0 ) {
    sus_jet_MV_phi_20_25=mv;
    fill(tool,sus_jet_MV_phi_20_25);}
  else if ( std::abs(jet->phi()) > 1.5 ) {
    sus_jet_MV_phi_15_20=mv;
    fill(tool,sus_jet_MV_phi_15_20);}
  else if ( std::abs(jet->phi()) > 1.0 ) {
    sus_jet_MV_phi_10_15=mv;
    fill(tool,sus_jet_MV_phi_10_15);}
  else if ( std::abs(jet->phi()) > 0.5 ) {
    sus_jet_MV_phi_05_10=mv;
    fill(tool,sus_jet_MV_phi_05_10);}
  else if ( std::abs(jet->phi()) > 0.0 ) {
    sus_jet_MV_phi_00_05=mv;
    fill(tool,sus_jet_MV_phi_00_05);}

  auto sus_jet_eta_60tag = Monitored::Scalar<float>("sus_jet_eta_60tag",0);
  auto sus_jet_eta_70tag = Monitored::Scalar<float>("sus_jet_eta_70tag",0);
  auto sus_jet_eta_77tag = Monitored::Scalar<float>("sus_jet_eta_77tag",0);
  auto sus_jet_eta_85tag = Monitored::Scalar<float>("sus_jet_eta_85tag",0);

  auto sus_jet_phi_60tag = Monitored::Scalar<float>("sus_jet_phi_60tag",0);
  auto sus_jet_phi_70tag = Monitored::Scalar<float>("sus_jet_phi_70tag",0);
  auto sus_jet_phi_77tag = Monitored::Scalar<float>("sus_jet_phi_77tag",0);
  auto sus_jet_phi_85tag = Monitored::Scalar<float>("sus_jet_phi_85tag",0);

  if ( mv > m_WP85Cut ) {
    sus_jet_eta_85tag = jet->eta();
    sus_jet_phi_85tag = jet->phi();   
    fill(tool,sus_jet_eta_85tag,sus_jet_phi_85tag);
    if ( mv > m_WP77Cut ) {
      sus_jet_eta_77tag = jet->eta();
      sus_jet_phi_77tag = jet->phi();
      fill(tool,sus_jet_eta_77tag,sus_jet_phi_77tag);
      if ( mv > m_WP70Cut ) {
	sus_jet_eta_70tag = jet->eta();
	sus_jet_phi_70tag = jet->phi();
	fill(tool,sus_jet_eta_70tag,sus_jet_phi_70tag);
	if ( mv >  m_WP60Cut ) {
	  sus_jet_eta_60tag = jet->eta();
	  sus_jet_phi_60tag = jet->phi();
	  fill(tool,sus_jet_eta_60tag,sus_jet_phi_60tag);
	}
      }
    }
  }
  
  auto sus_jet_eta = Monitored::Scalar<float>("sus_jet_eta",0);
  sus_jet_eta = jet->eta();
  auto pass85e = Monitored::Scalar<bool>("pass85e",false);
  auto pass77e = Monitored::Scalar<bool>("pass77e",false);
  auto pass70e = Monitored::Scalar<bool>("pass70e",false);
  auto pass60e = Monitored::Scalar<bool>("pass60e",false);
  pass85e = mv > m_WP85Cut;
  pass77e = mv > m_WP77Cut;
  pass70e = mv > m_WP70Cut;
  pass60e = mv > m_WP60Cut;

  auto sus_jet_phi = Monitored::Scalar<float>("sus_jet_phi",0);
  auto pass85f = Monitored::Scalar<bool>("pass85f",false);
  auto pass77f = Monitored::Scalar<bool>("pass77f",false);
  auto pass70f = Monitored::Scalar<bool>("pass70f",false);
  auto pass60f = Monitored::Scalar<bool>("pass60f",false);
  sus_jet_phi = jet->phi();
  pass85f = mv > m_WP85Cut;
  pass77f = mv > m_WP77Cut;
  pass70f = mv > m_WP70Cut;
  pass60f = mv > m_WP60Cut;
  fill(tool,sus_jet_eta,pass85e,pass77e,pass70e,pass60e,sus_jet_phi,pass85f,pass77f,pass70f,pass60f);

  return;
}

void JetTagMonitorAlgorithm::fillExtraTaggerHistos(const xAOD::Jet *jet) const {

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_WARNING( "Could not retrieve b-tagging object from selected jet." );
    return;
  }

  auto tool = getGroup("JetTagMonitor");

  auto jet_MV_pu_good = Monitored::Scalar<float>("jet_MV_pu_good",0);
  auto jet_MV_pc_good = Monitored::Scalar<float>("jet_MV_pc_good",0);
  auto jet_MV_pb_good = Monitored::Scalar<float>("jet_MV_pb_good",0);

  double mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if (m_TaggerName=="DL1dv00" || m_TaggerName=="DL1r"){
    bTaggingObject->pu(m_TaggerName,mv_pu);
    bTaggingObject->pc(m_TaggerName,mv_pc);
    bTaggingObject->pb(m_TaggerName,mv_pb);
  }
  jet_MV_pu_good = mv_pu;
  jet_MV_pc_good = mv_pc;
  jet_MV_pb_good = mv_pb;
  
  fill(tool,jet_MV_pu_good,jet_MV_pc_good,jet_MV_pb_good);

  return;
}


JetTagMonitorAlgorithm::Jet_t JetTagMonitorAlgorithm::getQualityLabel(const xAOD::Jet *jet, float PV_Z) const {

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return badJet;
  }

  auto tool = getGroup("JetTagMonitor");

  float jetTrack_pT = 0;
  float jetTrack_eta = 0;
  float jetTrack_phi = 0;

  int nTracks = 0;
  int nLooseTracks = 0;
  int nKinTracks = 0;
  int nIPTracks = 0;
  int nIBLTracks = 0;

  auto JetTracks_n_0_all = Monitored::Scalar<int>("JetTracks_n_0_all",0);
  auto JetTracks_pT_0_all = Monitored::Scalar<float>("JetTracks_pT_0_all",0);
  auto JetTracks_eta_0_all = Monitored::Scalar<float>("JetTracks_eta_0_all",0);
  auto JetTracks_phi_0_all = Monitored::Scalar<float>("JetTracks_phi_0_all",0);
  auto JetTracks_DR_0_all = Monitored::Scalar<float>("JetTracks_DR_0_all",0);

  auto JetTracks_n_1_loose = Monitored::Scalar<int>("JetTracks_n_1_loose",0);
  auto JetTracks_pT_1_loose = Monitored::Scalar<float>("JetTracks_pT_1_loose",0);
  auto JetTracks_eta_1_loose = Monitored::Scalar<float>("JetTracks_eta_1_loose",0);
  auto JetTracks_phi_1_loose = Monitored::Scalar<float>("JetTracks_phi_1_loose",0);

  //Loose tracks --> fill hits and impact parameters distributions
  auto JetTracks_Hits_IBL = Monitored::Scalar<int>("JetTracks_Hits_IBL",0);
  auto JetTracks_Hits_IBL_expect = Monitored::Scalar<int>("JetTracks_Hits_IBL_expect",0);
  auto JetTracks_eta_loose_IBL = Monitored::Scalar<float>("JetTracks_eta_loose_IBL",0);
  auto JetTracks_phi_loose_IBL = Monitored::Scalar<float>("JetTracks_phi_loose_IBL",0);
  auto JetTracks_Hits_BL = Monitored::Scalar<int>("JetTracks_Hits_BL",0);
  auto JetTracks_Hits_BL_expect = Monitored::Scalar<int>("JetTracks_Hits_BL_expect",0);
  auto JetTracks_eta_loose_BL = Monitored::Scalar<float>("JetTracks_eta_loose_BL",0);
  auto JetTracks_phi_loose_BL = Monitored::Scalar<float>("JetTracks_phi_loose_BL",0);

  auto JetTracks_Hits_PIX = Monitored::Scalar<int>("JetTracks_Hits_PIX",0);
  auto JetTracks_Hits_SCT = Monitored::Scalar<int>("JetTracks_Hits_SCT",0);
  auto JetTracks_Hits_TRT = Monitored::Scalar<int>("JetTracks_Hits_TRT",0);

  auto JetTracks_Holes_PIX = Monitored::Scalar<int>("JetTracks_Holes_PIX",0);
  auto JetTracks_Holes_SCT = Monitored::Scalar<int>("JetTracks_Holes_SCT",0);
  auto JetTracks_Holes_TRT = Monitored::Scalar<int>("JetTracks_Holes_TRT",0);

  auto JetTracks_Hits_Si = Monitored::Scalar<int>("JetTracks_Hits_Si",0);
  auto JetTracks_Holes_Si = Monitored::Scalar<int>("JetTracks_Holes_Si",0);

  auto JetTracks_IP_d0 = Monitored::Scalar<float>("JetTracks_IP_d0",0);
  auto JetTracks_IP_d0s = Monitored::Scalar<float>("JetTracks_IP_d0s",0);
  auto JetTracks_IP_d0si = Monitored::Scalar<float>("JetTracks_IP_d0si",0);
  auto JetTracks_IP_z0 = Monitored::Scalar<float>("JetTracks_IP_z0",0);
  auto JetTracks_IP_z0s = Monitored::Scalar<float>("JetTracks_IP_z0s",0);
  auto JetTracks_IP_z0si = Monitored::Scalar<float>("JetTracks_IP_z0si",0);
  auto JetTracks_IP_z0sin = Monitored::Scalar<float>("JetTracks_IP_z0sin",0);

  //Complete selection for jet quality classification
  auto JetTracks_n_2_kin = Monitored::Scalar<int>("JetTracks_n_2_kin",0);
  auto JetTracks_eta_2_kin = Monitored::Scalar<float>("JetTracks_eta_2_kin",0);
  auto JetTracks_phi_2_kin = Monitored::Scalar<float>("JetTracks_phi_2_kin",0);

  auto JetTracks_n_3_IP = Monitored::Scalar<int>("JetTracks_n_3_IP",0);
  auto JetTracks_eta_3_IP = Monitored::Scalar<float>("JetTracks_eta_3_IP",0);
  auto JetTracks_phi_3_IP = Monitored::Scalar<float>("JetTracks_phi_3_IP",0);

  auto JetTracks_n_4_IBL = Monitored::Scalar<int>("JetTracks_n_4_IBL",0);
  auto JetTracks_eta_4_IBL = Monitored::Scalar<float>("JetTracks_eta_4_IBL",0);
  auto JetTracks_phi_4_IBL = Monitored::Scalar<float>("JetTracks_phi_4_IBL",0);

  TLorentzVector jet_TLV;
  jet_TLV.SetPtEtaPhiE(jet->pt(), jet->eta(), jet->phi(), jet->e());

  std::vector<ElementLink<xAOD::TrackParticleContainer>> assocTracks = bTaggingObject->auxdata< std::vector< ElementLink<xAOD::TrackParticleContainer > > >("BTagTrackToJetAssociator");

  nTracks = assocTracks.size();

  for ( const ElementLink< xAOD::TrackParticleContainer >& jetTracks : assocTracks ) {
    if ( not jetTracks.isValid() ) continue;
    const xAOD::TrackParticle* jetTrackItr = *jetTracks;

    jetTrack_pT = jetTrackItr->pt() / Gaudi::Units::GeV;
    jetTrack_eta = jetTrackItr->eta();
    jetTrack_phi = jetTrackItr->phi();

    JetTracks_pT_0_all = jetTrack_pT;
    JetTracks_eta_0_all = jetTrack_eta;
    JetTracks_phi_0_all = jetTrack_phi;
    
    TLorentzVector JetTrack_TLV;
    JetTrack_TLV.SetPtEtaPhiE(jetTrackItr->pt(), jetTrackItr->eta(), jetTrackItr->phi(), jetTrackItr->e());

    JetTracks_DR_0_all = jet_TLV.DeltaR(JetTrack_TLV);

    fill(tool,JetTracks_eta_0_all,JetTracks_phi_0_all,JetTracks_pT_0_all,JetTracks_DR_0_all);

    //Select Loose tracks
    if(m_TrackSelectionTool->accept(*jetTracks)){
    
      nLooseTracks++;
    
      JetTracks_pT_1_loose =  jetTrack_pT;
      JetTracks_eta_1_loose = jetTrack_eta;
      JetTracks_phi_1_loose = jetTrack_phi;
    
      //Hits, holes
      uint8_t expectHitIBL=0;
      uint8_t nHitIBL=0;
      jetTrackItr->summaryValue(expectHitIBL, xAOD::expectInnermostPixelLayerHit);
      jetTrackItr->summaryValue(nHitIBL, xAOD::numberOfInnermostPixelLayerHits);
      JetTracks_Hits_IBL = nHitIBL;
      if(expectHitIBL < 1)
	JetTracks_Hits_IBL_expect = -1;
      else{
	JetTracks_Hits_IBL_expect = nHitIBL;
	JetTracks_eta_loose_IBL = jetTrack_eta;
	JetTracks_phi_loose_IBL = jetTrack_phi;
      }

      uint8_t expectHitBL=0;
      uint8_t nHitBL=0;
      jetTrackItr->summaryValue(expectHitBL, xAOD::expectNextToInnermostPixelLayerHit);
      jetTrackItr->summaryValue(nHitBL, xAOD::numberOfNextToInnermostPixelLayerHits);
      JetTracks_Hits_BL = nHitBL;
      if(expectHitBL < 1)
	JetTracks_Hits_BL_expect = -1;
      else{
	JetTracks_Hits_BL_expect = nHitBL;
	JetTracks_eta_loose_BL = jetTrack_eta;
	JetTracks_phi_loose_BL = jetTrack_phi;
      }

      uint8_t nHitPix=0;
      uint8_t nHolePix=0;
      uint8_t nDeadPix;
      jetTrackItr->summaryValue(nHitPix, xAOD::numberOfPixelHits);
      jetTrackItr->summaryValue(nHolePix, xAOD::numberOfPixelHoles);
      jetTrackItr->summaryValue(nDeadPix, xAOD::numberOfPixelDeadSensors);
      nHitPix += std::max((int)nDeadPix, 0);
      JetTracks_Hits_PIX = nHitPix;
      JetTracks_Holes_PIX = nHolePix;

      uint8_t nHitSCT=0;
      uint8_t nHoleSCT=0;
      uint8_t nDeadSCT;
      jetTrackItr->summaryValue(nHitSCT, xAOD::numberOfSCTHits);
      jetTrackItr->summaryValue(nHoleSCT, xAOD::numberOfSCTHoles);
      jetTrackItr->summaryValue(nDeadSCT, xAOD::numberOfSCTDeadSensors);
      nHitSCT += std::max((int)nDeadSCT, 0);
      JetTracks_Hits_SCT = nHitSCT;
      JetTracks_Holes_SCT = nHoleSCT;

      uint8_t nHitTRT=0;
      jetTrackItr->summaryValue(nHitTRT, xAOD::numberOfTRTHits);
      JetTracks_Hits_TRT = nHitTRT;

      JetTracks_Hits_Si = nHitPix + nHitSCT;
      JetTracks_Holes_Si = nHolePix + nHoleSCT;

      //Impact parameters
      JetTracks_IP_d0 =   jetTrackItr->d0();
      JetTracks_IP_d0s =  sqrt( jetTrackItr->definingParametersCovMatrix()( Trk::d0, Trk::d0 ) ); //sigma
      JetTracks_IP_d0si = JetTracks_IP_d0/JetTracks_IP_d0s; //significance

      float PVZ = PV_Z;
      JetTracks_IP_z0 =   jetTrackItr->z0() + jetTrackItr->vz() - PVZ; 
      JetTracks_IP_z0s =  sqrt( jetTrackItr->definingParametersCovMatrix()( Trk::z0, Trk::z0 ) ); //sigma
      JetTracks_IP_z0si = JetTracks_IP_z0/JetTracks_IP_z0s; //significance
      JetTracks_IP_z0sin = JetTracks_IP_z0*sin(jetTrackItr->theta()); //z0_sin(theta)

      fill(tool,
	   JetTracks_eta_1_loose,JetTracks_phi_1_loose,JetTracks_pT_1_loose,
	   JetTracks_Hits_IBL,JetTracks_Hits_IBL_expect,JetTracks_eta_loose_IBL,JetTracks_phi_loose_IBL,
	   JetTracks_Hits_BL, JetTracks_Hits_BL_expect, JetTracks_eta_loose_BL, JetTracks_phi_loose_BL,
	   JetTracks_Hits_PIX,JetTracks_Holes_PIX,
	   JetTracks_Hits_SCT,JetTracks_Holes_SCT,
	   JetTracks_Hits_TRT,
	   JetTracks_Hits_Si,JetTracks_Holes_Si,
	   JetTracks_IP_d0,JetTracks_IP_d0s,JetTracks_IP_d0si,JetTracks_IP_z0,JetTracks_IP_z0s,JetTracks_IP_z0si,JetTracks_IP_z0sin
	   );

      //Tracks passing pT cut (1 GeV)
      if(JetTracks_pT_0_all > m_TrackPtCut){

	//Number of tracks passing pT cut
	nKinTracks++;

	JetTracks_eta_2_kin = jetTrack_eta;
	JetTracks_phi_2_kin = jetTrack_phi;

	fill(tool,JetTracks_eta_2_kin,JetTracks_phi_2_kin);

        // Tracks passing IP selection (d0, z0sin)
        if(std::abs(JetTracks_IP_d0) < m_Trackd0Cut && std::abs(JetTracks_IP_z0sin) < m_Trackz0sinCut){

	  //Number of tracks passing IP cuts
	  nIPTracks++;

          JetTracks_eta_3_IP = jetTrack_eta;
          JetTracks_phi_3_IP = jetTrack_phi;

          fill(tool,JetTracks_eta_3_IP,JetTracks_phi_3_IP);

          // Tracks passing IBL or BLayer selection (if expected, hit)
          if(m_TrackHitIBLCut == 0 || (JetTracks_Hits_IBL_expect != 0 && JetTracks_Hits_BL_expect != 0) ){

	    nIBLTracks++;

            JetTracks_eta_4_IBL = jetTrack_eta;
            JetTracks_phi_4_IBL = jetTrack_phi;

            fill(tool,JetTracks_eta_4_IBL,JetTracks_phi_4_IBL);

	  }//IBL sel
	}//IP sel
      }//kin sel
    }//loose sel
  }//jet tracks loop

  JetTracks_n_0_all = nTracks;
  JetTracks_n_1_loose = nLooseTracks;
  JetTracks_n_2_kin = nKinTracks;
  JetTracks_n_3_IP = nIPTracks;
  JetTracks_n_4_IBL = nIBLTracks;
  fill(tool,JetTracks_n_0_all,JetTracks_n_1_loose,JetTracks_n_2_kin,JetTracks_n_3_IP,JetTracks_n_4_IBL);
  
  // goodJet    = jet with at least one associated track passing loose selection
  // suspectJet = as goodJet but, at least one associated track does not pass IBL or BL hit additional selection
  // badJet     = not goodJet (i.e. no associated track passing loose selection)

  if(nLooseTracks < m_MinGoodTrackCut) return badJet;

  if(nIBLTracks < nIPTracks) return suspectJet;

  return goodJet;
}
