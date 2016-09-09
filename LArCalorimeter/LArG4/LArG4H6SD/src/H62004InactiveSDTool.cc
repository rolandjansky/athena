/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004InactiveSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4HEC/LocalCalibrationCalculator.h"

// Local includes
#include "LArFCAL1H62004CalibCalculator.h"
#include "LArFCAL2H62004CalibCalculator.h"

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
  {
    declareProperty("EMECVolumes", m_emecVolumes);
    declareProperty("HECVolumes", m_hecVolumes);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
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
        "LAr::EMEC::InnerModule::Inactive::H6",
        new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1),
        m_emecVolumes ) );
    }
    if (m_hecVolumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::HEC::Local::Inactive::H6",
        new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocInactive),
        m_hecVolumes ) );
    }
    if (m_fcal1Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::FCAL::Inactive1::H6",
        LArFCAL1H62004CalibCalculator::GetCalculator(),
        m_fcal1Volumes ) );
    }
    if (m_fcal2Volumes.size() > 0) {
      sdWrapper->addSD( makeOneSD(
        "LAr::FCAL::Inactive2::H6",
        LArFCAL2H62004CalibCalculator::GetCalculator(),
        m_fcal2Volumes ) );
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
