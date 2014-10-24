/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H62004SD.h"
#include "LArG4H6SD/LArG4H6COLDTCMod0Calculator.h"

#include "LArG4Code/LArG4Hit.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/RunOptions.h"
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"

#include "LArG4EC/EnergyCalculator.h"
#include "LArG4FCAL/LArFCAL1Calculator.h"
#include "LArG4FCAL/LArFCAL2Calculator.h"
#include "LArG4HEC/LArHECCalculator.h"
#include "LArG4HEC/LArHECLocalCalculator.h"
#include "LArG4HEC/LArHECLocalCalculator.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"
#include "LArSimEvent/LArHitContainer.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4VHit.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include <map>
#include <vector>
#include <climits>

#undef DEBUG_ME

static FADS::SensitiveDetectorEntryT<LArG4H62004SD> hecSliceLocal2004SD("LAr::HEC::Module::Depth::Slice::Local::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004SD> innerWheelSD("LAr::EMEC::InnerModule::H6");
//static FADS::SensitiveDetectorEntryT<LArG4H62004SD> outerWheelSD("LAr::EMEC::OuterWheel::H6");
//static FADS::SensitiveDetectorEntryT<LArG4H62004SD> backouterbarrettSD("LAr::EMEC::BackOuterBarrette::Module::Phidiv::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004SD> backinnerbarrettSD("LAr::EMEC::BackInnerBarrette::Module::Phidiv::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004SD> fcalMod1SD("LAr::FCAL::Module1::Gap::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004SD> fcalMod2SD("LAr::FCAL::Module2::Gap::H6");
static FADS::SensitiveDetectorEntryT<LArG4H62004SD> ColdTCSD("LAr::FCAL::ColdTC::H6");

LArG4H62004SD::LArG4H62004SD(G4String a_name)
  : FadsSensitiveDetector(a_name),
    m_calculator(0),
    m_timeBinWidth(2.5*CLHEP::ns),
    m_detectorName(a_name),
    m_numberInvalidHits(0),
    m_detStore(0),
    m_emecHitCollection(0),
    m_fcalHitCollection(0),
    m_hecHitCollection(0)
{

  // Set appropriate calculator
#ifdef DEBUG_ME
  std::cout<<"LArG4H62004SD::LArG4H62004SD name: "<<a_name<<std::endl;
#endif

  // Get pointer to the message service
  ATH_MSG_DEBUG ( "--- Call LArG4H62004SD for detector " << a_name );

  // Get appropriate calculator
  if(a_name.find("EMEC::InnerModule::H6")!=std::string::npos) {
    m_calculator= new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ);
    //  } else if(a_name.find("EMEC::BackOuterBarrette")!=std::string::npos) {
    //    m_calculator= new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteWheel);
  } else if(a_name.find("EMEC::BackInnerBarrette")!=std::string::npos) {
    m_calculator= new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackInnerBarretteModule);
  } else if(a_name.find("FCAL::Module1::Gap::H6")!=std::string::npos) {
    m_calculator = LArFCAL1Calculator::GetInstance();
  } else if(a_name.find("FCAL::Module2::Gap::H6")!=std::string::npos) {
    m_calculator = LArFCAL2Calculator::GetInstance();
  } else if(a_name.find("LAr::FCAL::ColdTC::H6")!=std::string::npos) {
    m_calculator = LArG4H6COLDTCMod0Calculator::GetInstance();
    ((LArG4H6COLDTCMod0Calculator *) m_calculator)->SetOutOfTimeCut(10000.);
  } else if(a_name.find("HEC::Module::Depth::Slice::Local::H6")!=std::string::npos) {
    m_calculator = LArHECLocalCalculator::GetCalculator();
    ((LArHECLocalCalculator *) m_calculator)->SetOutOfTimeCut(10000.);
    ((LArHECLocalCalculator *) m_calculator)->SetX(true);
  }

  // Get the value for TimeBinWidth from LArG4RunControl
  const LArG4GlobalOptions *glOpt(NULL);
  IService *pSvc(NULL);
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  if(svcLocator->service("DetectorStore",pSvc).isSuccess())
    {
      m_detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(m_detStore==0)  throw std::runtime_error("LArG4H62004SD: Dynamic cast StoreGateSvc* pointer return null!");
#ifdef DEBUG_ME
      std::cout << "try to retrieve LArG4GlobalOptions " << std::endl;
#endif
      if(m_detStore->retrieve(glOpt,"LArG4GlobalOptions").isSuccess())
        ATH_MSG_DEBUG ( "LArG4GlobalOptions retrieved!" );
      else
        ATH_MSG_DEBUG ( "Unable to retrieve LArG4GlobalOptions! Using default value for TimeBinWidth" );

      const CaloIdManager *caloIdManager(NULL);
      if(m_detStore->retrieve(caloIdManager).isSuccess())
        ATH_MSG_DEBUG ( "CaloIDManager retrieved!" );
      else
        throw std::runtime_error("LArG4H62004SD: Unable to retrieve CaloIDManager");

      m_larEmID = caloIdManager->getEM_ID();
      if(m_larEmID==0)
        throw std::runtime_error("LArG4H62004SD: Invalid LAr EM ID helper");

      m_larFcalID = caloIdManager->getFCAL_ID();
      if(m_larFcalID==0)
        throw std::runtime_error("LArG4H62004SD: Invalid FCAL ID helper");

      m_larHecID = caloIdManager->getHEC_ID();
      if(m_larHecID==0)
        throw std::runtime_error("LArG4H62004SD: Invalid HEC ID helper");
    }
  else
    throw std::runtime_error("LArG4H62004SD: Unable to retrieve Detector Store!");

  if(glOpt)
    m_timeBinWidth = glOpt->TimeBinWidth();

