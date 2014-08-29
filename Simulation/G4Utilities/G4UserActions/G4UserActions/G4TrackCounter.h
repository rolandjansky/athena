/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4TrackCounter_H
#define G4TrackCounter_H

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"
#include <string>

class G4TrackCounter: public FADS::UserAction, public FADS::TrackingAction {
private:

	unsigned int ntracks;
	unsigned int ntracks_tot;
	unsigned int ntracks_en;
	unsigned int ntracks_sec;
	unsigned int nevts;
	double avtracks;
	double avtracks_en;
	double avtracks_sec;
public:
 G4TrackCounter(std::string s):FADS::UserAction(s),FADS::TrackingAction(),ntracks(0),ntracks_tot(0),ntracks_en(0),
          ntracks_sec(0),nevts(0),avtracks(0),avtracks_en(0),avtracks_sec(0){};
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);
	void PreUserTrackingAction(const G4Track* aTrack);
	void PostUserTrackingAction(const G4Track* aTrack);
};


#endif
