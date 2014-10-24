/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H8SD/LArG4H8CalibSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "LArG4Code/VCalibrationCalculator.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArG4SD/EscapedEnergyProcessing.h"
#include "LArG4Code/CalibrationSensitiveDetector.h"
#include "LArG4Code/CalibrationDefaultCalculator.h"
#include "CaloG4Sim/CalibrationDefaultProcessing.h"
#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "G4SDManager.hh"

#include "G4VHit.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"

#include "LArG4H8SD/LArTBCryostatCalibrationCalculator.h"
#include "LArG4H8SD/H8CalibrationDefaultCalculator.h"

using namespace FADS;

// ---
bool LArG4H8CalibSD::m_initTBProcessing = false;

// ... here you create static sensitive detector entries ...
static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelCryDead("LAr::TBBarrelCryostat::Dead");

static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelPresMod("LAr::TBBarrel::Presampler::Module::Calibration");
static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelPresIn("LAr::TBBarrel::Presampler::Inactive");
static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelPresDead("LAr::TBBarrel::Presampler::Dead");
static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelIn("LAr::TBBarrel::Inactive");
static SensitiveDetectorEntryT<LArG4H8CalibSD> TBbarrelDead("LAr::TBBarrel::Dead");

// ---


LArG4H8CalibSD::LArG4H8CalibSD(G4String a_name):
  LArG4CalibSD(a_name,false)
{

  if(!m_initTBProcessing)
  {
    m_initTBProcessing = true;
    initializeProcessing();
  }

  // 
  if(a_name=="LAr::TBBarrel::Presampler::Inactive")
    this->SetCalculator(new LArG4::BarrelPresampler::CalibrationCalculator());
  else if(a_name=="LAr::TBBarrel::Presampler::Dead")
    this->SetCalculator(new LArG4::BarrelPresampler::CalibrationCalculator());
  else if(a_name.find("TBBarrel::Presampler::Module::Calibration") != std::string::npos)
    this->SetCalculator(new LArG4::BarrelPresampler::CalibrationCalculator());
  else if(a_name=="LAr::TBBarrel::Inactive")
    this->SetCalculator(new LArG4::Barrel::CalibrationCalculator());
  else if(a_name=="LAr::TBBarrel::Dead")
    this->SetCalculator(new LArG4::Barrel::CalibrationCalculator());
  else if(a_name.find("TBEMB::STAC::Calibration") != std::string::npos)
    this->SetCalculator(new LArG4::Barrel::CalibrationCalculator());
  else if(a_name=="LAr::TBBarrelCryostat::Dead")
    this->SetCalculator(new LArTBCryostatCalibrationCalculator());

  LArG4::BarrelPresampler::CalibrationCalculator* psCalculator =
    dynamic_cast<LArG4::BarrelPresampler::CalibrationCalculator*>(this->GetCalculator());
  if(psCalculator)
    psCalculator->detectorName("LArMgr");
  LArG4::Barrel::CalibrationCalculator* barCalculator =
    dynamic_cast<LArG4::Barrel::CalibrationCalculator*>(this->GetCalculator());
  if(barCalculator) barCalculator->detectorName("LArMgr");


  // ---

}

void LArG4H8CalibSD::initializeProcessing()
{

  std::cout << " **** Call  LArG4H8CalibSD::initializeProcessing() " << std::endl;
  // What should be the default name here?
  std::string defaultName = "Default::Dead::Uninstrumented::Calibration::Region";

  H8CalibrationDefaultCalculator* calculator = 
    new H8CalibrationDefaultCalculator();

  LArG4::CalibrationSensitiveDetector* defaultSD 
    = new LArG4::CalibrationSensitiveDetector(defaultName, calculator);
         
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector(defaultSD);
 
  static CaloG4::CalibrationDefaultProcessing cdp;
  cdp.SetDefaultSD(defaultSD);
 
  // Initialize the escaped energy processing for LAr volumes.
  CaloG4::VEscapedEnergyProcessing* eep = new EscapedEnergyProcessing();
  CaloG4::EscapedEnergyRegistry* registry = CaloG4::EscapedEnergyRegistry::GetInstance();
  registry->AddAndAdoptProcessing( "LAr::", eep );
}
