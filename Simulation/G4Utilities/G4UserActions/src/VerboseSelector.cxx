/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/VerboseSelector.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TruthStrategyManager.h"
#include <iostream>

static VerboseSelector vs("VerboseSelector");

void VerboseSelector::BeginOfEventAction(const G4Event*)
{
}

void VerboseSelector::EndOfEventAction(const G4Event*)
{
  evtCount++;
}

void VerboseSelector::BeginOfRunAction(const G4Run*)
{
  ParseProperties();  
}


void VerboseSelector::ParseProperties(){

  // here we read the property map and set the needed variables
  // first fill missing properties and issue warnings

  if(theProperties.find("targetEvent")==theProperties.end()){
    log()<<MSG::DEBUG<<"VerboseSelector: no targetEvent specified, setting to default (=all)"<<endreq;
    theProperties["targetEvent"]="*";
  };

  if(theProperties.find("targetTrack")==theProperties.end()){
    log()<<MSG::DEBUG<<"VerboseSelector: no targetTrack specified, setting to default (=all)"<<endreq;
    theProperties["targetTrack"]="*";
  };
  
  if(theProperties.find("targetBarcode")==theProperties.end()){
    log()<<MSG::DEBUG<<"VerboseSelector: no targetBarcode specified, setting to default (=all)"<<endreq;
    theProperties["targetBarcode"]="*";
  };

  if(theProperties.find("verboseLevel")==theProperties.end()){
    log()<<MSG::DEBUG<<"VerboseSelector: no verboseLevel specified, setting to default (=0)"<<endreq;
    theProperties["verboseLevel"]="0";
  };

  // now set members
  verboseLevel  = strtol(theProperties["verboseLevel"].c_str(),0,0);
  targetEvent   = theProperties["targetEvent"]=="*"?-1:strtol(theProperties["targetEvent"].c_str(),0,0);
  targetTrack   = theProperties["targetTrack"]=="*"?-1:strtol(theProperties["targetTrack"].c_str(),0,0);
  targetBarcode = theProperties["targetBarcode"]=="*"?-1:strtol(theProperties["targetBarcode"].c_str(),0,0);

  log()<<MSG::INFO<<"VerboseSelector: properties are targetEvent="<<targetEvent
	   <<" targetTrack="<<targetTrack
	   <<" targetBarcode="<<targetBarcode
	   <<" verboseLevel="<<verboseLevel
	   <<endreq;

}

void VerboseSelector::EndOfRunAction(const G4Run*)
{
}

void VerboseSelector::SteppingAction(const G4Step*)
{
}

void VerboseSelector::PreUserTrackingAction(const G4Track* aTrack){

	// std::cout<<" this is VerboseSelector::PreUserTrackingAction "<<std::endl;
  if(evtCount==targetEvent||targetEvent<0)
  {
    int trackID = aTrack->GetTrackID();
	G4Track *itr=const_cast<G4Track*>(aTrack);
	TrackHelper trackHelper(itr);
	EventInformation* eventInfo=TruthStrategyManager::GetStrategyManager()->
						GetEventInformation();
						
	//int primaryBarcode(0);
	int currentBarcode(0);
	
	if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
	{
	  //primaryBarcode=eventInfo->GetCurrentPrimary()->barcode();
	  currentBarcode=eventInfo->GetCurrentlyTraced()->barcode();
	}
	bool p1=targetTrack<0 && targetBarcode<0;
	bool p2=trackID==targetTrack ;
	bool p3=currentBarcode==targetBarcode;

    if(p1 || p2 || p3)
	{
	  log()<<MSG::WARNING<<std::endl<<"---------> Dumping now track #"<<trackID<<" barcode "
	           <<currentBarcode<<" in event "<<evtCount<<std::endl<<endreq;
      G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(verboseLevel);
	}
  }

}

void VerboseSelector::PostUserTrackingAction(const G4Track* aTrack)
{
  if(evtCount==targetEvent||targetEvent<0){
    if(aTrack->GetTrackID()==targetTrack||targetTrack<0)
      G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(0);
  }
}

