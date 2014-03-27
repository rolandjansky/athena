/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFP_SensitiveDetector_h
#define AFP_SensitiveDetector_h

#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
//#include "AFP_GeoModel/AFP_GeoModelTool.h"


#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "globals.hh"

//#include "StoreGate/StoreGateSvc.h"

// class StoreGateSvc;

#define TDMAXQEFF 0.15
#define TDMAXCNT  4000
#define SIDMAXCNT 1000

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

#include <string> 

class AFP_SensitiveDetector : public FADS::FadsSensitiveDetector {

public:
	AFP_SensitiveDetector(std::string);
	~AFP_SensitiveDetector();

	void Initialize (G4HCofThisEvent*);
	G4bool ProcessHits(G4Step*, G4TouchableHistory*);
	void EndOfEvent (G4HCofThisEvent*);

private:
	int m_nHitID;
	int m_nEventNumber;
	int m_nNumberOfTDSimHits;
	int m_nNumberOfSIDSimHits;
	
	int m_nNOfTDSimHits[4][32];
	int m_nNOfSIDSimHits[4];
	
	float delta_pixel_x, delta_pixel_y;
	float death_edge[4][10];
	float lower_edge[4][10];

	AFP_TDSimHitCollection* m_pTDSimHitCollection;
	AFP_SIDSimHitCollection* m_pSIDSimHitCollection;
	AthenaHitsCollectionHelper m_HitCollHelp;

	//int m_nPos1, m_nPos2;
	//int m_nNum[3];
	
//	AFP_GeoModelTool* m_pDetectorTool;
	
//	StoreGateSvc* m_pStoreGate;
// 	StoreGateSvc* m_pDetStore;

};

#endif //AFP_SensitiveDetector_h
