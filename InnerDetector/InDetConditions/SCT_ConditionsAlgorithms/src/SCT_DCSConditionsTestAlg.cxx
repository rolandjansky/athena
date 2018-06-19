/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file TestDCSConditions.cxx  Implementation file for TestDCSConditions class.
    @author Shaun Roe, based on Jorgen Dalmau example
*/

// Include SCT_DCSConditionsTestAlg and Svc
#include "SCT_DCSConditionsTestAlg.h"

// Include Athena stuff
#include "Identifier/Identifier.h"

// Include STL stuff
#include <string>
using namespace std;

SCT_DCSConditionsTestAlg::SCT_DCSConditionsTestAlg(const std::string& name, 
                                                   ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{ //nop
}

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::initialize(){  
  // Get the messaging service, print where you are
  ATH_MSG_INFO("in initialize()");
  //
  StatusCode sc(StatusCode::SUCCESS);
  sc = m_DCSConditionsTool.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Unable to get the DCS conditions service");
    return sc;
  }

  return sc;
} // SCT_DCSConditionsTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  ATH_MSG_DEBUG("in execute()");
  //
  StatusCode sc(StatusCode::SUCCESS);
  
  bool isgoodworks(false);
  float gethvworks=0.0;
  float gettempworks=0.0;
  //  bool side(false);
  //  bool strip(false);
  ATH_MSG_INFO(m_DCSConditionsTool);
  
  try{
    gettempworks =(m_DCSConditionsTool->sensorTemperature(Identifier(141015041),InDetConditions::SCT_STRIP));
    //isgoodworks =(m_DCSConditionsTool->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
    //module =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_MODULE));
    //strip =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    ATH_MSG_FATAL("Exception caught while trying to the gettemp method");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("gettemp(141015041,Strip) "<< (gettempworks?"successful":"failed"));
  ATH_MSG_INFO("gettemp(141015041,Strip) "<< gettempworks);
 

  try{
    gethvworks =(m_DCSConditionsTool->modHV(Identifier(141015041),InDetConditions::SCT_STRIP));
    //isgoodworks =(m_DCSConditionsTool->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
    //module =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_MODULE));
    //strip =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    ATH_MSG_FATAL("Exception caught while trying to the modHV method");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("gethv(141015041,Strip) "<< (gethvworks?"successful":"failed"));
  ATH_MSG_INFO("gethv(141015041,Strip) "<< (m_DCSConditionsTool->modHV(Identifier(141015041),InDetConditions::SCT_STRIP)));

  try{
    isgoodworks =(m_DCSConditionsTool->isGood(Identifier(141015041),InDetConditions::SCT_STRIP));
    //isgoodworks =(m_DCSConditionsTool->isGood(Identifier(208584704),InDetConditions::SCT_SIDE));
    //module =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_MODULE));
    //strip =(m_DCSConditionsTool->canReportAbout(InDetConditions::SCT_STRIP));
  }catch(...){
    ATH_MSG_FATAL("Exception caught while trying to the isGood method");
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO("isGood(141015041,Strip) "<< (isgoodworks?"successful":"failed"));

  return sc;
} // SCT_DCSConditionsTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_DCSConditionsTestAlg::finalize(){
  // Get the messaging service, print where you are
  ATH_MSG_INFO("in finalize()");
  return StatusCode::SUCCESS;
} // SCT_DCSConditionsTestAlg::finalize()
