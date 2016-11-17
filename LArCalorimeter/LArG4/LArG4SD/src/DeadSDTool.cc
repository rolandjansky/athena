/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DeadSDTool.h"

#include "LArG4Code/SDWrapper.h"

// The calculators that I need...
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"
#include "LArG4Barrel/CryostatCalibrationCalculator.h"
#include "LArG4Barrel/CryostatCalibrationMixedCalculator.h"
#include "LArG4Barrel/CryostatCalibrationLArCalculator.h"
#include "LArG4Barrel/DMCalibrationCalculator.h"
#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4EC/CryostatCalibrationCalculator.h"
#include "LArG4EC/CryostatCalibrationMixedCalculator.h"
#include "LArG4EC/CryostatCalibrationLArCalculator.h"
#include "LArG4EC/EMECSupportCalibrationCalculator.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"
#include "LArG4Code/CalibrationDefaultCalculator.h"

// For escaped energy
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "CaloG4Sim/CalibrationDefaultProcessing.h"
#include "LArG4Code/EscapedEnergyProcessing.h"

namespace LArG4
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  DeadSDTool::DeadSDTool(const std::string& type, const std::string& name,
                         const IInterface* parent)
    : CalibSDTool(type, name, parent),
      m_hitCollName("LArCalibrationHitDeadMaterial")
  {
    declareProperty("HitCollectionName", m_hitCollName);
    declareProperty("BarrelCryVolumes", m_barCryVolumes);
    declareProperty("BarrelCryLArVolumes", m_barCryLArVolumes);
    declareProperty("BarrelCryMixVolumes", m_barCryMixVolumes);
    declareProperty("DeadMaterialVolumes", m_DMVolumes);
    declareProperty("BarrelPresVolumes", m_barPresVolumes);
    declareProperty("BarrelVolumes", m_barVolumes);
    declareProperty("ECCryVolumes", m_ECCryVolumes);
    declareProperty("ECCryLArVolumes", m_ECCryLArVolumes);
    declareProperty("ECCryMixVolumes", m_ECCryMixVolumes);
    declareProperty("ECSupportVolumes", m_ECSupportVolumes);
    declareProperty("HECWheelVolumes", m_HECWheelVolumes);
    declareProperty("doEscapedEnergy", m_do_eep=false);
  }

  //---------------------------------------------------------------------------
  // Create SDs for the current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* DeadSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArDeadSDWrapper", m_hitCollName);

    // Create the SDs. Lots of singleton calculators !!!
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::Dead",
                                new BarrelCryostat::CalibrationCalculator(),
                                m_barCryVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::LAr::Dead",
                                new BarrelCryostat::CalibrationLArCalculator(),
                                m_barCryLArVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::Mixed::Dead",
                                new BarrelCryostat::CalibrationMixedCalculator(),
                                m_barCryMixVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::DM::Dead",
                                new DM::CalibrationCalculator(),
                                m_DMVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::Barrel::Presampler::Dead",
                                new BarrelPresampler::CalibrationCalculator(),
                                m_barPresVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::Barrel::Dead",
                                new Barrel::CalibrationCalculator(),
                                m_barVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::Dead",
                                new EndcapCryostat::CalibrationCalculator(),
                                m_ECCryVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::LAr::Dead",
                                new EndcapCryostat::CalibrationLArCalculator(),
                                m_ECCryLArVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::Mixed::Dead",
                                new EndcapCryostat::CalibrationMixedCalculator(),
                                m_ECCryMixVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EMECSupport::Dead",
                                new EMECSupportCalibrationCalculator(),
                                m_ECSupportVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::HEC::Wheel::Inactive",
                                new HEC::LArHECCalibrationWheelCalculator(HEC::kWheelDead),
                                m_HECWheelVolumes ) );

    // Take care of the default material
    if (m_do_eep) {
      const std::vector<std::string> noVolumes;
      auto uninstSD = makeOneSD("Default::Dead::Uninstrumented::Calibration::Region",
                                new CalibrationDefaultCalculator(),
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

      sdWrapper->addSD( std::move(uninstSD) );
    }

    return sdWrapper;
  }

} // namespace LArG4
