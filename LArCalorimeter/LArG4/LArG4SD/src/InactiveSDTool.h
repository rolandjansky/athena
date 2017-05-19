/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_INACTIVESDTOOL_H
#define LARG4SD_INACTIVESDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/CalibSDTool.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

namespace LArG4
{

  /// @class InactiveSDTool
  /// @brief Sensitive detector tool which manages inactive-area LAr calib SDs.
  ///
  /// Design is in flux.
  ///
  class InactiveSDTool : public CalibSDTool
  {

    public:

      /// Constructor
      InactiveSDTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

    private:

      /// Initialize Calculator Services
      StatusCode initializeCalculators() override final;

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// Hit collection name
      std::string m_hitCollName;

      /// @name SD volumes
      /// @{
      std::vector<std::string> m_barPreVolumes;
      std::vector<std::string> m_barVolumes;
      std::vector<std::string> m_ECPosInVolumes;
      std::vector<std::string> m_ECPosOutVolumes;
      std::vector<std::string> m_ECNegInVolumes;
      std::vector<std::string> m_ECNegOutVolumes;
      std::vector<std::string> m_HECWheelVolumes;
      std::vector<std::string> m_fcal1Volumes;
      std::vector<std::string> m_fcal2Volumes;
      std::vector<std::string> m_fcal3Volumes;
      std::vector<std::string> m_miniMomVolumes;
      std::vector<std::string> m_miniVolumes;
      std::vector<std::string> m_miniLayVolumes;
      /// @}

      ServiceHandle<ILArCalibCalculatorSvc> m_embpscalc; //BarrelPresampler::CalibrationCalculator
      ServiceHandle<ILArCalibCalculatorSvc> m_embcalc; //Barrel::CalibrationCalculator
      ServiceHandle<ILArCalibCalculatorSvc> m_emepiwcalc;//LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emeniwcalc;//LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emepowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emenowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1)
      ServiceHandle<ILArCalibCalculatorSvc> m_heccalc;  //LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelInactive)
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal1calc;
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal2calc;
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal3calc;
     ServiceHandle<ILArCalibCalculatorSvc> m_minfcalcalc; //LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive) 

  }; // class InactiveSDTool

} // namespace LArG4

#endif
