/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTRTHTHhypo.h"
#include "TrigCaloEvent/TrigRNNOutput.h"


TrigTRTHTHhypo::TrigTRTHTHhypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_minTRTHTHits(0),
  m_minCaloE(0.)
{
  declareProperty("AcceptAll",     m_acceptAll = false ); 
  declareProperty("MinTRTHTHits",  m_minTRTHTHits = 20); // Changed from 40 to 20
  declareProperty("MinHTRatio",    m_minHTratio = 0.37); // Changed from 0.2 to 0.37 with new algo
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltInitialize() {

  if(msgLvl() <= MSG::INFO) {
    msg() << MSG::INFO << "Initialising TrigTRTHTHhypo: " << name() << endreq;
  }


  return HLT::OK;
}

//-----------------------------------------------------------------------------
HLT::ErrorCode TrigTRTHTHhypo::hltBeginRun() {
  msg() << MSG::DEBUG << "hltBeginRun of " << name() << endreq;

  return HLT::OK;
}
//---------------------------------------------------------------------------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltExecute(const HLT::TriggerElement* outputTE, 
						    bool& pass) {
  pass = false;

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing this TrigTRTHTHhypo " << name() << endreq;
  }


  // 
  if( m_acceptAll ){
    pass=true;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Accepting all events in " << name() << endreq;
    return HLT::OK;
  }

  //get fex output
  const TrigRNNOutput *out = 0;
  if (getFeature(outputTE, out, "TrigTRTHTCounts") != HLT::OK ||  (out == 0)) {
    return HLT::NAV_ERROR;
  }
 
//   const size_t outsize = out->size();
  float trththits = out->at(0);  //Changed to cut on the number httrt and fraction httrt in the cone size of 0.015 in phi
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "trththits: " << trththits << endreq;
  
  float ratio = trththits - floor(trththits);
  if (ratio > m_minHTratio){
    if (trththits>m_minTRTHTHits)
      pass = true;
  }

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << name() << " hypo result " << pass << endreq;

  return HLT::OK;  
}

//-----------------------------------------------------------------------------

HLT::ErrorCode TrigTRTHTHhypo::hltFinalize() {
  msg() << MSG::DEBUG << " finalizing this TrigTRTHTHhypo : " << name() << endreq; 
  return HLT::OK;  
}
