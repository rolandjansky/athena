/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackRecorderSD.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"

static G4CosmicAndFilter ts1("G4CosmicAndFilter");

void G4CosmicAndFilter::BeginOfEventAction(const G4Event*)
{;}
void G4CosmicAndFilter::EndOfEventAction(const G4Event*)
{

  m_ntot++;

  const DataHandle <TrackRecordCollection> coll;

  StatusCode sc = m_storeGate->retrieve(coll,m_collectionName);

  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_collectionName );
  }

  int counter = coll->size();

  if (counter==0){
    G4RunManager::GetRunManager()->AbortEvent();
  } else {

    const DataHandle <TrackRecordCollection> coll2;
    
    StatusCode sc = m_storeGate->retrieve(coll2,m_collectionName2);
    
    if (sc.isFailure()) {
      ATH_MSG_INFO( "Cannot retrieve TrackRecordCollection " << m_collectionName2 );
    }

    counter = coll2->size();
    
    if (counter==0){
      G4RunManager::GetRunManager()->AbortEvent();
    } else m_npass++;

  }

}
void G4CosmicAndFilter::BeginOfRunAction(const G4Run*)
{
  if(theProperties.find("VolumeName")==theProperties.end()){
    ATH_MSG_INFO( "no VolumeName specified, setting to default (=TRTBarrelEntryLayer)" );
    theProperties["VolumeName"]="TRTBarrelEntryLayer";
  };
  m_collectionName = theProperties["VolumeName"].c_str();

  if(theProperties.find("VolumeName2")==theProperties.end()){
    ATH_MSG_INFO( "no VolumeName2 specified, setting to default (=CaloEntryLayer)" );
    theProperties["VolumeName2"]="CaloEntryLayer";
  };
  m_collectionName2 = theProperties["VolumeName2"].c_str();

  ATH_MSG_INFO( "using collectionName "<<m_collectionName << " and " <<m_collectionName2 );

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status =   svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()) {
    ATH_MSG_WARNING( "Could not access StoreGateSvc!" );
  }
}
void G4CosmicAndFilter::EndOfRunAction(const G4Run*)
{
  ATH_MSG_INFO( "processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter " );
}

void G4CosmicAndFilter::SteppingAction(const G4Step*)
{;}

