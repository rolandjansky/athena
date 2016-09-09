/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004HECSDTool.h"

// LArG4 includes
#include "LArG4HEC/LArHECLocalCalculator.h"
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004HECSDTool::H62004HECSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitHEC")
  {
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004HECSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArH62004HECSDWrapper", m_hitCollName);

    // Get the calculator and set some parameters. Probably not thread safe.
    auto aCalc = LArHECLocalCalculator::GetCalculator();
    aCalc->SetOutOfTimeCut(10000.);
    aCalc->SetX(true);

    // Add the SDs. In this case there is actually only one SD.
    sdWrapper->addSD( makeOneSD( "LAr::HEC::Module::Depth::Slice::Local::H6",
                                 aCalc, m_volumeNames ) );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
