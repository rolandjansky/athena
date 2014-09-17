/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StackingAction_H
#define StackingAction_H

#include "FadsActions/FadsStackingAction.h"
#include "G4Track.hh"
#include "G4ClassificationOfNewTrack.hh"

namespace FADS {

class StackingAction {
public:
	StackingAction() {Register();}
	virtual ~StackingAction() {}
	
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* /*aTrack*/)
	{
		return fUrgent;
	}
	virtual void NewStage() {}
	virtual void PrepareNewEvent() {}
	
	G4StackManager* GetStackManager()
	{
		return FadsStackingAction::GetStackingAction()->GetStackManager();
	}
private:
	void Register()
	{
		FadsStackingAction::GetStackingAction()->RegisterAction(this);
	}
};

}	// end namespace

#endif
