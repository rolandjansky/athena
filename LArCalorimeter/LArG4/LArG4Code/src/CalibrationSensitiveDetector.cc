/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationSensitiveDetector.h

// The sensitive-detector class for detectors that can have
// calibration hits in LArG4.

// 08-Jan-2004 Bill Seligman

#undef DEBUG_SD
#undef DEBUG_HITS

#include "LArG4Code/CalibrationSensitiveDetector.h"

#include "LArG4Code/VCalibrationCalculator.h"
#include "LArG4Code/LArVCalibHitMerger.h"
#include "LArG4Code/LArVCalibHitMergerFactory.h"

#include "G4VHit.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

#include "G4RunManager.hh"
#include "MCTruth/EventInformation.h"
#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "ISF_Event/ISFParticle.h"
#include "MCTruth/VTrackInformation.h"

#include <vector>


namespace LArG4 {
  
  CalibrationSensitiveDetector::CalibrationSensitiveDetector(G4String a_name, VCalibrationCalculator* a_calc):
    G4VSensitiveDetector(a_name),
    m_calculator(a_calc),
    m_detectorName(a_name),
    m_numberInvalidSteps(0),
    m_event_info(0),
    m_doCalibHitParticleID(false)
  {
  }


  CalibrationSensitiveDetector::~CalibrationSensitiveDetector() 
  {
    // Display the number of invalid hits in this sensitive detector.
    // Note: this code is only executed if the sensitive-detector object
    // is deleted; I'm not sure if G4 does this.
    if ( m_numberInvalidSteps > 0 ) 
      {
	G4cerr << "LArG4::CalibrationSensitiveDetector: Sensitive Detector <"
	       << m_detectorName
	       << "> had "
	       << m_numberInvalidSteps
	       << " G4Step energy deposits outside the region determined by its Calculator."
	       << G4endl;
      }
  }


  void CalibrationSensitiveDetector::Initialize(G4HCofThisEvent* /*HCE*/)
  {
    if(m_hitMergers.size()==0)
    {
      ISvcLocator* svcLocator = Gaudi::svcLocator(); 
      IService* pSvc;
      StatusCode status = svcLocator->service("DetectorStore",pSvc);
    
      if(status.isFailure())
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
      
      for(;_factory_begin!=_factory_end;_factory_begin++)
	m_hitMergers.push_back( _factory_begin->getHitMerger());

      // read flag to produce calibration hits signed with primary particle ID
      LArG4GlobalOptions *globalOptions;
      status = detStore->retrieve(globalOptions, "LArG4GlobalOptions");
      if (status.isFailure()) {
        throw std::runtime_error("LArG4CalibSD: LArG4GlobalOption was not found in DetectorStore");
      } else {
        m_doCalibHitParticleID = globalOptions->GetDoCalibHitParticleID();
      }
    }

    for(unsigned int i=0; i<m_hitMergers.size(); i++)
      m_hitMergers[i]->BeginOfEvent(); 

    m_event_info = 0;    
  }


  G4bool CalibrationSensitiveDetector::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
  {
    // Convert the G4Step into identifier and energy.

    // Check that hit was valid.  (It might be invalid if, for example,
    // it occurred outside the sensitive region.  If such a thing
    // happens, it means that the geometry definitions in the
    // detector-construction routine and the calculator do not agree.)

    if (m_doCalibHitParticleID && !m_event_info) {
      m_event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
    }

    if(!(m_calculator->Process(a_step, VCalibrationCalculator::kEnergyAndID))) 
    {
      m_numberInvalidSteps++;
      std::cout<<"invalid step, returning false"<<std::endl;
      return false;
    }

    // access for primary particle identifier of G4Track
    unsigned int particle_id = 0;
    if ( m_doCalibHitParticleID && m_event_info )
      particle_id = m_event_info->GetCurrentPrimary()->barcode();
    
    for(unsigned int i=0; i<m_hitMergers.size(); i++)
    {
      if(m_hitMergers[i])
	m_hitMergers[i]->process(m_calculator->identifier()
				 ,m_calculator->energies()
				 ,particle_id);
    }

    return true;
  }


  void CalibrationSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/)
  {
    for(unsigned int i=0; i<m_hitMergers.size(); i++) {
      m_hitMergers[i]->EndOfEvent(m_detectorName);
    }
  }



  // Something special has happened (probably the detection of escaped
  // energy in CaloG4Sim/SimulationEnergies).  We have to bypass the
  // regular sensitive-detector processing.  Determine the identifier
  // (and only the identifier) using the calculator, then built a hit
  // with that identifier and the energies in the vector.

  G4bool CalibrationSensitiveDetector::SpecialHit(G4Step* a_step, 
						  const std::vector<G4double>& a_energies)
  {
    // Convert the G4Step into an identifier.

    // If we can't calculate the identifier, something is wrong.
    if(!(m_calculator->Process( a_step, VCalibrationCalculator::kOnlyID ))) return false;

    // access for primary particle identifier of G4Track
    unsigned int particle_id = 0;
    if( m_doCalibHitParticleID && m_event_info )
      particle_id = m_event_info->GetCurrentPrimary()->barcode();
        
    for(unsigned int i=0; i<m_hitMergers.size(); i++)
    {
      if(m_hitMergers[i])
	m_hitMergers[i]->process(m_calculator->identifier()
				 ,a_energies
				 ,particle_id);
    }

    return true;
  }

} // namespace LArG4

