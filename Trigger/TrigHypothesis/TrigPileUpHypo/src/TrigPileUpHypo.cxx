/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <iterator>
#include <sstream>
#include <cstdio>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigPileUpHypo/TrigPileUpHypo.h"

#include "TrigMissingEtEvent/TrigPileUpInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

TrigPileUpHypo::TrigPileUpHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("PileUpLabel", m_featureLabel, "Label for the pileup feature in the HLT navigation");

  declareProperty("CutType", m_CutType  = -2.0, "val<-1.5 => ForJet; -1.5<val<-0.5 FJet AND Primary Vertices; -0.5<val<0.5 => Primary Vertices; val>0.5 reject all;" );

  declareProperty("FJCut",  m_FJCut = 1,    "Maximum number of Forward Jets");
  declareProperty("PVCut",  m_PVCut = 9999, "Maximum number of Primary Vertices");

  declareProperty("FJ_Thr", m_minFJ_Thr = 0,     "Minimum passed threshold to be a Forward Jet");
  declareProperty("PV_Track", m_minPV_Track = 5, "Minimum number of tracks to be a Primary Vertex");

  declareMonitoredVariable("Hypo_PileUp_cuts",  m_cutCounter); // passed thresholds

  m_totTime=0;

}



HLT::ErrorCode TrigPileUpHypo::hltInitialize() {
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "hypo initialize()" << endreq;
  }

  if(doTiming() ) {
    m_totTime=addTimer("TrigPileUpHypoInternalTotalTime");
    if (m_totTime==0) {
      msg() << MSG::WARNING << "not able to initialize timer!" << endreq;
    }
  }

  m_accepted = 0;
  m_rejected = 0;
  m_errors   = 0;

  return HLT::OK;

}



HLT::ErrorCode TrigPileUpHypo::hltFinalize() {
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "in finalize()" << endreq;
    msg() << MSG::DEBUG << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  }


  return HLT::OK;

}



HLT::ErrorCode TrigPileUpHypo::hltExecute(const HLT::TriggerElement* outputTE, bool & pass){

  if (doTiming() && m_totTime) {
    m_totTime->start();
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "hypo hltExecute" << endreq;
  }

  
  pass = false;
  bool accepted = false;
  m_cutCounter = 0;

  std::vector<const TrigPileUpInfo*> vectorPileUp;
  
  HLT::ErrorCode stat = getFeatures(outputTE, vectorPileUp, m_featureLabel);
  
  if(stat != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get PileUp Features " << endreq;
    
    if (doTiming() && m_totTime) {
      m_totTime->stop();
    }
    
    return HLT::OK;
  }

  if( vectorPileUp.size() == 0 ){
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG<< " Got no PileUp's associated to the TE! " << endreq;
    }
    m_errors++;

    if (doTiming() && m_totTime) {
      m_totTime->stop();
    }    
    return HLT::OK;
  
} else {
    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " Got " << vectorPileUp.size()
	    << " vectorPileUp size associated to the TE" << endreq;
    }
  }

  const TrigPileUpInfo* pileUpInfo = vectorPileUp.front();

  std::vector<int> FJ = pileUpInfo->getFJVec();
  std::vector<int> PV = pileUpInfo->getPVVec();


  if(msgLvl() <= MSG::DEBUG) {
    for(unsigned int i = 0; i < FJ.size(); i++){
      msg() << MSG::DEBUG << "Forward Jet " << i << " has passed through threshold: " << FJ.at(i) << endreq;
    } 
    for(unsigned int i = 0; i < PV.size(); i++){
      msg() << MSG::DEBUG << "Primary Vertex " << i << " has number of tracks: " << PV.at(i) << endreq;
    } 
  }



  int numFJ = 0;
  for(unsigned int i = 0; i < FJ.size(); i++){
    if(FJ.at(i) >= m_minFJ_Thr) numFJ++;
  }

  int numPV = 0;
  for(unsigned int i = 0; i < PV.size(); i++){
    if(PV.at(i) >= m_minPV_Track) numPV++;
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Trig Pile Up info: " << endreq;
    msg() << MSG::DEBUG << "Num Qualifying Forward Jet: " << numFJ << endreq;
    msg() << MSG::DEBUG << "Num Qualifying Primary Vertex: " << numPV << endreq;
  }


  if(!accepted && m_CutType < -1.5){  // Passed FJ
    if( numFJ < m_FJCut ){
      accepted = true;
      m_cutCounter = 1;
    }
  }
  else if(!accepted && m_CutType < -0.5){  // Passed FJ AND Primary Vertex
    if( (numFJ < m_FJCut) && (numPV < m_PVCut) ){
      accepted = true;
      m_cutCounter = 2;
    }
  }
  else if(!accepted && m_CutType < 0.5){ // Passed Primary Vertex Cuts
    if( numPV < m_PVCut ){  
      accepted = true;
      m_cutCounter = 3;
    }
  }
  else{
    accepted = false;
  }

  if(accepted){
    pass = true;
    m_accepted++;
  }
  else{
    pass = false;
    m_rejected++;
  }
  

  if(doTiming() && m_totTime) {
    m_totTime->stop();
  }
  
  return HLT::OK;
  
}
