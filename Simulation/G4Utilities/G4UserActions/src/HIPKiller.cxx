/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/HIPKiller.h"
#include <iostream>

#include "G4Event.hh"
#include "G4Step.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"

#include "CLHEP/Units/PhysicalConstants.h"

void HIPKiller::Step(const G4Step* aStep)
{
  int PDGcode=aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  
  // check if PDG code compatible with HIP (Monopole:411xxx0 or Qball:100xxxx0)
  if ( ((abs(PDGcode)/100000==41) && (PDGcode/10000000==0)) || ((int)(abs(PDGcode)/10000000) == 1) ) {
    //    std::cout<<"SB: HIPKiller Step:" << PDGcode << ", energy=" << aStep->GetTrack()->GetKineticEnergy() 
    //	     <<std::endl;
    if (aStep->GetTrack()->GetKineticEnergy() < 1.0) {    // Energy is less than 1MeV
      aStep->GetTrack()->SetTrackStatus( fStopAndKill );
      ATH_MSG_WARNING("HIP " << PDGcode << " is being killed: kinetic energy="<< aStep->GetTrack()->GetKineticEnergy()/CLHEP::MeV << "MeV");
    }
  }
}


StatusCode HIPKiller::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
