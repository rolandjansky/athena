/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "H62004DeadSDTool.h"

// LArG4 includes
#include "LArG4Code/SDWrapper.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "LArG4Code/EscapedEnergyProcessing.h"

// Local includes
#include "LArG4H62004DeadCalibrationCalculator.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  H62004DeadSDTool::H62004DeadSDTool(const std::string& type, const std::string& name,
                                     const IInterface *parent)
    : H62004CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitDeadMaterial")
  {
    declareProperty("doEscapedEnergy", m_do_eep=false);
  }

  //---------------------------------------------------------------------------
  // Create the SD wrapper for current worker thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* H62004DeadSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArH62004DeadSDWrapper", m_hitCollName);

    // Add the SDs
    sdWrapper->addSD( makeOneSD( "LArDead::Dead::H6",
                                 new LArG4H62004DeadCalibrationCalculator(),
                                 m_volumeNames ) );

    // Take care of the default material
    if (m_do_eep) {
      const std::vector<std::string> noVolumes;
      auto uninstSD = makeOneSD("Default::Dead::Uninstrumented::Calibration::H6",
                                new LArG4H62004DeadCalibrationCalculator(),
                                noVolumes);

      // Initialize the escaped energy processing for LAr volumes.
      // This is from initialize processing in the former LArG4CalibSD.
      // I still think we can do better than this, though.
      // FIXME: I don't think this is thread safe!!
      ATH_MSG_DEBUG("Creating EscapedEnergyProcessing and adding to registry");
      CaloG4::VEscapedEnergyProcessing* eep =
        new EscapedEnergyProcessing( uninstSD.get() );
      CaloG4::EscapedEnergyRegistry* registry =
        CaloG4::EscapedEnergyRegistry::GetInstance();
      registry->AddAndAdoptProcessing( "LAr::", eep );
      CaloG4::VEscapedEnergyProcessing* eep1 =
        new EscapedEnergyProcessing( uninstSD.get() );
      registry->AddAndAdoptProcessing( "LArMgr::", eep1 );
      CaloG4::VEscapedEnergyProcessing* eep2 =
        new EscapedEnergyProcessing( uninstSD.get() );
      registry->AddAndAdoptProcessing( "LAr", eep2 );

      sdWrapper->addSD( std::move(uninstSD) );
    }

    // Return the wrapper as my SD
    return sdWrapper;
  }

} // namespace LArG4
