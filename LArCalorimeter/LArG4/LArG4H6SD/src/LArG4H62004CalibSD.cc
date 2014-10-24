/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H62004CalibSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "LArG4H6SD/LArG4H62004DeadCalibrationCalculator.h"
#include "LArG4H6SD/LArG4H6COLDTCMod0CalibCalculator.h"
#include "LArG4H6SD/LArFCAL1H62004CalibCalculator.h"
#include "LArG4H6SD/LArFCAL2H62004CalibCalculator.h"

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArVCalibHitMerger.h"
//#include "LArG4Code/LArVCalibHitMergerFactory.h"
//
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "StoreGate/StoreGateSvc.h"
//
#include "LArG4SD/EscapedEnergyProcessing.h"
//#include "LArG4Code/CalibrationSensitiveDetector.h"
//#include "LArG4Code/CalibrationDefaultCalculator.h"
#include "CaloG4Sim/CalibrationDefaultProcessing.h"
#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "G4SDManager.hh"

//#include "G4VHit.hh"
//#include "G4Step.hh"
//#include "G4TouchableHistory.hh"
//#include "G4ios.hh"
//#include "G4StepPoint.hh"
//#include "G4ThreeVector.hh"
//
#include "LArG4EC/CalibrationCalculator.h"
//#include "LArG4EC/EMECSupportCalibrationCalculator.h"
//#include "LArG4EC/PresamplerCalibrationCalculator.h"
//
#include "LArG4HEC/CalibrationCalculator.h"
#include "LArG4HEC/LocalCalibrationCalculator.h"
//#include "LArG4HEC/Geometry.h"
//#include "LArG4HEC/LocalGeometry.h"
//
//#include <vector>
//
#undef DEBUG_SD

