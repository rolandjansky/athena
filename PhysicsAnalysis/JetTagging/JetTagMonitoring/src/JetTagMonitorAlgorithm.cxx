/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//r22 header
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
   //add here other containers
{

  declareProperty("SkipJetQuality",         m_skipJetQuality    = false);

  declareProperty("AntiKt4EMTopoJets",m_jetContainerKey);
  declareProperty("Muons",m_muonContainerKey);
  declareProperty("Electrons",m_electronContainerKey);
 
  declareProperty("ElectronTrigger_201X", m_ElectronTrigger_201X = "HLT_e[2-9][0-9]_.*" ); // e20-e99_* triggers
  declareProperty("MuonTrigger_201X", m_MuonTrigger_201X = "HLT_mu[2-9][0-9]_.*" ); // mu20-mu99_* triggers 
  declareProperty("JetTrigger_201X", m_JetTrigger_201X = "HLT_j[1-9][0-9].*" ); // j10-j99* triggers 

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

  declareProperty("MV_algorithmName", m_mv_algorithmName="MV2c10");
  declareProperty("MV_nBins", m_mv_nBins=100);
  declareProperty("MV_rangeStart", m_mv_rangeStart=-1.);
  declareProperty("MV_rangeStop", m_mv_rangeStop=1.);
  declareProperty("MV_cFraction", m_mv_cFrac=0.08);//DL1
  declareProperty("MV_60_cut", m_mv_60_weight_cut=0.94);
  declareProperty("MV_70_cut", m_mv_70_weight_cut=0.83);
  declareProperty("MV_77_cut", m_mv_77_weight_cut=0.64);
  declareProperty("MV_85_cut", m_mv_85_weight_cut=0.11);
}

JetTagMonitorAlgorithm::~JetTagMonitorAlgorithm() {}

StatusCode JetTagMonitorAlgorithm::initialize() {

  //Initialize ReadHandles and ReadHandleKeys
  ATH_CHECK(m_jetContainerKey.initialize());
  ATH_CHECK(m_muonContainerKey.initialize());
  ATH_CHECK(m_electronContainerKey.initialize());

  ATH_CHECK(m_vertContainerKey.initialize());
  ATH_CHECK(m_trackContainerKey.initialize());

  return AthMonitorAlgorithm::initialize();
}


