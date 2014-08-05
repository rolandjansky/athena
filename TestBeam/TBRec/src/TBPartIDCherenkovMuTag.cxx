/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBPartIDCherenkovMuTag.h"
#include "TBEvent/TBIdentifiedParticle.h"
#include "TBEvent/TBScintillatorCont.h"

TBPartIDCherenkovMuTag::TBPartIDCherenkovMuTag(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator) { 
  declareProperty("SGScintkey",m_SGkeyscint = "ScintillatorCont");
  declareProperty("MuonTagName",m_muonTagName="muTag"); 
  declareProperty("CherenkovName",m_CherenkovName="C1");
  declareProperty("IdentifiedParticleKey",m_particleKey="TBIdentifiedParticle");
  declareProperty("CherenkovADCcut",m_cherenkovADCcut=410);
  declareProperty("MuonADCcut",m_muonADCcut=500);
  declareProperty("PrintSummary",m_printSummary=false);

  m_NElectrons=0;
  m_NPions=0;
  m_NMuons=0;
  m_Nunknown=0;
  m_StoreGate = 0;
  m_useCherenkovBelow = 0;
}

TBPartIDCherenkovMuTag::~TBPartIDCherenkovMuTag() 
{}

StatusCode TBPartIDCherenkovMuTag::initialize() {
  MsgStream log(messageService(),name());
  StatusCode sc;
  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  } 
  return StatusCode::SUCCESS;
}

StatusCode TBPartIDCherenkovMuTag::execute() {
  MsgStream log(messageService(),name());
  const TBScintillatorCont *scintCont;
  StatusCode sc = m_StoreGate->retrieve(scintCont, m_SGkeyscint);
  if (sc.isFailure()){
    log << MSG::WARNING << "Retrieval of TBScintillatorContainer with key "<<m_SGkeyscint<<" failed" << endreq;
    return StatusCode::SUCCESS;
  }
  TBIdentifiedParticle* myParticle = new TBIdentifiedParticle();
  TBScintillatorCont::const_iterator it   = scintCont->begin();
  TBScintillatorCont::const_iterator it_e = scintCont->end();
  //The following loop is broken as soon as the muon tag is found above the threshold
  //Hence this can overwrite a preceeding electron/pion judgement by the chrenkov. 
  for(;it!=it_e;it++) {
    if((*it)->getDetectorName()==m_muonTagName) {
      if ((*it)->getSignal()>m_muonADCcut) {
	myParticle->setParticle(TBIdentifiedParticle::MUON);
	log << MSG::DEBUG << " MuonTag=" << (*it)->getSignal();
	break;
      }
    }
    if ((*it)->getDetectorName()==m_CherenkovName) {
      if((*it)->getSignal()>m_cherenkovADCcut)
	myParticle->setParticle(TBIdentifiedParticle::ELECTRON);
      else 
	myParticle->setParticle(TBIdentifiedParticle::PION);
      log << MSG::DEBUG << " Cherenkov=" << (*it)->getSignal();
    }
  }
    
  if (myParticle->getParticle()==TBIdentifiedParticle::ELECTRON) {
    log << MSG::DEBUG << " Particle found to be an electron." << endreq;
    m_NElectrons++;
  }
  else if (myParticle->getParticle()==TBIdentifiedParticle::PION) {
    log << MSG::DEBUG << " Particle found to be a pion." << endreq;
    m_NPions++;
  }
  else if (myParticle->getParticle()==TBIdentifiedParticle::MUON) {
    log << MSG::DEBUG << " Particle found to be a muon." << endreq;
    m_NMuons++;
  }
  else {
    log << MSG::DEBUG << " Particle cannot be identified." << endreq;
    m_Nunknown++;
  }
  sc = m_StoreGate->record(myParticle,m_particleKey);
  if (sc.isFailure()){
    log << MSG::WARNING << "Cannot record TBIdentifiedParticle with key " << m_particleKey << endreq;
  }
  return StatusCode::SUCCESS;
}


StatusCode TBPartIDCherenkovMuTag::finalize() {
  if (m_printSummary) {
    MsgStream log(messageService(),name());
    const unsigned Ntotal=m_NElectrons+m_NPions+m_NMuons+m_Nunknown;
    log << MSG::INFO << "Particles found in the run:" << endreq;
    log << MSG::INFO << "Electrons: " << m_NElectrons << endreq;
    log << MSG::INFO << "Pions    : " << m_NPions << endreq;
    log << MSG::INFO << "Muons    : " << m_NMuons << endreq;
    log << MSG::INFO << "unkown   : " << m_Nunknown << endreq;
    log << MSG::INFO << "Total    : " << Ntotal << endreq;
  }
  return StatusCode::SUCCESS;
}
