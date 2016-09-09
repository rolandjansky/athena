/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004ActiveSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4HEC/LocalCalibrationCalculator.h"

// Local includes
#include "LArFCAL1H62004CalibCalculator.h"
#include "LArFCAL2H62004CalibCalculator.h"
#include "LArG4H6COLDTCMod0CalibCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004ActiveSDTool::H62004ActiveSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitActive")
  {
    declareProperty("EMECVolumes", m_emecVolumes);
    declareProperty("HECVolumes", m_hecVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCALColdVolumes", m_fcalColdVolumes);
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
        "EMEC::InnerModule::Calibration::H6",
        new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1),
        m_emecVolumes ) );
    }
    if (m_hecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "HEC::Module::Depth::Slice::Local::Calibration::H6",
        new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocActive),
        m_hecVolumes ) );
    }
    if (m_fcal1Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::FCAL::Module1::Gap::Calibration::H6",
        LArFCAL1H62004CalibCalculator::GetCalculator(),
        m_fcal1Volumes ) );
    }
    if (m_fcal2Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::FCAL::Module2::Gap::Calibration::H6",
        LArFCAL2H62004CalibCalculator::GetCalculator(),
        m_fcal2Volumes ) );
    }
    if (m_fcalColdVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::FCAL::ColdTC::Gap::Calibration::H6",
        new LArG4H6COLDTCMod0CalibCalculator(),
        m_fcalColdVolumes ) );
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
