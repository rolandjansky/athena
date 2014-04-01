/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4SD/LArG4SD.h"
#include "LArG4Code/LArG4Hit.h"

#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4Step.hh"
#include "G4TouchableHistory.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArG4Barrel/LArBarrelCalculator.h"
#include "LArG4Barrel/LArBarrelPresamplerCalculator.h"

#include "LArG4EC/EnergyCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"

#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4FCAL/LArFCAL3Calculator.h"

#include "LArG4MiniFCAL/MiniFCALCalculator.h"

#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/LArHECLocalCalculator.h"
#include "LArG4HEC/LArHECWheelCalculator.h"

using namespace FADS;

static SensitiveDetectorEntryT<LArG4SD> stacSD("LAr::EMB::STAC");
static SensitiveDetectorEntryT<LArG4SD> presampModuleSD("LAr::Barrel::Presampler::Module");
static SensitiveDetectorEntryT<LArG4SD> posInnerWheelSD("LAr::EMEC::Pos::InnerWheel");
static SensitiveDetectorEntryT<LArG4SD> posOuterWheelSD("LAr::EMEC::Pos::OuterWheel");
static SensitiveDetectorEntryT<LArG4SD> negInnerWheelSD("LAr::EMEC::Neg::InnerWheel");
static SensitiveDetectorEntryT<LArG4SD> negOuterWheelSD("LAr::EMEC::Neg::OuterWheel");
static SensitiveDetectorEntryT<LArG4SD> backouterbarrettSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv");
static SensitiveDetectorEntryT<LArG4SD> backinnerbarrettSD("LAr::EMEC::BackInnerBarrette::Module::Phidiv");
static SensitiveDetectorEntryT<LArG4SD> presampLArSD("LAr::Endcap::Presampler::LiquidArgon");
static SensitiveDetectorEntryT<LArG4SD> fcalMod1SD("LAr::FCAL::Module1::Gap");
static SensitiveDetectorEntryT<LArG4SD> fcalMod2SD("LAr::FCAL::Module2::Gap");
static SensitiveDetectorEntryT<LArG4SD> fcalMod3SD("LAr::FCAL::Module3::Gap");
static SensitiveDetectorEntryT<LArG4SD> minifcalLayerSD("MiniFCAL::Wafer");

static SensitiveDetectorEntryT<LArG4SD> hecSliceSD("LAr::HEC::Module::Depth::Slice");
static SensitiveDetectorEntryT<LArG4SD> hecSliceLocalSD("LAr::HEC::Module::Depth::Slice::Local");
static SensitiveDetectorEntryT<LArG4SD> hecSliceWheelSD("LAr::HEC::Module::Depth::Slice::Wheel");

LArG4SD::LArG4SD(G4String a_name)
  : FadsSensitiveDetector(a_name),
    m_calculator(0),
    m_detectorName(a_name),
    m_numberInvalidHits(0),
    m_log(0)
{
  // Get pointer to the message service
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc(0);
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if(status.isFailure())
    throw std::runtime_error("LArG4SD: Unable to retrieve Message Service!");

  m_log = new MsgStream(msgSvc, "LArG4SD");
  (*m_log) << MSG::DEBUG << "Constructor: Call LArG4SD for detector " << a_name << endreq;

  // Get appropriate calculator
  if(a_name.find("EMB::STAC")!=std::string::npos)
    {
      m_calculator = LArBarrelCalculator::GetCalculator();
      LArBarrelCalculator* calculator = LArBarrelCalculator::GetCalculator();
      calculator->detectorName("LArMgr");
      (*m_log) << MSG::DEBUG << "Constructor: after getting LArbarrelcalculator" <<  endreq;
    }
  else if(a_name.find("Barrel::Presampler::Module")!=std::string::npos)
    {
      m_calculator = LArBarrelPresamplerCalculator::GetCalculator();
      LArBarrelPresamplerCalculator* calculator = LArBarrelPresamplerCalculator::GetCalculator();
      calculator->detectorName("LArMgr");
      (*m_log) << MSG::DEBUG << "Constructor: after getting presamplerCalculator " <<  endreq;
    }
  else if(a_name.find("EMEC::Pos::InnerWheel")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(
                                                    LArWheelCalculator::InnerAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
    }
  else if(a_name.find("EMEC::Pos::OuterWheel")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(
                                                    LArWheelCalculator::OuterAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, 1);
    }
  else if(a_name.find("EMEC::Neg::InnerWheel")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(
                                                    LArWheelCalculator::InnerAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
    }
  else if(a_name.find("EMEC::Neg::OuterWheel")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(
                                                    LArWheelCalculator::OuterAbsorberWheel, LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT, -1);
    }
  else if(a_name.find("EMEC::BackOuterBarrette")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel);
    }
  else if(a_name.find("EMEC::BackInnerBarrette")!=std::string::npos)
    {
      m_calculator= new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackInnerBarretteWheel);
    }
  else if(a_name.find("Endcap::Presampler::LiquidArgon")!=std::string::npos)
    {
      m_calculator = LArEndcapPresamplerCalculator::GetCalculator();
    }
  else if(a_name.find("FCAL::Module1::Gap")!=std::string::npos)
    {
      m_calculator = LArFCAL1Calculator::GetInstance();
    }
  else if(a_name.find("FCAL::Module2::Gap")!=std::string::npos)
    {
      m_calculator = LArFCAL2Calculator::GetInstance();
    }
  else if(a_name.find("FCAL::Module3::Gap")!=std::string::npos)
    {
      m_calculator = LArFCAL3Calculator::GetInstance();
    }
  else if(a_name.find("HEC::Module::Depth::Slice::Local")!=std::string::npos)
    {
      m_calculator = LArHECLocalCalculator::GetCalculator();
    }
  else if(a_name.find("HEC::Module::Depth::Slice::Wheel")!=std::string::npos)
    {
      m_calculator = LArHECWheelCalculator::GetCalculator();
    }
  else if(a_name.find("HEC::Module::Depth::Slice")!=std::string::npos)
    {
      m_calculator = LArHECCalculator::GetCalculator();
    }
  else if(a_name.find("MiniFCAL::Wafer")!=std::string::npos)
    {
      m_calculator = MiniFCALCalculator::GetCalculator();
    }

  (*m_log) << MSG::DEBUG << "Constructor: end of constructor" << endreq;
}


