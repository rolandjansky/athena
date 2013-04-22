/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SecondaryTracksHelper_H
#define SecondaryTracksHelper_H

// Helper class for retrieving information on track created in the last step

#include "G4TrackingManager.hh"
#include <vector>

class SecondaryTracksHelper {
public:
	SecondaryTracksHelper();
	SecondaryTracksHelper(const G4TrackingManager *);
	static void SetTrackingManager(const G4TrackingManager *);
	static const G4TrackingManager *GetTrackingManager();  // awful..
	int NrOfNewSecondaries();
	inline void ResetNrOfSecondaries() { currentNrOfSecondaries=0; }
	inline int CurrentNrOfSecondaries() const {
	  return theTrackingManager->GimmeSecondaries()->size();
	}

	std::vector<G4Track*> GetSecondaries(int) const;
private:
	static const G4TrackingManager *theTrackingManager;
	int currentNrOfSecondaries;
};

#endif

