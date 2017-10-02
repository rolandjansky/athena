/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSD_LARFCS_STEPINFOSDTOOL_H
#define ISF_FASTCALOSIMSD_LARFCS_STEPINFOSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "FCS_StepInfoSDTool.h"
#include "LArG4Code/ILArCalculatorSvc.h"

namespace FCS_Param
{

  /// @class LArFCS_StepInfoSDTool
  /// @brief Sensitive detector tool which manages activate-area LAr calib SDs.
  ///
  /// Design is in flux.
  ///
  class LArFCS_StepInfoSDTool : public FCS_StepInfoSDTool
  {

  public:

    /// Constructor
    LArFCS_StepInfoSDTool(const std::string& type, const std::string& name,
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

    ServiceHandle<ILArCalculatorSvc> m_bpsmodcalc; //LArG4::BarrelPresampler::CalibrationCalculator
    ServiceHandle<ILArCalculatorSvc> m_embcalc;    //LArG4::Barrel::CalibrationCalculator
    ServiceHandle<ILArCalculatorSvc> m_emepiwcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1)
    ServiceHandle<ILArCalculatorSvc> m_emeniwcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1)
    ServiceHandle<ILArCalculatorSvc> m_emepowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1)
    ServiceHandle<ILArCalculatorSvc> m_emenowcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1)
    ServiceHandle<ILArCalculatorSvc> m_emepscalc; //LArG4::EC::PresamplerCalibrationCalculator
    ServiceHandle<ILArCalculatorSvc> m_emeobarcalc; //LArG4::EC::CalibrationCalculator(LArWheelCalculator::BackOuterBarretteWheelCalib, 1)
    ServiceHandle<ILArCalculatorSvc> m_heccalc;   //LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelActive)
    ServiceHandle<ILArCalculatorSvc> m_fcal1calc;
    ServiceHandle<ILArCalculatorSvc> m_fcal2calc;
    ServiceHandle<ILArCalculatorSvc> m_fcal3calc;
    ServiceHandle<ILArCalculatorSvc> m_minfcalcalc;  //LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kActive)

  }; // class LArFCS_StepInfoSDTool

} // namespace FCS_Param

#endif // ISF_FASTCALOSIMSD_LARFCS_STEPINFOSDTOOL_H
