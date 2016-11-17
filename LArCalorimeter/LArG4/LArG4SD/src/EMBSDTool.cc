/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EMBSDTool.h"

// Project includes
#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"
#include "LArG4Code/SDWrapper.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  EMBSDTool::EMBSDTool(const std::string& type, const std::string& name,
                       const IInterface* parent)
    : SimpleSDTool(type, name, parent),
      m_hitCollName("LArHitEMB")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("StacVolumes", m_stacVolumes);
    declareProperty("PresamplerVolumes", m_presVolumes);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* EMBSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new SimpleSDWrapper("LArEMBSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD( makeOneSD("LAr::Barrel::Presampler::Module",
                                LArBarrelPresamplerCalculator::GetCalculator(),
                                m_presVolumes) );
    sdWrapper->addSD( makeOneSD("LAr::EMB::STAC",
                                LArBarrelCalculator::GetCalculator(),
                                m_stacVolumes) );

    // Setup frozen shower SD
    if(useFrozenShowers()) {
      sdWrapper->addFastSimSD("BarrelFastSimDedicatedSD");
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
