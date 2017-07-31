/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FCALSDTool.h"

#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  FCALSDTool::FCALSDTool(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : SimpleSDTool(type, name, parent)
    , m_fcal1calc("FCAL1Calculator", name)
    , m_fcal2calc("FCAL2Calculator", name)
    , m_fcal3calc("FCAL3Calculator", name)
  {
    declareProperty("FCAL1Volumes", m_fcal1Volumes);
    declareProperty("FCAL2Volumes", m_fcal2Volumes);
    declareProperty("FCAL3Volumes", m_fcal3Volumes);

    declareProperty("FCAL1Calculator", m_fcal1calc);
    declareProperty("FCAL2Calculator", m_fcal2calc);
    declareProperty("FCAL3Calculator", m_fcal3calc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode FCALSDTool::initializeCalculators()
  {
    ATH_CHECK(m_fcal1calc.retrieve());
    ATH_CHECK(m_fcal2calc.retrieve());
    ATH_CHECK(m_fcal3calc.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* FCALSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArFCALSDWrapper", m_outputCollectionNames[0]);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module1::Gap", &*m_fcal1calc, m_fcal1Volumes) );
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module2::Gap", &*m_fcal2calc, m_fcal2Volumes) );
    sdWrapper->addSD( makeOneSD("LAr::FCAL::Module3::Gap", &*m_fcal3calc, m_fcal3Volumes) );

    // Setup frozen shower SD
    if(useFrozenShowers())
      {
        sdWrapper->addFastSimSD("FCALFastSimDedicatedSD");
      }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
