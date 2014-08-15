/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include <iostream>
#include "G4UImanager.hh"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackRecorderSD.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "GaudiKernel/MsgStream.h"

static G4CosmicAndFilter ts1("G4CosmicAndFilter");

void G4CosmicAndFilter::BeginOfEventAction(const G4Event*)
{;}
void G4CosmicAndFilter::EndOfEventAction(const G4Event*)
{

  m_ntot++;

  const DataHandle <TrackRecordCollection> coll;

  StatusCode sc = m_storeGate->retrieve(coll,m_collectionName);

  if (sc.isFailure()) {
    std::cout<< " G4CosmicAndFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << m_collectionName << std::endl;
  }

  int counter = coll->size();
  //  std::cout<< " G4CosmicAndFilter: EndOfEventAction counter is " <<counter<< std::endl;

  if (counter==0){
    G4RunManager::GetRunManager()->AbortEvent();
  } else {

    const DataHandle <TrackRecordCollection> coll2;
    
    StatusCode sc = m_storeGate->retrieve(coll2,m_collectionName2);
    
    if (sc.isFailure()) {
      std::cout<< " G4CosmicAndFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << m_collectionName2 << std::endl;
    }

    counter = coll2->size();
    //  std::cout<< " G4CosmicAndFilter: EndOfEventAction counter is " <<counter<< std::endl;
    
    if (counter==0){
      //    std::cout<< " G4CosmicAndFilter: counter is 0 aborting event ..."<< std::endl;
      G4RunManager::GetRunManager()->AbortEvent();
    } else m_npass++;

  }

}
void G4CosmicAndFilter::BeginOfRunAction(const G4Run*)
{
  if(theProperties.find("VolumeName")==theProperties.end()){
    std::cout<<"G4CosmicAndFilter: no VolumeName specified, setting to default (=TRTBarrelEntryLayer)"<<std::endl;
    theProperties["VolumeName"]="TRTBarrelEntryLayer";
  };
  m_collectionName = theProperties["VolumeName"].c_str();

  if(theProperties.find("VolumeName2")==theProperties.end()){
    std::cout<<"G4CosmicAndFilter: no VolumeName2 specified, setting to default (=CaloEntryLayer)"<<std::endl;
    theProperties["VolumeName2"]="CaloEntryLayer";
  };
  m_collectionName2 = theProperties["VolumeName2"].c_str();

  std::cout<<"G4CosmicAndFilter: using collectionName "<<m_collectionName << " and " <<m_collectionName2 <<std::endl;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status =   svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure())
    {
      std::cout<< " G4CosmicAndFilter: BeginOfRunAction could not access StoreGateSvc!"<<std::endl;
    }
}
void G4CosmicAndFilter::EndOfRunAction(const G4Run*)
{
  std::cout<<"G4CosmicAndFilter: processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter "<<std::endl;
}

void G4CosmicAndFilter::SteppingAction(const G4Step*)
{;}

