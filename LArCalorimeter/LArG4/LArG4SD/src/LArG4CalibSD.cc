/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4SD/LArG4CalibSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArVCalibHitMerger.h"
#include "LArG4Code/LArVCalibHitMergerFactory.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
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
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"

#include "LArG4Barrel/CryostatCalibrationCalculator.h"
#include "LArG4Barrel/CryostatCalibrationMixedCalculator.h"
#include "LArG4Barrel/CryostatCalibrationLArCalculator.h"
#include "LArG4Barrel/PresamplerCalibrationCalculator.h"
#include "LArG4Barrel/LArBarrelCalibrationCalculator.h"
#include "LArG4Barrel/DMCalibrationCalculator.h"
#include "LArG4EC/CryostatCalibrationCalculator.h"
#include "LArG4EC/CryostatCalibrationMixedCalculator.h"
#include "LArG4EC/CryostatCalibrationLArCalculator.h"
#include "LArG4EC/CalibrationCalculator.h"
#include "LArG4EC/EMECSupportCalibrationCalculator.h"
#include "LArG4EC/PresamplerCalibrationCalculator.h"

#include "LArG4HEC/CalibrationCalculator.h"
#include "LArG4HEC/LocalCalibrationCalculator.h"
#include "LArG4HEC/Geometry.h"
#include "LArG4HEC/LocalGeometry.h"
#include "LArG4HEC/LArHECCalibrationWheelCalculator.h"
#include "LArG4HEC/HECGeometry.h"
#include "LArG4FCAL/LArFCAL1CalibCalculator.h"
#include "LArG4FCAL/LArFCAL2CalibCalculator.h"
#include "LArG4FCAL/LArFCAL3CalibCalculator.h"
#include "LArG4MiniFCAL/MiniFCALCalibrationCalculator.h"
#include "LArG4MiniFCAL/MiniFCALAssignIdentifier.h"

#include "G4RunManager.hh"
#include "MCTruth/EventInformation.h"
#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "ISF_Event/ISFParticle.h"
#include "MCTruth/VTrackInformation.h"

#include <vector>

