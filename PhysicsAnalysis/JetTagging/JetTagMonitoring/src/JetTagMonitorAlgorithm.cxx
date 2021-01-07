/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMonitorAlgorithm.h"

#include "xAODJet/JetContainer.h"   
#include "xAODJet/JetAttributes.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"    
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxSecVertexInfo.h"

#include "LArRecEvent/LArEventBitInfo.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "xAODBTagging/BTaggingUtilities.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TEfficiency.h"

#include <vector>
#include <string>
#include <sstream>

JetTagMonitorAlgorithm::JetTagMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name,pSvcLocator)
  , m_jetContainerKey("AntiKt4EMTopoJets")
  , m_muonContainerKey("Muons")
  , m_electronContainerKey("Electrons")
{
  declareProperty("AntiKt4EMTopoJets",m_jetContainerKey);
  declareProperty("Muons",m_muonContainerKey);
  declareProperty("Electrons",m_electronContainerKey);
 
  declareProperty("SkipJetFilter", m_skipJetFilter);

  declareProperty("ElectronTrigger_201X", m_ElectronTrigger_201X);
  declareProperty("MuonTrigger_201X", m_MuonTrigger_201X);

  declareProperty( "JetEtaCut", m_JetEtaCut);
  declareProperty( "JetPtCut", m_JetPtCut);
  declareProperty( "SoftMuonPtCut", m_softMuonPtCut);

  declareProperty( "ElectronPtCut", m_ElectronPtCut);
  declareProperty( "MuonPtCut", m_MuonPtCut);
  declareProperty( "ElectronEtaCut", m_ElectronEtaCut);
  declareProperty( "MuonEtaCut", m_MuonEtaCut);
  declareProperty( "ElectronEtaCrackLowCut", m_ElectronEtaCrackLowCut);
  declareProperty( "ElectronEtaCrackHighCut", m_ElectronEtaCrackHighCut);

  declareProperty( "ElectronTopoEtCone20Cut", m_ElectronTopoEtCone20Cut);
  declareProperty( "MuonTopoEtCone20Cut", m_MuonTopoEtCone20Cut);
  declareProperty( "ElectronPtVarCone20Cut", m_ElectronPtVarCone20Cut);
  declareProperty( "MuonPtVarCone30Cut", m_MuonPtVarCone30Cut);

  declareProperty("MV_algorithmName", m_mv_algorithmName);
  declareProperty("MV_cFraction", m_mv_cFraction);
  declareProperty("MV_bFraction", m_mv_bFraction);
  declareProperty("MV_60_cut", m_mv_60_weight_cut);
  declareProperty("MV_70_cut", m_mv_70_weight_cut);
  declareProperty("MV_77_cut", m_mv_77_weight_cut);
  declareProperty("MV_85_cut", m_mv_85_weight_cut);
}

JetTagMonitorAlgorithm::~JetTagMonitorAlgorithm() {}

