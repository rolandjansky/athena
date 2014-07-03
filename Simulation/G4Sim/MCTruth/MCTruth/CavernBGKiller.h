/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CavernBGKiller_H
#define CavernBGKiller_H

#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

// Generic SD for recording track hits in any part of the detector
class CavernBGKiller : public FADS::FadsSensitiveDetector
{
public:
	CavernBGKiller(std::string name);
	~CavernBGKiller();
	/** member functions */
	void   Initialize(G4HCofThisEvent* );
	G4bool ProcessHits(G4Step* , G4TouchableHistory* );
	void   EndOfEvent(G4HCofThisEvent* ); 
private:

    TrackRecordCollection *trackRecordCollection;
    AthenaHitsCollectionHelper theHelper;

};

#endif
