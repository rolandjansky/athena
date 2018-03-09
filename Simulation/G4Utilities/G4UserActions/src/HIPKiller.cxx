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

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  HIPKiller::HIPKiller()
    : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                   "HIPKiller")
  {}

  //---------------------------------------------------------------------------
  void HIPKiller::UserSteppingAction(const G4Step* aStep)
  {
    int PDGcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    // check if PDG code compatible with HIP (Monopole:411xxx0 or Qball:100xxxx0)
    if ( ((abs(PDGcode)/100000==41) && (PDGcode/10000000==0)) ||
         ((int)(abs(PDGcode)/10000000) == 1) )
    {
      // Kill if energy is less than 1MeV
      if (aStep->GetTrack()->GetKineticEnergy() < 1.0) {
        aStep->GetTrack()->SetTrackStatus( fStopAndKill );
        ATH_MSG_WARNING("HIP " << PDGcode << " is being killed: kinetic energy=" <<
                        aStep->GetTrack()->GetKineticEnergy()/CLHEP::MeV << "MeV");
      }
    }
  }

} // namespace G4UA