StatusCode JetTagMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  using namespace Monitored;

  //----------------------Start Event Part---------------------------

  //check number of events via const EventContext& ctx
  //ctx.evt()

  // Declare the quantities which should be monitored
  //NB! The variables and histograms defined here must match the ones in the py file exactly!
  auto lb = Monitored::Scalar<int>("lb",0);
  auto run = Monitored::Scalar<int>("run",0);
  auto event = Monitored::Scalar<int>("event",0);

  //// Set the values of the monitored variables for the event		
  lb = GetEventInfo(ctx)->lumiBlock();
  run = GetEventInfo(ctx)->runNumber();
  event = GetEventInfo(ctx)->eventNumber();

  // auto lumiPerBCID = Monitored::Scalar<float>("lumiPerBCID",0.0);
  // lumiPerBCID = lbAverageInteractionsPerCrossing();

  //Fill. First argument is the tool name, all others are the variables to be saved.
  //fill("JetTagMonitor",lumiPerBCID,lb,random,pT,pT_passed);
  
  //Alternative fill method. Get the group yourself, and pass it to the fill function.
  auto tool = getGroup("JetTagMonitor");
  fill(tool,run);
  fill(tool,lb);

  //r21 event cutflow
  // m_cutflow->GetXaxis()->SetBinLabel(1,"All");
  // m_cutflow->GetXaxis()->SetBinLabel(2,"Good LAr");
  // m_cutflow->GetXaxis()->SetBinLabel(3,"PV Present");
  // m_cutflow->GetXaxis()->SetBinLabel(4,"PV Ntrk");
  // m_cutflow->GetXaxis()->SetBinLabel(5,"Trigger");

  auto Event_CutFlow = Monitored::Scalar<int>("Event_CutFlow",0);
  Event_CutFlow = 0;
  fill(tool,Event_CutFlow);

  if ( GetEventInfo(ctx)->errorState(xAOD::EventInfo::EventFlagSubDet::Tile) == xAOD::EventInfo::Error || 
       GetEventInfo(ctx)->errorState(xAOD::EventInfo::EventFlagSubDet::LAr)  == xAOD::EventInfo::Error ||
       GetEventInfo(ctx)->isEventFlagBitSet(xAOD::EventInfo::Core, 18) ) { 
    ATH_MSG_DEBUG("Event cleaning : skip event");
    return StatusCode::SUCCESS;    
  }

  Event_CutFlow = 1;
  fill(tool,Event_CutFlow);

  //////////////////////
  //* Get Pileup     *//
  //////////////////////

  auto mu = Monitored::Scalar<float>("mu",0);
  mu = GetEventInfo(ctx)->actualInteractionsPerCrossing();
  fill(tool,mu); 

  //////////////////////
  //* Primary vertex *//
  //////////////////////
 
  auto nPrimVtx = Monitored::Scalar<int>("nPrimVtx",0);
  auto xPrimVtx = Monitored::Scalar<float>("xPrimVtx",0);
  auto yPrimVtx = Monitored::Scalar<float>("yPrimVtx",0);
  auto zPrimVtx = Monitored::Scalar<float>("zPrimVtx",0);
  auto nTracks_PV = Monitored::Scalar<int>("nTracks_PV",0);
  
  SG::ReadHandle<xAOD::VertexContainer> vertices(m_vertContainerKey, ctx);
  if (!vertices.isValid()) {
    ATH_MSG_ERROR("Could not find vertex AOD container with name " << m_vertContainerKey);
    return StatusCode::FAILURE;
  }
  
  nPrimVtx = vertices->size();
  fill(tool,nPrimVtx);
  
  if (vertices->size() < 2) {//why < 2?
    ATH_MSG_DEBUG("No vertices reconstructed");
    return StatusCode::SUCCESS;
  }
  
  Event_CutFlow = 2;
  fill(tool,Event_CutFlow);

  bool foundVxTypePriVtx = false;

  for (const auto& vertItr : *vertices) {
    ATH_MSG_DEBUG("Simple vertex: x = " << xPrimVtx << ", y = " << yPrimVtx << ", z = " << zPrimVtx);
   
    if (vertItr->vertexType() == xAOD::VxType::PriVtx && vertItr->numberDoF() > 0 ) { 
      foundVxTypePriVtx = true;

      xPrimVtx = vertItr->x();
      yPrimVtx = vertItr->y();
      zPrimVtx = vertItr->z();
      ATH_MSG_DEBUG("Primary vertex: x = " << xPrimVtx << ", y = " << yPrimVtx << ", z = " << zPrimVtx);
      
      if (xPrimVtx == 0 && yPrimVtx == 0 && zPrimVtx == 0)
	ATH_MSG_WARNING("Primary Vertex is (0,0,0)");
      
      fill(tool,xPrimVtx);
      fill(tool,yPrimVtx);
      fill(tool,zPrimVtx);

      if (vertItr->nTrackParticles()>0) {
	nTracks_PV = vertItr->nTrackParticles();
	ATH_MSG_DEBUG("PV has "<< nTracks_PV <<" tracks");
      }
      
      fill(tool,nTracks_PV);

    }
  }
  
  if ( ! foundVxTypePriVtx ) {
    ATH_MSG_WARNING("xAOD::Vertex of type xAOD::VxType::PriVtx was not found in vertex container.");
    return StatusCode::SUCCESS;
  }

  //ATTENTION  
  // check PV > 4 tracks, if not, fail
  // if ( npv_trk <  m_pri_vtx_trk_min_cut )
  if ( nTracks_PV <  4 )
    return StatusCode::SUCCESS;

  Event_CutFlow = 3;
  fill(tool,Event_CutFlow);
  
  //////////////////////
  //* Triggers *//
  //////////////////////

  //removed october
  bool useTriggerDecisionTool = true; //how to implement in r22 python script?
  const auto& trigDecTool = getTrigDecisionTool();

  if (useTriggerDecisionTool && trigDecTool != 0) { // only apply trigger selection if bool is true (false for express stream) and trigDecTool is ok

    //ERROR for tool without explicit name
    //NEED TO:
    //find a way to put trigger name on python script (not hardcoded, by variable)
    //find a way to put wildcards * for 20-99 GeV electron and muon triggers
    
    ATH_MSG_DEBUG("TrigDecTool: " << trigDecTool);
    ATH_MSG_DEBUG("trigDecTool->isPassed(" << m_ElectronTrigger_201X << "): " << trigDecTool->isPassed(m_ElectronTrigger_201X));
    ATH_MSG_DEBUG("trigDecTool->isPassed(" << m_MuonTrigger_201X << "): " << trigDecTool->isPassed(m_MuonTrigger_201X));
    ATH_MSG_DEBUG("trigDecTool->isPassed(" << m_JetTrigger_201X << "): " << trigDecTool->isPassed(m_JetTrigger_201X));
    
    // auto chainGroup = trigDecTool->getChainGroup(".*");
    // for (auto & trig : chainGroup->getListOfTriggers()) {
    //   ATH_MSG_DEBUG("Found trigger " << trig);
    // }
    
    auto Trigger_CutFlow = Monitored::Scalar<int>("Trigger_CutFlow",0);
    Trigger_CutFlow = 0;
    fill(tool,Trigger_CutFlow);
   
    // 201X menu triggers
    if (trigDecTool->isPassed(m_ElectronTrigger_201X)) {
      Trigger_CutFlow = 1;
      fill(tool,Trigger_CutFlow);
    }
    if (trigDecTool->isPassed(m_MuonTrigger_201X)) {
      Trigger_CutFlow = 2;
      fill(tool,Trigger_CutFlow);
    }
    if (trigDecTool->isPassed(m_ElectronTrigger_201X) || trigDecTool->isPassed(m_MuonTrigger_201X)){
      Trigger_CutFlow = 3;
      fill(tool,Trigger_CutFlow);
    }
    if (trigDecTool->isPassed(m_ElectronTrigger_201X) && trigDecTool->isPassed(m_MuonTrigger_201X)){
      Trigger_CutFlow = 4;
      fill(tool,Trigger_CutFlow);
    }
    if (trigDecTool->isPassed(m_JetTrigger_201X)){
      Trigger_CutFlow = 5;
      fill(tool,Trigger_CutFlow);
    }

    //IMPORTANT    
    // Require e/mu trigger to have unbiased sample of jets (and larger fraction of b-jets since many of these are ttbar events)
    if (!trigDecTool->isPassed(m_ElectronTrigger_201X) && !trigDecTool->isPassed(m_MuonTrigger_201X)) // 201X menu triggers
      return StatusCode::SUCCESS;
  }
  
  Event_CutFlow = 4;
  fill(tool,Event_CutFlow);

  //Events are selected
  //Fill track plots with ID tracks
  //Fill jet histograms (after Jet Selection)  

  ///////////////////////////////
  //* TrackParticle container *//
  ///////////////////////////////
        
  auto nTracks = Monitored::Scalar<int>("nTracks",0);

  uint8_t nPixelHits  = 0;
  uint8_t nSCTHits    = 0;
  uint8_t nBLayerHits = 0;
  uint8_t nTRTHits    = 0;

  auto nHitsBLayer = Monitored::Scalar<float>("nHitsBLayer",0);
  auto nHitsPixel = Monitored::Scalar<float>("nHitsPixel",0);
  auto nHitsSCT = Monitored::Scalar<float>("nHitsSCT",0);
  auto nHitsSi = Monitored::Scalar<float>("nHitsSi",0);
  auto nHitsTRT = Monitored::Scalar<float>("nHitsTRT",0);
  auto nHitsID = Monitored::Scalar<float>("nHitsID",0);

  //RESTART FROM HERE
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_trackContainerKey, ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR("Could not find track AOD container with name " << m_trackContainerKey);
    return StatusCode::FAILURE;
  }
  
  nTracks = tracks->size();
  fill(tool,nTracks);

  for (const auto& trackItr : *tracks) {
    trackItr->summaryValue(nBLayerHits, xAOD::numberOfBLayerHits);
    nHitsBLayer = (float)nBLayerHits;
    fill(tool, nHitsBLayer);
    trackItr->summaryValue(nPixelHits, xAOD::numberOfPixelHits);
    nHitsPixel=(float)nPixelHits;
    fill(tool, nHitsPixel);
    trackItr->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
    nHitsSCT=(float)nSCTHits;
    fill(tool, nHitsSCT);
    trackItr->summaryValue(nTRTHits, xAOD::numberOfTRTHits);
    nHitsTRT=(float)nTRTHits;
    fill(tool, nHitsTRT);
    nHitsSi=(float)nPixelHits+(float)nSCTHits;
    fill(tool, nHitsSi);
    nHitsID=(float)nPixelHits+(float)nSCTHits+(float)nTRTHits;
    fill(tool, nHitsID);
        
    ATH_MSG_DEBUG("Track hits: BLayer = " << nHitsBLayer << ", PIX = " << nHitsBLayer);
    ATH_MSG_DEBUG("Track hits: SCT = " << nHitsSCT << ", Si = " << nHitsSi);
    ATH_MSG_DEBUG("Track hits: TRT = " << nHitsTRT << ", ID = " << nHitsID);
    
  }

  //----------------------Start Muon & Electron Part for ttbar events ------------------------

  //////////////////////
  //* Muon container *//
  //////////////////////

  auto muon_PT = Monitored::Scalar<double>("muon_PT",0.0);
  auto nIsoMuons = Monitored::Scalar<int>("nIsoMuons",0.0);
  int muonCharge = 0;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_muonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_muonContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto& muonItr : *muons) {
    ATH_MSG_DEBUG("Muon kinematics: eta = " << muonItr->eta() << ", phi= " << muonItr->phi() << ", pT= " << muonItr->pt() / Gaudi::Units::GeV);
    muon_PT = muonItr->pt();
    fill(tool,muon_PT);

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
    muonCharge = muonItr->charge();
    ++nIsoMuons;
  }

  if(nIsoMuons>0)
    ATH_MSG_DEBUG(nIsoMuons << " Muon(s) isolated in event "<< event);
  fill(tool,nIsoMuons);

  //////////////////////////
  //* Electron container *//
  //////////////////////////

  auto electron_PT = Monitored::Scalar<double>("electron_PT",0.0);
  auto nIsoElectrons = Monitored::Scalar<int>("nIsoElectrons",0.0);
  int electronCharge = 0;

  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronContainerKey, ctx);
  if (! electrons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain electron Collection with name "<< m_electronContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto& electronItr : *electrons) {
    ATH_MSG_DEBUG("Electron kinematics: eta = " << electronItr->eta() << ", phi= " << electronItr->phi() << ", pT= " << electronItr->pt() / Gaudi::Units::GeV);
    electron_PT = electronItr->pt();
    fill(tool,electron_PT);

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
    electronCharge = electronItr->charge();
    ++nIsoElectrons;
  }

  if(nIsoElectrons>0)
    ATH_MSG_DEBUG(nIsoElectrons << " Electron(s) isolated in event "<< event);

  fill(tool,nIsoElectrons);

  // require == 1 electron, == 1 muon and opposite charged leptons
  bool isTTbarEvent = false;
  if (nIsoElectrons == 1 && nIsoMuons == 1 && electronCharge*muonCharge == -1) isTTbarEvent = true;

  if(isTTbarEvent)
    ATH_MSG_DEBUG("This is ttbar event "<< event);

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

  //--> last Event CutFlow cut --> fill Jet CutFlow plot
 
  //r21 jet cutflow
  // m_cutflow_jet->GetXaxis()->SetBinLabel(1,"All");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(2,"Quality Cuts");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(3,"Kinematic Cuts");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(4,"JVT Cut");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(5,"Muon Overlap");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(6,"Taggable");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(7,"Taggable Good");
  // m_cutflow_jet->GetXaxis()->SetBinLabel(8,"Taggable Suspect");

  auto Jet_CutFlow = Monitored::Scalar<int>("Jet_CutFlow",0);
 
  auto nJets = Monitored::Scalar<int>("nJets",0.0);

  auto jet_PT = Monitored::Scalar<float>("jet_PT",0.0);
  auto jet_ETA = Monitored::Scalar<float>("jet_ETA",0.0);
  auto jet_PHI = Monitored::Scalar<float>("jet_PHI",0.0);

  auto jet_eta_all = Monitored::Scalar<float>("jet_eta_all",0.0);
  auto jet_phi_all = Monitored::Scalar<float>("jet_phi_all",0.0);
  auto jet_eta_quality = Monitored::Scalar<float>("jet_eta_quality",0.0);
  auto jet_phi_quality = Monitored::Scalar<float>("jet_phi_quality",0.0);
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

  auto jet_MV = Monitored::Scalar<double>("jet_MV",0.0);
  auto jet_MV_JVT_bef = Monitored::Scalar<double>("jet_MV_JVT_bef",0.0);
  auto jet_MV_JVT_aft = Monitored::Scalar<double>("jet_MV_JVT_aft",0.0);
  auto jet_DL = Monitored::Scalar<float>("jet_DL",0.0);

  //Variables for TTbar events
  const xAOD::Jet* firstTTbarJet = nullptr;
  auto nTTbarGoodJets = Monitored::Scalar<int>("nTTbarGoodJets",0.0);

  // soft muon observables
  auto nSoftMuons = Monitored::Scalar<int>("nSoftMuons",0.0);
  auto nSMTJets = Monitored::Scalar<int>("nSMTJets",0.0);
  auto drSMT = Monitored::Scalar<float>("drSMT",0.0);
  auto ptSMT_jet = Monitored::Scalar<float>("ptSMT_jet",0.0);
  auto ptSMT_mu = Monitored::Scalar<float>("ptSMT_mu",0.0);
  auto ptSMT_ratio = Monitored::Scalar<float>("ptSMT_ratio",0.0);
  auto mvSMT = Monitored::Scalar<float>("mvSMT",0.0);
  TLorentzVector smt_jet, smt_mu;
  auto pTrelSMT = Monitored::Scalar<float>("pTrelSMT",0.0);

  nJets = jets->size();
  fill(tool,nJets);

  for (const auto& jetItr : *jets) {
    
    jet_PT = jetItr->pt() / Gaudi::Units::GeV;
    jet_ETA = jetItr->eta();
    jet_PHI = jetItr->phi();
    ATH_MSG_DEBUG("Jet kinematics: eta = " << jet_ETA << ", phi= " << jet_PHI << ", pT= " << jet_PT);
    
    double mv = 0; 
    mv = getMVweight(jetItr); // get MV weight (MV2c10/DL1*)
    jet_MV = mv;

    fill(tool,jet_PT);
    fill(tool,jet_ETA);
    fill(tool,jet_PHI);
    fill(tool,jet_MV);
     
    // All jets
    Jet_CutFlow = 0;
    fill(tool,Jet_CutFlow);

    jet_eta_all = jetItr->eta();
    jet_phi_all = jetItr->phi();
    fill(tool,jet_eta_all,jet_phi_all);//fill 2D plot

    if( !passJetQualityCuts(jetItr) ) continue;
    
    // Jets passing quality cuts --> quality jets (standard jet cleaning cuts)
    Jet_CutFlow = 1;
    fill(tool,Jet_CutFlow);
    jet_eta_quality = jetItr->eta();
    jet_phi_quality = jetItr->phi();
    fill(tool,jet_eta_quality,jet_phi_quality);//fill 2D plot
    
    if ( !passKinematicCuts(jetItr) ) continue; 
    
    // Jets passing kinematic cuts
    Jet_CutFlow = 2;
    fill(tool,Jet_CutFlow);
    jet_eta_kin = jetItr->eta();
    jet_phi_kin = jetItr->phi();
    fill(tool,jet_eta_kin,jet_phi_kin);//fill 2D plot
    
    // fillTrackInJetHistograms(jetItr); // fill histograms with properties of associated tracks

    //jet_MV_JVT_bef = mv;
    //fill(tool,jet_MV_JVT_bef);

    if ( !passJVTCuts(jetItr) ) continue; 

    //jet_MV_JVT_aft = mv;
    //fill(tool,jet_MV_JVT_aft);

    // Jets passing JVT cuts
    Jet_CutFlow = 3;
    fill(tool,Jet_CutFlow);
    jet_eta_jvt = jetItr->eta();
    jet_phi_jvt = jetItr->phi();
    fill(tool,jet_eta_jvt,jet_phi_jvt);//fill 2D plot
 
    //if ( !passMuonOverlap(jetItr,ctx) ) continue; //developed inside

    // loop over muon container  
    int n_isoMuons = 0, n_ptMuons = 0;
    for (const auto& muonItr : *muons) {
      // do stuff with muons, i.e.  muonItr->pt();

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
    Jet_CutFlow = 4;
    fill(tool,Jet_CutFlow);
    jet_eta_overlap = jetItr->eta();
    jet_phi_overlap = jetItr->phi();
    fill(tool,jet_eta_overlap,jet_phi_overlap);//fill 2D plot
 
    Jet_t taggabilityLabel = getTaggabilityLabel(jetItr); // check if jet is taggable (defined as goodJet or suspectJet or badJet)

    if ( taggabilityLabel == goodJet ) {
      Jet_CutFlow = 5;
      fill(tool,Jet_CutFlow);
      jet_eta_good = jetItr->eta();
      jet_phi_good = jetItr->phi();
      fill(tool,jet_eta_good,jet_phi_good);//fill 2D plot

      fillGoodJetHistos(jetItr); //fill good jet histograms, also with b-tagging information

      //Fill MV plots vs <mu>
      auto jet_MV_mu_0_30 = Monitored::Scalar<float>("jet_MV_mu_0_30",0);
      auto jet_MV_mu_30_50 = Monitored::Scalar<float>("jet_MV_mu_30_50",0);
      auto jet_MV_mu_50_100 = Monitored::Scalar<float>("jet_MV_mu_50_100",0);
      
      if ( mu > 0. && mu < 30. ) {
	jet_MV_mu_0_30 = mv;
	fill(tool,jet_MV_mu_0_30);}
      else if ( mu > 30. && mu < 50. ) {
	jet_MV_mu_30_50 = mv;
	fill(tool,jet_MV_mu_30_50);}
      else if ( mu > 50. && mu < 100. ) {
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

      Jet_CutFlow = 8;
      fill(tool,Jet_CutFlow);

      nSoftMuons = 0; //restarting counter;

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
	drSMT = (jetItr->p4()).DeltaR(softMuonItr->p4());
	if ( drSMT >= 0.4) continue; //jet muon DR < 0.4
	ptSMT_mu = softMuonItr->pt() / Gaudi::Units::GeV;
	smt_mu.SetPtEtaPhiE(softMuonItr->pt(),softMuonItr->eta(),softMuonItr->phi(),softMuonItr->e());
	++nSoftMuons;
      }//end loop on muons

      ATH_MSG_DEBUG("Number of soft muons per jet is " << nSoftMuons);
      fill(tool,nSoftMuons);

      if( nSoftMuons == 1){
	ptSMT_jet = jetItr->pt() / Gaudi::Units::GeV;
	jet_eta_smt = jetItr->eta();
	jet_phi_smt = jetItr->phi();
	smt_jet.SetPtEtaPhiE(jetItr->pt(),jetItr->eta(),jetItr->phi(),jetItr->e());
	mvSMT = mv;
	++nSMTJets;
      }

    }
    else if ( taggabilityLabel == suspectJet ) {
      Jet_CutFlow = 6;
      fill(tool,Jet_CutFlow);
      jet_eta_suspect = jetItr->eta();
      jet_phi_suspect = jetItr->phi();
      fill(tool,jet_eta_suspect,jet_phi_suspect);//fill 2D plot

      // fillSuspectJetHistos(*jetItr); //other histograms to fill
    }
    else if ( taggabilityLabel == badJet ) {
      Jet_CutFlow = 7;
      fill(tool,Jet_CutFlow);
      jet_eta_bad = jetItr->eta();
      jet_phi_bad = jetItr->phi();
      fill(tool,jet_eta_bad,jet_phi_bad);//fill 2D plot

      //fillBadJetHistos(*jetItr); //other histograms to fill
    }
    
  }
  //end of jetItr loop

  //ATH_MSG_DEBUG("... done fillJetHistograms()");

  if(isTTbarEvent){
    ATH_MSG_DEBUG("Number of jets in ttbar events " << nTTbarGoodJets);
    fill(tool,nTTbarGoodJets);
  }

  ATH_MSG_DEBUG("Number of jets with == 1 soft muon per event is " << nSMTJets);
  fill(tool,nSMTJets);

  if (nSMTJets == 1){ //Fill SMT plots 
    fill(tool,drSMT);
    fill(tool,ptSMT_mu);
    fill(tool,ptSMT_jet);
    ptSMT_ratio = ptSMT_mu/ptSMT_jet;
    fill(tool,ptSMT_ratio);
    pTrelSMT = smt_mu.Perp((smt_jet+smt_mu).Vect()) / Gaudi::Units::GeV;
    fill(tool,pTrelSMT);
    fill(tool,jet_eta_smt,jet_phi_smt);//fill 2D plot
    fill(tool,mvSMT);
  }

  // Two variables (value and passed) needed for TEfficiency
  // auto pT = Monitored::Scalar<float>("pT",0.0);
  // auto pT_passed = Monitored::Scalar<float>("pT_passed",false);

  return StatusCode::SUCCESS;
}