StatusCode JetTagMonitorAlgorithm::initialize() {

  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_jetContainerKey.initialize());
  ATH_CHECK(m_muonContainerKey.initialize());
  ATH_CHECK(m_electronContainerKey.initialize());

  ATH_CHECK(m_vertContainerKey.initialize());
  ATH_CHECK(m_trackContainerKey.initialize());

  if (m_btagLinkKey.empty()) {
    m_btagLinkKey = m_jetContainerKey.key() + ".btaggingLink";
  }
  ATH_CHECK(m_btagLinkKey.initialize());

  if (m_btagResultKey.empty()) {
    if (m_mv_algorithmName=="DL1" || m_mv_algorithmName=="DL1r" || m_mv_algorithmName=="DL1rnn") {
      std::string rawJetContainerName = m_jetContainerKey.key();
      const size_t jetStringItr = rawJetContainerName.find("Jets");
      if (jetStringItr != std::string::npos) {
        rawJetContainerName = rawJetContainerName.replace(jetStringItr, std::string::npos, "");
      }
      m_btagResultKey = "BTagging_" + rawJetContainerName + "." + m_mv_algorithmName + "_pb";
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

  auto Run_lb = Monitored::Scalar<int>("Run_lb",0);
  auto Run_number = Monitored::Scalar<int>("Run_number",0);
  auto Run_event = Monitored::Scalar<int>("Run_event",0);
  auto Run_mu = Monitored::Scalar<float>("Run_mu",0);

  Run_lb = GetEventInfo(ctx)->lumiBlock();
  Run_number = GetEventInfo(ctx)->runNumber();
  Run_event = GetEventInfo(ctx)->eventNumber();
  Run_mu = lbInteractionsPerCrossing(ctx);

  auto tool = getGroup("JetTagMonitor");

  auto Cutflow_Event = Monitored::Scalar<int>("Cutflow_Event",0);
  Cutflow_Event = 0;
  fill(tool,Run_number,Run_lb,Run_event,Run_mu,Cutflow_Event);

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
  
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_vertContainerKey, ctx);
  if (!vertices.isValid()) {
    ATH_MSG_ERROR("Could not find vertex AOD container with name " << m_vertContainerKey);
    return StatusCode::FAILURE;
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

  for (const auto& vertItr : *vertices) {
    ATH_MSG_DEBUG("Simple vertex: x = " << PV_x << ", y = " << PV_y << ", z = " << PV_z);
   
    if (vertItr->vertexType() == xAOD::VxType::PriVtx && vertItr->numberDoF() > 0 ) { 
      foundVxTypePriVtx = true;

      PV_x = vertItr->x();
      PV_y = vertItr->y();
      PV_z = vertItr->z();
      ATH_MSG_DEBUG("Primary vertex: x = " << PV_x << ", y = " << PV_y << ", z = " << PV_z);
      
      if (PV_x == 0 && PV_y == 0 && PV_z == 0)
	ATH_MSG_WARNING("Primary Vertex is (0,0,0)");
      
      if (vertItr->nTrackParticles()>0) {
	PV_tracks_n = vertItr->nTrackParticles();
	ATH_MSG_DEBUG("PV has "<< PV_tracks_n <<" tracks");
      }

      fill(tool,PV_x,PV_y,PV_z,PV_tracks_n);

    }
  }
  
  if ( ! foundVxTypePriVtx ) {
    ATH_MSG_WARNING("xAOD::Vertex of type xAOD::VxType::PriVtx was not found in vertex container.");
    return StatusCode::SUCCESS;
  }

  if ( PV_tracks_n <  4 )
    return StatusCode::SUCCESS;

  Cutflow_Event = 3;
  fill(tool,Cutflow_Event);
  
  //////////////////////
  //* Triggers *//
  //////////////////////
  
  bool useTriggerDecisionTool = true;
  const auto* trigDecTool = (getTrigDecisionTool().empty() ?
                             nullptr : getTrigDecisionTool().operator->());

  // only apply trigger selection if bool is true (false for express stream) and trigDecTool is ok
  if (useTriggerDecisionTool && trigDecTool != 0) {
    
    ATH_MSG_DEBUG("TrigDecTool: " << trigDecTool);
    ATH_MSG_DEBUG("trigDecTool->isPassed(" << m_ElectronTrigger_201X << "): " << trigDecTool->isPassed(m_ElectronTrigger_201X));
    ATH_MSG_DEBUG("trigDecTool->isPassed(" << m_MuonTrigger_201X << "): " << trigDecTool->isPassed(m_MuonTrigger_201X));
    
    auto Cutflow_Trigger = Monitored::Scalar<int>("Cutflow_Trigger",0);
    Cutflow_Trigger = 0;
    fill(tool,Cutflow_Trigger);
   
    if (trigDecTool->isPassed(m_ElectronTrigger_201X) || trigDecTool->isPassed(m_MuonTrigger_201X)){
      Cutflow_Trigger = 3;
      fill(tool,Cutflow_Trigger);
      if (trigDecTool->isPassed(m_ElectronTrigger_201X)) {
	Cutflow_Trigger = 1;
	fill(tool,Cutflow_Trigger);
      }
      if (trigDecTool->isPassed(m_MuonTrigger_201X)) {
	Cutflow_Trigger = 2;
	fill(tool,Cutflow_Trigger);
      }
      if (trigDecTool->isPassed(m_ElectronTrigger_201X) && trigDecTool->isPassed(m_MuonTrigger_201X)){
	Cutflow_Trigger = 4;
	fill(tool,Cutflow_Trigger);
      }
    }

    // Require e/mu trigger to have unbiased sample of jets (and larger fraction of b-jets since many of these are ttbar events)
    if (!trigDecTool->isPassed(m_ElectronTrigger_201X) && !trigDecTool->isPassed(m_MuonTrigger_201X))
      return StatusCode::SUCCESS;
  }
  
  Cutflow_Event = 4;
  fill(tool,Cutflow_Event);

  //Events are selected
  //Fill track plots with ID tracks
  //Fill jet histograms (after Jet Selection)  

  ///////////////////////////////
  //* TrackParticle container *//
  ///////////////////////////////
        
  auto Tracks_n = Monitored::Scalar<int>("Tracks_n",0);

  uint8_t nPixelHits  = 0;
  uint8_t nSCTHits    = 0;
  uint8_t nBLayerHits = 0;
  uint8_t nTRTHits    = 0;

  auto Hits_IBL = Monitored::Scalar<float>("Hits_IBL",0);
  auto Hits_Pixel = Monitored::Scalar<float>("Hits_Pixel",0);
  auto Hits_SCT = Monitored::Scalar<float>("Hits_SCT",0);
  auto Hits_Si = Monitored::Scalar<float>("Hits_Si",0);
  auto Hits_TRT = Monitored::Scalar<float>("Hits_TRT",0);
  auto Hits_ID = Monitored::Scalar<float>("Hits_ID",0);

  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_trackContainerKey, ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR("Could not find track AOD container with name " << m_trackContainerKey);
    return StatusCode::FAILURE;
  }
  
  Tracks_n = tracks->size();
  fill(tool,Tracks_n);

  for (const auto& trackItr : *tracks) {
    trackItr->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    Hits_IBL = (float)nBLayerHits;
    trackItr->summaryValue(nPixelHits, xAOD::numberOfPixelHits);
    Hits_Pixel=(float)nPixelHits;
    trackItr->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    Hits_SCT=(float)nSCTHits;
    trackItr->summaryValue(nTRTHits, xAOD::numberOfTRTHits);
    Hits_TRT=(float)nTRTHits;
    Hits_Si=(float)nPixelHits+(float)nSCTHits;
    Hits_ID=(float)nPixelHits+(float)nSCTHits+(float)nTRTHits;
    fill(tool,Hits_IBL,Hits_Pixel,Hits_SCT, Hits_TRT,Hits_Si,Hits_ID);
        
    ATH_MSG_DEBUG("Track hits: BLayer = " << Hits_IBL << ", PIX = " << Hits_IBL);
    ATH_MSG_DEBUG("Track hits: SCT = " << Hits_SCT << ", Si = " << Hits_Si);
    ATH_MSG_DEBUG("Track hits: TRT = " << Hits_TRT << ", ID = " << Hits_ID);
    
  }

  //----------------------Start Muon & Electron Part for ttbar events ------------------------

  //////////////////////
  //* Muon container *//
  //////////////////////

  auto Muon_pT = Monitored::Scalar<double>("Muon_pT",0.0);
  auto IsolatedMuons_n = Monitored::Scalar<int>("IsolatedMuons_n",0.0);
  int Muon_charge = 0;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_muonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_muonContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto& muonItr : *muons) {
    ATH_MSG_DEBUG("Muon kinematics: eta = " << muonItr->eta() << ", phi= " << muonItr->phi() << ", pT= " << muonItr->pt() / Gaudi::Units::GeV);
    Muon_pT = muonItr->pt();
    fill(tool,Muon_pT);

    //Look for isolated muons (for ttbar events selection)
    //select muons which passed pT cut
    if (muonItr->pt() / Gaudi::Units::GeV < m_MuonPtCut) continue;
    bool inAcceptance = TMath::Abs(muonItr->eta()) < m_MuonEtaCut;
    if (!inAcceptance) continue;
    // medium muons
    if (muonItr->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)
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

  auto Electron_pT = Monitored::Scalar<double>("Electron_pT",0.0);
  auto IsolatedElectrons_n = Monitored::Scalar<int>("IsolatedElectrons_n",0.0);
  int Electron_charge = 0;

  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronContainerKey, ctx);
  if (! electrons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain electron Collection with name "<< m_electronContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto& electronItr : *electrons) {
    ATH_MSG_DEBUG("Electron kinematics: eta = " << electronItr->eta() << ", phi= " << electronItr->phi() << ", pT= " << electronItr->pt() / Gaudi::Units::GeV);
    Electron_pT = electronItr->pt();
    fill(tool,Electron_pT);

    //select electrons which passed author and pT cut
    if (!electronItr->author(xAOD::EgammaParameters::AuthorElectron)) continue; 
    if (electronItr->pt() / Gaudi::Units::GeV < m_ElectronPtCut) continue;
    bool inAcceptance = (TMath::Abs(electronItr->eta()) > m_ElectronEtaCrackHighCut || TMath::Abs(electronItr->eta()) < m_ElectronEtaCrackLowCut) 
      && TMath::Abs(electronItr->eta()) < m_ElectronEtaCut;
    if (!inAcceptance) continue;
    // select mediumLH electron
    bool lhmedium = false;
    electronItr->passSelection(lhmedium, "LHMedium");
    if (!lhmedium) continue;
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

  // require == 1 electron, == 1 muon and opposite charged leptons
  bool isTTbarEvent = false;
  if (IsolatedElectrons_n == 1 && IsolatedMuons_n == 1 && Electron_charge*Muon_charge == -1) isTTbarEvent = true;

  if(isTTbarEvent)
    ATH_MSG_DEBUG("This is ttbar event "<< Run_event);

  /////////////////////
  //* Jet container *//
  /////////////////////

  ATH_MSG_DEBUG("Start jet part");

  //----------------------Start Jet Part---------------------------

  SG::ReadHandle<xAOD::JetContainer> jets(m_jetContainerKey, ctx);
  if (!jets.isValid()) {
    ATH_MSG_ERROR("Could not find jet AOD container with name " << m_jetContainerKey);
    return StatusCode::FAILURE;
  }

 
  auto Cutflow_Jet = Monitored::Scalar<int>("Cutflow_Jet",0);
 
  auto jet_n_all = Monitored::Scalar<int>("jet_n_all",0.0);

  auto jet_pT_all = Monitored::Scalar<float>("jet_pT_all",0.0);
  auto jet_pT_good = Monitored::Scalar<float>("jet_pT_good",0.0);
  auto passGood = Monitored::Scalar<bool>("passGood",false);

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

  auto jet_MV_all = Monitored::Scalar<double>("jet_MV_all",0.0);
  auto jet_MV_JVT_bef = Monitored::Scalar<double>("jet_MV_JVT_bef",0.0);
  auto jet_MV_JVT_aft = Monitored::Scalar<double>("jet_MV_JVT_aft",0.0);

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

  for (const auto& jetItr : *jets) {
    
    jet_pT_all = jetItr->pt() / Gaudi::Units::GeV;
    jet_eta_all = jetItr->eta();
    jet_phi_all = jetItr->phi();
    ATH_MSG_DEBUG("Jet kinematics: eta = " << jet_eta_all << ", phi= " << jet_phi_all << ", pT= " << jet_pT_all);
    
    double mv = 0; 
    mv = getMVweight(jetItr);
    jet_MV_all = mv;

    // All jets
    Cutflow_Jet = 0;
    jet_eta_all = jetItr->eta();
    jet_phi_all = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_pT_all,jet_MV_all,jet_eta_all,jet_phi_all);

    if( !passJetFilterCuts(jetItr) ) continue;
    
    // Jets passing filter cuts --> filtered jets (standard jet cleaning cuts)
    Cutflow_Jet = 1;
    jet_eta_filter = jetItr->eta();
    jet_phi_filter = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_filter,jet_phi_filter);
    
    if ( !passKinematicCuts(jetItr) ) continue; 
    
    // Jets passing kinematic cuts
    Cutflow_Jet = 2;
    jet_eta_kin = jetItr->eta();
    jet_phi_kin = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_kin,jet_phi_kin);
   
    // fillTrackInJetHistograms(jetItr); // fill histograms with properties of associated tracks

    //jet_MV_JVT_bef = mv;
    //fill(tool,jet_MV_JVT_bef);

    if ( !passJVTCuts(jetItr) ) continue; 

    //jet_MV_JVT_aft = mv;
    //fill(tool,jet_MV_JVT_aft);

    // Jets passing JVT cuts
    Cutflow_Jet = 3;
    jet_eta_jvt = jetItr->eta();
    jet_phi_jvt = jetItr->phi();
    fill(tool,Cutflow_Jet,jet_eta_jvt,jet_phi_jvt);
 
    //if ( !passMuonOverlap(jetItr,ctx) ) continue; //developed inside

    // loop over muon container  
    int n_isoMuons = 0, n_ptMuons = 0;
    for (const auto& muonItr : *muons) {
      //select muons which passed pT cut
      if (muonItr->pt() / Gaudi::Units::GeV < m_MuonPtCut) continue;
      bool inAcceptance = TMath::Abs(muonItr->eta()) < m_MuonEtaCut;
      if (!inAcceptance) continue;
      // medium muons
      //if ((*muonItr)->quality() > 1) continue; // 0 tight, 1 medium, medium <= 1 (includes 0)

      if ( (muonItr->pt() > (0.9*jetItr->pt())) && (jetItr->p4().DeltaR(muonItr->p4())<0.4) ) ++n_ptMuons;

      float topoetcone20_value = -999.;
      float ptvarcone30_value = -999.;
      muonItr-> isolation(topoetcone20_value, xAOD::Iso::topoetcone20);
      muonItr-> isolation(ptvarcone30_value, xAOD::Iso::ptvarcone30);
      if (topoetcone20_value/muonItr->pt() > m_MuonTopoEtCone20Cut) continue;
      if (ptvarcone30_value/muonItr->pt() > m_MuonPtVarCone30Cut) continue;
      if (jetItr->p4().DeltaR(muonItr->p4())<0.4) ++n_isoMuons;
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

    // check if jet is taggable (defined as goodJet or suspectJet or badJet)
    Jet_t taggabilityLabel = getTaggabilityLabel(jetItr); 

    // Fill variable for 2D fraction plot (good jets / all jets before selection)
    passGood = taggabilityLabel == goodJet;
    fill(tool,passGood,jet_eta_all,jet_phi_all); 

    if ( taggabilityLabel == goodJet ) {
      Cutflow_Jet = 5;

      jet_pT_good = jetItr->pt() / Gaudi::Units::GeV;
      jet_eta_good = jetItr->eta();
      jet_phi_good = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_good,jet_phi_good,jet_pT_good);
      
      //fill good jet histograms, also with main High Level Tagger information
      fillGoodJetHistos(jetItr);

      //fill good jet histograms, with Low Level Taggers information
      fillLowLevelJetHistos(jetItr);

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
	  fillTTbarHistograms(jetItr); // fill histograms with b-tagging information
	  if (nTTbarGoodJets == 2) { //(at least) two good jets --> fill also for 1st jet
	    fillTTbarHistograms(firstTTbarJet); // fill histograms with b-tagging information
	  }
	}
	
      }

      //Fill SMT jet plots (loop over muons looking for soft muons)

      if (jetItr-> pt() / Gaudi::Units::GeV < m_JetPtCut) continue; // stronger Jet pT cut (25 GeV, eta < 2.5)
      bool inAcceptance = TMath::Abs(jetItr-> eta()) < m_JetEtaCut; // usual eta cut
      if (!inAcceptance) continue;

      SoftMuons_n = 0; //restarting counter;

      for (const auto& softMuonItr : *muons) {
	//Look for soft muons (for SMT jets selection)
	//select soft muons which pass eta and pT cut
	bool inAcceptance = TMath::Abs(softMuonItr->eta()) < m_MuonEtaCut;
	if (!inAcceptance) continue;
	if (softMuonItr->pt() / Gaudi::Units::GeV < m_softMuonPtCut) continue; //fixed pT cut 5 GeV
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
    else if ( taggabilityLabel == suspectJet ) {
      Cutflow_Jet = 6;
      jet_eta_suspect = jetItr->eta();
      jet_phi_suspect = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_suspect,jet_phi_suspect);
      // fillSuspectJetHistos(*jetItr); //other histograms to fill
    }
    else if ( taggabilityLabel == badJet ) {
      Cutflow_Jet = 7;
      jet_eta_bad = jetItr->eta();
      jet_phi_bad = jetItr->phi();
      fill(tool,Cutflow_Jet,jet_eta_bad,jet_phi_bad);
      //filRun_lbadJetHistos(*jetItr); //other histograms to fill
    }
    
  }
  //end of jetItr loop

  //ATH_MSG_DEBUG("... done fillJetHistograms()");

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

