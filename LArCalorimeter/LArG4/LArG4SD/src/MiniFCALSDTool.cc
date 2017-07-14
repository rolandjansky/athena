/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MiniFCALSDTool.h"

#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MiniFCALSDTool::MiniFCALSDTool(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : SimpleSDTool(type, name, parent)
    , m_calc("MiniFCALCalculator", name)
  {
    declareProperty("MiniVolumes", m_miniVolumes);

    declareProperty("MiniFCALCalculator", m_calc);
  }

  StatusCode MiniFCALSDTool::initializeCalculators()
  {
    ATH_CHECK(m_calc.retrieve());
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* MiniFCALSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArMiniFCALSDWrapper", m_outputCollectionNames[0]);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("MiniFCAL::Wafer", &*m_calc, m_miniVolumes) );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
