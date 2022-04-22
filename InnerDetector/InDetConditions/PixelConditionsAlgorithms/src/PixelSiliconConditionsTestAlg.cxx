/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  SG::ReadCondHandle<PixelChargeCalibCondData> calib(m_chargeDataKey);
  SG::ReadCondHandle<PixelDistortionData> distortion(m_distortionKey);

  for (int i=0; i<2048; i++) { 
    ATH_MSG_INFO("Module " << i << " "
                 << hv->getBiasVoltage(i) << " " 
                 << temp->getTemperature(i) << " " 
                 << m_lorentzAngleTool->getLorentzShift(IdentifierHash(i)));
    for (int j=0; j<16; j++) {
      try {
        // ignore invalid FEs
        calib->getAnalogThreshold(InDetDD::PixelDiodeType::NORMAL, i, j);
      } catch (const std::range_error &) {
        continue;
      }
      ATH_MSG_INFO("FE " << j << " "
                   << calib->getAnalogThreshold(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getAnalogThresholdSigma(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getAnalogThresholdNoise(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getInTimeThreshold(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getAnalogThreshold(InDetDD::PixelDiodeType::LONG, i, j) << " " 
                   << calib->getAnalogThresholdSigma(InDetDD::PixelDiodeType::LONG, i, j) << " " 
                   << calib->getAnalogThresholdNoise(InDetDD::PixelDiodeType::LONG, i, j) << " " 
                   << calib->getInTimeThreshold(InDetDD::PixelDiodeType::LONG, i, j) << " " 
                   << calib->getAnalogThreshold(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getAnalogThresholdSigma(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getAnalogThresholdNoise(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getInTimeThreshold(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getQ2TotA(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getQ2TotE(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getQ2TotC(InDetDD::PixelDiodeType::NORMAL, i, j) << " " 
                   << calib->getQ2TotA(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getQ2TotE(InDetDD::PixelDiodeType::GANGED, i, j) << " " 
                   << calib->getQ2TotC(InDetDD::PixelDiodeType::GANGED, i, j));
    }


    if (distortion->getId(i) != 0) {
      ATH_MSG_INFO("Distortion parameters for module 0x" << std::hex << distortion->getId(i) << std::dec);

      std::vector<float> map = distortion->getDistortionMap(i);
      std::stringstream s;
      for (float f : map)
        s << f << " ";
      ATH_MSG_INFO(s.str());

      Amg::Vector2D point1(-0.123, 0.456);
      Amg::Vector2D point2(1.789, -5.678);
      Amg::Vector3D dir(0,0,1); //unit z direction
      const Amg::Transform3D rotateAboutX(Amg::AngleAxis3D(45 * CLHEP::deg, Amg::Vector3D::UnitX()));
      const Amg::Transform3D rotateAboutY(Amg::AngleAxis3D(45 * CLHEP::deg, Amg::Vector3D::UnitY()));
      const Amg::Transform3D totalTransform = rotateAboutX * rotateAboutY;
      dir = totalTransform.linear() * dir;
      Amg::Vector2D corr;
      corr = distortion->correction(i, point1, dir);
      ATH_MSG_INFO(" Point 1: " << "(" << point1[0] << ", " <<  point1[1] << ")"
                      << ", correction: " << "(" << corr.x() << ", " << corr.y() << ")");
      corr = distortion->correction(i, point2, dir);
      ATH_MSG_INFO(" Point 2: " << "(" << point2[0] << ", " <<  point2[1] << ")"
                      << ", correction: " << "(" << corr.x() << ", " << corr.y() << ")");
      Amg::Vector2D newPoint2sim = distortion->correctSimulation(i, point2, dir);
      ATH_MSG_INFO(" Point 2 after correction in simul: " <<  "(" << newPoint2sim[0] << ", " <<  newPoint2sim[1] << ")");
      Amg::Vector2D newPoint2rec = distortion->correctReconstruction(i, point2, dir);
      ATH_MSG_INFO(" Point 2 after correction in reco: " <<  "(" << newPoint2rec[0] << ", " <<  newPoint2rec[1] << ")");
    }
  }

  return StatusCode::SUCCESS;
}
