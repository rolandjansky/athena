/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ConditionsSummaryTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Shaun Roe
 **/

#include "SCT_ConditionsSummaryTestAlg.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"


//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//Athena includes
#include "Identifier/Identifier.h"

//local includes
//#include "SCT_ConditionsServices/SCT_ConditionsSummarySvc.h"

SCT_ConditionsSummaryTestAlg::SCT_ConditionsSummaryTestAlg( 
                         const std::string& name, 
                         ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_pSummarySvc("InDetSCT_ConditionsSummarySvc", name)
{
  declareProperty("ConditionsSummarySvc", m_pSummarySvc);
}

SCT_ConditionsSummaryTestAlg::~SCT_ConditionsSummaryTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

//Initialize
StatusCode 
SCT_ConditionsSummaryTestAlg::initialize(){
  
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling initialize" << endmsg;
  sc = m_pSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR)<<"Could not retrieve the summary service"<<endmsg;
  }
  return sc;
}

//Execute
StatusCode 
SCT_ConditionsSummaryTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling execute" << endmsg;
  msg(MSG::INFO)<<"Dummy call to module id 0: module is "<<endmsg;
  bool result=m_pSummarySvc->isGood(Identifier(0));
  msg(MSG::INFO)<< (result?"good":"bad") << endmsg;
  msg(MSG::INFO)<<"Dummy call to module id 1: module is "<<endmsg;
  result=m_pSummarySvc->isGood(Identifier(1));
  msg(MSG::INFO)<< (result?"good":"bad") << endmsg;
  /** some bad strips
  SCT_ConfigurationConditionsSvc             INFO 216808130
  SCT_ConfigurationConditionsSvc             INFO 216808744
  SCT_ConfigurationConditionsSvc             INFO 216808580
  SCT_ConfigurationConditionsSvc             INFO 216186344
  SCT_ConfigurationConditionsSvc             INFO 217299023
  SCT_ConfigurationConditionsSvc             INFO 217299115
  SCT_ConfigurationConditionsSvc             INFO 217299213
  SCT_ConfigurationConditionsSvc             INFO 217299415
  SCT_ConfigurationConditionsSvc             INFO 217299496
  SCT_ConfigurationConditionsSvc             INFO 217299649
  **/
  result=m_pSummarySvc->isGood(Identifier(216808130), InDetConditions::SCT_STRIP);
   msg(MSG::INFO)<<"Strip  216808130 "<<(result?"good":"bad") << endmsg;
   result=m_pSummarySvc->isGood(Identifier(216808743), InDetConditions::SCT_STRIP);
    msg(MSG::INFO)<<"Strip  216808743 "<<(result?"good":"bad") << endmsg;
  return sc;
}


//Finalize
StatusCode
SCT_ConditionsSummaryTestAlg::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  msg(MSG::INFO)<< "Calling finalize" << endmsg;
  return sc;
}
