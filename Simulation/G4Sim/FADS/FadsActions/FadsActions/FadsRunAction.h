/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsRunAction_H
#define FadsRunAction_H

#include "FadsActions/UserAction.h"
#include "FadsActions/ApplicationAction.h"
#include "G4UserRunAction.hh"

class G4Run;

namespace FADS {

class FadsRunAction: public G4UserRunAction {
private:
	static FadsRunAction* thePointer;
	FadsRunAction():G4UserRunAction(),
			applAction(0) {}
	actionMap theBegin;
	actionMap theEnd;
	ApplicationAction *applAction;
public:	
	static FadsRunAction* GetRunAction();
	void RegisterAction(UserAction *a,actionType t,int i=5);
	void EndOfRunAction(const G4Run* aRun); 
	void BeginOfRunAction(const G4Run* aRun);
	void SetApplicationAction(ApplicationAction *a) {applAction=a;}
};

}	// end namespace

#endif
