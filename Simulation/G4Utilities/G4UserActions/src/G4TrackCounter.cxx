/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4TrackCounter.h"
#include "MCTruth/TrackHelper.h"
#include <iostream>

static G4TrackCounter ts1("G4TrackCounter");

void G4TrackCounter::BeginOfEventAction(const G4Event*)
{
	nevts++;
	ATH_MSG_DEBUG("increasing nevts to "<<nevts);
}

void G4TrackCounter::EndOfEventAction(const G4Event*)
{;}

void G4TrackCounter::BeginOfRunAction(const G4Run*)
{;}

void G4TrackCounter::EndOfRunAction(const G4Run*)
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

void G4TrackCounter::SteppingAction(const G4Step*)
{;}

void G4TrackCounter::PostUserTrackingAction(const G4Track*)
{;}

void G4TrackCounter::PreUserTrackingAction(const G4Track* aTrack)
{

  ntracks_tot++;

  TrackHelper theHelper(aTrack);

  if(theHelper.IsPrimary()||
     theHelper.IsRegeneratedPrimary()) ntracks++;

  if(theHelper.IsRegisteredSecondary()) ntracks_sec++;

  if(aTrack->GetKineticEnergy()>50) ntracks_en++;

}
