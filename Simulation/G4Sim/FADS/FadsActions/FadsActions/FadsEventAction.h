/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsEventAction_H
#define FadsEventAction_H

#include "FadsActions/UserAction.h"
#include "FadsActions/ApplicationAction.h"
#include "G4UserEventAction.hh"

class G4Event;

namespace FADS {

class FadsEventAction: public G4UserEventAction {
  private:
   static FadsEventAction* thePointer;
   FadsEventAction():G4UserEventAction(),mctAction(0),applAction(0) {}
   actionMap theBegin;
   actionMap theEnd;
   G4UserEventAction* mctAction;
   ApplicationAction *applAction;
  public:
   static FadsEventAction* GetEventAction();
   void RegisterAction(UserAction *a,actionType t,int i=5);
   void EndOfEventAction(const G4Event* anEvent); 
   void BeginOfEventAction(const G4Event* anEvent);
   void SetMCTEventAction(G4UserEventAction* action) { mctAction= action; }
   void SetApplicationAction(ApplicationAction *a) {applAction=a;}
};

}	// end namespace

#endif
