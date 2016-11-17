/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MiniFCALSDTool.h"

#include "LArG4Code/SDWrapper.h"
#include "LArG4MiniFCAL/MiniFCALCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  MiniFCALSDTool::MiniFCALSDTool(const std::string& type, const std::string& name,
                                 const IInterface* parent)
    : SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitMiniFCAL")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("MiniVolumes", m_miniVolumes);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* MiniFCALSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArMiniFCALSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("MiniFCAL::Wafer",
                                MiniFCALCalculator::GetCalculator(),
                                m_miniVolumes) );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
