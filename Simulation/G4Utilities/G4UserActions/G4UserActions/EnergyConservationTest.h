/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EnergyConservationTest_H
#define EnergyConservationTest_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"
#include <string>

class EnergyConservationTest: public FADS::ActionsBase , public FADS::UserAction, public FADS::TrackingAction {

  private:
   double e_in , e_out , e_dep;

  public:
   EnergyConservationTest(std::string s):FADS::ActionsBase(s),FADS::UserAction(s),FADS::TrackingAction(),e_in(0),e_out(0),e_dep(0) {};
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void SteppingAction(const G4Step*);
   void PreUserTrackingAction(const G4Track*);
   void PostUserTrackingAction(const G4Track*);

};

#endif
