/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityConditionsTestAlg.cxx
 *
 * @brief 
 * 
 *
 * @author gwilliam@mail.cern.ch
 **/

#include "SCT_MajorityConditionsTestAlg.h"

//Athena includes
#include "Identifier/Identifier.h"

SCT_MajorityConditionsTestAlg::SCT_MajorityConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

//Initialize
StatusCode SCT_MajorityConditionsTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");

  // Retrieve link masking service
  ATH_CHECK(m_majorityTool.retrieve());
 
  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_MajorityConditionsTestAlg::execute(const EventContext& ctx) const {
  ATH_MSG_INFO("Calling execute");

  ATH_MSG_INFO("Detector is " << (m_majorityTool->isGood(ctx)   ? "GOOD" : "BAD"));
  ATH_MSG_INFO("ECC is      " << (m_majorityTool->isGood(-2, ctx) ? "GOOD" : "BAD"));
  ATH_MSG_INFO("Barrel is   " << (m_majorityTool->isGood(0, ctx)  ? "GOOD" : "BAD"));
  ATH_MSG_INFO("ECA is      " << (m_majorityTool->isGood(2, ctx)  ? "GOOD" : "BAD"));

  return StatusCode::SUCCESS;
}


//Finalize
StatusCode SCT_MajorityConditionsTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
