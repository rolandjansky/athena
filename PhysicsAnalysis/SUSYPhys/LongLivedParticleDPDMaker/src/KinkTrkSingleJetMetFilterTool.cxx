/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/KinkTrkSingleJetMetFilterTool.h"
#include "FourMomUtils/P4Helpers.h"
#include <vector>

class sortJetContainer{
public:
  bool operator () (const xAOD::Jet *left, const xAOD::Jet *right) const{
    return (left->pt() > right->pt());
  }
};

/// Service Constructor
DerivationFramework::KinkTrkSingleJetMetFilterTool::KinkTrkSingleJetMetFilterTool( const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p ) : 
  AthAlgTool(t,n,p),
  m_ntot(0),
  m_npass(0),
  m_passAll(false),
  m_LeptonVeto(false),
  m_isolatedTrack(false),
  m_metHtCut(-1),
  m_metTerm("Final"),
  m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
  m_muonIDKey("Medium"),
  m_electronIDKey("LHTight"),
  m_metCut(-1),
  m_jetPtCuts(std::vector<float>()),
  m_jetEtaMax(3.2),
  m_jetNumCut(1),
  m_jetMetDphiMin(-1),
  m_jetMetPtMin(0.0),
  m_leptonPtCut(30.0),
  m_leptonEtaMax(2.5)
  {
    declareInterface<DerivationFramework::ISkimmingTool>(this);
    declareProperty("passAll", m_passAll); 
    declareProperty("LeptonVeto", m_LeptonVeto);
    declareProperty("IsolatedTrack", m_isolatedTrack);
    declareProperty("MetHtCut", m_metHtCut);
    declareProperty("MetTerm", m_metTerm);
    declareProperty("MuonIDKey", m_muonIDKey);
    declareProperty("ElectronIDKey", m_electronIDKey);
    declareProperty("MetCut", m_metCut);
    declareProperty("JetPtCuts", m_jetPtCuts);
    declareProperty("JetEtaMax", m_jetEtaMax);
    declareProperty("JetNumCut", m_jetNumCut);
    declareProperty("JetMetDphiMin", m_jetMetDphiMin);
    declareProperty("JetMetPtMin", m_jetMetPtMin);
    declareProperty("LeptonPtCut", m_leptonPtCut);
    declareProperty("LeptonEtaMax", m_leptonEtaMax);
  }
 

/// Destructor
DerivationFramework::KinkTrkSingleJetMetFilterTool::~KinkTrkSingleJetMetFilterTool() {}


// Athena initialize and finalize
StatusCode DerivationFramework::KinkTrkSingleJetMetFilterTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  ATH_CHECK(m_jetSGKey.initialize());
  ATH_CHECK(m_metSGKey.initialize());
  ATH_CHECK(m_muonSGKey.initialize());
  ATH_CHECK(m_electronSGKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::KinkTrkSingleJetMetFilterTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_ntot << " events, " << m_npass << " events passed filter ");
  ATH_MSG_INFO("Efficiency = "<< 100.0*(float)m_npass/(float)m_ntot <<" % ");
  return StatusCode::SUCCESS;
}


