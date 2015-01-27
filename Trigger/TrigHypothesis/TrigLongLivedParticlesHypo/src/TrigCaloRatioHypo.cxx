/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************************
//
// NAME:     TrigCaloRatioHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// ************************************************************

#include "TrigLongLivedParticlesHypo/TrigCaloRatioHypo.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CxxUtils/fpcompare.h"
#include "FourMomUtils/P4DescendingSorters.h"


//** ----------------------------------------------------------------------------------------------------------------- **//


struct DescendingEt:std::binary_function<const xAOD::Jet*, const xAOD::Jet*, bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCaloRatioHypo::TrigCaloRatioHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("EtCut",        m_etCut = 35*CLHEP::GeV, "cut value for L2 jet et"); 
  declareProperty("LogRatioCut",  m_logRatioCut = 1., "cut value for L2 jet log10 of had over em energy ratio"); 
  declareProperty("EtaCut",       m_etaCut = 2.5, "cut value for Eta of the Jet");  
  declareProperty("doMonitoring", m_doMonitoring = true, "switch on/off monitoring" );
  declareProperty("AcceptAll",    m_acceptAll=false);

  declareMonitoredStdContainer("JetEt",      m_jetEt);
  declareMonitoredStdContainer("JetEta",     m_jetEta);
  declareMonitoredStdContainer("JetPhi",     m_jetPhi);
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("logRatio",   m_logRatio);  
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCaloRatioHypo::~TrigCaloRatioHypo() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltInitialize() {

  msg() << MSG::INFO << "in initialize()" << endreq;

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltFinalize() {

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors: "<< m_accepted <<" / "<<m_rejected <<" / "<< m_errors << endreq;
  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigCaloRatioHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  m_cutCounter = -1;

  bool passCut = false;
  pass=false;

  double jetRatio = -1.;
  m_logRatio = -1.;

  //clear monitoring vectors
  m_jetEt.clear();
  m_jetPhi.clear();
  m_jetEta.clear();

  const xAOD::JetContainer* vectorOfJets = 0;
  HLT::ErrorCode status = getFeature(outputTE, vectorOfJets);

  m_cutCounter = 0;

  if(status != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get the xAOD jet container" << endreq;
    m_errors++;
    return HLT::ERROR;
  } else if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Got the xAOD jet container" << endreq;
  
  if(vectorOfJets->size() == 0) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "The xAOD jet container is empty" << endreq;
    m_errors++;
    return HLT::OK;
  } else {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << vectorOfJets->size() << " jets are found" << endreq;
  }

  std::vector<const xAOD::Jet*> theJets(vectorOfJets->begin(), vectorOfJets->end());

  std::sort (theJets.begin(), theJets.end(), DescendingEt());

  for (const xAOD::Jet* aJet : theJets) {
   
    double jetEt  = aJet->p4().Et();
    double jetEta = aJet->eta();
    double jetPhi = aJet->phi();
    //object is not decorated?
    //double jetEMF = aJet->getAttribute<float>("EMFrac");
    //double jetEMF = aJet->getAttribute<float>("xAOD::JetAttribute::EMFrac");
    double jetEMF = 0.5;

    if (jetEMF)
      jetRatio = log10(1./jetEMF);
    else
      jetRatio = 999;
    
    if (jetEt > m_etCut && std::fabs(jetEta) <= m_etaCut && jetRatio >= m_logRatioCut) {

      m_jetEt.push_back(jetEt/CLHEP::GeV);
      m_jetEta.push_back(jetEta); 
      m_jetPhi.push_back(jetPhi);
      m_logRatio = jetRatio;
      
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet with Et " << jetEt << ", eta " << jetEta << ", phi " << jetPhi 
	      << ", log-ratio " << jetRatio << endreq;

      passCut = true;
    }
  }

  if(passCut || m_acceptAll) {

    pass = true;
    m_cutCounter = 1;

    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Minimum calo-ratio value is " 
	    << m_logRatioCut << " and a jet with " << m_logRatio << " is found; event is accepted" << endreq;
  } else {
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Minimum calo-ratio value is " 
	    << m_logRatioCut << ". No jets above the threshold are found; event is rejected" << endreq;
  }

  return HLT::OK;  
}
