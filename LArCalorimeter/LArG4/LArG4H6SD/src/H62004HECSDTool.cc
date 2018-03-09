/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004HECSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004HECSDTool::H62004HECSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004SimpleSDTool(type, name, parent)
    , m_hitCollName("LArHitHEC")
    , m_calculator("LArH62004HECLocalCalculator", name)
  {
    declareProperty("Calculator", m_calculator);
  }

  StatusCode H62004HECSDTool::initializeCalculators()
  {
    ATH_CHECK(m_calculator.retrieve());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004HECSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArH62004HECSDWrapper", m_hitCollName);

    // Add the SDs. In this case there is actually only one SD.
    sdWrapper->addSD( makeOneSD( "LAr::HEC::Module::Depth::Slice::Local::H6", &*m_calculator, m_volumeNames ) );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
