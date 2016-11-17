/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ActiveSDTool.h"

#include "LArG4Code/SDWrapper.h"

// All the calculators I need
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"
#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4EC/PresamplerCalibrationCalculator.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4EC/PresamplerCalibrationCalculator.h"
#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCAL2CalibCalculator.h"
#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4MiniFCAL/MiniFCALCalibrationCalculator.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  ActiveSDTool::ActiveSDTool(const std::string& type, const std::string& name,
                             const IInterface *parent)
    : CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitActive")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("StacVolumes", m_stacVolumes);
    declareProperty("PresamplerVolumes", m_presBarVolumes);
    declareProperty("PosIWVolumes", m_posIWVolumes);
    declareProperty("NegIWVolumes", m_negIWVolumes);
    declareProperty("PosOWVolumes", m_posOWVolumes);
    declareProperty("NegOWVolumes", m_negOWVolumes);
    declareProperty("PresVolumes", m_presECVolumes);
    declareProperty("BOBarretteVolumes", m_bobVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCAL3Volumes", m_fcal3Volumes);
    declareProperty("SliceVolumes", m_sliceVolumes);
    declareProperty("MiniVolumes", m_miniVolumes);
  }

  //---------------------------------------------------------------------------
  // Create SD wrapper for current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* ActiveSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArActiveSDWrapper", m_hitCollName);

    // Create the SDs.
    // Lots of singleton calculators !!!
    sdWrapper->addSD( makeOneSD(
      "Barrel::Presampler::Module::Calibration",
      new BarrelPresampler::CalibrationCalculator(),
      m_presBarVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMB::STAC::Calibration",
      new Barrel::CalibrationCalculator(),
      m_stacVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMEC::Pos::InnerWheel::Calibration",
      new EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1),
      m_posIWVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMEC::Neg::InnerWheel::Calibration",
      new EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1),
      m_negIWVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMEC::Pos::OuterWheel::Calibration",
      new EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1),
      m_posOWVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMEC::Neg::OuterWheel::Calibration",
      new EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1),
      m_negOWVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "Endcap::Presampler::LiquidArgon::Calibration",
      new EC::PresamplerCalibrationCalculator(),
      m_presECVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "EMEC::BackOuterBarrette::Calibration",
      new EC::CalibrationCalculator(LArWheelCalculator::BackOuterBarretteWheelCalib, 1),
      m_bobVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "FCAL::Module1::Gap::Calibration",
      FCAL::LArFCAL1CalibCalculator::GetCalculator(),
      m_fcal1Volumes ) );
    sdWrapper->addSD( makeOneSD(
      "FCAL::Module2::Gap::Calibration",
      FCAL::LArFCAL2CalibCalculator::GetCalculator(),
      m_fcal2Volumes ) );
    sdWrapper->addSD( makeOneSD(
      "FCAL::Module3::Gap::Calibration",
      FCAL::LArFCAL3CalibCalculator::GetCalculator(),
      m_fcal3Volumes ) );
    sdWrapper->addSD( makeOneSD(
      "HEC::Module::Depth::Slice::Wheel::Calibration",
      new HEC::LArHECCalibrationWheelCalculator(HEC::kWheelActive),
      m_sliceVolumes ) );
    sdWrapper->addSD( makeOneSD(
      "MiniFCAL::Wafer",
      new MiniFCAL::MiniFCALCalibrationCalculator(MiniFCAL::kActive),
      m_miniVolumes ) );

    return sdWrapper;
  }

} // namespace LArG4