bool LArG4CalibSD::m_initProcessing = false;

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryDead("LAr::BarrelCryostat::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryDead2("LArDead::BarrelCryostat::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryLarDead("LAr::BarrelCryostat::LAr::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryLarDead2("LArDead::BarrelCryostat::LAr::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryMixDead("LAr::BarrelCryostat::Mixed::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelCryMixDead2("LArDead::BarrelCryostat::Mixed::Dead");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> dmDead("LAr::DM::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> dmDead2("LArDead::DM::Dead");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelPresMod("LAr::Barrel::Presampler::Module::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelPresIn("LAr::Barrel::Presampler::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelPresDead("LAr::Barrel::Presampler::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelPresDead2("LArDead::Barrel::Presampler::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelIn("LAr::Barrel::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelDead("LAr::Barrel::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> barrelDead2("LArDead::Barrel::Dead");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryDead("LAr::EndcapCryostat::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryDead2("LArDead::EndcapCryostat::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryLarDead("LAr::EndcapCryostat::LAr::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryLarDead2("LArDead::EndcapCryostat::LAr::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryMixDead("LAr::EndcapCryostat::Mixed::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapCryMixDead2("LArDead::EndcapCryostat::Mixed::Dead");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> embStac("LAr::EMB::STAC::Calibration");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecPosInWheelCalib("LAr::EMEC::Pos::InnerWheel::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecPosOutWheelCalib("LAr::EMEC::Pos::OuterWheel::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecPosInWheelIna("LAr::EMEC::Pos::InnerWheel::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecPosOutWheelIna("LAr::EMEC::Pos::OuterWheel::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecNegInWheelCalib("LAr::EMEC::Neg::InnerWheel::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecNegOutWheelCalib("LAr::EMEC::Neg::OuterWheel::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecNegInWheelIna("LAr::EMEC::Neg::InnerWheel::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecNegOutWheelIna("LAr::EMEC::Neg::OuterWheel::Inactive");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecBackOutBarrCalib("LAr::EMEC::BackOuterBarrette::Calibration");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecSuppDead("LAr::EMECSupport::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> emecSuppDead2("LArDead::EMECSupport::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> endcapPresLar("LAr::Endcap::Presampler::LiquidArgon::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecMod("LAr::HEC::Module::Depth::Slice::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecModLoc("LAr::HEC::Module::Depth::Slice::Local::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecIna("LAr::HEC::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecInaLoc("LAr::HEC::Local::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDead("LAr::HEC::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDead2("LArDead::HEC::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDeadLoc("LAr::HEC::Local::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDeadLoc2("LArDead::HEC::Local::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecModWheel("LAr::HEC::Module::Depth::Slice::Wheel::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecInaWheel("LAr::HEC::Wheel::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDeadWheel("LAr::HEC::Wheel::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> hecDeadWheel2("LArDead::HEC::Wheel::Dead");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalMod1Calib("LAr::FCAL::Module1::Gap::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalMod2Calib("LAr::FCAL::Module2::Gap::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalMod3Calib("LAr::FCAL::Module3::Gap::Calibration");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalIna1("LAr::FCAL::Inactive1");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalIna2("LAr::FCAL::Inactive2");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> fcalIna3("LAr::FCAL::Inactive3");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalInaMoth("MiniFCALMother::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalIna("MiniFCAL::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalInaLay("MiniFCAL::Layer::Inactive");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalCalib("MiniFCAL::Wafer::Calibration");

static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalDeadMoth("MiniFCALMother::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalDead("MiniFCAL::Dead");
static FADS::SensitiveDetectorEntryT<LArG4CalibSD> minifcalDeadLay("MiniFCAL::Layer::Dead");


LArG4CalibSD::LArG4CalibSD(G4String a_name, bool doInit):
  FadsSensitiveDetector(a_name),
  m_calculator(0),
  m_detectorName(a_name),
  m_numberInvalidSteps(0),
  m_event_info(0),
  m_doCalibHitParticleID(false)
{
  // read flag to produce calibration hits signed with primary particle ID
  StoreGateSvc *detStore(NULL);
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  if ( svcLocator->service("DetectorStore", detStore).isFailure() ) {
    throw std::runtime_error("LArG4CalibSD: Unable to retrieve Detector Store!");
  } else {
    LArG4GlobalOptions *globalOptions(NULL);
    if (detStore->retrieve(globalOptions, "LArG4GlobalOptions").isFailure()) {
      throw std::runtime_error("LArG4CalibSD: LArG4GlobalOption was not found in DetectorStore");
    } else {
      m_doCalibHitParticleID = globalOptions->GetDoCalibHitParticleID();
    }
  }

  if(a_name.find("LArDead") != std::string::npos) doInit=false;
  // Initialize processings
  if (doInit)
    {
      if(!m_initProcessing)
        {
          m_initProcessing = true;
          initializeProcessing();
        }
    }

  if(a_name=="LAr::BarrelCryostat::Dead" || a_name=="LArDead::BarrelCryostat::Dead")
    m_calculator = new LArG4::BarrelCryostat::CalibrationCalculator();
  else if(a_name=="LAr::BarrelCryostat::LAr::Dead" || a_name=="LArDead::BarrelCryostat::LAr::Dead")
    m_calculator = new LArG4::BarrelCryostat::CalibrationLArCalculator();
  else if(a_name=="LAr::BarrelCryostat::Mixed::Dead"  || a_name=="LArDead::BarrelCryostat::Mixed::Dead")
    m_calculator = new LArG4::BarrelCryostat::CalibrationMixedCalculator();
  else if(a_name=="LAr::DM::Dead" || a_name=="LArDead::DM::Dead")
    m_calculator = new LArG4::DM::CalibrationCalculator();
  else if(a_name.find("Barrel::Presampler::Module::Calibration") != std::string::npos)
    m_calculator = new LArG4::BarrelPresampler::CalibrationCalculator();
  else if(a_name=="LAr::Barrel::Presampler::Inactive")
    m_calculator = new LArG4::BarrelPresampler::CalibrationCalculator();
  else if(a_name=="LAr::Barrel::Presampler::Dead" ||  a_name=="LArDead::Barrel::Presampler::Dead")
    m_calculator = new LArG4::BarrelPresampler::CalibrationCalculator();
  else if(a_name=="LAr::Barrel::Inactive")
    m_calculator = new LArG4::Barrel::CalibrationCalculator();
  else if(a_name=="LAr::Barrel::Dead" ||  a_name=="LArDead::Barrel::Dead")
    m_calculator = new LArG4::Barrel::CalibrationCalculator();
  else if(a_name=="LAr::EndcapCryostat::Dead" ||  a_name=="LArDead::EndcapCryostat::Dead")
    m_calculator = new LArG4::EndcapCryostat::CalibrationCalculator();
  else if(a_name=="LAr::EndcapCryostat::LAr::Dead" ||  a_name=="LArDead::EndcapCryostat::LAr::Dead")
    m_calculator = new LArG4::EndcapCryostat::CalibrationLArCalculator();
  else if(a_name=="LAr::EndcapCryostat::Mixed::Dead" ||  a_name=="LArDead::EndcapCryostat::Mixed::Dead")
    m_calculator = new LArG4::EndcapCryostat::CalibrationMixedCalculator();
  else if(a_name.find("EMB::STAC::Calibration") != std::string::npos)
    m_calculator = new LArG4::Barrel::CalibrationCalculator();

  else if(a_name.find("EMEC::Pos::InnerWheel::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1);
  else if(a_name.find("EMEC::Pos::OuterWheel::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1);
  else if(a_name=="LAr::EMEC::Pos::InnerWheel::Inactive")
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, 1);
  else if(a_name=="LAr::EMEC::Pos::OuterWheel::Inactive")
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, 1);
  else if(a_name.find("EMEC::Neg::InnerWheel::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1);
  else if(a_name.find("EMEC::Neg::OuterWheel::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1);
  else if(a_name=="LAr::EMEC::Neg::InnerWheel::Inactive")
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel, -1);
  else if(a_name=="LAr::EMEC::Neg::OuterWheel::Inactive")
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::OuterAbsorberWheel, -1);
  else if(a_name.find("EMEC::BackOuterBarrette::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::BackOuterBarretteWheelCalib, 1);
  else if(a_name=="LAr::EMECSupport::Dead" ||  a_name=="LArDead::EMECSupport::Dead")
    m_calculator = new LArG4::EMECSupportCalibrationCalculator();

  else if(a_name.find("Endcap::Presampler::LiquidArgon::Calibration") != std::string::npos)
    m_calculator = new LArG4::EC::PresamplerCalibrationCalculator();
  else if(a_name.find("HEC::Module::Depth::Slice::Calibration") != std::string::npos)
    m_calculator = new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kActive);
  else if(a_name.find("HEC::Module::Depth::Slice::Local::Calibration") != std::string::npos)
    m_calculator = new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocActive);
  else if(a_name=="LAr::HEC::Inactive")
    m_calculator = new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kInactive);
  else if(a_name=="LAr::HEC::Local::Inactive")
    m_calculator = new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocInactive);
  else if(a_name=="LAr::HEC::Dead" ||  a_name=="LArDead::HEC::Dead")
    m_calculator =new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kDead);
  else if(a_name=="LAr::HEC::Local::Dead" || a_name=="LArDead::HEC::Local::Dead")
    m_calculator =new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocDead);
  else if(a_name.find("HEC::Module::Depth::Slice::Wheel::Calibration") != std::string::npos)
    m_calculator = new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelActive);
  else if(a_name=="LAr::HEC::Wheel::Inactive")
    m_calculator = new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelInactive);
  else if(a_name=="LAr::HEC::Wheel::Dead" || a_name=="LArDead::HEC::Wheel::Dead")
    m_calculator =new LArG4::HEC::LArHECCalibrationWheelCalculator(LArG4::HEC::kWheelDead);


  else if(a_name.find("FCAL::Module1::Gap::Calibration") != std::string::npos)
    m_calculator =LArG4::FCAL::LArFCAL1CalibCalculator::GetCalculator();
  else if(a_name.find("FCAL::Module2::Gap::Calibration") != std::string::npos)
    m_calculator =LArG4::FCAL::LArFCAL2CalibCalculator::GetCalculator();
  else if(a_name.find("FCAL::Module3::Gap::Calibration") != std::string::npos)
    m_calculator =LArG4::FCAL::LArFCAL3CalibCalculator::GetCalculator();
  else if(a_name=="LAr::FCAL::Inactive1")
    m_calculator =LArG4::FCAL::LArFCAL1CalibCalculator::GetCalculator();
  else if(a_name=="LAr::FCAL::Inactive2")
    m_calculator =LArG4::FCAL::LArFCAL2CalibCalculator::GetCalculator();
  else if(a_name=="LAr::FCAL::Inactive3")
    m_calculator =LArG4::FCAL::LArFCAL3CalibCalculator::GetCalculator();

  else if(a_name=="MiniFCALMother::Inactive" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive);
  else if(a_name=="MiniFCAL::Inactive" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive);
  else if(a_name=="MiniFCAL::Layer::Inactive" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kInactive);
  else if(a_name=="MiniFCAL::Wafer::Calibration" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kActive);

  else if(a_name=="MiniFCALMother::Dead" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead);
  else if(a_name=="MiniFCAL::Dead" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead);
  else if(a_name=="MiniFCAL::Layer::Dead" )
    m_calculator =new LArG4::MiniFCAL::MiniFCALCalibrationCalculator(LArG4::MiniFCAL::kDead);

  // Set detector name for presampler calculator
  LArG4::BarrelPresampler::CalibrationCalculator* psCalculator =
    dynamic_cast<LArG4::BarrelPresampler::CalibrationCalculator*>(m_calculator);
  if(psCalculator)
    psCalculator->detectorName("LArMgr");
  else
    {
      // Set detector name for barrel calculator
      LArG4::Barrel::CalibrationCalculator* barCalculator =
        dynamic_cast<LArG4::Barrel::CalibrationCalculator*>(m_calculator);
      if(barCalculator) barCalculator->detectorName("LArMgr");
    }

}


LArG4CalibSD::~LArG4CalibSD()
{
  // Display the number of invalid hits in this sensitive detector.
  // Note: this code is only executed if the sensitive-detector object
  // is deleted; I'm not sure if G4 does this.
  if(m_numberInvalidSteps>0)
    ATH_MSG_DEBUG ( "Destructor:  " << m_detectorName << "> had " << m_numberInvalidSteps
                    << " G4Step energy deposits outside the region determined by its Calculator."
                    );
}


void LArG4CalibSD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  if(m_hitMergers.size()==0)
    {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IService* pSvc;
      StatusCode status = svcLocator->service("DetectorStore",pSvc);

      if(status.isSuccess())
        ATH_MSG_DEBUG ( "Initialize: Detector Store retrieved" );
      else
        throw std::runtime_error("LArG4CalibSD: Unable to retrieve Detector Store!");

      StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(!detStore)
        throw std::runtime_error("LArG4CalibSD: 0 pointer to the Detector Store!");

      // Retrieve Hit Merger factory from DS and initialize m_hitMerger
      const DataHandle<LArVCalibHitMergerFactory> _factory_begin;
      const DataHandle<LArVCalibHitMergerFactory> _factory_end;

      status = detStore->retrieve(_factory_begin,_factory_end);

      if(status.isFailure())
        throw std::runtime_error("LArG4CalibSD: Unable to retrieve Hit Merger Factories!");
      else
        ATH_MSG_DEBUG ( "Initialize: Hit Merger Factories retrieved" );

      for(;_factory_begin!=_factory_end;_factory_begin++)
        m_hitMergers.push_back( _factory_begin->getHitMerger());

      if(m_hitMergers.size()==0)
        ATH_MSG_DEBUG ( "Initialize: No Hit Merger assigned to SD!" );
    }

  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->BeginOfEvent();

  // if not using ISF, get EventInformation to use for HepMC barcode later
  m_event_info=0;

}

G4bool LArG4CalibSD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  // Convert the G4Step into identifier and energy.

  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)

  if (!m_event_info && m_doCalibHitParticleID)
    m_event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());

  if(!(m_calculator->Process(a_step, LArG4::VCalibrationCalculator::kEnergyAndID)))
    {
      m_numberInvalidSteps++;
      return false;
    }

  // access for primary particle identifier of G4Track
  unsigned int particle_id = 0;
  if( m_doCalibHitParticleID )
    particle_id = m_event_info->GetCurrentPrimary()->barcode();

  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    {
      if(m_hitMergers[i]
         && (!m_hitMergers[i]->process(m_calculator->identifier(),
                                       m_calculator->energies(),
                                       particle_id))) {
        ATH_MSG_DEBUG ( "ProcessHits: Wrong Identifier after conversion from LArG4Identifier"
                        << std::string(m_calculator->identifier()) );
      }
    }
  return true;
}

void LArG4CalibSD::PlotHitPos(G4Step* a_step,LArG4Identifier id)
{
  G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  G4ThreeVector p = pre_step_point->GetPosition();
  G4double x=p.x();
  G4double r=sqrt(p.y()*p.y()+p.z()*p.z());
  G4double rloc=0;
  const G4NavigationHistory* g4navigation = pre_step_point->GetTouchable()->GetHistory();
  G4int ndep = g4navigation->GetDepth();
  for (G4int ii=0;ii<=ndep;ii++) {
    G4VPhysicalVolume* v1 = g4navigation->GetVolume(ii);
    G4String vName = v1->GetName();
    if ( vName=="CALO::CALO") {
      // compute local radius in CALO::CALO frame (<=> atlas frame for calorimeter)
      const G4AffineTransform transformation = g4navigation->GetTransform(ii);
      G4ThreeVector startPointinLocal = transformation.TransformPoint(p);
      rloc = sqrt(startPointinLocal.x()*startPointinLocal.x() +
                  startPointinLocal.y()*startPointinLocal.y());
      break;
    }
  }

  int type=id[0];
  if (type==10) {
    int subdet=id[1];
    int type=id[2];
    int sampling=id[3];
    int region=id[4];
    if (subdet==4 && type==1 && sampling==0)
      std::cout << "HitPos D4T1S0 " << x << " " << r << " " << rloc <<std::endl;
    if (subdet==4 && type==1 && sampling==1 && region==0)
      std::cout << "HitPos D4T1S1R0 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==1 && region==1)
      std::cout << "HitPos D4T1S1R1 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==1 && region==2)
      std::cout << "HitPos D4T1S1R2 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==1 && region==3)
      std::cout << "HitPos D4T1S1R3 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==1 && region>3)
      std::cout << "HitPos D4T1S1R4 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==2 && region==0)
      std::cout << "HitPos D4T1S2R0 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==2 && region==1)
      std::cout << "HitPos D4T1S2R1 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==2 && region==2)
      std::cout << "HitPos D4T1S2R2 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==2 && region>=3)
      std::cout << "HitPos D4T1S2R3 " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==1 && sampling==3)
      std::cout << "HitPos D4T1S3   " << x << " " << r << " " << rloc << std::endl;
    if (subdet==4 && type==2)
      std::cout << "HitPos D4T2     " << x << " " << r << " " << rloc << std::endl;
    if (subdet==5) std::cout << "HitPos D5 " << x << " " << r << " " << rloc << std::endl;
  }
}


void LArG4CalibSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->EndOfEvent(m_detectorName);
}

// Something special has happened (probably the detection of escaped
// energy in CaloG4Sim/SimulationEnergies).  We have to bypass the
// regular sensitive-detector processing.  Determine the identifier
// (and only the identifier) using the calculator, then built a hit
// with that identifier and the energies in the vector.

G4bool LArG4CalibSD::SpecialHit(G4Step* a_step,
                                const std::vector<G4double>& a_energies)
{
  // Convert the G4Step into an identifier.

  // If we can't calculate the identifier, something is wrong.
  if (!(m_calculator->Process( a_step, LArG4::VCalibrationCalculator::kOnlyID))) return false;

  // retreive particle ID
  unsigned int particle_id = 0;
  if( m_doCalibHitParticleID ) {
    particle_id = m_event_info->GetCurrentPrimary()->barcode();
  }

  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    {
      if(m_hitMergers[i]
         && (!m_hitMergers[i]->process(m_calculator->identifier(),
                                       a_energies,
                                       particle_id))) {
        ATH_MSG_DEBUG ( "SpecialHit: Wrong Identifier after conversion from LArG4Identifier"
                        << std::string(m_calculator->identifier()) );
      }
    }

  return true;
}

void LArG4CalibSD::initializeProcessing()
{
#ifdef DEBUG_SD
  std::cout << "*** in LArG4CalibSD::initializeProcessing() " << std::endl;
#endif

  //std::cout<<"creating new CalibrationDefaultCalculator"<<std::endl;

  std::string defaultName = "Default::Dead::Uninstrumented::Calibration::Region";
  LArG4::CalibrationDefaultCalculator* calculator
    = new LArG4::CalibrationDefaultCalculator();

  //std::cout<<"creating new CalibrationSensitiveDetector"<<std::endl;
  LArG4::CalibrationSensitiveDetector* defaultSD
    = new LArG4::CalibrationSensitiveDetector(defaultName, calculator);

  //std::cout<<"getting SDMpointer"<<std::endl;
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  //std::cout<<"adding SD to SDMan"<<std::endl;
  SDman->AddNewDetector(defaultSD);

  //std::cout<<"CalibrationDefaultProcessing"<<std::endl;
  static CaloG4::CalibrationDefaultProcessing cdp;
  //std::cout<<"setting default SD"<<std::endl;
  cdp.SetDefaultSD(defaultSD);

  //std::cout<<"new EscapedEnergyProcessing"<<std::endl;
  // Initialize the escaped energy processing for LAr volumes.
  CaloG4::VEscapedEnergyProcessing* eep = new EscapedEnergyProcessing();

  //std::cout<<"EER instance"<<std::endl;
  CaloG4::EscapedEnergyRegistry* registry = CaloG4::EscapedEnergyRegistry::GetInstance();
  //std::cout<<"addAndAdoptProcessing"<<std::endl;
  registry->AddAndAdoptProcessing( "LAr::", eep );
}
