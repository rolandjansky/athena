/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/Units/SystemOfUnits.h"
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
    ATH_MSG_DEBUG("Module " << i << " "
                            << hv->getBiasVoltage(i) << " " 
                            << temp->getTemperature(i) << " " 
                            << deadmap->getModuleStatus(i) << " " 
                            << m_lorentzAngleTool->getLorentzShift(IdentifierHash(i)));
    for (int j=0; j<16; j++) {
      ATH_MSG_DEBUG("FE " << j << " "
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::LONG) << " " 
                          << calib->getAnalogThreshold(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getAnalogThresholdSigma(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getAnalogThresholdNoise(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getInTimeThreshold(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotA(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotE(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotC(i,j,PixelChargeCalibCondData::NORMAL) << " " 
                          << calib->getQ2TotA(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotE(i,j,PixelChargeCalibCondData::GANGED) << " " 
                          << calib->getQ2TotC(i,j,PixelChargeCalibCondData::GANGED));
    }


    if (distortion->getId(i) != 0) {
      ATH_MSG_DEBUG("Distortion parameters for module 0x" << std::hex << distortion->getId(i) << std::dec);

      std::vector<float> map = distortion->getDistortionMap(i);
      std::stringstream s;
      for (float f : map)
        s << f << " ";
      ATH_MSG_DEBUG(s.str());

      Amg::Vector2D point1(-0.123, 0.456);
      Amg::Vector2D point2(1.789, -5.678);
      Amg::Vector3D dir(0,0,1); //unit z direction
      const Amg::Transform3D rotateAboutX(Amg::AngleAxis3D(45 * CLHEP::deg, Amg::Vector3D::UnitX()));
      const Amg::Transform3D rotateAboutY(Amg::AngleAxis3D(45 * CLHEP::deg, Amg::Vector3D::UnitY()));
      const Amg::Transform3D totalTransform = rotateAboutX * rotateAboutY;
      dir = totalTransform.linear() * dir;
      Amg::Vector2D corr;
      corr = distortion->correction(i, point1, dir);
      ATH_MSG_DEBUG(" Point 1: " << "(" << point1[0] << ", " <<  point1[1] << ")"
                      << ", correction: " << "(" << corr.x() << ", " << corr.y() << ")");
      corr = distortion->correction(i, point2, dir);
      ATH_MSG_DEBUG(" Point 2: " << "(" << point2[0] << ", " <<  point2[1] << ")"
                      << ", correction: " << "(" << corr.x() << ", " << corr.y() << ")");
      Amg::Vector2D newPoint2sim = distortion->correctSimulation(i, point2, dir);
      ATH_MSG_DEBUG(" Point 2 after correction in simul: " <<  "(" << newPoint2sim[0] << ", " <<  newPoint2sim[1] << ")");
      Amg::Vector2D newPoint2rec = distortion->correctReconstruction(i, point2, dir);
      ATH_MSG_DEBUG(" Point 2 after correction in reco: " <<  "(" << newPoint2rec[0] << ", " <<  newPoint2rec[1] << ")");
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelSiliconConditionsTestAlg::finalize(){
  ATH_MSG_INFO("Calling finalize");
  return StatusCode::SUCCESS;
}