bool JetTagMonitorAlgorithm::passJetQualityCuts(const xAOD::Jet *jet) const {

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
	
  if (m_skipJetQuality) //In case of Ion-Ion or Ion-proton collision
    pass_cuts = true; //Skip the Jet Quality cuts (thresholds tuned on pp event jets)

  return pass_cuts;
}

bool JetTagMonitorAlgorithm::passKinematicCuts(const xAOD::Jet *jet) const {
  
  ATH_MSG_DEBUG("passKinematicCuts()");
  ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt() / Gaudi::Units::GeV);

  // kinematic cuts defined as properties
  if ( jet->pt()  / Gaudi::Units::GeV < m_JetPtCut || fabs(jet->eta()) > m_JetEtaCut )
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

void JetTagMonitorAlgorithm::fillTTbarHistograms(const xAOD::Jet *jet) const {
  
  double mv = getMVweight(jet); // get MV weight (MV2c10/DL1*)

  auto tool = getGroup("JetTagMonitor");

  auto mvTTbarJets = Monitored::Scalar<float>("mvTTbarJets",0);

  mvTTbarJets=mv;
  fill(tool,mvTTbarJets);

  //Fill simple plots with number and pT distribution of ttbar jets
  auto nTTbarJets_60tag = Monitored::Scalar<int>("nTTbarJets_60tag",0);
  auto nTTbarJets_70tag = Monitored::Scalar<int>("nTTbarJets_70tag",0);
  auto nTTbarJets_77tag = Monitored::Scalar<int>("nTTbarJets_77tag",0);
  auto nTTbarJets_85tag = Monitored::Scalar<int>("nTTbarJets_85tag",0);

  auto ptTTbarJets_60tag = Monitored::Scalar<float>("ptTTbarJets_60tag",0);
  auto ptTTbarJets_70tag = Monitored::Scalar<float>("ptTTbarJets_70tag",0);
  auto ptTTbarJets_77tag = Monitored::Scalar<float>("ptTTbarJets_77tag",0);
  auto ptTTbarJets_85tag = Monitored::Scalar<float>("ptTTbarJets_85tag",0);

  if (mv > m_mv_85_weight_cut) {
    fill(tool,nTTbarJets_85tag);
    ptTTbarJets_85tag=jet->pt() / Gaudi::Units::GeV;
    fill(tool,ptTTbarJets_85tag);
    if (mv > m_mv_77_weight_cut) {
      fill(tool,nTTbarJets_77tag);
      ptTTbarJets_77tag=jet->pt() / Gaudi::Units::GeV;
      fill(tool,ptTTbarJets_77tag);
      if (mv > m_mv_70_weight_cut) {
	fill(tool,nTTbarJets_70tag);
	ptTTbarJets_70tag=jet->pt() / Gaudi::Units::GeV;
	fill(tool,ptTTbarJets_70tag);
	if (mv > m_mv_60_weight_cut) {
	  fill(tool,nTTbarJets_60tag);
	  ptTTbarJets_60tag=jet->pt() / Gaudi::Units::GeV;
	  fill(tool,ptTTbarJets_60tag);
	}
      }
    }
  }

  //fill Efficiency plots
  auto nTTbarJets = Monitored::Scalar<int>("nTTbarJets",0);
  //nTTbarJets_pretag=1; //useless, variable should be 0
  //ATH_MSG_INFO("TTbar event variable --> "+nTTbarJets_pretag);
  auto pass85n = Monitored::Scalar<bool>("pass85n",false);
  auto pass77n = Monitored::Scalar<bool>("pass77n",false);
  auto pass70n = Monitored::Scalar<bool>("pass70n",false);
  auto pass60n = Monitored::Scalar<bool>("pass60n",false);
  pass85n = mv > m_mv_85_weight_cut;
  pass77n = mv > m_mv_77_weight_cut;
  pass70n = mv > m_mv_70_weight_cut;
  pass60n = mv > m_mv_60_weight_cut;
  fill(tool,nTTbarJets,pass85n,pass77n,pass70n,pass60n);
  
  auto ptTTbarJets = Monitored::Scalar<float>("ptTTbarJets",0);
  ptTTbarJets=jet->pt() / Gaudi::Units::GeV;
  auto pass85p = Monitored::Scalar<bool>("pass85p",false);
  auto pass77p = Monitored::Scalar<bool>("pass77p",false);
  auto pass70p = Monitored::Scalar<bool>("pass70p",false);
  auto pass60p = Monitored::Scalar<bool>("pass60p",false);
  pass85p = mv > m_mv_85_weight_cut;
  pass77p = mv > m_mv_77_weight_cut;
  pass70p = mv > m_mv_70_weight_cut;
  pass60p = mv > m_mv_60_weight_cut;
  fill(tool,ptTTbarJets,pass85p,pass77p,pass70p,pass60p);

  return;
}


