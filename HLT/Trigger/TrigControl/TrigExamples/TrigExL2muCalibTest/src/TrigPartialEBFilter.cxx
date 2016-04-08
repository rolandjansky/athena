/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExL2muCalibTest/TrigPartialEBFilter.h"
#include "EventInfo/EventInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <bitset>

/////////////////////////////////////////////////////////////////////////////

TrigPartialEBFilter::TrigPartialEBFilter(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator)
{
   // Declare the properties
   declareProperty("HLTInstance",           m_hltInstance="L2");
   declareProperty("HLTResultName",         m_hltResultName="HLTResult");
   declareProperty("AcceptRate",            m_acceptRate=1.0);
   declareProperty("CalibrationStreamName", m_calStreamName="");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::initialize() {

   // Get the messaging service
   ATH_MSG_INFO("initialize()");

   // Print out the property values
   ATH_MSG_INFO(" HLT Instance                      = " << m_hltInstance);
   ATH_MSG_INFO(" HLT Result name                   = " << m_hltResultName);
  
   m_hltResultSGKey = m_hltResultName.value() + "_" + m_hltInstance.value();
  
   ATH_MSG_INFO(" Storegate key for retrieving the HLT result = " << m_hltResultSGKey);
  
   ATH_MSG_INFO(" Fraction of events to be accepted = " << m_acceptRate);
   ATH_MSG_INFO(" Calibration stream to be accepted = " << m_calStreamName);

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::execute() {

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), name());

   if (outputLevel() <= MSG::DEBUG)  ATH_MSG_DEBUG("execute()");
  
   // get EventInfo
   const EventInfo* p_EventInfo(0);
   StatusCode sc = evtStore()->retrieve(p_EventInfo);
   if(sc.isFailure() || p_EventInfo == 0){
      ATH_MSG_FATAL("Can't get EventInfo object for updating the StreamTag and TriggerInfo");
      return StatusCode::FAILURE;
   }
  
   // get to the HLT result
   HLT::HLTResult* pHltResult(0);
   sc = evtStore()->retrieve(pHltResult,m_hltResultSGKey);
   if  (sc.isFailure() || pHltResult == 0) {
      ATH_MSG_FATAL("HLT result not found in StoreGate"); 
      return StatusCode::FAILURE;
   }
   
   //--------------------------------------------------------------------------
   // Loop over all the stream tag and apply selection
   //--------------------------------------------------------------------------
  
   //bool isAccepted = pHltResult->isAccepted();
   bool havePhysicsStream = false;
    
   StreamTagVector_t vecStreamTags = p_EventInfo->trigger_info()->streamTags();
   int activeTags = vecStreamTags.size();
  
   ATH_MSG_DEBUG("Retrieved stream tag vector of size " << activeTags);
  
   for (StreamTagVector_t::const_iterator it = vecStreamTags.begin();
        it != vecStreamTags.end(); ++it) {
      if ( (*it).type() == "physics") havePhysicsStream = true;
      if (outputLevel() <= MSG::DEBUG) {
         ATH_MSG_DEBUG("  .. " << (*it).type() << " : " << (*it).name());
      }
   }
  
   //if(havePhysicsStream != isAccepted) {
   //  ATH_MSG_ERROR("Event accepted without any physics stream");
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
      ATH_MSG_DEBUG("After applying selection:");
      for (StreamTagVector_t::const_iterator it = vecStreamTags.begin();
           it != vecStreamTags.end(); ++it) {
         ATH_MSG_DEBUG("  .. " << (*it).type() << " : " << (*it).name());
      }
   }
 
   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TrigPartialEBFilter::finalize() {

   // Get the messaging service
   MsgStream log(msgSvc(), name());
   ATH_MSG_INFO("finalize()");

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
