/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ScoringVolumeTrackKiller_H
#define ScoringVolumeTrackKiller_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class ScoringVolumeTrackKiller: public FADS::ActionsBase , public FADS::UserAction {

  public:
   ScoringVolumeTrackKiller(std::string s):
     FADS::ActionsBase(s),FADS::UserAction(s),killCount(0) {}

   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

  private:
   unsigned long killCount;

}; 

#endif

