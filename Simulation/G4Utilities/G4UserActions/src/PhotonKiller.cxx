/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/PhotonKiller.h"

#include "G4Step.hh"
#include "G4Event.hh"

#include "G4Gamma.hh"
#include "G4RunManagerKernel.hh"

namespace G4UA
{

  //---------------------------------------------------------------------------
  PhotonKiller::PhotonKiller()
    : m_count(0), m_energy(0)
  {}

  //---------------------------------------------------------------------------
  void PhotonKiller::PreUserTrackingAction(const G4Track*)
  {
    // reset counters
    m_count=0;
    m_energy=0;
  }

  //---------------------------------------------------------------------------
  void PhotonKiller::UserSteppingAction(const G4Step* aStep)
  {
    if ( fabs(m_energy-aStep->GetTrack()->GetKineticEnergy())<0.00001 ){
      // same energy as last time
      m_count++;
    } else {
      m_count=0;
      m_energy = aStep->GetTrack()->GetKineticEnergy();
      return;
    }

    if (aStep->GetTrack()->GetKineticEnergy() < 0.0001){ // Less than one hundred eV
      if ( (m_count>3 && aStep->GetTrack()->GetDefinition() == G4Gamma::Gamma() ) ||
           (m_count>10000) ){ // more than three steps with less than one keV of energy...
        // Drop the photon
        aStep->GetTrack()->SetTrackStatus( fStopAndKill );
      }
    } else if (m_count>1000000){
      // Looper Killer functionality
      aStep->GetTrack()->SetTrackStatus( fStopAndKill );
      G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
      rmk->GetEventManager()->AbortCurrentEvent();
      rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
    }
  }

} // namespace G4UA
