/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     TrigLeptonJetFexAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "TrigBjetHypo/TrigLeptonJetFexAllTE.h"

#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"



//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetFexAllTE::TrigLeptonJetFexAllTE(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator),
  m_cutCounter(0),
  m_deltaEtaPass(0),
  m_deltaPhiPass(0)
{
  declareProperty("WorkingMode", m_workingMode = 1);
  declareProperty("DeltaRCut",   m_deltaRCut   = 0.4);
  declareProperty("EtThreshold", m_etThreshold = 10*CLHEP::GeV);

  declareMonitoredVariable("CutCounter",   m_cutCounter);
  declareMonitoredVariable("DeltaEtaPass", m_deltaEtaPass);
  declareMonitoredVariable("DeltaPhiPass", m_deltaPhiPass);
  declareMonitoredVariable("MuonEFEta",    m_muonEFEta);
  declareMonitoredVariable("MuonEFPhi",    m_muonEFPhi);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigLeptonJetFexAllTE::~TrigLeptonJetFexAllTE() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetFexAllTE::hltInitialize() {

  if (msgLvl() <= MSG::INFO)
    msg() << MSG::INFO << "TrigLeptonJetFexAllTE initialization" << endreq;

  
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigLeptonJetFexAllTE::phiCorr(float phi) {

  if (phi < -M_PI) phi += 2*M_PI;
  if (phi >  M_PI) phi -= 2*M_PI;

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigLeptonJetFexAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Executing TrigLeptonJetFexAllTE" << endreq;

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
    msg() << MSG::DEBUG << "No muon TE found" << endreq;
    return HLT::OK;
  }

  if(inputTE[1].size() == 0) {
    msg() << MSG::DEBUG << "No jet TE found" << endreq;
    return HLT::OK;
  }

  HLT::TEVec allTEs;

  //* Retrieve EF muons *//
  const TrigMuonEFInfoContainer* muons;
  HLT::ErrorCode statusMuons = getFeature(inputTE[0].front(), muons);

  //* Retrieve EF jets *//
  const JetCollection* jets_EF(0);
  HLT::ErrorCode statusJets = getFeature(inputTE[1].front(), jets_EF);

  if (statusMuons != HLT::OK || statusJets  != HLT::OK) {

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Failed to retrieve features" << endreq;

    return HLT::NAV_ERROR;
  } 

  if(muons==0 || jets_EF==0) {

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing feature." << endreq;

    return HLT::MISSING_FEATURE;

  } else if (muons->size()!=1) {

    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Wrong feature." << endreq;

    return HLT::MISSING_FEATURE;

  }

  if ((inputTE.size()>0) && (inputTE[0].size()>0)) allTEs.push_back(inputTE[0][0]);
  if ((inputTE.size()>0) && (inputTE[1].size()>0)) allTEs.push_back(inputTE[1][0]);

  float muonEta=0, muonPhi=0;
  float jetEta=0,  jetPhi=0;
  float deltaEta=0, deltaPhi=0;

  TrigMuonEFInfoContainer::const_iterator Muon = muons->begin();
  TrigMuonEFInfoContainer::const_iterator lastMuon = muons->end();

  std::vector<const Jet*> jets(jets_EF->begin(), jets_EF->end());

  if(jets.size()==0 ){

    if (msgLvl() <= MSG::DEBUG)
      msg()<< MSG::DEBUG << " Size of JetCollection is 0" << endreq;

    m_cutCounter=0;

    if( m_workingMode!=2 ) return HLT::OK;
  }

  std::sort (jets.begin(), jets.end(), P4Sorters::Descending::Et());
 
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << muons->size() << " muons and " << jets_EF->size() << " jets" << endreq; 

  for (unsigned int j=0 ; Muon != lastMuon; Muon++, j++) {

    const TrigMuonEFInfo* muonInfo = (*Muon);

    if (!muonInfo->hasTrack()) continue;

    if (m_workingMode==2) pass = true;

    TrigMuonEFInfoTrackContainer *tc = muonInfo->TrackContainer();
    TrigMuonEFInfoTrackContainer::const_iterator Track = tc->begin();
    TrigMuonEFInfoTrackContainer::const_iterator lastTrack = tc->end();

    for ( int k=0; Track!=lastTrack; Track++,k++) {

      const TrigMuonEFInfoTrack* muonInfoTrack = (*Track);

      if (muonInfoTrack->MuonType()!=1) continue;

      const TrigMuonEFTrack* muonEFTrack = 0;

      if      ((*Track)->hasCombinedTrack())     { muonEFTrack = (*Track)->CombinedTrack(); }
      else if ((*Track)->hasExtrapolatedTrack()) { muonEFTrack = (*Track)->ExtrapolatedTrack(); }
      else if ((*Track)->hasSpectrometerTrack()) { muonEFTrack = (*Track)->SpectrometerTrack(); }

      if (!muonEFTrack) continue;

      muonEta = muonEFTrack->eta();
      muonPhi = muonEFTrack->phi();

      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Muon "<< j+1 << "; ipt " << muonEFTrack->iPt() << "; eta "<< muonEta << "; phi " << muonPhi << endreq;

      unsigned int i=0;
      for (const Jet* jet : jets) {
        ++i;
	
	if (jet->et() < m_etThreshold) {
	  if (msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_etThreshold/1000 << " GeV threshold; Et " << jet->et()/1000 << endreq;
	  continue;
	}

	jetEta = jet->eta();
	jetPhi = jet->phi();
	
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << "; Et " << jet->et()/1000 << "; eta "<< jetEta << "; phi " << jetPhi << endreq;
	  
	deltaEta = muonEta - jetEta;
	deltaPhi = phiCorr(phiCorr(muonPhi) - phiCorr(jetPhi));
	
	double dR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);

	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "deltaR = "<< dR << endreq; 

        switch (m_workingMode) {

        case 1:
          if (dR < m_deltaRCut) {
	    m_deltaEtaPass = deltaEta; m_deltaPhiPass = deltaPhi; 
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


HLT::ErrorCode TrigLeptonJetFexAllTE::hltFinalize() {

  msg() << MSG::INFO << "Finalizing TrigLeptonJetFexAllTE" << endreq;

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrigLeptonJetFexAllTE::reset() {

  if ( !AllTEAlgo::reset() ) return false;
  AllTEAlgo::reset();

  return true;
}
