/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthSteppingAction_H
#define MCTruthSteppingAction_H

#include "G4UserSteppingAction.hh"
#include <map>
#include <string>

class SensitiveDetectorEntry;
class G4VSensitiveDetector;

typedef std::map<std::string, int, std::less<std::string> > volMap;
typedef std::map<std::string,G4VSensitiveDetector*, std::less<std::string> > sdMap;
typedef volMap::const_iterator volMapIterator;

class MCTruthSteppingAction : public G4UserSteppingAction
{
friend class TruthStrategyManager;
public:
	void UserSteppingAction(const G4Step* aStep);
private:
    	volMap vm;
	sdMap sdm;
	void AddRecordingEnvelope(std::string,int,std::string sdName=""); 
	void ListRecordingEnvelopes();
	MCTruthSteppingAction();
	MCTruthSteppingAction(const MCTruthSteppingAction&);

	SensitiveDetectorEntry *sde;
};

#endif
