/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

//Athena includes
#include "Identifier/Identifier.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"

//Gaudi includes

SCT_ConditionsSummaryTestAlg::SCT_ConditionsSummaryTestAlg(const std::string& name,
                                                           ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {
  //nop
}

//Initialize
StatusCode 
SCT_ConditionsSummaryTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  ATH_CHECK(m_pSummaryTool.retrieve());
  return StatusCode::SUCCESS;
}

//Execute
StatusCode 
SCT_ConditionsSummaryTestAlg::execute(const EventContext& /*ctx*/) const {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  ATH_MSG_INFO("Calling execute");
  ATH_MSG_INFO("Dummy call to module id 0: module is ");
  bool result{m_pSummaryTool->isGood(Identifier(0))};
  ATH_MSG_INFO((result ? "good" : "bad"));
  ATH_MSG_INFO("Dummy call to module id 1: module is ");
  result=m_pSummaryTool->isGood(Identifier(1));
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
  result=m_pSummaryTool->isGood(Identifier(216808130), InDetConditions::SCT_STRIP);
  ATH_MSG_INFO("Strip  216808130 " << (result ? "good" : "bad"));
  result=m_pSummaryTool->isGood(Identifier(216808743), InDetConditions::SCT_STRIP);
  ATH_MSG_INFO("Strip  216808743 " << (result ? "good" : "bad"));
  return StatusCode::SUCCESS;
}


//Finalize
StatusCode
SCT_ConditionsSummaryTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