bool JetTagMonitorAlgorithm::passJetFilterCuts(const xAOD::Jet *jet) const {

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
	(hecf>0.5 && fabs(hecq)>0.5) || (fabs(negE) > 60*Gaudi::Units::GeV) ||
	(emf>0.95 && fabs(jetQuality)>0.8 && fabs(jet->eta()) < 2.8) || 
	(fabs(jetTime)>25) ||
	(emf<0.05 && chf<0.05 && fabs(jet->eta())<2) ||
	(emf<0.05 && fabs(jet->eta())>= 2) ||
	(fracSamplingMax>0.99 && fabs(jet->eta())<2)
	) 
       ){
    pass_cuts = true; 
  }
  
  if (m_skipJetFilter) //In case of Ion-Ion or Ion-proton collision
    pass_cuts = true; //Skip the Jet Filter cuts (thresholds tuned on pp event jets)

  return pass_cuts;
}

bool JetTagMonitorAlgorithm::passKinematicCuts(const xAOD::Jet *jet) const {
  
  ATH_MSG_DEBUG("passKinematicCuts()");
  ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt() / Gaudi::Units::GeV);

  if ( jet->pt() / Gaudi::Units::GeV < m_JetPtCut || fabs(jet->eta()) > m_JetEtaCut )
    return false;

  return true;
}


