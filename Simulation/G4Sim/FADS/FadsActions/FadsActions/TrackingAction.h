/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackingAction_H
#define TrackingAction_H

#include "FadsActions/FadsTrackingAction.h"
#include "G4Track.hh"
#include "G4RunManager.hh"

namespace FADS {

class TrackingAction {
public:
	TrackingAction() 
	{
		Register();
	}
	virtual ~TrackingAction() {}
	virtual void PreUserTrackingAction(const G4Track*) {}
	virtual void PostUserTrackingAction(const G4Track*) {}
	G4TrackingManager* GetTrackingManager()
	{
		return FadsTrackingAction::GetTrackingAction()->GetTrackingManager();
	}
private:
	void Register()
	{
		FadsTrackingAction::GetTrackingAction()->RegisterAction(this);
	}
};

}	// end namespace

#endif
