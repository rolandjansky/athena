/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SiliconConditionsTestAlg.cxx
 *
 * @brief Implementation file for the SCT_SiliconConditionsTestAlg class 
 * in package SCT_ConditionsAlgorithms
 *
 * @author Shaun Roe
 **/

#include "SCT_SiliconConditionsTestAlg.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

SCT_SiliconConditionsTestAlg::SCT_SiliconConditionsTestAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator)
{
  //nop
}

//Initialize
StatusCode SCT_SiliconConditionsTestAlg::initialize() {
  ATH_MSG_INFO("Calling initialize");
  ATH_CHECK(m_siliconTool.retrieve());
  return StatusCode::SUCCESS;
}

//Execute
StatusCode SCT_SiliconConditionsTestAlg::execute(const EventContext& /*ctx*/) const {
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  
  ATH_MSG_INFO("Calling execute");
  ATH_MSG_INFO("Temperature  = " << m_siliconTool->temperature(IdentifierHash{804}));
  ATH_MSG_INFO("Depletion V  = " << m_siliconTool->depletionVoltage(IdentifierHash{804}));
  ATH_MSG_INFO("Bias Voltage = " << m_siliconTool->biasVoltage(IdentifierHash{804})); // was 216808130
  return StatusCode::SUCCESS;
}

//Finalize
StatusCode
SCT_SiliconConditionsTestAlg::finalize() {
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
