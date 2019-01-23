/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_StripVetoTestAlg.cxx
 *
 * @brief Class to test SCT_StripVetoTool (implementation)
 *
 * @author Susumu.Oda@cern.ch
 **/

#include "SCT_StripVetoTestAlg.h"

// Athena includes
#include "InDetConditionsSummaryService/InDetHierarchy.h"

// STL includes
#include <vector>

SCT_StripVetoTestAlg::SCT_StripVetoTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator) {
}

//Initialize
StatusCode SCT_StripVetoTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  
  // Retrieve strip veto tool
  if (m_stripVetoTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve the link masking tool");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_StripVetoTestAlg::execute(const EventContext& /*ctx*/) const {
  std::vector<Identifier::value_type> stripIds{576522359582752768ULL,
                                               576522475009998848ULL,
                                               576522475278434304ULL,
                                               576522475546869760ULL,
                                               576522475815305216ULL,
                                               576522475815305216ULL};

  for (Identifier::value_type stripId : stripIds) {
    ATH_MSG_INFO("Strip " << stripId << " " << (m_stripVetoTool->isGood(Identifier{stripId}, InDetConditions::SCT_STRIP) ? "not vetoed" : "vetoed"));
  }

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_StripVetoTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
