/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsSteppingAction_H
#define FadsSteppingAction_H

#include "FadsActions/UserAction.h"
#include "G4UserSteppingAction.hh"

class G4Step;

namespace FADS {

class FadsSteppingAction: public G4UserSteppingAction {
  private:
   static FadsSteppingAction* thePointer;
   FadsSteppingAction():G4UserSteppingAction(),mctAction(0) {}
   actionMap theMap;
   G4UserSteppingAction* mctAction;
  public:
   static FadsSteppingAction* GetSteppingAction();
   void RegisterAction(UserAction *a,int i=5);
   void UserSteppingAction(const G4Step* aStep);
   void SetMCTSteppingAction(G4UserSteppingAction* action) { mctAction= action; }
};

}	// end namespace

#endif
