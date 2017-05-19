/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004EMECSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004EMECSDTool::H62004EMECSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004SimpleSDTool(type, name, parent)
    , m_hitCollName("LArHitEMEC")
    , m_calculator("EMECPosInnerWheel_ECOR_GADJCalculator", name)
  {
    declareProperty("Calculator", m_calculator);
  }

  StatusCode H62004EMECSDTool::initializeCalculators()
  {
    ATH_CHECK(m_calculator.retrieve());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004EMECSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArH62004EMECSDWrapper", m_hitCollName);

    // Add the SDs. In this case there is actually only one SD.
    sdWrapper->addSD(
      makeOneSD( "LAr::EMEC::InnerModule::H6", &*m_calculator, m_volumeNames )
    );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
