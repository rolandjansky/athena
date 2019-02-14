/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_LinkMaskingTestAlg.cxx
 *
 * @brief 
 * 
 *
 * @author gwilliam@mail.cern.ch
 **/

#include "SCT_LinkMaskingTestAlg.h"

//Athena includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"

SCT_LinkMaskingTestAlg::SCT_LinkMaskingTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator) {
}

//Initialize
StatusCode SCT_LinkMaskingTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  
  // Retrieve link masking tool
  if (m_linkMaskingTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve the link masking tool");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_LinkMaskingTestAlg::execute(const EventContext& ctx) const {

  ATH_MSG_INFO("Wafer 167772160 is " << (m_linkMaskingTool->isGood(Identifier{167772160}, ctx) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167773184 is " << (m_linkMaskingTool->isGood(Identifier{167773184}, ctx) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167786496 is " << (m_linkMaskingTool->isGood(Identifier{167786496}, ctx) ? "not masked" : "masked"));
  ATH_MSG_INFO("Wafer 167787520 is " << (m_linkMaskingTool->isGood(Identifier{167787520}, ctx) ? "not masked" : "masked"));

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_LinkMaskingTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