#ifdef DEBUG_ME
  std::cout << " m_timeBinWidth " << m_timeBinWidth << std::endl;
#endif


  // 11-Jul-2005 WGS: Get the time bin width from the user options.
  // ******************* FIXME *************************************
  //  LArG4::RunOptions* runOptions = LArG4::RunOptions::GetInstance();
  //  std::string option = runOptions->GetOption( LArG4::kOutOfTimeCut );

  // Convert the option from a text string into a number.
  //  std::istringstream inputStream( option );
  //  inputStream >> m_timeBinWidth;

  if (glOpt)
    ATH_MSG_DEBUG ( " option='" << glOpt->TimeBinWidth() << "', m_timeBinWidth=" <<  m_timeBinWidth/ CLHEP::ns << " ns" );
  else
    ATH_MSG_DEBUG ( " no option defined m_timeBinWidth " <<  m_timeBinWidth/ CLHEP::ns );

  ATH_MSG_DEBUG ( "end of LArG4H62004SD::LArG4H62004SD" );

}

LArG4H62004SD::~LArG4H62004SD()
{
  // Display the number of invalid hits in this sensitive detector.
  // Note: this code is only executed if the sensitive-detector object
  // is deleted; I'm not sure if G4 does this.

  if(m_numberInvalidHits > 0)
    {
      ATH_MSG_DEBUG ( "~LArG4H62004SD: Sensitive Detector <" << m_detectorName << "> had " << m_numberInvalidHits
                      << " G4Step energy deposits outside the region determined by its Calculator." );
    }
}

void LArG4H62004SD::Initialize(G4HCofThisEvent* /*m_HCE*/)
{
  if(m_hitMergers.size()==0)
    {
      // retrieve Hit Merger Factories form Detector Store and
      // initialize m_hitMegrers
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      IService* pSvc;
      if(svcLocator->service("DetectorStore",pSvc).isSuccess())
        ATH_MSG_DEBUG ( "Initialize: Detector Store retrieved" );
      else
        throw std::runtime_error("LArG4H62004SD::Initialize: Unable to retrieve Detector Store!");

      StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(!detStore)
        throw std::runtime_error("LArG4H62004SD::Initialize: 0 pointer to the Detector Store!");

      // Retrieve Hit Merger factory from DS and initialize m_hitMerger
      const DataHandle<LArVHitMergerFactory> _factory_begin;
      const DataHandle<LArVHitMergerFactory> _factory_end;

      if(detStore->retrieve(_factory_begin,_factory_end).isFailure())
        throw std::runtime_error("LArG4H62004SD::Initialize: Unable to retrieve Hit Merger Factories!");
      else
        ATH_MSG_DEBUG ( "Initialize: Hit Merger Factories retrieved" );

      for(;_factory_begin!=_factory_end;_factory_begin++)
        m_hitMergers.push_back( _factory_begin->getHitMerger());

      if(m_hitMergers.size()==0)
        ATH_MSG_DEBUG ( "Initialize: No Hit Merger assigned to SD!" );
    }

  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->BeginOfEvent();
}


G4bool LArG4H62004SD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)

  G4double edep = a_step->GetTotalEnergyDeposit();
#ifdef DEBUG_ME
  ATH_MSG_DEBUG ( "ProcessHits: my name: "<<this->GetName()<<", calc.: "<<m_calculator<<", energy: "<<edep );
#endif
  if (edep == 0.) return false;

  // Convert the G4Step into (eta,phi,sampling).
  G4bool valid = m_calculator->Process(a_step);

  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)

  if ( ! valid )
    {
      m_numberInvalidHits++;
      return false;
    }

  // A calculator can determine that a given energy deposit results
  // in more than one hit in the simulation.  FOr each such hit...

  for(int ihit=0;ihit<m_calculator->getNumHits();ihit++)
    {
      LArG4Identifier ident = m_calculator->identifier(ihit);
      if(ident[0] == 10) { // dead hit, where to put it ?
        // probably skip in the regular container
        continue;
      }
      G4double time = m_calculator->time(ihit);
      G4double energy = m_calculator->energy(ihit);
      // Changing the ident to comply with H6 dictionary
      if(ident[1] == 1) { // EMEC module
        ident[6] += 20; // change phi id
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
      for(unsigned int i=0; i<m_hitMergers.size(); i++)
        {
          LArVHitMerger* _hitMerger = m_hitMergers[i];
          if(_hitMerger && (!_hitMerger->process(a_step,ident,time,energy)))
            ATH_MSG_DEBUG ( "ProcessHits: Wrong Identifier after conversion from LArG4Identifier "
                            << std::string(ident) );
        }
    }// for each hit return by the calculator.
  return true;

}


void LArG4H62004SD::EndOfEvent(G4HCofThisEvent* /*m_HCE*/)
{
  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->EndOfEvent();
}
