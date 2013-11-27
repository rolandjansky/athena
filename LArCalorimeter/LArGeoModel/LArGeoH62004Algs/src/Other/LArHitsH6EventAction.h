/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHitEventAction_H
#define LArHitEventAction_H

#include "FadsActions/UserAction.h"

class G4Event;
class G4Run;
class StoredLArHitContainers;

#include "SimHelpers/AthenaHitsCollectionHelper.h"
//#include "LArGeoH62004Algs/BuildH6HitCollections.h"

class LArHitsH6EventAction: public FADS::UserAction {
public:
	LArHitsH6EventAction(std::string s);
	~LArHitsH6EventAction();
	void BeginOfRunAction(const G4Run*);
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);
private:
	AthenaHitsCollectionHelper helper;

	StoreGateSvc*   m_storeGate;
        IMessageSvc*  m_msgSvc;
};

#endif