bool JetTagMonitorAlgorithm::passJVTCuts(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("passJVTCuts()");
  
  static SG::AuxElement::Accessor<float> JVT( "Jvt" ); //JVT > 0.59 (60 GeV)
  double jvt = JVT(*jet);

  if( !( ((jet->pt()<60000)&&(abs(jet->eta())<2.4)&&(jvt > 0.59))||((jet->pt()<60000)&&(abs(jet->eta())>2.4))||(jet->pt()>60000) ) ) return false;

  return true;
}

JetTagMonitorAlgorithm::Jet_t JetTagMonitorAlgorithm::getTaggabilityLabel(const xAOD::Jet *jet) const {
  
  // goodJet    = jet with at least one associated track passing track selection
  // suspectJet = as goodJet but, at least one associated track does not pass deadBLayer, nHitBLayer or nSiHit requirement
  // badJet     = not goodJet (i.e. no associated track passing track selection)

  // ATH_MSG_DEBUG("in applyTaggabilityCuts()");

  if(jet->pt()<25000) //fill properly
    return badJet;

  if(TMath::Abs(jet->eta())>2.5) //fill properly
    return badJet;

  return goodJet;
}


double JetTagMonitorAlgorithm::getMVweight(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("retrieving MV2c10/DL1* weight");

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return 0;
  }

  double mv = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if(m_mv_algorithmName=="MV2c10" || m_mv_algorithmName=="MV2c10r" || m_mv_algorithmName=="MV2c10rnn"){
    bTaggingObject->MVx_discriminant(m_mv_algorithmName,mv);
  }
  else if (m_mv_algorithmName=="DL1" || m_mv_algorithmName=="DL1r" || m_mv_algorithmName=="DL1rnn"){
    bTaggingObject->pu(m_mv_algorithmName,mv_pu);
    bTaggingObject->pc(m_mv_algorithmName,mv_pc);
    bTaggingObject->pb(m_mv_algorithmName,mv_pb);
    //DL1* formula (standard)
    if ( mv_pb != 0 && (mv_pu != 0 || mv_pc != 0)) {
      mv = log( mv_pb / ( mv_pu * ( 1 - m_mv_cFraction ) + mv_pc * m_mv_cFraction ) );
    }
    //DL1*c formula (for DL1c)
    //mv = log( mv_pb / ( mv_pu * ( 1 - m_mv_bFraction ) + mv_pc * m_mv_bFraction ) );
  }
  return mv;
}

