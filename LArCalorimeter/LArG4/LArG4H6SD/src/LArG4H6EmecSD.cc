/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H6EmecSD.h"
//#include "LArG4H6SD/LArHitsEventAction.h"
#include "LArG4Code/LArG4Hit.h"

#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/RunOptions.h"
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4Code/LArVHitMergerFactory.h"

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

#include "LArG4EC/EnergyCalculator.h"
#include "LArG4EC/LArEndcapPresamplerCalculator.h"

#include "LArG4RunControl/LArG4GlobalOptions.h"
#include "LArSimEvent/LArHitContainer.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"

using namespace FADS;

LArG4H6EmecSD::LArG4H6EmecSD(G4String a_name)
  : FadsSensitiveDetector(a_name),
    m_calculator(0),
    m_timeBinWidth(2.5*CLHEP::ns),
    m_detectorName(a_name),
    m_numberInvalidHits(0),
    m_detStore(0)//,
    //    m_emecHitCollection(0)
{

  // Next lines fail.  The LArTBECHitsEventAction cannot be instantiated
  // until it compiles :).

  //static LArTBECHitsEventAction* TBapplEventAction = 0;
  //if(TBapplEventAction == 0) TBapplEventAction = new LArTBECHitsEventAction;

  // Get pointer to the message service
  ATH_MSG_DEBUG ( "--- Call LArG4H6EmecSD for detector " << a_name );

  // Get appropriate calculator
  if(a_name == "LAr::EMEC::InnerModule::SDin_gapadj")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_gapold")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_OLD);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_gap_e")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_E);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_gap_s")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_S);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_gap_se")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_SE);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_chcoll")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_chcoll1")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL1);
    }
  else if(a_name == "LAr::EMEC::InnerModule::SDin_ropt")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::InnerAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_gapadj")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_gapold")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_OLD);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_gap_e")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_E);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_gap_s")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_S);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_gap_se")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_SE);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_chcoll")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_chcoll1")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL1);
    }
  else if(a_name == "LAr::EMEC::OuterModule::SDout_ropt")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::OuterAbsorberModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gapadj")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gapold")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_OLD);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_e")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_E);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_s")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_S);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_gap_se")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_GADJ_SE);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_chcoll")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_chcoll1")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_CHCL1);
    }
  else if(a_name == "LAr::EMEC::BackOuterBarrette::Module::Phidiv::SDout_ropt")
    {
      m_calculator = new LArG4::EC::EnergyCalculator(LArWheelCalculator::BackOuterBarretteModule,
                                                     LArG4::EC::EnergyCalculator::EMEC_ECOR_ROPT);
    }

  // Get the value for TimeBinWidth from LArG4RunControl
  const LArG4GlobalOptions *glOpt(NULL);
  IService *pSvc(NULL);
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  if(svcLocator->service("DetectorStore",pSvc).isSuccess())
    {
      m_detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(m_detStore==0) throw std::runtime_error("LArG4H6EmecSD: Dynamic cast StoreGateSvc* pointer return null!");

      ATH_MSG_DEBUG ( "try to retrieve LArG4GlobalOptions " );
      if(m_detStore->retrieve(glOpt,"LArG4GlobalOptions").isSuccess())
        ATH_MSG_DEBUG ( "LArG4GlobalOptions retrieved!" );
      else
        ATH_MSG_DEBUG ( "Unable to retrieve LArG4GlobalOptions! Using default value for TimeBinWidth");

      const CaloIdManager *caloIdManager(NULL);
      if(m_detStore->retrieve(caloIdManager).isSuccess())
        ATH_MSG_DEBUG ( "CaloIDManager retrieved!" );
      else
        throw std::runtime_error("LArG4H6EmecSD: Unable to retrieve CaloIDManager");

      m_larEmID = caloIdManager->getEM_ID();
      if(m_larEmID==0)
        throw std::runtime_error("LArG4H6EmecSD: Invalid LAr EM ID helper");
    }
  else
    throw std::runtime_error("LArG4H6EmecSD: Unable to retrieve Detector Store!");

  if(glOpt)
    m_timeBinWidth = glOpt->TimeBinWidth();

  std::cout << " m_timeBinWidth " << m_timeBinWidth << std::endl;


  if (glOpt)
    ATH_MSG_DEBUG ( " option='" << glOpt->TimeBinWidth() << "', m_timeBinWidth=" <<  m_timeBinWidth/ CLHEP::ns << " ns" );
  else
    ATH_MSG_DEBUG ( " no option defined m_timeBinWidth " <<  m_timeBinWidth/ CLHEP::ns );

  ATH_MSG_DEBUG ( "end of LArG4H6EmecSD::LArG4H6EmecSD" );
}


LArG4H6EmecSD::~LArG4H6EmecSD()
{
  // Display the number of invalid hits in this sensitive detector.
  // Note: this code is only executed if the sensitive-detector object
  // is deleted; I'm not sure if G4 does this.

  if(m_numberInvalidHits > 0)
    {
      ATH_MSG_DEBUG ( "~LArG4H6EmecSD(): Sensitive Detector <" << m_detectorName << "> had " << m_numberInvalidHits
                      << " G4Step energy deposits outside the region determined by its Calculator." );
    }
}

void LArG4H6EmecSD::Initialize(G4HCofThisEvent* /*m_HCE*/)
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
        throw std::runtime_error("LArG4H6EmecSD::Initialize: Unable to retrieve Detector Store!");

      StoreGateSvc* detStore = dynamic_cast<StoreGateSvc*>(pSvc);
      if(!detStore)
        throw std::runtime_error("LArG4H6EmecSD::Initialize: 0 pointer to the Detector Store!");

      // Retrieve Hit Merger factory from DS and initialize m_hitMerger
      const DataHandle<LArVHitMergerFactory> _factory_begin;
      const DataHandle<LArVHitMergerFactory> _factory_end;

      if(detStore->retrieve(_factory_begin,_factory_end).isFailure())
        throw std::runtime_error("LArG4H6EmecSD::Initialize: Unable to retrieve Hit Merger Factories!");
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


G4bool LArG4H6EmecSD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)

  G4double edep = a_step->GetTotalEnergyDeposit();
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
      G4double time = m_calculator->time(ihit);
      G4double energy = m_calculator->energy(ihit);

      for(unsigned int i=0; i<m_hitMergers.size(); i++)
        {
          LArVHitMerger* _hitMerger = m_hitMergers[i];
          if(_hitMerger && (!_hitMerger->process(a_step,ident,time,energy)))
            ATH_MSG_DEBUG ( "ProcessHits: Wrong Identifier after conversion from LArG4Identifier"
                            << std::string(ident) );
        }
    }// for each hit return by the calculator.
  return true;

}

void LArG4H6EmecSD::EndOfEvent(G4HCofThisEvent* /*m_HCE*/)
{
  for(unsigned int i=0; i<m_hitMergers.size(); i++)
    m_hitMergers[i]->EndOfEvent();
}
