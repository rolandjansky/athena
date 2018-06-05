/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSiliconConditionsTestAlg.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

PixelSiliconConditionsTestAlg::PixelSiliconConditionsTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //nop
}

//Initialize
StatusCode PixelSiliconConditionsTestAlg::initialize()
{  
  ATH_MSG_INFO("Calling initialize");
  if (m_siliconTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the summary service");
  }
  return StatusCode::SUCCESS;
}

//Execute
StatusCode PixelSiliconConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients

  for (int i=0; i<2048; i++) {
    std::cout << "Bias Voltage = " << m_siliconTool->biasVoltage(IdentifierHash(i)) << " Temperature  = " << m_siliconTool->temperature(IdentifierHash(i)) << " Depletion V  = " << m_siliconTool->depletionVoltage(IdentifierHash(i)) << std::endl;
  }

  return StatusCode::SUCCESS;
}


//Finalize
StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
