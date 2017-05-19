/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004InactiveSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004InactiveSDTool::H62004InactiveSDTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : H62004CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitInactive")
    , m_emepiwcalc("EMECPosInnerWheelCalibrationCalculator", name)
    , m_heccalc("LocalCalibrationInactiveCalculator", name)
    , m_fcal1calc("LArFCAL1H62004CalibCalculator", name)
    , m_fcal2calc("LArFCAL2H62004CalibCalculator", name)
  {
    declareProperty("EMECVolumes", m_emecVolumes);
    declareProperty("HECVolumes", m_hecVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("EMECPosIWCalibrationCalculator", m_emepiwcalc);
    declareProperty("HECWheelInactiveCalculator", m_heccalc);
    declareProperty("FCAL1CalibCalculator", m_fcal1calc);
    declareProperty("FCAL2CalibCalculator", m_fcal2calc);
  }

  StatusCode H62004InactiveSDTool::initializeCalculators()
  {
    ATH_CHECK(m_emepiwcalc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004InactiveSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArH62004InactiveSDWrapper", m_hitCollName);

    // Add the SDs.
    // Lots of singleton calculators !!!

    if (m_emecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::EMEC::InnerModule::Inactive::H6", &*m_emepiwcalc, m_emecVolumes ) );
    }
    if (m_hecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::HEC::Local::Inactive::H6", &*m_heccalc, m_hecVolumes ) );
    }
    if (m_fcal1Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::FCAL::Inactive1::H6", &*m_fcal1calc, m_fcal1Volumes ) );
    }
    if (m_fcal2Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
                                  "LAr::FCAL::Inactive2::H6", &*m_fcal2calc, m_fcal2Volumes ) );
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
