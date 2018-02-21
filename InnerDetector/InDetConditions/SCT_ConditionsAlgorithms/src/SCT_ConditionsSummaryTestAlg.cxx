/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsSummaryTestAlg.cxx
 *
 * @brief Implementation file for the SCT_ConditionsSummaryTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Shaun Roe
 **/

#include "SCT_ConditionsSummaryTestAlg.h"

//Gaudi includes

//Athena includes
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"

SCT_ConditionsSummaryTestAlg::SCT_ConditionsSummaryTestAlg( 
                                                           const std::string& name, 
                                                           ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_pSummarySvc("SCT_ConditionsSummarySvc", name){
  //nop
}

//Initialize
StatusCode 
SCT_ConditionsSummaryTestAlg::initialize(){
  
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO("Calling initialize");
  sc = m_pSummarySvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_ERROR("Could not retrieve the summary service");
  }
  return sc;
}

//Execute
StatusCode 
SCT_ConditionsSummaryTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  StatusCode sc(StatusCode::SUCCESS);
  ATH_MSG_INFO("Calling execute");
  ATH_MSG_INFO("Dummy call to module id 0: module is ");
  bool result=m_pSummarySvc->isGood(Identifier(0));
  ATH_MSG_INFO((result ? "good" : "bad"));
  ATH_MSG_INFO("Dummy call to module id 1: module is ");
  result=m_pSummarySvc->isGood(Identifier(1));
  ATH_MSG_INFO((result ? "good" : "bad"));
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
  ATH_MSG_INFO("Strip  216808130 " << (result ? "good" : "bad"));
  result=m_pSummarySvc->isGood(Identifier(216808743), InDetConditions::SCT_STRIP);
  ATH_MSG_INFO("Strip  216808743 " << (result ? "good" : "bad"));
  return sc;
}


//Finalize
StatusCode
SCT_ConditionsSummaryTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
