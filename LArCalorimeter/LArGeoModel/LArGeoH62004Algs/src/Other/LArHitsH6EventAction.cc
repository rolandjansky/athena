/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoH62004Algs/LArHitsH6EventAction.h"
//#include "LArGeoH62004Algs/BuildH6HitCollections.h"
#include "LArG4H6SD/LArG4H6WarmTCHit.h"

//#include "LArSimEvent/LArHitContainer.h"
#include "LArG4TBSimEvent/LArG4H6WarmTCHitCollection.h"
#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

#include "G4Run.hh"
#include "G4Event.hh"

#include <iostream>
#include <vector>

#define DEBUG_HITS

static LArHitsH6EventAction H6EventAction("H6EventAction");

LArHitsH6EventAction::LArHitsH6EventAction(std::string s): FADS::UserAction(s)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  if (status.isFailure()) {
     std::cout << "LArTBH6EventAction::LArTBH6EventAction could not get the MessageSvc !"<<std::endl;
  }
  MsgStream msgStr = MsgStream(m_msgSvc, "LArTBH6EventAction");

  msgStr << MSG::DEBUG  <<"LArTBH6EventAction::LArTBH6EventAction constructor"<<endreq;

  status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()) {
     msgStr << MSG::ERROR << " could not fetch the StoraGateSvc !!!" << endreq;
  }

//  FadsEventAction::GetEventAction()->SetApplicationAction(this);
//  FadsRunAction::GetRunAction()->SetApplicationAction(this);

}


LArHitsH6EventAction::~LArHitsH6EventAction()
{
}

void LArHitsH6EventAction::BeginOfRunAction(const G4Run*)
{
  // We have to defer the creation of collectionBuilder until
  // well into the application initialization process.  The reason is
  // the "static" declaration above; if we put the following statement
  // in the constructor, then the object will try to create
  // collectionBuilder immediately when the library is loaded.  The
  // problem is that collectionBuilder depends on the detector
  // description, which isn't loaded into memory until later.  The
  // following code attempts to get around that issue.

}

void LArHitsH6EventAction::BeginOfEventAction(const G4Event*)
{

}

void LArHitsH6EventAction::EndOfEventAction(const G4Event* evt)
{
  MsgStream msgStr = MsgStream(m_msgSvc, "LArHitsH6EndOfEventAction");
  msgStr << MSG::DEBUG <<"LArHitsH6EventAction::EndOfEventAction"<<endreq;
  StatusCode status;


  G4HCofThisEvent *HCE = evt->GetHCofThisEvent();

  LArG4H6WarmTCHitCollection *wtcColl = new LArG4H6WarmTCHitCollection("H6WarmTCColl");
  LArG4H6FrontHitCollection *BeamColl = new  LArG4H6FrontHitCollection("H6BeamColl");

  G4VHitsCollection* hc;
  if ( HCE != 0) {
     int maxNumberHitCollections = HCE->GetNumberOfCollections();
#ifdef DEBUG_HITS
	     msgStr<< MSG::DEBUG  <<" Number of collections: "<<maxNumberHitCollections<<endreq;
#endif
     for ( int h = 0; h != maxNumberHitCollections; h++ ) {
          hc = HCE->GetHC(h);
//          LArG4H6LeakHitsCollection * collection = 0;
          LArG4H6FrontHitsCollection * collection_f = 0;
          LArG4H6WarmTCHitsCollection * collection_w = 0;
	  if ( hc != 0 ) {
#ifdef DEBUG_HITS
	     msgStr<< MSG::DEBUG  <<" Have collection: "<<hc->GetName()<<endreq;
#endif
//             collection = dynamic_cast<LArG4H6LeakHitsCollection*>( hc );
             collection_f = dynamic_cast<LArG4H6FrontHitsCollection*>( hc );
             collection_w = dynamic_cast<LArG4H6WarmTCHitsCollection*>( hc );
	     if (collection_w != 0 ) {
                int numberHits = collection_w->entries();
#ifdef DEBUG_HITS
                msgStr<< MSG::DEBUG  <<" --- WarmTC hits: "<<numberHits<<endreq;
#endif
                for (int i = 0; i < numberHits; i++) {
                   LArG4H6WarmTCHit* hit = new LArG4H6WarmTCHit(*(*collection_w)[i]);
                   wtcColl->push_back(hit);
                }
	     }
	     if (collection_f != 0 ) {
                int numberHits = collection_f->entries();
#ifdef DEBUG_HITS
                msgStr<< MSG::DEBUG  <<" --- Beam hits: "<<numberHits<<endreq;
#endif
                for (int i = 0; i < numberHits; i++) {
                   LArG4H6FrontHit* hit = new LArG4H6FrontHit(*(*collection_f)[i]);
                   BeamColl->push_back(hit);
                }
	     }
           } else {
#ifdef DEBUG_HITS
	     msgStr<< MSG::DEBUG  <<" Do not have collection num: "<<h <<endreq;
	     msgStr<< MSG::DEBUG  <<" WarmTCSciXCollection has id: "<<G4SDManager::GetSDMpointer()->GetCollectionID("WarmTCSciXCollection")<<endreq;
#endif
           }
    }
  } else {
#ifdef DEBUG_HITS
	     msgStr<< MSG::DEBUG  <<" Do not have collection of this event !!" <<endreq;
#endif
  }

  status = m_storeGate->record(wtcColl,"LArG4H6WarmTCHitCollection",false);
  if (status.isFailure()) {
         msgStr << MSG::ERROR << "Failed to record LArG4H6WarmTCHitCollection  in StoreGate!" << endreq;
  }
  status = m_storeGate->setConst(wtcColl);
  if (status.isFailure()) {
         msgStr  << MSG::ERROR << "Failed to lock LArG4H6WarmTCHitCollection  in StoreGate!" << endreq;
  }

  status = m_storeGate->record(BeamColl,"LArG4H6FrontHitCollection",false);
  if (status.isFailure()) {
         msgStr << MSG::ERROR << "Failed to record LArG4H6FrontHitCollection  in StoreGate!" << endreq;
  }
  status = m_storeGate->setConst(BeamColl);
  if (status.isFailure()) {
         msgStr  << MSG::ERROR << "Failed to lock LArG4H6BeamHitCollection  in StoreGate!" << endreq;
  }

  // Note that if this is not a calibration run, the size of calibList
  // will be zero.
#ifdef DEBUG_HITS
#endif
}
