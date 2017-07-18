/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HECSDTool.h"

#include "LArG4Code/SDWrapper.h"


namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  HECSDTool::HECSDTool(const std::string& type, const std::string& name,
                       const IInterface* parent)
    : SimpleSDTool(type, name, parent)
    , m_heccalc("HECWheelCalculator", name)
  {
    //declareProperty("SliceVolumes", m_sliceVolumes);
    //declareProperty("LocalVolumes", m_localVolumes);
    declareProperty("WheelVolumes", m_wheelVolumes);

    declareProperty("HECWheelCalculator", m_heccalc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode HECSDTool::initializeCalculators()
  {
    ATH_CHECK(m_heccalc.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* HECSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArHECSDWrapper", m_outputCollectionNames[0]);

    // Add the SDs
    //sdWrapper->addSD( makeOneSD("LAr::HEC::Module::Depth::Slice",
    //                            LArHECCalculator::GetCalculator(),
    //                            m_sliceVolumes) );
    //sdWrapper->addSD( makeOneSD("LAr::HEC::Module::Depth::Slice::Local",
    //                            LArHECLocalCalculator::GetCalculator(),
    //                            m_localVolumes) );

    sdWrapper->addSD( makeOneSD("LAr::HEC::Module::Depth::Slice::Wheel", &*m_heccalc, m_wheelVolumes) );

    return sdWrapper;
  }

} // namespace LArG4
