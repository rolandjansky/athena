/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonCaloTagHypo.h"

#include "xAODTracking/TrackParticleContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

TrigMuonCaloTagHypo::TrigMuonCaloTagHypo(const std::string& name, ISvcLocator* pSvcLocator)
  :
  HLT::HypoAlgo(name, pSvcLocator),
  m_acceptAll(false),
  m_storeGate(nullptr),
  m_ptThresholds(),
  m_etaMax(0.1),
  m_doLH(true),
  m_lhCut(0.95),
  m_doTight(true),
  m_maxMissedCells(1),
  m_cbthresh(0),
  m_ctTrackContName("MuonEFInfo_CaloTagTrackParticles")
{
  declareProperty("AcceptAll", m_acceptAll);
  declareProperty("PtThresholds", m_ptThresholds);
  declareProperty("EtaMax", m_etaMax);
  declareProperty("UseLH",        m_doLH);
  declareProperty("LHCut",        m_lhCut);
  declareProperty("TightCaloTag", m_doTight);
  declareProperty("MaxMissingCells", m_maxMissedCells);
  declareProperty("CombinedMuonThreshold",m_cbthresh);
  declareProperty("TrackContainerName", m_ctTrackContName,"MuonEFInfo_CaloTagTrackParticles");
}

TrigMuonCaloTagHypo::~TrigMuonCaloTagHypo()
{
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltInitialize()
{
  if (m_acceptAll) {
    ATH_MSG_INFO("Accepting all events");
  }

  std::string thrsh_str;
  for(const auto& t : m_ptThresholds)
    thrsh_str = std::to_string(t) + "GeV, ";

  ATH_MSG_INFO("Initialized TrigMuonCaloTagHypo with thresholds " << thrsh_str);
  ATH_MSG_DEBUG("AcceptAll: "<<m_acceptAll);
  ATH_MSG_DEBUG("EtaMax: "<<m_etaMax);
  ATH_MSG_DEBUG("UseLH: "<<m_doLH);
  ATH_MSG_DEBUG("LHCut: "<<m_lhCut);
  ATH_MSG_DEBUG("TightCaloTag: "<<m_doTight);
  ATH_MSG_DEBUG("MaxMissingCells: "<<m_maxMissedCells);
  ATH_MSG_DEBUG("TrackContainerName: "<<m_ctTrackContName);
  ATH_MSG_INFO("Completed intialization successfully");
  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  m_storeGate = store();

  ATH_MSG_DEBUG("In execute");
  
  if (m_acceptAll) {
    pass = true;
    ATH_MSG_DEBUG("Accepting event");
    return HLT::OK;
  }

  m_tmp_eta.clear();
  m_tmp_phi.clear();
  m_tmp_etaCT.clear();
  m_tmp_phiCT.clear();

  // Get muonContainer linked to the outputTE
  const xAOD::MuonContainer* muonContainer=0;
  if(getFeature(outputTE, muonContainer)!=HLT::OK) {
    ATH_MSG_DEBUG("no xAOD::MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  } else {
    if (!muonContainer) {
      ATH_MSG_DEBUG("null xAOD::MuonContainer Feature found");
      return HLT::MISSING_FEATURE;
    }
    ATH_MSG_DEBUG("xAOD::MuonContainer found with size " << muonContainer->size());
  } 

  //Check whether a particle passes the selection
  unsigned int nPassed = 0;
  //first look for combined muons
  // loop on the muons within the RoI
  for (unsigned int j=0; j< muonContainer->size(); ++j ) {
    const xAOD::Muon* muon = muonContainer->at(j);
    if (!muon) {
      ATH_MSG_DEBUG("No xAOD::Muon found.");
      continue;
    } else {
      ATH_MSG_DEBUG("muon type: "<<muon->muonType());
      if (muon->primaryTrackParticle()) { // was there a muon in this RoI ?

	const xAOD::TrackParticle *tr =  muon->trackParticle(xAOD::Muon::CombinedTrackParticle);  // get the TrackContainer
	if(muon->muonType()!=3){
	  if (!tr) {
	    ATH_MSG_DEBUG("No CombinedTrackParticle found.");
	  } else {
	    ATH_MSG_DEBUG("Retrieved CombinedTrackParticle with abs pt "<< (*tr).pt()/CLHEP::GeV << "GeV. Charge=" << tr->charge()<<" eta, phi: "<<(*tr).eta()<<" "<<(*tr).phi());

	    // check if muon passes threshold 
	    if (fabs(tr->pt()) > m_cbthresh){ 
	      m_tmp_eta.push_back(tr->eta());
	      m_tmp_phi.push_back(tr->phi());

	    }
	  }
	}
            
    
  
  
  


	//calo tag muons
	if(muon->muonType()==3){
	  const xAOD::TrackParticle* tp = muon->primaryTrackParticle();
	  ATH_MSG_DEBUG("Track Particle: pt, eta, phi, ptcut = " << tp->pt() << ", " << tp->eta() << ", " << tp->phi() << ", " << m_ptThresholds[nPassed]);

	  if (tp->pt() < m_ptThresholds[nPassed]) continue;
	  if (fabs(tp->eta()) > m_etaMax) continue;
    
	  if (m_doLH) {
	    auto lh      = tp->auxdata<double>("CaloTagLH");
	    ATH_MSG_DEBUG("Track Particle: lh, lhcut = " << ", LH: " << lh << ", cut: " << m_lhCut);
	    if (lh < m_lhCut) continue;
	  } 
	  else {
	    auto calotag = tp->auxdata<unsigned short>("CaloTag");
	    ATH_MSG_DEBUG("Track Particle: ct = " << calotag );
	    if (m_doTight){
	      if (calotag < (4 - m_maxMissedCells)*10) continue;
	    }else{
	      if (calotag < (4 - m_maxMissedCells)) continue;
	    }
	  }

	  m_tmp_etaCT.push_back(tp->eta());
	  m_tmp_phiCT.push_back(tp->phi());

	  ++nPassed; 
	}
      }
    }
  }
  
  //Check that calo tag and combined muons are not the same!
  float deta,dphi=10;
  float mindR=1.0;
  float deltaR=1.0;
  pass=false;
  for(unsigned int j=0; j<m_tmp_etaCT.size(); j++){
    mindR=1.0;
    for(unsigned int i=0; i<m_tmp_eta.size(); i++){
      deta = fabs(m_tmp_eta[i]-m_tmp_etaCT[j]);
      dphi = getdphi(m_tmp_phi[i],m_tmp_phiCT[j]);
      deltaR=deta*deta+dphi*dphi;

      if(deltaR<mindR){
	mindR=deltaR;
      }
    }
    if(mindR<0.001 && nPassed>0) nPassed=nPassed-1;
  }

  if(nPassed >= m_ptThresholds.size()) pass=true;
  ATH_MSG_DEBUG("Found muons: " << nPassed << ", mindR(calotag,cb) "<<mindR<<", passed = " << pass);

  return HLT::OK;
}

HLT::ErrorCode TrigMuonCaloTagHypo::hltFinalize()
{
  ATH_MSG_DEBUG("In finalize");

  return HLT::OK;
}

float TrigMuonCaloTagHypo::getdphi(float phi1, float phi2){
  float dphi = phi1-phi2;
  if(dphi > TMath::Pi()) dphi -= TMath::TwoPi();
  if(dphi < -1*TMath::Pi()) dphi += TMath::TwoPi();
  return fabs(dphi);
}
