/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// OLD  ATH_CHECK(m_siliconTool.retrieve());
  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_lorentzAngleTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::execute(){
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients

  SG::ReadCondHandle<PixelModuleData> hv(m_readKeyHV);
  SG::ReadCondHandle<PixelModuleData> temp(m_readKeyTemp);
  SG::ReadCondHandle<PixelModuleData> deadmap(m_moduleDataKey);

  // Check HV
  for (int i=0; i<2048; i++) { std::cout << "PIXEL HV : " << i << " " << hv->getBiasVoltage(i) << std::endl; }
// OLD  for (int i=0; i<2048; i++) { std::cout << "PIXEL HV : " << i << " " << m_siliconTool->biasVoltage(IdentifierHash(i)) << std::endl; }

  // Check temperature
  for (int i=0; i<2048; i++) { std::cout << "PIXEL Temperature : " << i << " " << temp->getTemperature(i) << std::endl; }
// OLD  for (int i=0; i<2048; i++) { std::cout << "PIXEL Temperature : " << i << " " << m_siliconTool->temperature(IdentifierHash(i)) << std::endl; }

  // Check deadmap
  for (int i=0; i<2048; i++) { std::cout << "PIXEL Deadmap : " << i << " " << deadmap->getModuleStatus(i) << std::endl; }
// OLD  for (int i=0; i<2048; i++) { std::cout << "PIXEL Deadmap : " << i << " " << deadmap->getModuleStatus(IdentifierHash(i)) << std::endl; }

  for (int i=0; i<2048; i++) { std::cout << "PIXEL LorentzAngle : " << i << " " << m_lorentzAngleTool->getLorentzShift(IdentifierHash(i)) << std::endl; }

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
