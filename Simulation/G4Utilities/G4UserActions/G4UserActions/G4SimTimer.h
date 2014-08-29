/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SimTimer_H
#define G4SimTimer_H

#include "FadsActions/UserAction.h"
#include <string>

#include "G4Timer.hh"

class G4SimTimer: public FADS::UserAction {
private:
	double runTime;
	double eventTime;
	double accumulatedEventTime;
	double accumulatedEventTimeSquared;
	unsigned int nrOfEntries;
	G4Timer* runTimer;
	G4Timer* eventTimer;
	double averageTimePerEvent();
	double Sigma();
public:
	G4SimTimer(std::string s):FADS::UserAction(s),runTime(0),eventTime(0),
				  accumulatedEventTime(0),accumulatedEventTimeSquared(0),
	                          nrOfEntries(0)
	{
		runTimer=new G4Timer;
		eventTimer=new G4Timer;
	}
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

};


#endif
