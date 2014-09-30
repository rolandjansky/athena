/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VerboseSelector_H
#define VerboseSelector_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"
#include <string>

class VerboseSelector: public FADS::ActionsBase , public FADS::UserAction, public FADS::TrackingAction {

  public:
   VerboseSelector(std::string s): FADS::ActionsBase(s), FADS::UserAction(s), FADS::TrackingAction(),
                                   targetEvent(0), targetTrack(0), targetBarcode(0), verboseLevel(0), evtCount(1)
   {;}

   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);
   void PreUserTrackingAction(const G4Track* aTrack);
   void PostUserTrackingAction(const G4Track* aTrack);

  private:
   int targetEvent;
   int targetTrack;
   int targetBarcode;
   int verboseLevel;
   int evtCount;
   void ParseProperties();

};

#endif
