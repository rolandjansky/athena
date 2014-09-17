/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsStackingAction_H
#define FadsStackingAction_H

#include "G4UserStackingAction.hh"
#include "FadsActions/ApplicationStackingAction.h"

class G4RunManager;

#include <vector>

namespace FADS {

class StackingAction;

class FadsStackingAction: public G4UserStackingAction {
public:
	~FadsStackingAction();
	static FadsStackingAction* GetStackingAction();
	void RegisterAction(StackingAction*);
	void RegisterApplicationAction(ApplicationStackingAction *a)
	{
		applAction=a;
	}
	
	G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
	void NewStage();
	void PrepareNewEvent();
	
	G4StackManager* GetStackManager() const {return stackManager;}
private:
	FadsStackingAction();
	std::vector<StackingAction*> actionList;
	static FadsStackingAction* thePointer;
	ApplicationStackingAction *applAction;
};

}	// end namespace

#endif