void JetTagMonitorAlgorithm::fillTTbarHistograms(const xAOD::Jet *jet) const {
  
  double mv = getMVweight(jet);

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

  if (mv > m_mv_85_weight_cut) {
    fill(tool,TTbarJets_n_85tag);
    TTbarJets_pT_85tag=jet->pt() / Gaudi::Units::GeV;
    fill(tool,TTbarJets_pT_85tag);
    if (mv > m_mv_77_weight_cut) {
      fill(tool,TTbarJets_n_77tag);
      TTbarJets_pT_77tag=jet->pt() / Gaudi::Units::GeV;
      fill(tool,TTbarJets_pT_77tag);
      if (mv > m_mv_70_weight_cut) {
	fill(tool,TTbarJets_n_70tag);
	TTbarJets_pT_70tag=jet->pt() / Gaudi::Units::GeV;
	fill(tool,TTbarJets_pT_70tag);
	if (mv > m_mv_60_weight_cut) {
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
  pass85n = mv > m_mv_85_weight_cut;
  pass77n = mv > m_mv_77_weight_cut;
  pass70n = mv > m_mv_70_weight_cut;
  pass60n = mv > m_mv_60_weight_cut;
  
  auto TTbarJets_pT = Monitored::Scalar<float>("TTbarJets_pT",0);
  TTbarJets_pT=jet->pt() / Gaudi::Units::GeV;
  auto pass85p = Monitored::Scalar<bool>("pass85p",false);
  auto pass77p = Monitored::Scalar<bool>("pass77p",false);
  auto pass70p = Monitored::Scalar<bool>("pass70p",false);
  auto pass60p = Monitored::Scalar<bool>("pass60p",false);
  pass85p = mv > m_mv_85_weight_cut;
  pass77p = mv > m_mv_77_weight_cut;
  pass70p = mv > m_mv_70_weight_cut;
  pass60p = mv > m_mv_60_weight_cut;
  fill(tool,TTbarJets_n,pass85n,pass77n,pass70n,pass60n,TTbarJets_pT,pass85p,pass77p,pass70p,pass60p);

  return;
}


void JetTagMonitorAlgorithm::fillGoodJetHistos(const xAOD::Jet *jet) const {

  double mv = getMVweight(jet);

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

  if      ( fabs(jet->eta()) > 2.0 ) {
    jet_MV_eta_20_25=mv;
    fill(tool,jet_MV_eta_20_25);}
  else if ( fabs(jet->eta()) > 1.5 ) {
    jet_MV_eta_15_20=mv;
    fill(tool,jet_MV_eta_15_20);}
  else if ( fabs(jet->eta()) > 1.0 ) {
    jet_MV_eta_10_15=mv;
    fill(tool,jet_MV_eta_10_15);}
  else if ( fabs(jet->eta()) > 0.5 ) {
    jet_MV_eta_05_10=mv;
    fill(tool,jet_MV_eta_05_10);}
  else if ( fabs(jet->eta()) > 0.0 ) {
    jet_MV_eta_00_05=mv;
    fill(tool,jet_MV_eta_00_05);}

  if      ( fabs(jet->phi()) > 2.5 ) {
    jet_MV_phi_25_31=mv;
    fill(tool,jet_MV_phi_25_31);}
  else if ( fabs(jet->phi()) > 2.0 ) {
    jet_MV_phi_20_25=mv;
    fill(tool,jet_MV_phi_20_25);}
  else if ( fabs(jet->phi()) > 1.5 ) {
    jet_MV_phi_15_20=mv;
    fill(tool,jet_MV_phi_15_20);}
  else if ( fabs(jet->phi()) > 1.0 ) {
    jet_MV_phi_10_15=mv;
    fill(tool,jet_MV_phi_10_15);}
  else if ( fabs(jet->phi()) > 0.5 ) {
    jet_MV_phi_05_10=mv;
    fill(tool,jet_MV_phi_05_10);}
  else if ( fabs(jet->phi()) > 0.0 ) {
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
  
  if ( mv > m_mv_85_weight_cut ) {
    jet_eta_85tag = jet->eta();
    jet_phi_85tag = jet->phi();   
    fill(tool,jet_eta_85tag,jet_phi_85tag);
    if ( mv > m_mv_77_weight_cut ) {
      jet_eta_77tag = jet->eta();
      jet_phi_77tag = jet->phi();
      fill(tool,jet_eta_77tag,jet_phi_77tag);
      if ( mv > m_mv_70_weight_cut ) {
	jet_eta_70tag = jet->eta();
	jet_phi_70tag = jet->phi();
	fill(tool,jet_eta_70tag,jet_phi_70tag);
	if ( mv >  m_mv_60_weight_cut ) {
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
  pass85e = mv > m_mv_85_weight_cut;
  pass77e = mv > m_mv_77_weight_cut;
  pass70e = mv > m_mv_70_weight_cut;
  pass60e = mv > m_mv_60_weight_cut;

  auto jet_phi = Monitored::Scalar<float>("jet_phi",0);
  auto pass85f = Monitored::Scalar<bool>("pass85f",false);
  auto pass77f = Monitored::Scalar<bool>("pass77f",false);
  auto pass70f = Monitored::Scalar<bool>("pass70f",false);
  auto pass60f = Monitored::Scalar<bool>("pass60f",false);
  jet_phi = jet->phi();
  pass85f = mv > m_mv_85_weight_cut;
  pass77f = mv > m_mv_77_weight_cut;
  pass70f = mv > m_mv_70_weight_cut;
  pass60f = mv > m_mv_60_weight_cut;
  fill(tool,jet_eta,pass85e,pass77e,pass70e,pass60e,jet_phi,pass85f,pass77f,pass70f,pass60f);
  
  return;
}

void JetTagMonitorAlgorithm::fillLowLevelJetHistos(const xAOD::Jet *jet) const {

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return;
  }

  auto tool = getGroup("JetTagMonitor");

  auto jet_MV_pu_good = Monitored::Scalar<float>("jet_MV_pu_good",0);
  auto jet_MV_pc_good = Monitored::Scalar<float>("jet_MV_pc_good",0);
  auto jet_MV_pb_good = Monitored::Scalar<float>("jet_MV_pb_good",0);

  auto jet_IP2D_good = Monitored::Scalar<float>("jet_IP2D_good",0);
  auto jet_IP3D_good = Monitored::Scalar<float>("jet_IP3D_good",0);
  auto jet_SV1_good = Monitored::Scalar<float>("jet_SV1_good",0);
  auto jet_JetFitter_good = Monitored::Scalar<float>("jet_JetFitter_good",0);
  auto jet_RNNIP_good = Monitored::Scalar<float>("jet_RNNIP_good",0);

  double rnnip_llr = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if (m_mv_algorithmName=="DL1" || m_mv_algorithmName=="DL1r" || m_mv_algorithmName=="DL1rnn"){
    bTaggingObject->pu(m_mv_algorithmName,mv_pu);
    bTaggingObject->pc(m_mv_algorithmName,mv_pc);
    bTaggingObject->pb(m_mv_algorithmName,mv_pb);
  }
  jet_MV_pu_good = mv_pu;
  jet_MV_pc_good = mv_pc;
  jet_MV_pb_good = mv_pb;

  jet_IP2D_good = bTaggingObject->IP2D_loglikelihoodratio();
  jet_IP3D_good = bTaggingObject->IP3D_loglikelihoodratio();
  jet_SV1_good = bTaggingObject->SV1_loglikelihoodratio();
  jet_JetFitter_good = bTaggingObject->JetFitter_loglikelihoodratio();
  bTaggingObject->loglikelihoodratio("rnnip",rnnip_llr);
  jet_RNNIP_good = rnnip_llr;

  fill(tool,jet_MV_pu_good,jet_MV_pc_good,jet_MV_pb_good,jet_IP2D_good,jet_IP3D_good,jet_SV1_good,jet_JetFitter_good,jet_RNNIP_good);

  return;
}
