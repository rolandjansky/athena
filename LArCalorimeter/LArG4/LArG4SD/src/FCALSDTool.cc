/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCALSDTool.h"

#include "LArG4Code/SDWrapper.h"
#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCAL3Calculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  FCALSDTool::FCALSDTool(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitFCAL")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCAL3Volumes", m_fcal3Volumes);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* FCALSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArFCALSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module1::Gap",
                                LArFCAL1Calculator::GetInstance(),
                                m_fcal1Volumes) );
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module2::Gap",
                                LArFCAL2Calculator::GetInstance(),
                                m_fcal2Volumes) );
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module3::Gap",
                                LArFCAL3Calculator::GetInstance(),
                                m_fcal3Volumes) );

    // Setup frozen shower SD
    if(useFrozenShowers()) {
      sdWrapper->addFastSimSD("FCALFastSimDedicatedSD");
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
