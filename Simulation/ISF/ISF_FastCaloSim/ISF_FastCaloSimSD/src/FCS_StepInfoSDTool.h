/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSD_FCS_STEPINFOSDTOOL_H
#define ISF_FASTCALOSIMSD_FCS_STEPINFOSDTOOL_H

/// @file FCS_StepInfoSDTool.h
/// @brief Defines the FCS_StepInfoSDTool class
/// @author Steve Farrell <Steven.Farrell@cern.ch>
/// @date 2016-03-26

// System includes
#include <string>
#include <vector>

// G4Atlas includes
#include "G4AtlasTools/SensitiveDetectorBase.h"

// Local includes
#include "FCS_StepInfoSD.h"

// Forward declarations
class ILArCalculatorSvc;
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class ITileCalculator;

namespace FCS_Param
{

  /// @class FCS_StepInfoSDTool
  /// @brief A base class for tools that manage FCS_StepInfoSDs.
  ///
  /// @todo Add more details.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class FCS_StepInfoSDTool : public SensitiveDetectorBase
  {

  public:

    /// Constructor
    FCS_StepInfoSDTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

    /// Initialize the tool
    StatusCode initialize() override final;

    /// Calls down to all the SDs to pack their hits into one collection
    StatusCode Gather() override final;

  private:

    /// Create the SD wrapper for current worker thread
    G4VSensitiveDetector* makeSD() override final;

    /// Initialize Calculator Services
    virtual StatusCode initializeCalculators();

    /// Helper method to create one SD
    std::unique_ptr<FCS_StepInfoSD>
    makeOneLArSD(const std::string& name, ILArCalculatorSvc* calc,
              const std::vector<std::string>& volumes) const;

    /// Helper method to create one SD
    std::unique_ptr<FCS_StepInfoSD>
    makeOneTileSD(const std::string& name, ITileCalculator* calc,
                  const std::vector<std::string>& volumes) const;

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
    std::vector<std::string> m_tileVolumes;
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
    ServiceHandle<ITileCalculator> m_tileCalculator;

    /// @name Calo identifier helpers
    /// @{
    const LArEM_ID*       m_larEmID;
    const LArFCAL_ID*     m_larFcalID;
    const LArHEC_ID*      m_larHecID;
    const LArMiniFCAL_ID* m_larMiniFcalID;
    /// @}
    FCS_Param::Config m_config;

  }; // class FCS_StepInfoSDTool

} // namespace FCS_Param

#endif
