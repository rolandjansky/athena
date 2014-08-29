/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/PhotonKiller.h"
#include <iostream>
#include <cmath>

static PhotonKiller phk("PhotonKiller");

void PhotonKiller::BeginOfEventAction(const G4Event* /*anEvent*/){;}
void PhotonKiller::EndOfEventAction(const G4Event* /*anEvent*/){;}
void PhotonKiller::BeginOfRunAction(const G4Run* /*aRun*/){;}
void PhotonKiller::EndOfRunAction(const G4Run* /*aRun*/){;}

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4RunManagerKernel.hh"
#include "G4Event.hh"

void PhotonKiller::SteppingAction(const G4Step* aStep)
{
  static G4Track * lastTrack = aStep->GetTrack();
  static int count = 0;
  static double energy = 0;

  if (lastTrack != aStep->GetTrack()){
    lastTrack = aStep->GetTrack();
    count = 0;
    return;
  }

  if ( fabs(energy-aStep->GetTrack()->GetKineticEnergy())<0.00001 ){
    // same energy as last time
    count++;
  } else {
    count=0;
    energy = aStep->GetTrack()->GetKineticEnergy();
    return;
  }

  if (aStep->GetTrack()->GetKineticEnergy() < 0.0001){ // Less than one hundred eV
    if ( (count>3 && aStep->GetTrack()->GetDefinition() == G4Gamma::Gamma() ) ||
         (count>10000) ){ // more than three steps with less than one keV of energy...
      // Drop the photon
      aStep->GetTrack()->SetTrackStatus( fStopAndKill );
    }
  } else if (count>1000000){
    // Looper Killer functionality
    aStep->GetTrack()->SetTrackStatus( fStopAndKill );
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    rmk->GetEventManager()->AbortCurrentEvent();
    rmk->GetEventManager()->GetNonconstCurrentEvent()->SetEventAborted();
  }
}
