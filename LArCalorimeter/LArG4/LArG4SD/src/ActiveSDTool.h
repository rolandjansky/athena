/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_ACTIVESDTOOL_H
#define LARG4SD_ACTIVESDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/CalibSDTool.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

namespace LArG4
{

  /// @class ActiveSDTool
  /// @brief Sensitive detector tool which manages activate-area LAr calib SDs.
  ///
  /// Design is in flux.
  ///
  class ActiveSDTool : public CalibSDTool
  {

    public:

      /// Constructor
      ActiveSDTool(const std::string& type, const std::string& name,
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
      std::vector<std::string> m_stacVolumes;
      std::vector<std::string> m_presBarVolumes;
      std::vector<std::string> m_posIWVolumes;
      std::vector<std::string> m_negIWVolumes;
      std::vector<std::string> m_posOWVolumes;
      std::vector<std::string> m_negOWVolumes;
      std::vector<std::string> m_presECVolumes;
      std::vector<std::string> m_bobVolumes;
      std::vector<std::string> m_fcal1Volumes;
      std::vector<std::string> m_fcal2Volumes;
      std::vector<std::string> m_fcal3Volumes;
      std::vector<std::string> m_sliceVolumes;
      std::vector<std::string> m_miniVolumes;
      /// @}

      ServiceHandle<ILArCalibCalculatorSvc> m_bpsmodcalc; //LArG4::BarrelPresampler::CalibrationCalculator
      ServiceHandle<ILArCalibCalculatorSvc> m_embcalc;    //LArG4::Barrel::CalibrationCalculator
      ServiceHandle<ILArCalibCalculatorSvc> m_emepiwcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emeniwcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emepowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emenowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1)
      ServiceHandle<ILArCalibCalculatorSvc> m_emepscalc; //LArG4::EC::PresamplerCalibrationCalculator
      ServiceHandle<ILArCalibCalculatorSvc> m_emeobarcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::BackOuterBarretteWheelCalib, 1)
      ServiceHandle<ILArCalibCalculatorSvc> m_heccalc;   //LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelActive)
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal1calc;
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal2calc;
      ServiceHandle<ILArCalibCalculatorSvc> m_fcal3calc;
      ServiceHandle<ILArCalibCalculatorSvc> m_minfcalcalc;  //LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kActive)
  }; // class ActiveSDTool

} // namespace LArG4

#endif
