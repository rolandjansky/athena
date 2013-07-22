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
		// Sadly, none of these have names that can be printed
		//std::cout<<" run action "<<G4RunManager::GetRunManager()->GetUserRunAction()->GetName()<<std::endl;
		//std::cout<<" event action "<<G4RunManager::GetRunManager()->GetUserEventAction()->GetName()<<std::endl;
		//std::cout<<" tracking action "<<G4RunManager::GetRunManager()->GetUserTrackingAction()->GetName()<<std::endl;
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
