 /*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/////////////////////////////////////////////////////////////////
// VHLowTrackJetFilterTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LongLivedParticleDPDMaker/VHLowTrackJetFilterTool.h"
#include <vector>
#include <string>
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "TLorentzVector.h"
#include "xAODTracking/VertexContainer.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include <math.h>


// Constructor
DerivationFramework::VHLowTrackJetFilterTool::VHLowTrackJetFilterTool( const std::string& t,
                                                                      const std::string& n,
                                                                      const IInterface* p ) :
AthAlgTool(t,n,p),
m_nEventsTotal(0),
m_nEventsPass(0),
m_nEventsPassJet(0),
m_nEventsPassElectron(0),
m_nEventsPassMuon(0),
m_nJetsPassAlphaMax(0),
m_nJetsPassCHF(0),
m_debug(true),
m_jetSGKey("AntiKt4EMTopoJets"),
m_jetPtCut(0),
m_jetEtaCut(2.1),
m_TrackMinPt(400.0),
m_TrackZ0Max(0.3),
m_TrackD0Max(0.5),
m_AlphaMaxCut(0.03),
m_CHFCut(0.3),
m_nJetsReq(0),
m_electronSGKey("Electrons"),
m_electronIDKey("Medium"),
m_electronPtCut(0),
m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
m_muonSGKey("Muons"),
m_muonIDKey("Medium"),
m_muonPtCut(0)


{
  declareInterface<DerivationFramework::ISkimmingTool>(this);
  declareProperty("Debug", m_debug);
  
  declareProperty("JetContainerKey", m_jetSGKey);
  declareProperty("JetPtCut", m_jetPtCut);
  declareProperty("JetEtaCut", m_jetEtaCut);
  declareProperty("TrackMinPt", m_TrackMinPt);
  declareProperty("TrackZ0Max", m_TrackZ0Max);
  declareProperty("TrackD0Max", m_TrackD0Max);
  declareProperty("JetAlphaMaxCut", m_AlphaMaxCut);
  declareProperty("JetCHFCut", m_CHFCut);
  declareProperty("NJetsRequired", m_nJetsReq);
  
  declareProperty("ElectronContainerKey", m_electronSGKey);
  declareProperty("ElectronIDKey", m_electronIDKey);
  declareProperty("ElectronPtCut", m_electronPtCut);
  
  declareProperty("MuonContainerKey", m_muonSGKey);
  declareProperty("MuonIDKey", m_muonIDKey);
  declareProperty("MuonPtCut", m_muonPtCut);
  
}

// Destructor
DerivationFramework::VHLowTrackJetFilterTool::~VHLowTrackJetFilterTool() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::VHLowTrackJetFilterTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  
  
  return StatusCode::SUCCESS;
  
}
StatusCode DerivationFramework::VHLowTrackJetFilterTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed "<< m_nEventsTotal <<" events, "<< m_nEventsPass<<" events passed filter ");
  ATH_MSG_INFO("Percent of events passed Jet filter "<< 100.0*(float)m_nEventsPassJet/(float)m_nEventsTotal <<" % ");
  ATH_MSG_INFO("Percent of events passing CHF "<< 100.0*(float)m_nJetsPassCHF/(float)m_nEventsTotal <<" % ");
  ATH_MSG_INFO("Percent of events passing alphaMax "<< 100.0*(float)m_nJetsPassAlphaMax/(float)m_nEventsTotal <<" % ");
  ATH_MSG_INFO("Percent of events passed Electron filter "<< 100.0*(float)m_nEventsPassElectron/(float)m_nEventsTotal <<" % ");
  ATH_MSG_INFO("Percent events passed Muon filter "<< 100.0*(float)m_nEventsPassMuon/(float)m_nEventsTotal <<" % ");
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------------------------

// The filter itself
bool DerivationFramework::VHLowTrackJetFilterTool::eventPassesFilter() const
{
  typedef std::vector<const xAOD::TrackParticle*> Particles;
  typedef ElementLink<xAOD::TrackParticleContainer> TrackLink;
  typedef std::vector<TrackLink> TrackLinks;
  
  bool passesEl=false, passesMu=false, passesJet=false;
  m_nEventsTotal++;
  
  xAOD::EventInfo* eventInfo = nullptr;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve event info" );
  }
  
  //Vertex Container
  const xAOD::VertexContainer* vertices(0);
  sc = evtStore()->retrieve(vertices, "PrimaryVertices");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("No vertex collection with name PrimaryVertices found in StoreGate!");
    return false;
  }
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  //electron portion
  const xAOD::ElectronContainer* electrons(0);
  sc = evtStore()->retrieve(electrons,m_electronSGKey);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("No electron collection with name " << m_electronSGKey << " found in StoreGate!");
    return false;
  }
  
  // loop over the electrons in the container
  for (auto electron : *electrons){
    
    if(electron->pt()<m_electronPtCut) continue;
    if( (fabs(electron->caloCluster()->etaBE(2))>1.37 && fabs(electron->caloCluster()->etaBE(2)<1.52))
       || fabs(electron->caloCluster()->etaBE(2))>2.47 ) continue;
    if(electron->isolation(xAOD::Iso::topoetcone20)/electron->pt()>0.2) continue;
    
    bool passID=false;
    if (!electron->passSelection(passID, m_electronIDKey)) {
      ATH_MSG_WARNING("Cannot find the electron quality flag " << m_electronIDKey);
      continue;
    }
    if(!passID) continue;
    
    for (auto vertex: *vertices){
      // Select good primary vertex
      if (vertex->vertexType() == xAOD::VxType::PriVtx) {
        const xAOD::TrackParticle* tp = electron->trackParticle() ; //your input track particle from the electron
        float sigd0 =fabs( xAOD::TrackingHelpers::d0significance( tp, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(), eventInfo->beamPosSigmaXY() ));
        double delta_z0 = tp->z0() + tp->vz() - vertex->z();
        double theta = tp->theta();
        float z0sintheta = fabs(delta_z0 * sin(theta));
        
        if (sigd0>5) continue;
        if (z0sintheta>0.5) continue;
        
        passesEl=true;
        break;
      }
    }
    if (passesEl){
      m_nEventsPassElectron++;
      break;
    }
  }
  
    
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  //muon portion
  int qflag=0;
  if (m_muonIDKey == "VeryLoose") {
    qflag = xAOD::Muon::VeryLoose;
  } else if (m_muonIDKey == "Loose") {
    qflag = xAOD::Muon::Loose;
  } else if (m_muonIDKey == "Medium") {
    qflag = xAOD::Muon::Medium;
  } else if (m_muonIDKey == "Tight") {
    qflag = xAOD::Muon::Tight;
  } else {
    ATH_MSG_FATAL("Cannot find the muon quality flag " << m_muonIDKey << ".");
    return false;
  }
  
  const xAOD::MuonContainer* muons(0);
  sc = evtStore()->retrieve(muons,m_muonSGKey);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("No muon collection with name " << m_muonSGKey << " found in StoreGate!");
    return false;
  }
  
  for(auto muon : *muons){
    if (muon->pt()<m_muonPtCut) continue;
    if (fabs(muon->eta())>2.5) continue;
    if (!(m_muonSelectionTool->getQuality(*muon) <= qflag)) continue;
    if (muon->isolation(xAOD::Iso::topoetcone20)/muon->pt()>0.3) continue;
    
    for (auto vertex : *vertices) {	// Select good primary vertex
      if (vertex->vertexType() == xAOD::VxType::PriVtx) {
        const xAOD::TrackParticle* tp = muon->primaryTrackParticle() ; //your input track particle from the electron
        
        double d0sig = xAOD::TrackingHelpers::d0significance( tp, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(), eventInfo->beamPosSigmaXY() );
        if (fabs(d0sig)>3) continue;
          
	float delta_z0 = tp->z0() + tp->vz() - vertex->z();
        float theta = tp->theta();
        double z0sintheta = delta_z0 * sin(theta);
        if (fabs(z0sintheta)>0.5) continue;
        
        passesMu = true;
        break;
      }
    }
    if (passesMu) {
      m_nEventsPassMuon++;
      break;
    }
  }
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  //Jet portion
  ATH_MSG_INFO("Jet");
  
  int nJetsPassed=0;
  const xAOD::JetContainer* jets(0);
  sc=evtStore()->retrieve(jets,m_jetSGKey);
  if( sc.isFailure() || !jets ) {
    msg(MSG::WARNING) << "No Jet container found, will skip this event" << endmsg;
    return StatusCode::FAILURE;
  }
  
  std::vector<const xAOD::Jet*> goodJets;
  for (auto jet : *jets) {
    TLorentzVector VJet = TLorentzVector(0.0,0.0,0.0,0.0);
    VJet.SetPtEtaPhiE(jet->pt(), jet->eta(), jet->phi(), jet->e());
    
    float minDeltaR = 100;
    for (auto electron : *electrons ){
      if (!electron->passSelection("Loose")) continue;
      TLorentzVector VElec=electron->p4();
      float deltaR = VJet.DeltaR(VElec);
      if (deltaR<minDeltaR) minDeltaR=deltaR;
      std::cout<<"Delta R: "<<minDeltaR<<std::endl;
    }
    
    std::cout<<"LooseDR="<<minDeltaR<<std::endl;
    if (minDeltaR<0.2) continue;
    
    goodJets.push_back(jet);
    std::cout<<"jet passed LooseDR"<<std::endl;
  }
  
  
  int jetNo=0;
  for (auto jet : goodJets){
    jetNo++;
    if (jetNo>=3) break;  //Only consider two leading jets
    
    if (jet->pt() < m_jetPtCut) continue;
    if (fabs(jet->eta()) > m_jetEtaCut) continue;
    
    TLorentzVector CHFNum = TLorentzVector(0.0,0.0,0.0,0.0);
    const xAOD::BTagging *bjet(nullptr);
    bjet = jet->btagging();
    TrackLinks assocTracks = bjet->auxdata<TrackLinks>("BTagTrackToJetAssociator");
    
    std::vector<const xAOD::TrackParticle*> goodTracks;
    for (auto track : assocTracks) {
      if (!track.isValid()) continue;
      goodTracks.push_back(*track);
    }
    
    float alpha_max=-999;
    for (auto vertex : *vertices) {
      TLorentzVector alphaDen = TLorentzVector(0.0,0.0,0.0,0.0);
      TLorentzVector alphaNum = TLorentzVector(0.0,0.0,0.0,0.0);
      float alpha;
      
      for(auto track : goodTracks) {
        if (track->pt() < m_TrackMinPt) continue;
        
        TLorentzVector VTrack = TLorentzVector(0.0,0.0,0.0,0.0);
        VTrack.SetPtEtaPhiE(track->pt(),track->eta(), track->phi(), track->e());
        alphaDen=alphaDen+VTrack;
        if (track->d0() > m_TrackD0Max) continue;
        
        float z0 = track->z0() + track->vz() - vertex->z();
        float theta = track->theta();
        if (fabs(z0*sin(theta)) > m_TrackZ0Max) continue;
        
        alphaNum=alphaNum+VTrack;
      }
      if (alphaDen.Pt()==0) alpha=-999;
      else alpha = alphaNum.Pt()/alphaDen.Pt();
      
      if (alpha > alpha_max) alpha_max=alpha;
    }
    
    CHFNum = TLorentzVector(0.0,0.0,0.0,0.0);
    for(auto track : goodTracks) {
      if (track->pt() < m_TrackMinPt) continue;
      if (track->d0() > m_TrackD0Max) continue;
      
      TLorentzVector VTrack = TLorentzVector(0.0,0.0,0.0,0.0);
      VTrack.SetPtEtaPhiE(track->pt(),track->eta(), track->phi(), track->e());
      CHFNum=CHFNum+VTrack;
    }
    float chf = CHFNum.Pt()/jet->pt();
    
    if (alpha_max < m_AlphaMaxCut) m_nJetsPassAlphaMax++;
    if (chf < m_CHFCut) m_nJetsPassCHF++;
    
    if (chf > m_CHFCut && alpha_max > m_AlphaMaxCut) continue;
    nJetsPassed++;
  }
  
  if (nJetsPassed >= m_nJetsReq){
    passesJet=true;
    m_nEventsPassJet++;
  }
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  if (passesJet && (passesEl || passesMu)){
    m_nEventsPass++;
    return true;
  }
  else return false;
}

