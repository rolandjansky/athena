/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigLeptonJetMatchAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigLeptonJetMatchAllTE.h"

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "JetEvent/Jet.h"

#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigParticle/TrigEFBjet.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetMatchAllTE::TrigLeptonJetMatchAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_cutCounter(0),
  m_deltaEtaPass(0),
  m_deltaPhiPass(0)
{
  declareProperty("WorkingMode", m_workingMode = 1);
  declareProperty("DeltaRCut",   m_deltaRCut   = 0.4);
  declareProperty("DeltaZCut",   m_deltaZCut   = 2);

  declareMonitoredVariable("CutCounter",   m_cutCounter);
  declareMonitoredVariable("DeltaEtaPass", m_deltaEtaPass);
  declareMonitoredVariable("DeltaPhiPass", m_deltaPhiPass);
  declareMonitoredVariable("MuonEFEta",    m_muonEFEta);
  declareMonitoredVariable("MuonEFPhi",    m_muonEFPhi);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetMatchAllTE::~TrigLeptonJetMatchAllTE() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "Initializing TrigLeptonJetMatchAllTE" << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " WorkingMode = " << m_workingMode  << endreq; 
    msg() << MSG::DEBUG << " DeltaRCut = "   << m_deltaRCut << endreq; 
    msg() << MSG::DEBUG << " DeltaZCut = "   << m_deltaZCut << endreq; 
  }
  
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigLeptonJetMatchAllTE::phiCorr(float phi) {

  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigLeptonJetMatchAllTE" << endreq;

  bool pass = false;

  beforeExecMonitors().ignore();

  m_cutCounter   = -1;
  m_deltaEtaPass = -9; m_muonEFEta = -9;
  m_deltaPhiPass = -9; m_muonEFPhi = -9;

  if (inputTE.size() < 2) {
    msg() << MSG::ERROR << "Number of input TEs is " <<  inputTE.size() << " and not 2. Configuration problem." << endreq;  
    return HLT::MISSING_FEATURE;
  }

  if(inputTE[0].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No muon TE found" << endreq;
    return HLT::OK;
  }

  if(inputTE[1].size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "No jet TE found" << endreq;
    return HLT::OK;
  }

  //* Retrieve EF muons *//
  const TrigMuonEFInfoContainer* m_muons(0);
  HLT::ErrorCode statusMuons = getFeature(inputTE[0].front(), m_muons);
  if (statusMuons != HLT::OK) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to retrieve muons" << endreq;
    return HLT::NAV_ERROR;
  } else if (m_muons==0) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing muon feature." << endreq;
    return HLT::MISSING_FEATURE;
  } else if (m_muons->size()==0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Muon vector empty." << endreq;
    return HLT::OK;
  } else {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Retrieved " << m_muons->size() << " muons." << endreq;
  } 

  //* Retrieve EF jets *//
  const TrigEFBjetContainer* m_jet;
  std::vector<const TrigEFBjet*> m_jets;

  for (unsigned int i=0; i<inputTE[1].size(); i++) {

    m_jet=0;

    HLT::ErrorCode statusJet = getFeature(inputTE[1][i], m_jet);

    if (statusJet != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Failed to retrieve jet" << endreq;
      return HLT::NAV_ERROR;
    } else if (m_jet==0) {
      if (msgLvl() <= MSG::WARNING)
	msg() << MSG::WARNING << "Missing jet feature." << endreq;
      return HLT::MISSING_FEATURE;
    } else if (m_jet->size()!=1) {
      if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "More than one jet per TE." << endreq;
    }

    m_jets.push_back((m_jet)->front());
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Retrieved " << m_jets.size() << " jets." << endreq;

  HLT::TEVec allTEs;
  
  if ((inputTE.size()>0) && (inputTE[0].size()>0)) 
    allTEs.push_back(inputTE[0][0]);

  if ((inputTE.size()>0) && (inputTE[1].size()>0))
    for (unsigned int i=0; i<inputTE[1].size(); i++)
      allTEs.push_back(inputTE[1][i]);

  if(m_jets.size()==0 ) {
    m_cutCounter=0;
    if( m_workingMode!=2 ) 
      return HLT::OK;
  }

  float muonEta=0, muonPhi=0, muonZ=0;
  float jetEta=0,  jetPhi=0, jetZ=0;
  float m_deltaEta=0, m_deltaPhi=0, m_deltaZ=0;

  TrigMuonEFInfoContainer::const_iterator Muon     = m_muons->begin();
  TrigMuonEFInfoContainer::const_iterator lastMuon = m_muons->end();

  std::vector<const TrigEFBjet*>::const_iterator Jet     = m_jets.begin();
  std::vector<const TrigEFBjet*>::const_iterator lastJet = m_jets.end();
  
  for (unsigned int j=0 ; Muon != lastMuon; Muon++, j++) {
    
    TrigMuonEFInfo* muonInfo = (*Muon);
    
    if (!muonInfo->hasTrack()) continue;
    
    if (m_workingMode==2) pass = true;
    
    TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
    TrigMuonEFInfoTrackContainer::const_iterator Track = tc->begin();
    TrigMuonEFInfoTrackContainer::const_iterator lastTrack = tc->end();
    
    for ( int k=0; Track!=lastTrack; Track++,k++) {
      
      TrigMuonEFInfoTrack* muonInfoTrack = (*Track);
      
      if (muonInfoTrack->MuonType()!=1) continue;
      
      const TrigMuonEFTrack* muonEFTrack = 0;
      
      if      ((*Track)->hasCombinedTrack())     { muonEFTrack = (*Track)->CombinedTrack(); }
      else if ((*Track)->hasExtrapolatedTrack()) { muonEFTrack = (*Track)->ExtrapolatedTrack(); }
      else if ((*Track)->hasSpectrometerTrack()) { muonEFTrack = (*Track)->SpectrometerTrack(); }
      
      if (!muonEFTrack) continue;
      
      muonEta = muonEFTrack->eta();
      muonPhi = muonEFTrack->phi();
      muonZ   = muonEFTrack->z0();
      
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Muon "<< j+1 << "; eta "<< muonEta << "; phi " << muonPhi << "; z " << muonZ << endreq;
      
      for(unsigned int i=0; Jet!=lastJet; Jet++,i++) {
	
	jetEta = (*Jet)->eta();
	jetPhi = (*Jet)->phi();
	jetZ   = (*Jet)->prmVtx();
	
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i+1 << "; eta "<< jetEta << "; phi " << jetPhi << "; z " << jetZ << endreq;
	  
	m_deltaEta = muonEta - jetEta;
	m_deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
	m_deltaZ   = fabs(muonZ-jetZ);
	
	double dR = sqrt(m_deltaEta*m_deltaEta + m_deltaPhi*m_deltaPhi);

	if (msgLvl() <= MSG::DEBUG) 
	  msg() << MSG::DEBUG << "deltaR = "<< dR << "; deltaZ = " << m_deltaZ <<  endreq; 

        switch (m_workingMode) {

        case 1:
          if (dR < m_deltaRCut && m_deltaZ <= m_deltaZCut) {
	    m_deltaEtaPass = m_deltaEta; m_deltaPhiPass = m_deltaPhi; 
	    m_muonEFEta = muonEta; m_muonEFPhi = muonPhi;
	    pass = true;
	    break;
	  }
          break;
	  
        case 2:
          if (dR < m_deltaRCut) {
            pass = false;
	    break;
	  }
          break;
        }
      }

      if (pass) {
	
	if(msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Accepting the event" << endreq;
	
	m_cutCounter=1;
        
	break;
        
      } else {
	
	if(msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Rejecting the event" << endreq;
	
	m_cutCounter=0;
         
      }
    }
  }

  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
  outputTE->setActiveState(false); 

  if (pass==true){
    outputTE->setActiveState(true);
  }
 
  afterExecMonitors().ignore();

  return HLT::OK;  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetMatchAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigLeptonJetMatchAllTE" << endreq;

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigLeptonJetMatchAllTE::reset() {

  if ( !AllTEAlgo::reset() ) return false;
  AllTEAlgo::reset();

  return true;
}
