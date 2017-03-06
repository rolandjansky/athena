/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DeadSDTool.h"

#include "LArG4Code/SDWrapper.h"

#include "CalibrationDefaultCalculator.h"

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
    : CalibSDTool(type, name, parent)
    , m_hitCollName("LArCalibrationHitDeadMaterial")
    , m_embccalc("BarrelCryostatCalibrationCalculator", name)
    , m_embclarcalc("BarrelCryostatCalibrationLArCalculator", name)
    , m_mixcalc("BarrelCryostatCalibrationMixedCalculator", name)
    , m_dmcalc("DMCalibrationCalculator", name)
    , m_embpscalc("BarrelPresamplerCalibrationCalculator", name)
    , m_embcalc("BarrelCalibrationCalculator", name)
    , m_emeccalc("EndcapCryostatCalibrationCalculator", name)
    , m_emecclarcalc("EndcapCryostatCalibrationLArCalculator", name)
    , m_ememixcalc("EndcapCryostatCalibrationMixedCalculator", name)
    , m_emesupcalc("EMECSupportCalibrationCalculator", name)
    , m_heccalc("HECCalibrationWheelDeadCalculator", name)
    , m_defcalc("CalibrationDefaultCalculator", name)
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

    declareProperty("EMBCryoCalibrationCalculator",m_embccalc);
    declareProperty("EMBCryoLArCalibrationCalculator",m_embclarcalc);
    declareProperty("EMBCryoMixCalibrationCalculator",m_mixcalc);
    declareProperty("DMCalibrationCalculator",m_dmcalc);
    declareProperty("EMBPSCalibrationCalculator",m_embpscalc);
    declareProperty("EMBCalibrationCalculator",m_embcalc);
    declareProperty("ECCryoCalibrationCalculator",m_emeccalc);
    declareProperty("ECCryoLArCalibrationCalculator",m_emecclarcalc);
    declareProperty("ECCryoMixCalibrationCalculator",m_ememixcalc);
    declareProperty("EMECSuppCalibrationCalculator",m_emesupcalc);
    declareProperty("HECWheelDeadCalculator",m_heccalc);
    declareProperty("DefaultCalibrationCalculator",m_defcalc);
  }

  //---------------------------------------------------------------------------
  // Initialization of Athena-components
  //---------------------------------------------------------------------------
  StatusCode DeadSDTool::initializeCalculators()
  {
    // Lots of calculators !!!
    ATH_CHECK(m_embccalc.retrieve());
    ATH_CHECK(m_embclarcalc.retrieve());
    ATH_CHECK(m_mixcalc.retrieve());
    ATH_CHECK(m_dmcalc.retrieve());
    ATH_CHECK(m_embpscalc.retrieve());
    ATH_CHECK(m_embcalc.retrieve());
    ATH_CHECK(m_emeccalc.retrieve());
    ATH_CHECK(m_emecclarcalc.retrieve());
    ATH_CHECK(m_ememixcalc.retrieve());
    ATH_CHECK(m_emesupcalc.retrieve());
    ATH_CHECK(m_heccalc.retrieve());
    // Take care of the default material
    if (m_do_eep)
      {
        ATH_CHECK(m_defcalc.retrieve());
      }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create SDs for the current thread
  //---------------------------------------------------------------------------
  G4VSensitiveDetector* DeadSDTool::makeSD()
  {
    // Create the wrapper
    auto sdWrapper = new CalibSDWrapper("LArDeadSDWrapper", m_hitCollName);

    // Create the SDs.
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::Dead", &*m_embccalc, m_barCryVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::LAr::Dead", &*m_embclarcalc, m_barCryLArVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::BarrelCryostat::Mixed::Dead", &*m_mixcalc, m_barCryMixVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::DM::Dead", &*m_dmcalc, m_DMVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::Barrel::Presampler::Dead", &*m_embpscalc, m_barPresVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::Barrel::Dead", &*m_embcalc, m_barVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::Dead", &*m_emeccalc, m_ECCryVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::LAr::Dead", &*m_emecclarcalc, m_ECCryLArVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EndcapCryostat::Mixed::Dead", &*m_ememixcalc, m_ECCryMixVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::EMECSupport::Dead", &*m_emesupcalc, m_ECSupportVolumes ) );
    sdWrapper->addSD( makeOneSD("LArDead::HEC::Wheel::Inactive", &*m_heccalc, m_HECWheelVolumes ) );

    // Take care of the default material
    if (m_do_eep)
      {
        const std::vector<std::string> noVolumes;
        auto uninstSD = makeOneSD("Default::Dead::Uninstrumented::Calibration::Region", &*m_defcalc, noVolumes);

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
