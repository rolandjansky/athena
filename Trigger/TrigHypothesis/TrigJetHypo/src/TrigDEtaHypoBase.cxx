/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventKernel/INavigable4Momentum.h"
#include "TrigJetHypo/TrigDEtaHypoBase.h"

//#include "TrigCaloEvent/TrigT2Jet.h"

TrigDEtaHypoBase::TrigDEtaHypoBase(const std::string& name, ISvcLocator* pSvcLocator) : HLT::HypoAlgo(name, pSvcLocator) {
 
  declareProperty("NGaps", m_nGaps, "Number of gaps above threshold");
  
  declareMonitoredVariable("GapCounter", m_gapCounter);
  declareMonitoredVariable("GapSize", m_gapSize);
  
}

TrigDEtaHypoBase::~TrigDEtaHypoBase(){
  
}

HLT::ErrorCode TrigDEtaHypoBase::hltInitialize(){
 
  msg()<<MSG::DEBUG<<" from initialize()"<<endreq;
  
  m_nJets = 2 * m_nGaps;
  
  return HLT::OK;
}

HLT::ErrorCode TrigDEtaHypoBase::hltFinalize(){
  
  msg()<<MSG::DEBUG<<" from finalize()"<<endreq;
  
  return HLT::OK;
}

HLT::ErrorCode TrigDEtaHypoBase::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  
  msg()<<MSG::DEBUG<<"Hello from TrigDEtaHypoBase::hltExecute"<<endreq;
  
  m_gapCounter = -1;
  m_gapSize=-999.;  
  std::vector<const INavigable4Momentum*> jets;
  
  HLT::ErrorCode status = getJets(outputTE, jets);
  
  if(status != HLT::OK){
    
    msg() << MSG::WARNING <<" Could not get the jets"<<endreq;
    pass = false;
    return status;
  }
    

  int nJets = jets.size();
  
  m_gapCounter = nJets / 2;
  
  if(m_nJets > nJets){
    pass = false;
  }else{
    pass = true;
    if(nJets>1)m_gapSize=std::fabs(jets[0]->eta()-jets[1]->eta());
  }
  msg()<<MSG::DEBUG<<"There are "<<nJets<<" jets!"<<endreq;
  
  msg()<<MSG::DEBUG<< " Event pass = "<<pass<<endreq;
  
  return HLT::OK;
}
