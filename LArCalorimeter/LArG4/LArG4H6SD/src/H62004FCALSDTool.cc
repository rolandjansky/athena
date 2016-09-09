/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004FCALSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"
#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"

// Local includes
#include "LArG4H6COLDTCMod0Calculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004FCALSDTool::H62004FCALSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitFCAL")
  {
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCALColdVolumes", m_fcalColdVolumes);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004FCALSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArH62004FCALSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD( makeOneSD( "LAr::FCAL::Module1::Gap::H6",
                                 LArFCAL1Calculator::GetInstance(),
                                 m_fcal1Volumes ) );
    sdWrapper->addSD( makeOneSD( "LAr::FCAL::Module2::Gap::H6",
                                 LArFCAL2Calculator::GetInstance(),
                                 m_fcal2Volumes ) );
    // Probably not thread safe
    auto aCalc = LArG4H6COLDTCMod0Calculator::GetInstance();
    aCalc->SetOutOfTimeCut(10000.);
    sdWrapper->addSD( makeOneSD("LAr::FCAL::ColdTC::H6", aCalc, m_fcalColdVolumes) );

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
