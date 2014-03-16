/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExL2muCalibTest/TrigPartialEBFilter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include <iostream>

#include <iomanip>
#include <cassert>
#include <bitset>

/////////////////////////////////////////////////////////////////////////////

TrigPartialEBFilter::TrigPartialEBFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), 
  m_storeGateSvc( "StoreGateSvc", name )
{
  // Declare the properties
  declareProperty("HLTInstance",           m_hltInstance="L2");
  declareProperty("HLTResultName",         m_hltResultName="HLTResult");
  declareProperty("AcceptRate",            m_acceptRate=1.0);
  declareProperty("CalibrationStreamName", m_calStreamName="");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::initialize(){

  // Get the messaging service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  // Print out the property values
  log << MSG::INFO << " HLT Instance                      = " << m_hltInstance << endreq;
  log << MSG::INFO << " HLT Result name                   = " << m_hltResultName << endreq;
  
  m_hltResultSGKey = m_hltResultName.value() + "_" + m_hltInstance.value();
  
  log << MSG::INFO << " Storegate key for retrieving the HLT result = " << m_hltResultSGKey << endreq;
  
  log << MSG::INFO << " Fraction of events to be accepted = " << m_acceptRate << endreq;
  log << MSG::INFO << " Calibration stream to be accepted = " << m_calStreamName << endreq;


  // Locate the StoreGateSvc
  StatusCode sc =  m_storeGateSvc.retrieve();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::execute() {

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());

  if (outputLevel() <= MSG::DEBUG)  log << MSG::DEBUG << "execute()" << endreq;
  
  // get EventInfo
  const EventInfo* p_EventInfo(0);
  StatusCode sc = m_storeGateSvc->retrieve(p_EventInfo);
  if(sc.isFailure() || p_EventInfo == 0){
    log << MSG::FATAL << "Can't get EventInfo object for updating the StreamTag and TriggerInfo" << endreq;
    return StatusCode::FAILURE;
  }
  
  // get to the HLT result
  HLT::HLTResult* pHltResult(0);
  sc = m_storeGateSvc->retrieve(pHltResult,m_hltResultSGKey);
  if  (sc.isFailure() || pHltResult == 0) {
     log << MSG::FATAL << "HLT result not found in StoreGate" << endreq; 
     return StatusCode::FAILURE;
  }
   
  //--------------------------------------------------------------------------
  // Loop over all the stream tag and apply selection
  //--------------------------------------------------------------------------
  
  //bool isAccepted = pHltResult->isAccepted();
  bool havePhysicsStream = false;
    
  StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
  int activeTags = vecStreamTags.size();
  
  if (outputLevel() <= MSG::DEBUG) log << MSG::DEBUG 
            << "Retrieved stream tag vector of size " << activeTags << endreq;
  
  for (StreamTagVector_t::const_iterator it = vecStreamTags.begin();
	     it != vecStreamTags.end(); ++it) {
    if ( (*it).type() == "physics") havePhysicsStream = true;
    if (outputLevel() <= MSG::DEBUG) {
      log << MSG::DEBUG << "  .. " << (*it).type() << " : " << (*it).name()
                        << endreq;
    }
  }
  
  //if(havePhysicsStream != isAccepted) {
  //  log << MSG::ERROR << "Event accepted without any physics stream" << endreq;
  //  return StatusCode::FAILURE;
  //}
  
  
  //remove the calibration stream tags different from what specified
  if (m_calStreamName.value()!="") {
    StreamTagVector_t::iterator it = vecStreamTags.begin();
      while(it != vecStreamTags.end()) {
        if( (*it).type() == "calibration" &&
	    (*it).name() != m_calStreamName.value()) it = vecStreamTags.erase(it);
        else ++it;
      }
    p_EventInfo->trigger_info()->setStreamTags(vecStreamTags); 
  }

  if (havePhysicsStream) {  // rescale the rate
    bool accept = randomAccept(m_acceptRate);
    
    if( !accept) {
      StreamTagVector_t::iterator it = vecStreamTags.begin();
    
      while(it != vecStreamTags.end()) {
        if( (*it).type() == "physics") it = vecStreamTags.erase(it);
	else if( (*it).type() == "express") it = vecStreamTags.erase(it);
        else ++it;
      }
      
      if (vecStreamTags.size() == 0) pHltResult->setAccepted( false );
    
      p_EventInfo->trigger_info()->setStreamTags(vecStreamTags);
    }
  }
  
  //retrieve back the stream tag vector and check
  vecStreamTags = p_EventInfo->trigger_info()->streamTags();
  if (outputLevel() <= MSG::DEBUG) {
      log << MSG::DEBUG << "After applying selection:" << endreq;
      for (StreamTagVector_t::const_iterator it = vecStreamTags.begin();
	                      it != vecStreamTags.end(); ++it) {
        log << MSG::DEBUG << "  .. " << (*it).type() << " : " << (*it).name()
                          << endreq;
      }
  }
 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::finalize() {

  // Get the messaging service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}

bool TrigPartialEBFilter::randomAccept(double rate) {
  bool decision = true;
  if (rate < 0.) {
    decision = false;
  } else if (rate > 1.) {
    decision = true;
  } else if ((double)std::rand()/double(RAND_MAX) > rate) {
    decision = false;
  }
  return decision;
}
