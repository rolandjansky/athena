/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ApplicationStackingAction_H
#define ApplicationStackingAction_H

#include "G4Track.hh"
#include "G4ClassificationOfNewTrack.hh"

class G4StackManager;

//class FadsStackingAction;

namespace FADS {

class ApplicationStackingAction {
public:
	ApplicationStackingAction() {Register();}
	virtual ~ApplicationStackingAction() {}
	
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* /*aTrack*/)
	{
		return fUrgent;
	}
	virtual void NewStage() {}
	virtual void PrepareNewEvent() {}
	
	G4StackManager* GetStackManager();
private:
	void Register();
};

}	// end namespace

#endif
