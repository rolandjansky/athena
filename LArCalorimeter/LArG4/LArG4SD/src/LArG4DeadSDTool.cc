/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4DeadSDTool.h"

#include "LArG4Code/LArG4CalibSD.h"

#include "LArG4Code/LArG4Hit.h"
#include "CxxUtils/make_unique.h"

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

LArG4DeadSDTool::LArG4DeadSDTool(const std::string& type, const std::string& name,
                                 const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitDeadMaterial")
  , m_barCrySD(nullptr)
  , m_barCryLArSD(nullptr)
  , m_barCryMixSD(nullptr)
  , m_DMSD(nullptr)
  , m_barPresSD(nullptr)
  , m_barSD(nullptr)
  , m_ECCrySD(nullptr)
  , m_ECCryLArSD(nullptr)
  , m_ECCryMixSD(nullptr)
  , m_ECSupportSD(nullptr)
  , m_HECWheelSD(nullptr)
  , m_uninstSD(nullptr)
{
  declareProperty("BarrelCryVolumes",m_barCryVolumes);
  declareProperty("BarrelCryLArVolumes",m_barCryLArVolumes);
  declareProperty("BarrelCryMixVolumes",m_barCryMixVolumes);
  declareProperty("DeadMaterialVolumes",m_DMVolumes);
  declareProperty("BarrelPresVolumes",m_barPresVolumes);
  declareProperty("BarrelVolumes",m_barVolumes);
  declareProperty("ECCryVolumes",m_ECCryVolumes);
  declareProperty("ECCryLArVolumes",m_ECCryLArVolumes);
  declareProperty("ECCryMixVolumes",m_ECCryMixVolumes);
  declareProperty("ECSupportVolumes",m_ECSupportVolumes);
  declareProperty("HECWheelVolumes",m_HECWheelVolumes);
  //declareProperty("HECVolumes",m_HECVolumes={"LArDead::HEC::Dead"});
  //declareProperty("HECLocalVolumes",m_HECLocVolumes={"LArDead::HEC::Local::Dead"});
  //declareProperty("MiniMomVolumes",m_miniMomVolumes={"MiniFCALMother::Dead"});
  //declareProperty("MiniVolumes",m_miniVolumes={"MiniFCAL::Dead"});
  //declareProperty("MiniLayVolumes",m_miniLayVolumes={"MiniFCAL::Layer::Dead"});
  declareProperty("doEscapedEnergy",m_do_eep=false);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4DeadSDTool::initializeSD()
{
  // Lots of singleton calculators !!!
  m_barCrySD    = new LArG4CalibSD( "LArDead::BarrelCryostat::Dead" , new LArG4::BarrelCryostat::CalibrationCalculator() , m_doPID);
  m_barCryLArSD = new LArG4CalibSD( "LArDead::BarrelCryostat::LAr::Dead" , new LArG4::BarrelCryostat::CalibrationLArCalculator() , m_doPID);
  m_barCryMixSD = new LArG4CalibSD( "LArDead::BarrelCryostat::Mixed::Dead" , new LArG4::BarrelCryostat::CalibrationMixedCalculator() , m_doPID);
  m_DMSD        = new LArG4CalibSD( "LArDead::DM::Dead" , new LArG4::DM::CalibrationCalculator() , m_doPID);
  m_barPresSD   = new LArG4CalibSD( "LArDead::Barrel::Presampler::Dead" , new LArG4::BarrelPresampler::CalibrationCalculator() , m_doPID);
  m_barSD       = new LArG4CalibSD( "LArDead::Barrel::Dead" , new LArG4::Barrel::CalibrationCalculator() , m_doPID);
  m_ECCrySD     = new LArG4CalibSD( "LArDead::EndcapCryostat::Dead" , new LArG4::EndcapCryostat::CalibrationCalculator() , m_doPID);
  m_ECCryLArSD  = new LArG4CalibSD( "LArDead::EndcapCryostat::LAr::Dead" , new LArG4::EndcapCryostat::CalibrationLArCalculator() , m_doPID);
  m_ECCryMixSD  = new LArG4CalibSD( "LArDead::EndcapCryostat::Mixed::Dead" , new LArG4::EndcapCryostat::CalibrationMixedCalculator() , m_doPID);
  m_ECSupportSD = new LArG4CalibSD( "LArDead::EMECSupport::Dead" , new LArG4::EMECSupportCalibrationCalculator() , m_doPID);
  m_HECWheelSD  = new LArG4CalibSD( "LArDead::HEC::Wheel::Inactive" , new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelDead) , m_doPID);
  //m_HECSD       = new LArG4CalibSD( "LArDead::HEC::Inactive" , new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kDead) , m_doPID);
  //m_HECLocSD    = new LArG4CalibSD( "LArDead::HEC::Local::Inactive" , new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocDead) , m_doPID);
  //m_miniMomSD   = new LArG4CalibSD( "MiniFCALMother::Dead" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead) , m_doPID);
  //m_miniSD      = new LArG4CalibSD( "MiniFCAL::Dead" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead) , m_doPID);
  //m_miniLaySD   = new LArG4CalibSD( "MiniFCAL::Layer::Dead" , new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead) , m_doPID);
  // Take care of the default material
  if (m_do_eep) m_uninstSD    = new LArG4CalibSD( "Default::Dead::Uninstrumented::Calibration::Region" , new LArG4::CalibrationDefaultCalculator() , m_doPID );
    
  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_barCrySD]    = &m_barCryVolumes;
  configuration[m_barCryLArSD] = &m_barCryLArVolumes;
  configuration[m_barCryMixSD] = &m_barCryMixVolumes;
  configuration[m_DMSD]        = &m_DMVolumes;
  configuration[m_barPresSD]   = &m_barPresVolumes;
  configuration[m_barSD]       = &m_barVolumes;
  configuration[m_ECCrySD]     = &m_ECCryVolumes;
  configuration[m_ECCryLArSD]  = &m_ECCryLArVolumes;
  configuration[m_ECCryMixSD]  = &m_ECCryMixVolumes;
  configuration[m_ECSupportSD] = &m_ECSupportVolumes;
  configuration[m_HECWheelSD]  = &m_HECWheelVolumes;
  //configuration[m_HECSD]       = &m_HECVolumes;
  //configuration[m_HECLocSD]    = &m_HECLocVolumes;
  //configuration[m_miniMomSD]   = &m_miniMomVolumes;
  //configuration[m_miniSD]      = &m_miniVolumes;
  //configuration[m_miniLaySD]   = &m_miniLayVolumes;
  if (m_do_eep) configuration[m_uninstSD]    = new std::vector<std::string>; // No volumes for this guy
  setupAllSDs(configuration);

  // Make sure they have the helpers they need
  setupHelpers(m_barCrySD);
  setupHelpers(m_barCryLArSD);
  setupHelpers(m_barCryMixSD);
  setupHelpers(m_DMSD);
  setupHelpers(m_barPresSD);
  setupHelpers(m_barSD);
  setupHelpers(m_ECCrySD);
  setupHelpers(m_ECCryLArSD);
  setupHelpers(m_ECCryMixSD);
  setupHelpers(m_ECSupportSD);
  setupHelpers(m_HECWheelSD);
  //setupHelpers(m_HECSD);
  //setupHelpers(m_HECLocSD);
  //setupHelpers(m_miniMomSD);
  //setupHelpers(m_miniSD);
  //setupHelpers(m_miniLaySD);
  if (m_do_eep) setupHelpers(m_uninstSD);

  // This is from initialize processing in the former LArG4CalibSD
  // I still think we can do better than this, though
  if (m_do_eep){
    ATH_MSG_INFO("new EscapedEnergyProcessing");
    // Initialize the escaped energy processing for LAr volumes.
    CaloG4::VEscapedEnergyProcessing* eep = new EscapedEnergyProcessing(m_uninstSD);
  
    ATH_MSG_INFO("EER instance");
    CaloG4::EscapedEnergyRegistry* registry = CaloG4::EscapedEnergyRegistry::GetInstance();
    ATH_MSG_INFO("addAndAdoptProcessing");
    registry->AddAndAdoptProcessing( "LAr::", eep );
  }

  return StatusCode::SUCCESS;
}

