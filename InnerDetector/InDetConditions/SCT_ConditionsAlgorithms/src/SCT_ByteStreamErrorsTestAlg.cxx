/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTestAlg.cxx
 *
 * @brief Class to test SCT_ByteStreamErrorsTool (implementation)
 *
 * @author Susumu.Oda@cern.ch
 **/

#include "SCT_ByteStreamErrorsTestAlg.h"

SCT_ByteStreamErrorsTestAlg::SCT_ByteStreamErrorsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator) {
}

//Initialize
StatusCode SCT_ByteStreamErrorsTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  
  // Retrieve bytestream error tool
  ATH_CHECK(m_bsErrTool.retrieve());

  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_ByteStreamErrorsTestAlg::execute(const EventContext& ctx) const {
  ATH_MSG_INFO("Calling execute");

  ATH_MSG_ALWAYS("------------------------------------------------------------");
  for (unsigned int iErr{0}; iErr<SCT_ByteStreamErrors::NUM_ERROR_TYPES; iErr++) {
    ATH_MSG_ALWAYS("BS Error index: " << iErr << " type " << SCT_ByteStreamErrors::ErrorTypeDescription[iErr].c_str());
    const std::set<IdentifierHash>  errorSet =  m_bsErrTool->getErrorSet(iErr,  ctx);
    for (const IdentifierHash& hash: errorSet) {
      ATH_MSG_ALWAYS("  Wafer hash : " << hash);
    }
    ATH_MSG_ALWAYS("------------------------------------------------------------");
  }

  return StatusCode::SUCCESS;
}

//Finalize
StatusCode SCT_ByteStreamErrorsTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
