/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilter.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackRecorderSD.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"

static G4CosmicOrFilter ts1("G4CosmicOrFilter");

void G4CosmicOrFilter::BeginOfEventAction(const G4Event*)
{;}
void G4CosmicOrFilter::EndOfEventAction(const G4Event*)
{
  int counterOne(0), counterTwo(0), counterThree(0);
  //need way to get "and" or "or" in
  m_ntot++;

  const DataHandle <TrackRecordCollection> coll;
  StatusCode sc = m_storeGate->retrieve(coll,m_collectionName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
  } else {
    counterOne = coll->size();
  }

  const DataHandle <TrackRecordCollection> coll2;
  sc = m_storeGate->retrieve(coll2,m_collectionName2);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
  } else {
    counterTwo = coll2->size();
  }

  const DataHandle <TrackRecordCollection> coll3;
  sc = m_storeGate->retrieve(coll3,m_collectionName3);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection" );
  } else {
    counterThree = coll3->size();
  }

  if (counterOne==0 && counterTwo==0  && counterThree==0){
    ATH_MSG_DEBUG( "counter is 0 aborting event ..." );
    G4RunManager::GetRunManager()->AbortEvent();
  } else {
    ATH_MSG_VERBOSE( "counter is> 0 not aborting event ... passed events "<<m_npass );
    m_npass++;
  }

}
void G4CosmicOrFilter::BeginOfRunAction(const G4Run*)
{

  if(theProperties.find("VolumeName")==theProperties.end()){
    ATH_MSG_INFO( "no VolumeName specified, setting to default (=TRTBarrelEntryLayer)" );
    theProperties["VolumeName"]="TRTBarrelEntryLayer";
  }
  m_collectionName = theProperties["VolumeName"].c_str();

  if(theProperties.find("VolumeName2")==theProperties.end()){
    ATH_MSG_INFO( "no VolumeName2 specified, setting to default (=CaloEntryLayer)" );
    theProperties["VolumeName2"]="CaloEntryLayer";
  }
  m_collectionName2 = theProperties["VolumeName2"].c_str();

  m_collectionName3 = theProperties["VolumeName3"].c_str();

  ATH_MSG_INFO( "G4CosmicOrFilter: using collectionName(s) "<<m_collectionName << " and " <<m_collectionName2 << "and, with OR " << m_collectionName3 );

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status =   svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()){
    ATH_MSG_WARNING( "Could not access StoreGateSvc!" );
  }

}
void G4CosmicOrFilter::EndOfRunAction(const G4Run*)
{
  ATH_MSG_INFO( "Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter " );
}

void G4CosmicOrFilter::SteppingAction(const G4Step*)
{;}