bool LArG4H62004CalibSD::m_initProcessing = false;

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> emecInWheelCalib("LAr::EMEC::InnerModule::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> emecInWheelIna("LAr::EMEC::InnerModule::Inactive::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> emecBackInBarrCalib("LAr::EMEC::BackInnerBarrette::Calibration::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> emecSuppDead("LAr::EMECSupport::Dead::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> emecSuppDead2("LArDead::EMECSupport::Dead::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecMod("LAr::HEC::Module::Depth::Slice::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecModLoc("LAr::HEC::Module::Depth::Slice::Local::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecIna("LAr::HEC::Inactive::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecInaLoc("LAr::HEC::Local::Inactive::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecDead("LAr::HEC::Dead::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecDead2("LArDead::HEC::Dead::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecDeadLoc("LAr::HEC::Local::Dead::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> hecDeadLoc2("LArDead::HEC::Local::Dead::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalMod1Calib("LAr::FCAL::Module1::Gap::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalMod2Calib("LAr::FCAL::Module2::Gap::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalColdTCCalib("LAr::FCAL::ColdTC::Gap::Calibration::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalIna1("LAr::FCAL::Inactive1::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalIna2("LAr::FCAL::Inactive2::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> fcalIna3("LAr::FCAL::InactiveColdTC::H6");

static FADS::SensitiveDetectorEntryT<LArG4H62004CalibSD> Dead("LArDead::Dead::H6");

LArG4H62004CalibSD::LArG4H62004CalibSD(G4String a_name, bool doInit):
  CalibrationSensitiveDetector(a_name,0),
  FadsSensitiveDetector(a_name)
  //  m_calculator(0),
  //  m_numberInvalidSteps(0)
{
#ifdef DEBUG_SD
  std::cout << " in LArG4H62004CalibSD:::LArG4H62004CalibSD a_name " << a_name << std::endl;
#endif

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

  if(a_name.find("EMEC::InnerModule::Calibration::H6") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel,1);
  else if(a_name=="LAr::EMEC::InnerModule::Inactive::H6")
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::InnerAbsorberWheel,1);
  else if(a_name.find("EMEC::BackInnerBarrette::Calibration::H6") != std::string::npos)
    m_calculator = new LArG4::EC::CalibrationCalculator(LArWheelCalculator::BackInnerBarretteWheelCalib,1);
  else if(a_name=="LAr::EMECSupport::Dead::H6" ||  a_name=="LArDead::EMECSupport::Dead::H6" || a_name.find("LArDead::Dead")!= std::string::npos )
    m_calculator = new LArG4H62004DeadCalibrationCalculator();
  else if(a_name.find("HEC::Module::Depth::Slice::Calibration::H6") != std::string::npos)
    m_calculator = new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kActive);
  else if(a_name.find("HEC::Module::Depth::Slice::Local::Calibration::H6") != std::string::npos)
    m_calculator = new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocActive);
  else if(a_name=="LAr::HEC::Inactive::H6")
    m_calculator = new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kInactive);
  else if(a_name=="LAr::HEC::Local::Inactive::H6")
    m_calculator = new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocInactive);
  else if(a_name=="LAr::HEC::Dead::H6" ||  a_name=="LArDead::HEC::Dead::H6")
    m_calculator = new LArG4::HEC::CalibrationCalculator(LArG4::HEC::kDead);
  else if(a_name=="LAr::HEC::Local::Dead::H6" || a_name=="LArDead::HEC::Local::Dead::H6")
    m_calculator = new LArG4::HEC::LocalCalibrationCalculator(LArG4::HEC::kLocDead);
  else if(a_name.find("LAr::FCAL::Module1::Gap::Calibration::H6") != std::string::npos)
    m_calculator = LArFCAL1H62004CalibCalculator::GetCalculator();
  else if(a_name.find("LAr::FCAL::Module2::Gap::Calibration::H6") != std::string::npos)
    m_calculator = LArFCAL2H62004CalibCalculator::GetCalculator();
  else if(a_name.find("LAr::FCAL::ColdTC::Gap::Calibration::H6") != std::string::npos)
    m_calculator = new LArG4H6COLDTCMod0CalibCalculator();
  else if(a_name=="LAr::FCAL::Inactive1::H6")
    m_calculator = LArFCAL1H62004CalibCalculator::GetCalculator();
  else if(a_name=="LAr::FCAL::Inactive2::H6")
    m_calculator = LArFCAL2H62004CalibCalculator::GetCalculator();
  else if(a_name=="LAr::FCAL::InactiveColdTC::H6"){
    m_calculator = new LArG4H6COLDTCMod0CalibCalculator();
  }
}

/*
  LArG4H62004CalibSD::~LArG4H62004CalibSD()
  {
  // Display the number of invalid hits in this sensitive detector.
  // Note: this code is only executed if the sensitive-detector object
  // is deleted; I'm not sure if G4 does this.
  if ( m_numberInvalidSteps > 0 && m_msgSvc )
  {
  log << MSG::DEBUG << m_detectorName << "> had " << m_numberInvalidSteps
  << " G4Step energy deposits outside the region determined by its Calculator."
  << endreq;
  }
  }
*/
void LArG4H62004CalibSD::Initialize(G4HCofThisEvent* m_HCE)
{
  LArG4::CalibrationSensitiveDetector::Initialize(m_HCE);
}
/*
  void LArG4H62004CalibSD::Initialize(G4HCofThisEvent* m_HCE)
  {
  if(m_hitMergersH6.size()==0) {
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  //    StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  //    if(status.isFailure())
  //      throw std::runtime_error("LArG4H62004CalibSD: Unable to retrieve Message Service!");

  IService* pSvc;
  StatusCode status = svcLocator->service("DetectorStore",pSvc);

  if(!status.isSuccess())
  throw std::runtime_error("LArG4H62004CalibSD: Unable to retrieve Detector Store!");

  StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
  if(!detStore)
  throw std::runtime_error("LArG4H62004CalibSD: 0 pointer to the Detector Store!");

  // Retrieve Hit Merger factory from DS and initialize m_hitMerger
  const DataHandle<LArVCalibHitMergerFactory> _factory_begin;
  const DataHandle<LArVCalibHitMergerFactory> _factory_end;

  status = detStore->retrieve(_factory_begin,_factory_end);

  if(status.isFailure())
  throw std::runtime_error("LArG4H62004CalibSD: Unable to retrieve Hit Merger Factories!");

  for(;_factory_begin!=_factory_end;_factory_begin++)
  m_hitMergersH6.push_back( _factory_begin->getHitMerger());

  if(m_hitMergersH6.size()==0)
  std::cout << "No Hit Merger assigned to SD!" << std::endl;;
  }

  for(unsigned int i=0; i<m_hitMergersH6.size(); i++)
  m_hitMergersH6[i]->BeginOfEvent();
  }
*/
G4bool LArG4H62004CalibSD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
#ifdef DEBUG_SD
  std::cout << "my name: "<<this->GetName()<<", calc: "<<LArG4::m_calculator<<std::endl;
#endif
  // Convert the G4Step into identifier and energy.
  G4bool valid = m_calculator->Process(a_step, LArG4::VCalibrationCalculator::kEnergyAndID);

  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)

  if ( ! valid )
    {
      m_numberInvalidSteps++;
      std::cout<< " Invalid step, not counted: "<<FADS::FadsSensitiveDetector::GetName()<<", calc: "<<m_calculator<<" !!!"<<std::endl;
      return false;
    }

  // Fetch the values from the calculator.
  LArG4Identifier ident = m_calculator->identifier();
  std::vector<G4double> energies = m_calculator->energies();
  // Changing the ident to comply with H6 dictionary
  if(ident[1] == 1) { // EMEC module
    ident[6] += 8; // change phi id
    if(ident[6] < 20) ident[6] = 20;
    if(ident[6] > 27) ident[6] = 27;
  }else if(ident[1] == 2) { // HEC module
    if(ident[4] == 0) {
      if(ident[3]==0 && ident[5] < 6) ident[5]=6;
      if(ident[3]==2 && ident[5] < 8) ident[5]=8;
    } else {
      if(ident[3]==1 && ident[5] > 2) ident[5]=2;
    }
  }

  bool processed = false;
  for(unsigned int i=0; i<m_hitMergers.size(); ++i) {
    LArVCalibHitMerger* _hitMerger = m_hitMergers[i];
    if(_hitMerger) {
      if(!_hitMerger->process(ident,energies)) std::cout << "Wrong Identifier after conversion from LArG4Identifier" << std::string(ident) << std::endl;
      processed = true;
    }
  }
  if(!processed) std::cout<<"LArG4H62004CalibSD name: "<<m_detectorName<<" no mergers !!"<<std::endl;
  return true;
}
/*
  void LArG4H62004CalibSD::PlotHitPos(G4Step* a_step,LArG4Identifier id)
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
*/
void LArG4H62004CalibSD::EndOfEvent(G4HCofThisEvent* m_HCE)
{
  LArG4::CalibrationSensitiveDetector::EndOfEvent(m_HCE);
}
/*
  void LArG4H62004CalibSD::EndOfEvent(G4HCofThisEvent* m_HCE)
  {
  for(unsigned int i=0; i<m_hitMergersH6.size(); ++i)
  m_hitMergersH6[i]->EndOfEvent(m_detectorName);
  }

  // Something special has happened (probably the detection of escaped
  // energy in CaloG4Sim/SimulationEnergies).  We have to bypass the
  // regular sensitive-detector processing.  Determine the identifier
  // (and only the identifier) using the calculator, then built a hit
  // with that identifier and the energies in the vector.
  */

G4bool LArG4H62004CalibSD::SpecialHit(G4Step* a_step,
                                      const std::vector<G4double>& a_energies)
{
#ifdef DEBUG_SD
  std::cout<<"LArG4H62004CalibSD::SpecialHit called, calling calculator..."<<std::endl;
#endif
  // Convert the G4Step into an identifier.
  G4bool valid = m_calculator->Process( a_step, LArG4::VCalibrationCalculator::kOnlyID );

  // If we can't calculate the identifier, something is wrong.
  if ( ! valid ) return false;

  // Fetch the identifier from the calculator.
  LArG4Identifier ident = m_calculator->identifier();
  // Changing the ident to comply with H6 dictionary
  if(ident[1] == 1) { // EMEC module
    ident[6] += 8; // change phi id
    if(ident[6] < 20) ident[6] = 20;
    if(ident[6] > 27) ident[6] = 27;
  }else if(ident[1] == 2) { // HEC module
    if(ident[4] == 0) {
      if(ident[3]==0 && ident[5] < 6) ident[5]=6;
      if(ident[3]==2 && ident[5] < 8) ident[5]=8;
    } else {
      if(ident[3]==1 && ident[5] > 2) ident[5]=2;
    }
  }


  for(unsigned int i=0; i<m_hitMergers.size(); ++i)
    {
      LArVCalibHitMerger* _hitMerger = m_hitMergers[i];
      if(_hitMerger && (!_hitMerger->process(ident,a_energies)))
        ATH_MSG_DEBUG ( "Wrong Identifier after conversion from LArG4Identifier" << std::string(ident) );
    }
  return true;
}


void LArG4H62004CalibSD::initializeProcessing()
{
#ifdef DEBUG_SD
  std::cout << "*** in LArG4H62004CalibSD::initializeProcessing() " << std::endl;
#endif
  std::string defaultName = "Default::Dead::Uninstrumented::Calibration::H6";
  LArG4H62004DeadCalibrationCalculator * calculator = new LArG4H62004DeadCalibrationCalculator();
  /*
    LArG4::CalibrationDefaultCalculator* calculator
    = new LArG4::CalibrationDefaultCalculator();
  */
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
  CaloG4::VEscapedEnergyProcessing* eep1 = new EscapedEnergyProcessing();
  registry->AddAndAdoptProcessing( "LArMgr::", eep1 );
  CaloG4::VEscapedEnergyProcessing* eep2 = new EscapedEnergyProcessing();
  registry->AddAndAdoptProcessing( "LAr", eep2 );
}
