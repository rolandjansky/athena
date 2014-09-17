/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ApplicationTrackingAction_H
#define ApplicationTrackingAction_H

#include "G4Track.hh"
#include "G4RunManager.hh"

namespace FADS {

class ApplicationTrackingAction {
public:
	ApplicationTrackingAction()
	{
		Register();
	}
	virtual ~ApplicationTrackingAction() {}
	virtual void PreUserTrackingAction(const G4Track*) {}
	virtual void PostUserTrackingAction(const G4Track*) {}
	G4TrackingManager* GetTrackingManager();
	
private:
	void Register();
};

}	// end namespace

#endif
