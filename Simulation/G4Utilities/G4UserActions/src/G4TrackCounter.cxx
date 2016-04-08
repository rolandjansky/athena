/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4TrackCounter.h"
#include "MCTruth/TrackHelper.h"
#include <iostream>

//static G4TrackCounter ts1("G4TrackCounter");

void G4TrackCounter::BeginOfEvent(const G4Event*)
{
	nevts++;
	ATH_MSG_DEBUG("increasing nevts to "<<nevts);
}


void G4TrackCounter::EndOfRun(const G4Run*)
{
  if (nevts>0){
    avtracks=ntracks/(double)nevts;
    avtracks_en=ntracks_en/(double)nevts;
    avtracks_sec=ntracks_sec/(double)nevts;
  } else {
    avtracks=0;
    avtracks_en=0;
    avtracks_sec=0;
  }

  ATH_MSG_INFO("*****************************************************");
  ATH_MSG_INFO("number of G4Events has been "<<nevts<<", total ntracks: "<<ntracks_tot);
  ATH_MSG_INFO("average number of primary tracks per event has been "<<avtracks);
  ATH_MSG_INFO("average number of registered secondary tracks per event has been "<<avtracks_sec);
  ATH_MSG_INFO("average number of tracks with E>50MeV has been "<<avtracks_en);
  ATH_MSG_INFO("*****************************************************");
}


void G4TrackCounter::PreTracking(const G4Track* aTrack)
{

  ntracks_tot++;

  TrackHelper theHelper(aTrack);

  if(theHelper.IsPrimary()||
     theHelper.IsRegeneratedPrimary()) ntracks++;

  if(theHelper.IsRegisteredSecondary()) ntracks_sec++;

  if(aTrack->GetKineticEnergy()>50) ntracks_en++;

}

StatusCode G4TrackCounter::initialize()
{
	return StatusCode::SUCCESS;
}


StatusCode G4TrackCounter::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
