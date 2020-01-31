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

  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readKeyTemp.initialize());
  ATH_CHECK(m_readKeyHV.initialize());
  ATH_CHECK(m_lorentzAngleTool.retrieve());
  ATH_CHECK(m_chargeDataKey.initialize());
  ATH_CHECK(m_distortionKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::execute(){

  SG::ReadCondHandle<PixelDCSHVData> hv(m_readKeyHV);
  SG::ReadCondHandle<PixelDCSTempData> temp(m_readKeyTemp);
  SG::ReadCondHandle<PixelModuleData> deadmap(m_moduleDataKey);
  SG::ReadCondHandle<PixelChargeCalibCondData> calib(m_chargeDataKey);
  SG::ReadCondHandle<PixelDistortionData> distortion(m_distortionKey);

  for (int i=0; i<2048; i++) { 
    ATH_MSG_DEBUG("PIXEL Module hash=" << i 
                    << " HV=" << hv->getBiasVoltage(i) 
                    << " Temperature=" << temp->getTemperature(i) 
                    << " Status=" << deadmap->getModuleStatus(i)
                    << " LorentzShift=" << m_lorentzAngleTool->getLorentzShift(IdentifierHash(i)));
    ATH_MSG_DEBUG("Charge:");
    for (int j=0; j<16; j++) {
      ATH_MSG_DEBUG("  FE=" << j 
                    << " Threshold=" << calib->getAnalogThreshold(i,j,0)
                    << " Parameter A=" << calib->getQ2TotA(i,j,0)
                    << " Parameter E=" << calib->getQ2TotE(i,j,0)
                    << " Parameter C=" << calib->getQ2TotC(i,j,0));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
