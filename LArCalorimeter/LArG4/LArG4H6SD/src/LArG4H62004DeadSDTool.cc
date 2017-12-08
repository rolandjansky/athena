/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004DeadSDTool.h"

#include "LArG4H62004CalibSD.h"

#include "CxxUtils/make_unique.h"

// For escaped energy
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "LArG4Code/EscapedEnergyProcessing.h"

LArG4H62004DeadSDTool::LArG4H62004DeadSDTool(const std::string& type, const std::string& name, const IInterface *parent)
  : LArG4SDTool(type,name,parent)
  , m_HitColl("LArCalibrationHitDeadMaterial")
  , m_calculator("LArG4H62004DeadCalibrationCalculator", name)
  , m_deadSD(nullptr)
  , m_uninstSD(nullptr)
{
  declareProperty("doEscapedEnergy",m_do_eep=false);
  declareProperty("Calculator", m_calculator);
  declareInterface<ISensitiveDetector>(this);
}

StatusCode LArG4H62004DeadSDTool::initializeCalculators()
{
  ATH_CHECK(m_calculator.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004DeadSDTool::initializeSD()
{
  // Lots of singleton calculators !!!
  m_deadSD  = new LArG4H62004CalibSD( "LArDead::Dead::H6" , &*m_calculator, m_doPID);
  // Take care of the default material
  if (m_do_eep) m_uninstSD    = new LArG4H62004CalibSD( "Default::Dead::Uninstrumented::Calibration::H6" , &*m_calculator, m_doPID );

  std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration;
  configuration[m_deadSD]  = &m_volumeNames;
  if (m_do_eep) configuration[m_uninstSD]    = new std::vector<std::string>; // No volumes for this guy
  setupAllSDs(configuration);

  // Make sure they have the helpers they need
  setupHelpers(m_deadSD);
  if (m_do_eep) setupHelpers(m_uninstSD);

  // This is from initialize processing in the former LArG4CalibSD
  // I still think we can do better than this, though
  if (m_do_eep){
    std::cout<<"new EscapedEnergyProcessing"<<std::endl;
    // Initialize the escaped energy processing for LAr volumes.
    CaloG4::VEscapedEnergyProcessing* eep = new EscapedEnergyProcessing(m_uninstSD);

    std::cout<<"EER instance"<<std::endl;
    CaloG4::EscapedEnergyRegistry* registry = CaloG4::EscapedEnergyRegistry::GetInstance();
    registry->AddAndAdoptProcessing( "LAr::", eep );
    CaloG4::VEscapedEnergyProcessing* eep1 = new EscapedEnergyProcessing(m_uninstSD);
    registry->AddAndAdoptProcessing( "LArMgr::", eep1 );
    CaloG4::VEscapedEnergyProcessing* eep2 = new EscapedEnergyProcessing(m_uninstSD);
    registry->AddAndAdoptProcessing( "LAr", eep2 );
  }

  return StatusCode::SUCCESS;
}

StatusCode LArG4H62004DeadSDTool::Gather()
{
  // In this case, *unlike* other SDs, the *tool* owns the collection
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<CaloCalibrationHitContainer>(m_HitColl.name());
  m_deadSD ->EndOfAthenaEvent( &*m_HitColl );
  if (m_do_eep) m_uninstSD     ->EndOfAthenaEvent( &*m_HitColl );
  return StatusCode::SUCCESS;
}