// The filter itself
bool DerivationFramework::KinkTrkSingleJetMetFilterTool::eventPassesFilter() const
{
  ++m_ntot;
  bool acceptEvent(false);

  // If passAll is selected, accept all events
  if(m_passAll){
      ATH_MSG_DEBUG("Event passed because passAll is true.");
      return acceptEvent; 
  }


  // Retrieve MET container 
  const xAOD::MissingET* met(0);
  SG::ReadHandle<xAOD::MissingETContainer> metContainer(m_metSGKey);
  if( !metContainer.isValid() ) {
    msg(MSG::WARNING) << "No MET container found, will skip this event" << endmsg;
    return false;
  }
  met = (*metContainer)[m_metTerm];
  if (!met) {
    ATH_MSG_ERROR("Cannot retrieve MissingET term " << m_metTerm << " in " << m_metSGKey);
    return acceptEvent; 
  }
  //ATH_MSG_DEBUG("MET and phi are " << met->met() << " " << met->phi());
  if (met->met() < m_metCut) return acceptEvent; 

  // Loop over jets
  SG::ReadHandle<xAOD::JetContainer> jetContainer(m_jetSGKey);
  if( !jetContainer.isValid() ) {
    msg(MSG::WARNING) << "No jet container found, will skip this event" << endmsg;
    return false;
  }
  ATH_MSG_DEBUG("retrieved jet collection size "<< jetContainer->size());

  std::vector<const xAOD::Jet *> sortedJetContainer;
  for(xAOD::JetContainer::const_iterator jet = jetContainer->begin(); jet!= jetContainer->end(); jet++){
    sortedJetContainer.push_back(*jet);
  }

  // leading jet pt > 120 GeV
  // Good Jets pt > 50 GeV
  // Pt cut size = 2
  std::sort(sortedJetContainer.begin(), sortedJetContainer.end(), sortJetContainer());
  float valHt = 0.0;
  int nJetRequired = 0;
  std::vector<const xAOD::Jet*> goodJets;
  int nJet = sortedJetContainer.size();
  //for (const auto jet: *jetContainer) {
  for(int i=0;i<nJet;i++){
    // search for good jets which are used for dphi calculation
    if (sortedJetContainer.at(i)->pt() > m_jetMetPtMin && std::abs(sortedJetContainer.at(i)->eta()) < m_jetEtaMax) {
      goodJets.push_back(sortedJetContainer.at(i));
      valHt += sortedJetContainer.at(i)->pt();
    }
    
    // search for required jets
    if(nJetRequired < m_jetNumCut){
      if (sortedJetContainer.at(i)->pt() > m_jetPtCuts[nJetRequired] && std::abs(sortedJetContainer.at(i)->eta()) < m_jetEtaMax){
	nJetRequired++;
      }
    }
  }// for jet
  
  if(nJetRequired < m_jetNumCut) return acceptEvent;

  if((met->met()/1000.0)/sqrt(valHt/1000.0) < m_metHtCut) return acceptEvent;
  
  // dPhi > 1.0 cut
  float minDphi = 9999;
  for (unsigned int i=0; i<goodJets.size(); i++) {
    if (i >= m_jetPtCuts.size()) break;
    if (std::abs(P4Helpers::deltaPhi(goodJets.at(i)->phi(), met->phi())) < minDphi) {
      minDphi = std::abs(P4Helpers::deltaPhi(goodJets.at(i)->phi(), met->phi()));
    }
  }
  if (minDphi < m_jetMetDphiMin) return acceptEvent; 
  
  // Lepton VETO
  if (m_LeptonVeto) {
    // Retrieve muon container	
    SG::ReadHandle<xAOD::MuonContainer> muons(m_muonSGKey);
    if( !muons.isValid() ) {
      msg(MSG::WARNING) << "No muons container found, will skip this event" << endmsg;
      return false;
    }
    int qflag(0);
    if (m_muonIDKey == "VeryLoose") {
      qflag = xAOD::Muon::VeryLoose;
    } else if (m_muonIDKey == "Loose") {
      qflag = xAOD::Muon::Loose;
    } else if (m_muonIDKey == "Medium") {
      qflag = xAOD::Muon::Medium;
    } else if (m_muonIDKey == "Tight") {
      qflag = xAOD::Muon::Tight;
    } else {
      ATH_MSG_WARNING("Cannot find the muon quality flag " << m_muonIDKey << ". Use Medium instead.");
      qflag = xAOD::Muon::Medium;
    }

    for (auto muon: *muons) {
      bool passID(false);
      if (m_muonSelectionTool->getQuality(*muon) <= qflag) {
        passID = true;
      }
      if (muon->pt() > m_leptonPtCut && std::abs(muon->eta()) < m_leptonEtaMax && passID) {
        return acceptEvent; 
      }
    }

    // Retrieve electron container	
    SG::ReadHandle<xAOD::ElectronContainer> electrons(m_electronSGKey);
    if( !electrons.isValid() ) {
      msg(MSG::WARNING) << "No electron container found, will skip this event" << endmsg;
      return false;
    }
    for (auto ele: *electrons) {
      bool passID(false);
      if (!ele->passSelection(passID, m_electronIDKey)) {
        ATH_MSG_WARNING("Cannot find the electron quality flag " << m_muonIDKey);
      }
      if (ele->pt() > m_leptonPtCut && std::abs(ele->eta()) < m_leptonEtaMax && passID) {
        return acceptEvent; 
      }
    }
  }

  // at least 1 isolated pixel tracklet OR standard track
  if(m_isolatedTrack){

    // Find IsolatedTracklet
    SG::ReadHandle<xAOD::TrackParticleContainer> pixelTrackletContainer("InDetDisappearingTrackParticles");
    if( !pixelTrackletContainer.isValid() ) {
      msg(MSG::WARNING) << "No pixel tracklet container found, will skip this event" << endmsg;
      return false;
    }
 
    SG::ReadHandle<xAOD::VertexContainer> vertices("PrimaryVertices");
    if( !vertices.isValid() ) {
      msg(MSG::WARNING) << "No primary vertices container found, will skip this event" << endmsg;
      return false;
    }
    const xAOD::Vertex* pv = 0;
    for( const auto& v: *vertices ){
      if( v->vertexType() == xAOD::VxType::PriVtx ){
        pv = v;
        break;
      }
    }
    if( !pv ){
      ATH_MSG_WARNING("Cannot find a PV in the event; reject it!");
      return false;
    }
    
    bool passIsolatedTracklet = false;
    for(const auto& Tracklet : *pixelTrackletContainer){
      passIsolatedTracklet = true;
      for(unsigned int i=0;i<goodJets.size();i++){
	double deltaPhi = (std::abs(Tracklet->phi() - goodJets.at(i)->phi()) > M_PI) ? 2.0*M_PI-std::abs(Tracklet->phi()-goodJets.at(i)->phi()) : std::abs(Tracklet->phi()-goodJets.at(i)->phi());
	double deltaEta = std::abs(Tracklet->eta() - goodJets.at(i)->eta());
	double deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);

	//if(P4Helpers::deltaR(Tracklet->p4(), goodJets.at(i)->eta(), goodJets.at(i)->phi()) < 0.3){
	if(deltaR < 0.3){
	  passIsolatedTracklet = false;
	}
      }// for goodJets
      
      if(passIsolatedTracklet==false)
	continue;

      if( Tracklet->pt() < 20000.0 ){
        passIsolatedTracklet = false;
        continue;
      } 
      
      if(TMath::Abs(Tracklet->eta()) < 0.1 || TMath::Abs(Tracklet->eta()) > 1.9){
	passIsolatedTracklet = false;
	continue;
      }
      
      if(TMath::Prob(Tracklet->chiSquared(), Tracklet->numberDoF()) < 0.1){
	passIsolatedTracklet = false;
	continue;
      }

      if(Tracklet->auxdata<UChar_t>("numberOfContribPixelLayers")<3){
	passIsolatedTracklet = false;
	continue;
      }
      
      if(Tracklet->auxdata<UChar_t>("numberOfPixelSpoiltHits")>0){
	passIsolatedTracklet = false;
	continue;
      }

      double z0SinTheta = (Tracklet->z0() + Tracklet->vz() - pv->z() ) * TMath::Sin(Tracklet->p4().Theta());
      if( std::abs(z0SinTheta) > 5.0 ){
        passIsolatedTracklet = false;
        continue;
      }
      
      if(passIsolatedTracklet) break;

    }// for Tracklet
    
    if(passIsolatedTracklet==false){
      return acceptEvent; // std track OFF

      bool passIsolatedStdTrack = false;
      SG::ReadHandle<xAOD::TrackParticleContainer> standardTrackContainer("InDetTrackParticles");
      if( !standardTrackContainer.isValid() ) {
        msg(MSG::WARNING) << "No Standard Track container found, will skip this event" << endmsg;
        return false;
      }
      
      
      for(const auto& StdTrack : *standardTrackContainer){
	if(StdTrack->pt()/1000.0 < 20.0)
	  continue;

	passIsolatedStdTrack = true;

	for(unsigned int i=0;i<goodJets.size();i++){
	  double deltaPhi = (std::abs(StdTrack->phi() - goodJets.at(i)->phi()) > M_PI) ? 2.0*M_PI-std::abs(StdTrack->phi()-goodJets.at(i)->phi()) : std::abs(StdTrack->phi()-goodJets.at(i)->phi());
	  double deltaEta = std::abs(StdTrack->eta() - goodJets.at(i)->eta());
	  double deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	  //if(P4Helpers::deltaR(StdTrack->p4(), goodJets.at(i)->eta(), goodJets.at(i)->phi()) < 0.3){
	  if(deltaR < 0.3){
	    passIsolatedStdTrack = false;
	  }
	}// for goodJets
	
	if(passIsolatedStdTrack){
	  double ptcone20 = 0.0;
	  for(auto Track : *standardTrackContainer){
	    if(Track->pt()/1000.0 < 0.4)
	      continue;

	    double deltaPhi = (std::abs(StdTrack->phi() - Track->phi()) > M_PI) ? 2.0*M_PI-std::abs(StdTrack->phi()-Track->phi()) : std::abs(StdTrack->phi()-Track->phi());
	    double deltaEta = std::abs(StdTrack->eta() - Track->eta());
	    double deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	    //if(P4Helpers::deltaR(StdTrack->p4(), Track->p4()) < 0.2)
	    if(deltaR < 0.2)
	      ptcone20 += Track->pt();
	  }// for Track
	  
	  ptcone20 = (ptcone20 - StdTrack->pt())/StdTrack->pt();
	  if(ptcone20 < 0.04){
	    break;
	  }else{
	    passIsolatedStdTrack = false;
	  }
	}
      }// for StdTrack

      if(passIsolatedStdTrack==false)
	return acceptEvent;
    }// if there is no isolated pixel tracklet, find isolated standard track
    
  }// m_isolatedTracklet
  
  acceptEvent = true;
  ++m_npass;
  return acceptEvent; 

}
