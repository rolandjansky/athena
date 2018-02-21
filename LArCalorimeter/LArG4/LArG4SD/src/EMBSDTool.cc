/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMBSDTool.h"

// Project includes
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EMBSDTool::EMBSDTool(const std::string& type, const std::string& name,
                       const IInterface* parent)
    : SimpleSDTool(type, name, parent)
    , m_embcalc("EMBCalculator", name)
    , m_pscalc("EMBPresamplerCalculator", name)
  {
    declareProperty("StacVolumes", m_stacVolumes);
    declareProperty("PresamplerVolumes", m_presVolumes);

    declareProperty("EMBCalculator",m_embcalc);
    declareProperty("EMBPSCalculator",m_pscalc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode EMBSDTool::initializeCalculators()
  {
    ATH_CHECK(m_pscalc.retrieve());
    ATH_CHECK(m_embcalc.retrieve());

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* EMBSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArEMBSDWrapper", m_outputCollectionNames[0]);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("LAr::Barrel::Presampler::Module", &*m_pscalc, m_presVolumes) );
    sdWrapper->addSD( makeOneSD("LAr::EMB::STAC", &*m_embcalc, m_stacVolumes) );

    // Setup frozen shower SD
    if(useFrozenShowers())
      {
        sdWrapper->addFastSimSD("BarrelFastSimDedicatedSD");
      }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