StatusCode LArG4DeadSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  m_barCrySD   ->EndOfAthenaEvent( &*m_HitColl );
  m_barCryLArSD->EndOfAthenaEvent( &*m_HitColl );
  m_barCryMixSD->EndOfAthenaEvent( &*m_HitColl );
  m_DMSD       ->EndOfAthenaEvent( &*m_HitColl );
  m_barPresSD  ->EndOfAthenaEvent( &*m_HitColl );
  m_barSD      ->EndOfAthenaEvent( &*m_HitColl );
  m_ECCrySD    ->EndOfAthenaEvent( &*m_HitColl );
  m_ECCryLArSD ->EndOfAthenaEvent( &*m_HitColl );
  m_ECCryMixSD ->EndOfAthenaEvent( &*m_HitColl );
  m_ECSupportSD->EndOfAthenaEvent( &*m_HitColl );
  m_HECWheelSD ->EndOfAthenaEvent( &*m_HitColl );
  //m_HECSD      ->EndOfAthenaEvent( &*m_HitColl );
  //m_HECLocSD   ->EndOfAthenaEvent( &*m_HitColl );
  //m_miniMomSD  ->EndOfAthenaEvent( &*m_HitColl );
  //m_miniSD     ->EndOfAthenaEvent( &*m_HitColl );
  //m_miniLaySD  ->EndOfAthenaEvent( &*m_HitColl );
  if (m_do_eep) m_uninstSD     ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