void JetTagMonitorAlgorithm::fillGoodJetHistos(const xAOD::Jet *jet) const {

  double mv = getMVweight(jet); // get MV weight (MV2c10/DL1*)

  auto tool = getGroup("JetTagMonitor");

  auto jet_MV_good = Monitored::Scalar<float>("jet_MV_good",0);
  
  jet_MV_good = mv;
  fill(tool,jet_MV_good);

  // Fill general jet kinematic plots vs High level tagger
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

  //fill jet MV plots in eta and phi bins
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

  //fill 2D plots of eta and phi distributions vs MV tag WP
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

  //fill Efficiency plots
  auto jet_eta = Monitored::Scalar<float>("jet_eta",0);
  jet_eta = jet->eta();
  auto pass85e = Monitored::Scalar<bool>("pass85e",false);
  auto pass77e = Monitored::Scalar<bool>("pass77e",false);
  auto pass70e = Monitored::Scalar<bool>("pass70e",false);
  auto pass60e = Monitored::Scalar<bool>("pass60e",false);
  pass85e = mv > m_mv_85_weight_cut;
  pass77e= mv > m_mv_77_weight_cut;
  pass70e = mv > m_mv_70_weight_cut;
  pass60e = mv > m_mv_60_weight_cut;
  fill(tool,jet_eta,pass85e,pass77e,pass70e,pass60e);

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
  fill(tool,jet_phi,pass85f,pass77f,pass70f,pass60f);
  
  return;
}
