/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedConditionTestAlg.cxx
 *
 * @brief Class to test SCT_FlaggedConditionTool (implementation)
 *
 * @author Susumu.Oda@cern.ch
 **/

#include "SCT_FlaggedConditionTestAlg.h"

SCT_FlaggedConditionTestAlg::SCT_FlaggedConditionTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator) {
}

//Initialize
StatusCode SCT_FlaggedConditionTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  
  // Retrieve flagged condition tool
  ATH_CHECK(m_flaggedTool.retrieve());

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_FlaggedConditionTestAlg::execute(const EventContext& ctx) const {
  ATH_MSG_INFO("Calling execute");

  ATH_MSG_ALWAYS("------------------------------------------------------------");
  ATH_MSG_ALWAYS(" numBadIds " << m_flaggedTool->numBadIds(ctx));
  const SCT_FlaggedCondData* badIds{m_flaggedTool->getBadIds(ctx)};
  for (const std::pair<const IdentifierHash, std::string>& badId : *badIds) {
    ATH_MSG_ALWAYS("  Wafer hash " << badId.first << " reason " << badId.second);
  }

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_FlaggedConditionTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
