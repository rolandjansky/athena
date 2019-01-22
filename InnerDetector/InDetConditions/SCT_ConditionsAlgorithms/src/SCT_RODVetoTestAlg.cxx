/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_RODVetoTestAlg.cxx
 *
 * @brief Implementation file for the SCT_RODVetoTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Daiki Hayakawa
 **/

#include "SCT_RODVetoTestAlg.h"

// Athena include
#include "Identifier/IdentifierHash.h"

SCT_RODVetoTestAlg::SCT_RODVetoTestAlg(const std::string& name, 
                                       ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm( name, pSvcLocator ) {
  //nop
}

//Initialize
StatusCode 
SCT_RODVetoTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  ATH_CHECK(m_pRODVetoTool.retrieve());
  return StatusCode::SUCCESS;
}

//Execute
StatusCode 
SCT_RODVetoTestAlg::execute(const EventContext& /*ctx*/) const {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  ATH_MSG_INFO("Calling execute");
  for (unsigned int hash{0}; hash<8176; hash+=2) {
    bool result{m_pRODVetoTool->isGood(IdentifierHash(hash))};//invented, no idea what this is
    ATH_MSG_INFO("Call to module in ROD : Module (hash=" << hash << ") is " << (result?"good":"bad"));
  }

  return StatusCode::SUCCESS;
}


//Finalize
StatusCode
SCT_RODVetoTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
