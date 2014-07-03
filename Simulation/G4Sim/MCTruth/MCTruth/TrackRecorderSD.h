/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackRecorderSD_H
#define TrackRecorderSD_H

#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

// Generic SD for recording track hits in any part of the detector
class TrackRecorderSD : public FADS::FadsSensitiveDetector
{
public:
	TrackRecorderSD(std::string name);
	~TrackRecorderSD();
	/** member functions */
	void   Initialize(G4HCofThisEvent* );
	G4bool ProcessHits(G4Step* , G4TouchableHistory* );
	void   EndOfEvent(G4HCofThisEvent* ); 
	TrackRecordCollection::const_iterator HitsBegin(){return trackRecordCollection->begin();};
	TrackRecordCollection::const_iterator HitsEnd(){return trackRecordCollection->end();};

private:
	TrackRecordCollection *trackRecordCollection;
	AthenaHitsCollectionHelper theHelper;
};

#endif
