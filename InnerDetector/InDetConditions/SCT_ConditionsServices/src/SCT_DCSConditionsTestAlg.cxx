/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TestDCSConditions.cxx  Implementation file for TestDCSConditions class.
 @author Shaun Roe, based on Jorgen Dalmau example
 */

// Include SCT_DCSConditionsTestAlg and Svc
#include "SCT_DCSConditionsTestAlg.h"
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h"

// Include Athena stuff
#include "Identifier/Identifier.h"

// Include Gaudi stuff
#include "GaudiKernel/StatusCode.h"

// Read Handle
#include "StoreGate/ReadHandle.h"

// Include STL stuff
#include <string>
using namespace std;

SCT_DCSConditionsTestAlg::SCT_DCSConditionsTestAlg(
                                                   const std::string& name, 
                                                   ISvcLocator* pSvcLocator ) : 
AthAlgorithm( name, pSvcLocator ),
m_currentEventKey(std::string("EventInfo")),
m_DCSConditionsSvc("SCT_DCSConditionsSvc",name)//use SCT_DCSConditionsSvc if you are not running with InDetRecExample
{ //nop
}
//
SCT_DCSConditionsTestAlg::~SCT_DCSConditionsTestAlg(){ }

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::initialize(){  
  // Get the messaging service, print where you are
  msg(MSG::INFO) << "in initialize()" << endmsg;
  //
  StatusCode sc(StatusCode::SUCCESS);
  sc = m_DCSConditionsSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) return (msg(MSG::ERROR) << "Unable to get the DCS conditions service" << endmsg), sc;

  // Read Handle
  ATH_CHECK(m_currentEventKey.initialize());
  
  return sc;
} // SCT_DCSConditionsTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in execute()" << endmsg;
  //
  StatusCode sc(StatusCode::SUCCESS);
  
  // Get the current event
  SG::ReadHandle<xAOD::EventInfo> currentEvent(m_currentEventKey);
  if ( not currentEvent.isValid() ) return (msg(MSG::ERROR) << "Could not get event info" << endmsg), sc;
  //
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Current Run.Event,Time: "
  << "[" << currentEvent->runNumber()
  << "." << currentEvent->eventNumber()
  << "," << currentEvent->timeStamp()
  << "]" << endmsg;
  
  bool DCSfilled(false);
  bool isgoodworks(false);
  float gethvworks=0.0;
  float gettempworks=0.0;
//  bool side(false);
//  bool strip(false);
  msg(MSG::INFO) << m_DCSConditionsSvc << endmsg;
  
  try{
      DCSfilled =(m_DCSConditionsSvc->filled());
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to determine whether the data object was filled" << endmsg;
    return StatusCode::FAILURE;
  }
  
  try{
     gettempworks =(m_DCSConditionsSvc->sensorTemperature(Identifier(141015041),InDetConditions::SCT_STRIP));
     //isgoodworks =(m_DCSConditionsSvc->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
     //module =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_MODULE));
     //strip =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to the gettemp method" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "gettemp(141015041,Strip) "<< (gettempworks?"successful":"failed") << endmsg;
  msg(MSG::INFO) << "gettemp(141015041,Strip) "<< gettempworks << endmsg;
 

  try{
     gethvworks =(m_DCSConditionsSvc->modHV(Identifier(141015041),InDetConditions::SCT_STRIP));
     //isgoodworks =(m_DCSConditionsSvc->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
     //module =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_MODULE));
     //strip =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to the modHV method" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::INFO) << "gethv(141015041,Strip) "<< (gethvworks?"successful":"failed") << endmsg;
  msg(MSG::INFO) << "gethv(141015041,Strip) "<< (m_DCSConditionsSvc->modHV(Identifier(141015041),InDetConditions::SCT_STRIP)) << endmsg;

  try{
     isgoodworks =(m_DCSConditionsSvc->isGood(Identifier(141015041),InDetConditions::SCT_STRIP));
     //isgoodworks =(m_DCSConditionsSvc->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
     //module =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_MODULE));
     //strip =(m_DCSConditionsSvc->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    msg(MSG::FATAL) << "Exception caught while trying to the isGood method" << endmsg;
    return StatusCode::FAILURE;
  }
  
  msg(MSG::INFO) << "fillData "<< (DCSfilled?"successful":"failed") << endmsg;
  msg(MSG::INFO) << "isGood(141015041,Strip) "<< (isgoodworks?"successful":"failed") << endmsg;

  //msg(MSG::INFO) << "canReportAbout(Module) "<< (module?"successful":"failed") << endmsg;
  //msg(MSG::INFO) << "canReportAbout(Strip) "<< (strip?"successful":"failed") << endmsg;
  return sc;
} // SCT_DCSConditionsTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::finalize(){
  // Get the messaging service, print where you are
  msg(MSG::INFO) << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
} // SCT_DCSConditionsTestAlg::finalize()


