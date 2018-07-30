/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsTestAlg.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

PixelSiliconConditionsTestAlg::PixelSiliconConditionsTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
}

StatusCode PixelSiliconConditionsTestAlg::initialize()
{  
  ATH_MSG_INFO("Calling initialize");

  ATH_CHECK(m_siliconTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients

  for (int i=0; i<2048; i++) {
    std::cout << "Hash ID=" << IdentifierHash(i) << " Bias Voltage=" << m_siliconTool->biasVoltage(IdentifierHash(i)) << " Temperature=" << m_siliconTool->temperature(IdentifierHash(i)) << " Depletion V=" << m_siliconTool->depletionVoltage(IdentifierHash(i)) << " FSM state=" << m_siliconTool->PixelFSMState(IdentifierHash(i)) << " FSM status=" << m_siliconTool->PixelFSMStatus(IdentifierHash(i)) <<  std::endl;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
