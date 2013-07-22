/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsTrackingAction_H
#define FadsTrackingAction_H

#include "FadsActions/ApplicationTrackingAction.h"
#include "G4UserTrackingAction.hh"
#include "G4VTrajectory.hh"

class G4RunManager;

#include <vector>

namespace FADS {

class TrackingAction;

class FadsTrackingAction: public G4UserTrackingAction {
public:
	static FadsTrackingAction* GetTrackingAction();
	~FadsTrackingAction();
	void RegisterAction(TrackingAction*);
	void PreUserTrackingAction(const G4Track*);
	void PostUserTrackingAction(const G4Track*);
	void SetTraj(G4VTrajectory *t);
	G4VTrajectory* GetTraj();
	void ResetTraj();
	G4TrackingManager* GetTrackingManager() const {return fpTrackingManager;}
	void RegisterApplicationAction(ApplicationTrackingAction *a)
	{
		applAction=a;
	}
private:
	std::vector<TrackingAction*> actionList;
	FadsTrackingAction();
	static FadsTrackingAction* thePointer;
	ApplicationTrackingAction* applAction;
};

}	// end namespace

#endif
