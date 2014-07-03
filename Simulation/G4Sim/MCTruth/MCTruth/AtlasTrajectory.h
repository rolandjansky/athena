/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AtlasTrajectory_H
#define AtlasTrajectory_H

#include "G4Trajectory.hh"
#include "G4TrackingManager.hh"

#include "SimHelpers/SecondaryTracksHelper.h"

class AtlasTrajectory: public G4Trajectory {
public:
	AtlasTrajectory();
	AtlasTrajectory(const G4Track* aTrack);
	~AtlasTrajectory() {}
	void AppendStep(const G4Step* aStep);
	void DrawTrajectory(G4int) const;
private:
	const G4TrackingManager *theTrackManager;
	SecondaryTracksHelper theSecondaryTrackHelper;
};

#endif
