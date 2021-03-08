/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TrkTrack/Track.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TEfficiency.h"

#include <vector>
#include <string>
#include <sstream>

JetTagMonitorAlgorithm::JetTagMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  :AthMonitorAlgorithm(name,pSvcLocator)
{
  declareProperty("JetsCollection",m_JetContainerKey);
  declareProperty("MuonsCollection",m_MuonContainerKey);
  declareProperty("ElectronsCollection",m_ElectronContainerKey);

  declareProperty("SkipJetFilter", m_SkipJetFilter);
  declareProperty("DoExtraTaggerHistos", m_DoExtraTaggerHistos);
 
  declareProperty("ElectronTrigger_201X", m_ElectronTrigger_201X);
  declareProperty("MuonTrigger_201X", m_MuonTrigger_201X);

  declareProperty("JetEtaCut", m_JetEtaCut);
  declareProperty("JetPtCut", m_JetPtCut);
  declareProperty("SoftMuonPtCut", m_SoftMuonPtCut);

  declareProperty("MinGoodTrackCut", m_MinGoodTrackCut);
  declareProperty("TrackPtCut", m_TrackPtCut);
  declareProperty("TrackEtaCut", m_TrackEtaCut);
  declareProperty("Trackd0Cut", m_Trackd0Cut);
  declareProperty("Trackz0sinCut", m_Trackz0sinCut);
  declareProperty("TrackChi2ndfCut", m_TrackChi2ndfCut);
  declareProperty("TrackHitIBLCut", m_TrackHitIBLCut);
  declareProperty("TrackHitPixCut", m_TrackHitPixCut);
  declareProperty("TrackHolePixCut", m_TrackHolePixCut);
  declareProperty("TrackHitSCTCut", m_TrackHitSCTCut);
  declareProperty("TrackHoleSCTCut", m_TrackHoleSCTCut);
  declareProperty("TrackHitSiCut", m_TrackHitSiCut);
  declareProperty("TrackHoleSiCut", m_TrackHoleSiCut);

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
  declareProperty("bFraction", m_bFraction);
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
    if (m_TaggerName=="DL1" || m_TaggerName=="DL1r" || m_TaggerName=="DL1rnn") {
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
    ATH_MSG_ERROR("Could not find vertex AOD container with name " << m_VertContainerKey);
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

  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrackContainerKey, ctx);
  if (!tracks.isValid()) {
    ATH_MSG_ERROR("Could not find track AOD container with name " << m_TrackContainerKey);
    return StatusCode::FAILURE;
  }
  
  Tracks_n = tracks->size();
  fill(tool,Tracks_n);

  for (const auto trackItr : *tracks) {
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

  auto Muon_pT = Monitored::Scalar<float>("Muon_pT",0.0);
  auto IsolatedMuons_n = Monitored::Scalar<int>("IsolatedMuons_n",0.0);
  int Muon_charge = 0;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto muonItr : *muons) {
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

  auto Electron_pT = Monitored::Scalar<float>("Electron_pT",0.0);
  auto IsolatedElectrons_n = Monitored::Scalar<int>("IsolatedElectrons_n",0.0);
  int Electron_charge = 0;

  SG::ReadHandle<xAOD::ElectronContainer> electrons(m_ElectronContainerKey, ctx);
  if (! electrons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain electron Collection with name "<< m_ElectronContainerKey);
    return StatusCode::FAILURE;
  }

  for (const auto electronItr : *electrons) {
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
    ATH_MSG_WARNING("This is a candidate ttbar event "<< Run_event);

  /////////////////////
  //* Jet container *//
  /////////////////////

  ATH_MSG_DEBUG("Start jet part");

  //----------------------Start Jet Part---------------------------

  SG::ReadHandle<xAOD::JetContainer> jets(m_JetContainerKey, ctx);
  if (!jets.isValid()) {
    ATH_MSG_ERROR("Could not find jet AOD container with name " << m_JetContainerKey);
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
   
    // fill histograms with properties of jet associated tracks
    fillJetTracksHistos(jetItr, PVZ);

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
 
    // loop over muon container  
    int n_isoMuons = 0, n_ptMuons = 0;
    for (const auto muonItr : *muons) {
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
    Jet_t qualityLabel = getQualityLabel(jetItr, PVZ); 

    // Fill variable for 2D fraction plot (good jets / all jets before selection)
    passGood = qualityLabel == goodJet;
    fill(tool,passGood,jet_eta_all,jet_phi_all); 

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
	if (softMuonItr->pt() / Gaudi::Units::GeV < m_SoftMuonPtCut) continue; //fixed pT cut 5 GeV
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
  
  if (m_SkipJetFilter) //In case of Ion-Ion or Ion-proton collision
    pass_cuts = true; //Skip the Jet Filter cuts (thresholds tuned on pp event jets)

  return pass_cuts;
}

bool JetTagMonitorAlgorithm::passKinematicCut(const xAOD::Jet *jet) const {
  
  ATH_MSG_DEBUG("passKinematicCut()");
  ATH_MSG_DEBUG("Jet kinematics: eta = " << jet->eta() << ", phi= " << jet->phi() << ", pT= " << jet->pt() / Gaudi::Units::GeV);

  if ( jet->pt() / Gaudi::Units::GeV < m_JetPtCut || fabs(jet->eta()) > m_JetEtaCut )
    return false;

  return true;
}


bool JetTagMonitorAlgorithm::passJVTCut(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("passJVTCut()");
  
  static SG::AuxElement::Accessor<float> JVT( "Jvt" );
  float jvt = JVT(*jet);

  if( !(
	( (jet->pt() / Gaudi::Units::GeV < m_JVTpTCut)&&(fabs(jet->eta())<m_JVTetaCut)&&(jvt > m_JVTCut) )
	||( (jet->pt() / Gaudi::Units::GeV<m_JVTpTCut)&&(fabs(jet->eta())>m_JVTetaCut) )
	||(jet->pt() / Gaudi::Units::GeV>m_JVTpTCut)
	)
      ) return false;

  return true;
}

double JetTagMonitorAlgorithm::getTaggerWeight(const xAOD::Jet *jet) const {

  ATH_MSG_DEBUG("retrieving MV2c10/DL1* weight");

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return 0;
  }

  double mv = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if(m_TaggerName=="MV2c10" || m_TaggerName=="MV2c10r" || m_TaggerName=="MV2c10rnn"){
    bTaggingObject->MVx_discriminant(m_TaggerName,mv);
  }
  else if (m_TaggerName=="DL1" || m_TaggerName=="DL1r" || m_TaggerName=="DL1rnn"){
    bTaggingObject->pu(m_TaggerName,mv_pu);
    bTaggingObject->pc(m_TaggerName,mv_pc);
    bTaggingObject->pb(m_TaggerName,mv_pb);
    //DL1* formula (standard)
    if ( mv_pb != 0 && (mv_pu != 0 || mv_pc != 0)) {
      mv = log( mv_pb / ( mv_pu * ( 1 - m_cFraction ) + mv_pc * m_cFraction ) );
    }
    //DL1*c formula (for DL1c)
    //mv = log( mv_pb / ( mv_pu * ( 1 - m_bFraction ) + mv_pc * m_bFraction ) );
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

  if ( fabs(jet->eta()) > 2.0 ) {
    sus_jet_MV_eta_20_25=mv;
    fill(tool,sus_jet_MV_eta_20_25);}
  else if ( fabs(jet->eta()) > 1.5 ) {
    sus_jet_MV_eta_15_20=mv;
    fill(tool,sus_jet_MV_eta_15_20);}
  else if ( fabs(jet->eta()) > 1.0 ) {
    sus_jet_MV_eta_10_15=mv;
    fill(tool,sus_jet_MV_eta_10_15);}
  else if ( fabs(jet->eta()) > 0.5 ) {
    sus_jet_MV_eta_05_10=mv;
    fill(tool,sus_jet_MV_eta_05_10);}
  else if ( fabs(jet->eta()) > 0.0 ) {
    sus_jet_MV_eta_00_05=mv;
    fill(tool,sus_jet_MV_eta_00_05);}

  if ( fabs(jet->phi()) > 2.5 ) {
    sus_jet_MV_phi_25_31=mv;
    fill(tool,sus_jet_MV_phi_25_31);}
  else if ( fabs(jet->phi()) > 2.0 ) {
    sus_jet_MV_phi_20_25=mv;
    fill(tool,sus_jet_MV_phi_20_25);}
  else if ( fabs(jet->phi()) > 1.5 ) {
    sus_jet_MV_phi_15_20=mv;
    fill(tool,sus_jet_MV_phi_15_20);}
  else if ( fabs(jet->phi()) > 1.0 ) {
    sus_jet_MV_phi_10_15=mv;
    fill(tool,sus_jet_MV_phi_10_15);}
  else if ( fabs(jet->phi()) > 0.5 ) {
    sus_jet_MV_phi_05_10=mv;
    fill(tool,sus_jet_MV_phi_05_10);}
  else if ( fabs(jet->phi()) > 0.0 ) {
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

  auto jet_IP3D_nTrack_good = Monitored::Scalar<int>("jet_IP3D_nTrack_good",0);
  auto jet_SV1_nTrack_good = Monitored::Scalar<int>("jet_SV1_nTrack_good",0);

  double rnnip_llr = 0, mv_pu = 0, mv_pb = 0, mv_pc = 0;  

  if (m_TaggerName=="DL1" || m_TaggerName=="DL1r" || m_TaggerName=="DL1rnn"){
    bTaggingObject->pu(m_TaggerName,mv_pu);
    bTaggingObject->pc(m_TaggerName,mv_pc);
    bTaggingObject->pb(m_TaggerName,mv_pb);
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

  bTaggingObject->taggerInfo(jet_SV1_nTrack_good, xAOD::SV1_NGTinSvx);
  jet_IP3D_nTrack_good = bTaggingObject->nIP3D_TrackParticles();

  fill(tool,jet_MV_pu_good,jet_MV_pc_good,jet_MV_pb_good,jet_IP2D_good,jet_IP3D_good,jet_SV1_good,jet_JetFitter_good,jet_RNNIP_good,jet_IP3D_nTrack_good,jet_SV1_nTrack_good);

  return;
}


void JetTagMonitorAlgorithm::fillJetTracksHistos(const xAOD::Jet *jet, float PV_Z) const {

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return;
  }

  auto tool = getGroup("JetTagMonitor");

  auto JetTracks_nSV = Monitored::Scalar<int>("JetTracks_nSV",0);
  auto JetTracks_n = Monitored::Scalar<int>("JetTracks_n",0);
  auto JetTracks_pT = Monitored::Scalar<float>("JetTracks_pT",0);
  auto JetTracks_eta = Monitored::Scalar<float>("JetTracks_eta",0);
  auto JetTracks_phi = Monitored::Scalar<float>("JetTracks_phi",0);
  auto JetTracks_DR = Monitored::Scalar<float>("JetTracks_DR",0);

  auto JetTracks_d0 = Monitored::Scalar<float>("JetTracks_d0",0);
  auto JetTracks_d0s = Monitored::Scalar<float>("JetTracks_d0s",0);
  auto JetTracks_d0si = Monitored::Scalar<float>("JetTracks_d0si",0);
  auto JetTracks_z0 = Monitored::Scalar<float>("JetTracks_z0",0);
  auto JetTracks_z0s = Monitored::Scalar<float>("JetTracks_z0s",0);
  auto JetTracks_z0si = Monitored::Scalar<float>("JetTracks_z0si",0);

  auto JetTracks_Hits_IBL = Monitored::Scalar<float>("JetTracks_Hits_IBL",0);
  auto JetTracks_Hits_Pixel = Monitored::Scalar<float>("JetTracks_Hits_Pixel",0);
  auto JetTracks_Hits_SCT = Monitored::Scalar<float>("JetTracks_Hits_SCT",0);
  auto JetTracks_Hits_TRT = Monitored::Scalar<float>("JetTracks_Hits_TRT",0);

  uint8_t jt_nBLayerHits = 0;
  uint8_t jt_nPixHits    = 0;
  uint8_t jt_nSCTHits    = 0;
  uint8_t jt_nTRTHits    = 0;

  TLorentzVector jet_TLV;
  jet_TLV.SetPtEtaPhiE(jet->pt(), jet->eta(), jet->phi(), jet->e());

  std::vector<ElementLink<xAOD::VertexContainer>> SVs = bTaggingObject->auxdata<std::vector< ElementLink< xAOD::VertexContainer > > >("SV1_vertices");

  JetTracks_nSV = SVs.size();

  std::vector<ElementLink<xAOD::TrackParticleContainer>> assocTracks = bTaggingObject->auxdata< std::vector< ElementLink<xAOD::TrackParticleContainer > > >("BTagTrackToJetAssociator");

  JetTracks_n = assocTracks.size();

  fill(tool,JetTracks_n,JetTracks_nSV);

  for ( const ElementLink< xAOD::TrackParticleContainer >& jetTracks : assocTracks ) {
    if ( not jetTracks.isValid() ) continue;
    const xAOD::TrackParticle* jetTrackItr = *jetTracks;

    JetTracks_pT =   jetTrackItr->pt() / Gaudi::Units::GeV;
    JetTracks_eta =  jetTrackItr->eta();
    JetTracks_phi =  jetTrackItr->phi();

    TLorentzVector JetTrack_TLV;
    JetTrack_TLV.SetPtEtaPhiE(jetTrackItr->pt(), jetTrackItr->eta(), jetTrackItr->phi(), jetTrackItr->e());

    JetTracks_DR = jet_TLV.DeltaR(JetTrack_TLV);

    JetTracks_d0 =   jetTrackItr->d0();
    JetTracks_d0s =  sqrt( jetTrackItr->definingParametersCovMatrix()( Trk::d0, Trk::d0 ) ); //sigma
    JetTracks_d0si = JetTracks_d0/JetTracks_d0s; //significance

    float PVZ = PV_Z;
    JetTracks_z0 =   jetTrackItr->z0() + jetTrackItr->vz() - PVZ; 
    JetTracks_z0s =  sqrt( jetTrackItr->definingParametersCovMatrix()( Trk::z0, Trk::z0 ) ); //sigma
    JetTracks_z0si = JetTracks_z0/JetTracks_z0s; //significance

    if (jetTrackItr->summaryValue(jt_nBLayerHits, xAOD::numberOfBLayerHits)) JetTracks_Hits_IBL = jt_nBLayerHits;
    if (jetTrackItr->summaryValue(jt_nPixHits, xAOD::numberOfPixelHits)) JetTracks_Hits_Pixel = jt_nPixHits;
    if (jetTrackItr->summaryValue(jt_nSCTHits, xAOD::numberOfSCTHits)) JetTracks_Hits_SCT = jt_nSCTHits;
    if (jetTrackItr->summaryValue(jt_nTRTHits, xAOD::numberOfTRTHits)) JetTracks_Hits_TRT = jt_nTRTHits;

    fill(tool,JetTracks_pT,JetTracks_eta,JetTracks_phi,JetTracks_DR,JetTracks_d0,JetTracks_d0s,JetTracks_d0si,JetTracks_z0,JetTracks_z0s,JetTracks_z0si,JetTracks_Hits_IBL,JetTracks_Hits_Pixel,JetTracks_Hits_SCT,JetTracks_Hits_TRT);

  }

  return;
}


JetTagMonitorAlgorithm::Jet_t JetTagMonitorAlgorithm::getQualityLabel(const xAOD::Jet *jet, float PV_Z) const {

  const xAOD::BTagging *bTaggingObject = xAOD::BTaggingUtilities::getBTagging( *jet );
  if ( !bTaggingObject ) {
    ATH_MSG_ERROR( "Could not retrieve b-tagging object from selected jet." );
    return badJet;
  }

  auto tool = getGroup("JetTagMonitor");

  auto SelTracks_n_all = Monitored::Scalar<int>("SelTracks_n_all",0);
  auto SelTracks_pT_all = Monitored::Scalar<float>("SelTracks_pT_all",0);
 
  auto SelTracks_d0 = Monitored::Scalar<float>("SelTracks_d0",0);
  auto SelTracks_z0sin = Monitored::Scalar<float>("SelTracks_z0sin",0);

  auto SelTracks_chi2ndf = Monitored::Scalar<float>("SelTracks_chi2ndf",0);
 
  auto SelTracks_HitIBL = Monitored::Scalar<int>("SelTracks_HitIBL",0);
  auto SelTracks_HitPix = Monitored::Scalar<int>("SelTracks_HitPix",0);
  auto SelTracks_HitSCT = Monitored::Scalar<int>("SelTracks_HitSCT",0);
  auto SelTracks_HitSi = Monitored::Scalar<int>("SelTracks_HitSi",0);

  auto SelTracks_HolePix = Monitored::Scalar<int>("SelTracks_HolePix",0);
  auto SelTracks_HoleSCT = Monitored::Scalar<int>("SelTracks_HoleSCT",0);
  auto SelTracks_HoleSi = Monitored::Scalar<int>("SelTracks_HoleSi",0);
  
  int nTracks = 0;
  auto SelTracks_n_pass = Monitored::Scalar<int>("SelTracks_n_pass",0);

  bool passTrackSelection = true;
  bool passHitSelection = true;
  bool passIBLSiSelection = true;

  auto SelTracks_eta_all = Monitored::Scalar<float>("SelTracks_eta_all",0.0);
  auto SelTracks_phi_all = Monitored::Scalar<float>("SelTracks_phi_all",0.0);
  auto SelTracks_eta_pT = Monitored::Scalar<float>("SelTracks_eta_pT",0.0);
  auto SelTracks_phi_pT = Monitored::Scalar<float>("SelTracks_phi_pT",0.0);
  auto SelTracks_eta_d0 = Monitored::Scalar<float>("SelTracks_eta_d0",0.0);
  auto SelTracks_phi_d0 = Monitored::Scalar<float>("SelTracks_phi_d0",0.0);
  auto SelTracks_eta_z0sin = Monitored::Scalar<float>("SelTracks_eta_z0sin",0.0);
  auto SelTracks_phi_z0sin = Monitored::Scalar<float>("SelTracks_phi_z0sin",0.0);
  auto SelTracks_eta_fit = Monitored::Scalar<float>("SelTracks_eta_fit",0.0);
  auto SelTracks_phi_fit = Monitored::Scalar<float>("SelTracks_phi_fit",0.0);
  auto SelTracks_eta_IBL = Monitored::Scalar<float>("SelTracks_eta_IBL",0.0);
  auto SelTracks_phi_IBL = Monitored::Scalar<float>("SelTracks_phi_IBL",0.0);
  auto SelTracks_eta_Pix = Monitored::Scalar<float>("SelTracks_eta_Pix",0.0);
  auto SelTracks_phi_Pix = Monitored::Scalar<float>("SelTracks_phi_Pix",0.0);
  auto SelTracks_eta_SCT = Monitored::Scalar<float>("SelTracks_eta_SCT",0.0);
  auto SelTracks_phi_SCT = Monitored::Scalar<float>("SelTracks_phi_SCT",0.0);
  auto SelTracks_eta_Si = Monitored::Scalar<float>("SelTracks_eta_Si",0.0);
  auto SelTracks_phi_Si = Monitored::Scalar<float>("SelTracks_phi_Si",0.0);
  auto SelTracks_eta_pass = Monitored::Scalar<float>("SelTracks_eta_pass",0.0);
  auto SelTracks_phi_pass = Monitored::Scalar<float>("SelTracks_phi_pass",0.0);

  std::vector<ElementLink<xAOD::TrackParticleContainer>> assocTracks = bTaggingObject->auxdata< std::vector< ElementLink<xAOD::TrackParticleContainer > > >("BTagTrackToJetAssociator");

  for ( const ElementLink< xAOD::TrackParticleContainer >& jetTracks : assocTracks ) {
    if ( not jetTracks.isValid() ) continue;
    const xAOD::TrackParticle* jetTrackItr = *jetTracks;
    
    SelTracks_pT_all = jetTrackItr->pt() / Gaudi::Units::GeV;
    SelTracks_eta_all = jetTrackItr->eta();
    SelTracks_phi_all = jetTrackItr->phi();

    passTrackSelection = true;
    passHitSelection = true;

    // Tracks failing pT and eta cuts
    if(SelTracks_pT_all < m_TrackPtCut  || fabs(SelTracks_eta_all) > m_TrackEtaCut){
      passTrackSelection = false;
      SelTracks_eta_pT = jetTrackItr->eta();
      SelTracks_phi_pT = jetTrackItr->phi();
      fill(tool,SelTracks_eta_pT,SelTracks_phi_pT);
    }

    SelTracks_d0 = jetTrackItr->d0();
    
    // Tracks failing d0 cuts
    if(fabs(SelTracks_d0) > m_Trackd0Cut){
      passTrackSelection = false;
      SelTracks_eta_d0 = jetTrackItr->eta();
      SelTracks_phi_d0 = jetTrackItr->phi();
      fill(tool,SelTracks_eta_d0,SelTracks_phi_d0);
    }

    float PVZ = PV_Z;
    SelTracks_z0sin = (jetTrackItr->z0() + jetTrackItr->vz() - PVZ)*sin(jetTrackItr->theta());

    // Tracks failing z0sin cuts
    if(fabs(SelTracks_z0sin) > m_Trackz0sinCut){
      passTrackSelection = false;
      SelTracks_eta_z0sin = jetTrackItr->eta();
      SelTracks_phi_z0sin = jetTrackItr->phi();
      fill(tool,SelTracks_eta_z0sin,SelTracks_phi_z0sin);
    }

    if(jetTrackItr->numberDoF() > 0 && jetTrackItr->chiSquared() >= 0.) {
      SelTracks_chi2ndf = jetTrackItr->chiSquared()/jetTrackItr->numberDoF();
    }

    // Tracks failing fit cuts
    if(jetTrackItr->numberDoF() == 0 || SelTracks_chi2ndf > m_TrackChi2ndfCut){
      passTrackSelection = false;
      SelTracks_eta_fit = jetTrackItr->eta();
      SelTracks_phi_fit = jetTrackItr->phi();
      fill(tool,SelTracks_eta_fit,SelTracks_phi_fit);
    }

    uint8_t nHitIBL=0;
    jetTrackItr->summaryValue(nHitIBL, xAOD::numberOfInnermostPixelLayerHits);
    uint8_t nHitPix=0;
    jetTrackItr->summaryValue(nHitPix, xAOD::numberOfPixelHits);
    uint8_t nHolePix=0;
    jetTrackItr->summaryValue(nHolePix, xAOD::numberOfPixelHoles);
    uint8_t nDeadPix;
    jetTrackItr->summaryValue(nDeadPix, xAOD::numberOfPixelDeadSensors);
    nHitPix += std::max((int)nDeadPix, 0);
    uint8_t nHitSCT=0;
    jetTrackItr->summaryValue(nHitSCT, xAOD::numberOfSCTHits);
    uint8_t nHoleSCT=0;
    jetTrackItr->summaryValue(nHoleSCT, xAOD::numberOfSCTHoles);
    uint8_t nDeadSCT;
    jetTrackItr->summaryValue(nDeadSCT, xAOD::numberOfSCTDeadSensors);
    nHitSCT += std::max((int)nDeadSCT, 0);

    SelTracks_HitIBL = (int)nHitIBL;

    // Tracks failing IBL cuts
    if(SelTracks_HitIBL < m_TrackHitIBLCut){
      passHitSelection = false;
      SelTracks_eta_IBL = jetTrackItr->eta();
      SelTracks_phi_IBL = jetTrackItr->phi();
      fill(tool,SelTracks_eta_IBL,SelTracks_phi_IBL);
    }
    
    SelTracks_HitPix = (int)nHitPix;
    SelTracks_HolePix = (int)nHolePix;

    // Tracks failing Pixel cuts
    if(SelTracks_HitPix < m_TrackHitPixCut || SelTracks_HolePix >= m_TrackHolePixCut){
      passHitSelection = false;
      SelTracks_eta_Pix = jetTrackItr->eta();
      SelTracks_phi_Pix = jetTrackItr->phi();
      fill(tool,SelTracks_eta_Pix,SelTracks_phi_Pix);
    }

    SelTracks_HitSCT = (int)nHitSCT;
    SelTracks_HoleSCT = (int)nHoleSCT;

    // Tracks failing SCT cuts
    if(SelTracks_HitSCT < m_TrackHitSCTCut || SelTracks_HoleSCT >= m_TrackHoleSCTCut){
      passHitSelection = false;
      SelTracks_eta_SCT = jetTrackItr->eta();
      SelTracks_phi_SCT = jetTrackItr->phi();
      fill(tool,SelTracks_eta_SCT,SelTracks_phi_SCT);
    }

    SelTracks_HitSi = (int)nHitPix + (int)nHitSCT;
    SelTracks_HoleSi = (int)nHolePix + (int)nHoleSCT;

    // Tracks failing Si (Pix+SCT) cuts
    if(SelTracks_HitSi < m_TrackHitSiCut || SelTracks_HoleSi >= m_TrackHoleSiCut){
      passHitSelection = false;
      SelTracks_eta_Si = jetTrackItr->eta();
      SelTracks_phi_Si = jetTrackItr->phi();
      fill(tool,SelTracks_eta_Si,SelTracks_phi_Si);
    }

    if(passTrackSelection && passHitSelection){
      nTracks++;
      SelTracks_eta_pass = jetTrackItr->eta();
      SelTracks_phi_pass = jetTrackItr->phi();
      fill(tool,SelTracks_eta_pass,SelTracks_phi_pass);
    }

    fill(tool,SelTracks_pT_all,SelTracks_eta_all,SelTracks_phi_all,SelTracks_d0,SelTracks_z0sin,SelTracks_chi2ndf,SelTracks_HitIBL,SelTracks_HitPix,SelTracks_HolePix,SelTracks_HitSCT,SelTracks_HoleSCT,SelTracks_HitSi,SelTracks_HoleSi);

    if(passTrackSelection && (SelTracks_HitIBL < m_TrackHitIBLCut || SelTracks_HitSi < m_TrackHitSiCut) )
      passIBLSiSelection = false;
  }

  SelTracks_n_all = assocTracks.size();
  SelTracks_n_pass = nTracks;
  fill(tool,SelTracks_n_all,SelTracks_n_pass);
  
  // goodJet    = jet with at least one associated track passing track and hit selection
  // suspectJet = as goodJet but, at least one associated track does not pass IBL or Si (Pix+SCT) hit selection
  // badJet     = not goodJet (i.e. no associated track passing track or hit selection)

  if(nTracks < m_MinGoodTrackCut) return badJet;

  if(!passIBLSiSelection) return suspectJet;

  return goodJet;
}

