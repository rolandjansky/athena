/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestAction2_H
#define TestAction2_H

#include "FadsActions/ActionsBase.h"
#include "FadsActions/UserAction.h"
#include <string>

class TestAction2: public FADS::ActionsBase , public FADS::UserAction {

  public:
   TestAction2(std::string s): FADS::ActionsBase(s) , FADS::UserAction(s) {}
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);
   void BeginOfRunAction(const G4Run*);
   void EndOfRunAction(const G4Run*);
   void SteppingAction(const G4Step*);

};

#endif