LArG4SD::~LArG4SD()
{
  // Display the number of invalid hits in this sensitive detector.
  // Note: this code is only executed if the sensitive-detector object
  // is deleted; I'm not sure if G4 does this.

  if(m_numberInvalidHits > 0)
    (*m_log) << MSG::DEBUG
             << "Destructor: Sensitive Detector <" << m_detectorName << "> had " << m_numberInvalidHits
             << " G4Step energy deposits outside the region determined by its Calculator." << endreq;
  delete m_log;
}

void LArG4SD::Initialize(G4HCofThisEvent* /*HCE*/)
{
  if(m_hitMergers.size()==0)
    {
      // retrieve Hit Merger Factories form Detector Store and
      // initialize m_hitMegrers
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IService* pSvc;
      StatusCode status = svcLocator->service("DetectorStore",pSvc);

      if(status.isSuccess())
        (*m_log) << MSG::DEBUG << "Initialize: Detector Store retrieved" << endreq;
      else
        throw std::runtime_error("LArG4SD: Unable to retrieve Detector Store!");

      StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(!detStore)
        throw std::runtime_error("LArG4SD: 0 pointer to the Detector Store!");

      // Retrieve Hit Merger factory from DS and initialize m_hitMerger
      const DataHandle<LArVHitMergerFactory> _factory_begin;
      const DataHandle<LArVHitMergerFactory> _factory_end;

      status = detStore->retrieve(_factory_begin,_factory_end);

      if(status.isFailure())
        throw std::runtime_error("LArG4SD: Unable to retrieve Hit Merger Factories!");
      else
        (*m_log) << MSG::DEBUG << "Initialize: Hit Merger Factories retrieved" << endreq;

      for(;_factory_begin!=_factory_end;_factory_begin++)
        m_hitMergers.push_back( _factory_begin->getHitMerger());

      if(m_hitMergers.size()==0)
        (*m_log) << MSG::DEBUG << "Initialize: No Hit Merger assigned to SD!" << endreq;
    }

  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->BeginOfEvent();
}


G4bool LArG4SD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)
  if(a_step->GetTotalEnergyDeposit() == 0.) return false;

  // Convert the G4Step into (eta,phi,sampling).
  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)
  if(!(m_calculator->Process(a_step))) {
    m_numberInvalidHits++;
    return false;
  }

  // A calculator can determine that a given energy deposit results
  // in more than one hit in the simulation.  FOr each such hit...
  unsigned i(0);


  for(int ihit=0; ihit<m_calculator->getNumHits(); ihit++) {
    for(i=0; i<m_hitMergers.size(); i++) {
      if(m_hitMergers[i]
         && (!m_hitMergers[i]->process(a_step,
                                       m_calculator->identifier(ihit),
                                       m_calculator->time(ihit),
                                       m_calculator->energy(ihit)))
         && (m_log->level()==MSG::DEBUG))
        (*m_log) << MSG::DEBUG << "ProcessHits: Wrong Identifier after conversion from LArG4Identifier"
                 << std::string(m_calculator->identifier(ihit)) << endreq;
    }
  }// for each hit return by the calculator.
  return true;
}


void LArG4SD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->EndOfEvent(); 
}
