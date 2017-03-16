/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004ActiveSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004ActiveSDTool::H62004ActiveSDTool(const std::string& type, const std::string& name,
                                         const IInterface *parent)
    : H62004CalibSDTool(type, name, parent)
    , m_hitCollName("LArCalibrationHitActive")
    , m_emepiwcalc("EMECPosInnerWheelCalibrationCalculator", name)
    , m_heccalc("LocalCalibrationActiveCalculator", name)
    , m_fcal1calc("LArFCAL1H62004CalibCalculator", name)
    , m_fcal2calc("LArFCAL2H62004CalibCalculator", name)
    , m_fcalcoldcalc("LArG4H6COLDTCMod0CalibCalculator", name)
  {
    declareProperty("EMECPosIWCalibrationCalculator", m_emepiwcalc);
    declareProperty("HECWheelActiveCalculator", m_heccalc);
    declareProperty("FCAL1CalibCalculator", m_fcal1calc);
    declareProperty("FCAL2CalibCalculator", m_fcal2calc);
    declareProperty("FCALCOLDMod0CalibCalculator", m_fcalcoldcalc);
    declareProperty("EMECVolumes", m_emecVolumes);
    declareProperty("HECVolumes", m_hecVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCALColdVolumes", m_fcalColdVolumes);
  }

  StatusCode H62004ActiveSDTool::initializeCalculators()
  {
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    ATH_CHECK(m_fcalcoldcalc.retrieve());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004ActiveSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArH62004ActiveSDWrapper", m_hitCollName);

    // Add the SDs.
    // Lots of singleton calculators !!!

    if (m_emecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "EMEC::InnerModule::Calibration::H6", &*m_emepiwcalc, m_emecVolumes ) );
    }
    if (m_hecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "HEC::Module::Depth::Slice::Local::Calibration::H6", &*m_heccalc, m_hecVolumes ) );
    }
    if (m_fcal1Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::FCAL::Module1::Gap::Calibration::H6", &*m_fcal1calc, m_fcal1Volumes ) );
    }
    if (m_fcal2Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::FCAL::Module2::Gap::Calibration::H6", &*m_fcal2calc, m_fcal2Volumes ) );
    }
    if (m_fcalColdVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::FCAL::ColdTC::Gap::Calibration::H6", &*m_fcalcoldcalc, m_fcalColdVolumes ) );
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
