/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CosmicTRSD_H
#define CosmicTRSD_H

#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

// Generic SD for recording track hits in any part of the detector
class CosmicTRSD : public FADS::FadsSensitiveDetector
{
public:
	CosmicTRSD(std::string name);
	~CosmicTRSD();
	/** member functions */
	void   Initialize(G4HCofThisEvent* );
	G4bool ProcessHits(G4Step* , G4TouchableHistory* );
	void   EndOfEvent(G4HCofThisEvent* ); 
private:
	TrackRecordCollection *trackRecordCollection;
	AthenaHitsCollectionHelper theHelper;
};

#endif
