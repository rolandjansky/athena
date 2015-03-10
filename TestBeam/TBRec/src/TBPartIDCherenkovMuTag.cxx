/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBPartIDCherenkovMuTag.h"
#include "TBEvent/TBIdentifiedParticle.h"
#include "TBEvent/TBScintillatorCont.h"

TBPartIDCherenkovMuTag::TBPartIDCherenkovMuTag(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator) { 
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
  m_useCherenkovBelow = 0;
}

TBPartIDCherenkovMuTag::~TBPartIDCherenkovMuTag() 
{}

StatusCode TBPartIDCherenkovMuTag::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TBPartIDCherenkovMuTag::execute() {
  const TBScintillatorCont *scintCont;
  StatusCode sc = evtStore()->retrieve(scintCont, m_SGkeyscint);
  if (sc.isFailure()){
    ATH_MSG_WARNING ( "Retrieval of TBScintillatorContainer with key "<<m_SGkeyscint<<" failed" );
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
	ATH_MSG_DEBUG (" MuonTag=" << (*it)->getSignal() );
	break;
      }
    }
    if ((*it)->getDetectorName()==m_CherenkovName) {
      if((*it)->getSignal()>m_cherenkovADCcut)
	myParticle->setParticle(TBIdentifiedParticle::ELECTRON);
      else 
	myParticle->setParticle(TBIdentifiedParticle::PION);
      ATH_MSG_DEBUG (" Cherenkov=" << (*it)->getSignal() );
    }
  }
    
  if (myParticle->getParticle()==TBIdentifiedParticle::ELECTRON) {
    ATH_MSG_DEBUG ( " Particle found to be an electron." );
    m_NElectrons++;
  }
  else if (myParticle->getParticle()==TBIdentifiedParticle::PION) {
    ATH_MSG_DEBUG ( " Particle found to be a pion." );
    m_NPions++;
  }
  else if (myParticle->getParticle()==TBIdentifiedParticle::MUON) {
    ATH_MSG_DEBUG ( " Particle found to be a muon." );
    m_NMuons++;
  }
  else {
    ATH_MSG_DEBUG ( " Particle cannot be identified." );
    m_Nunknown++;
  }
  sc = evtStore()->record(myParticle,m_particleKey);
  if (sc.isFailure()){
    ATH_MSG_WARNING ( "Cannot record TBIdentifiedParticle with key " << m_particleKey );
  }
  return StatusCode::SUCCESS;
}


StatusCode TBPartIDCherenkovMuTag::finalize() {
  if (m_printSummary) {
    const unsigned Ntotal=m_NElectrons+m_NPions+m_NMuons+m_Nunknown;
    ATH_MSG_INFO ( "Particles found in the run:" );
    ATH_MSG_INFO ( "Electrons: " << m_NElectrons );
    ATH_MSG_INFO ( "Pions    : " << m_NPions );
    ATH_MSG_INFO ( "Muons    : " << m_NMuons );
    ATH_MSG_INFO ( "unkown   : " << m_Nunknown );
    ATH_MSG_INFO ( "Total    : " << Ntotal );
  }
  return StatusCode::SUCCESS;
}
