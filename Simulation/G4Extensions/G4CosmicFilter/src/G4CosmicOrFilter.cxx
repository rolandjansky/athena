/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilter.h"
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

static G4CosmicOrFilter ts1("G4CosmicOrFilter");

void G4CosmicOrFilter::BeginOfEventAction(const G4Event*)
{
  //  MsgStream log(msgSvc(), "G4CosmicOrFilter");
  //  log << MSG::DEBUG << "BeginOfEventAction " << endreq;

}
void G4CosmicOrFilter::EndOfEventAction(const G4Event*)
{
  //  MsgStream log(msgSvc(), "G4CosmicOrFilter");
  //  log << MSG::INFO << "EndOfEventAction " << endreq;
  //std::cout<< " G4CosmicOrFilter: in EndOfEventAction " << std::endl;

  int counterOne(0), counterTwo(0), counterThree(0);
  //need way to get "and" or "or" in
  m_ntot++;

  const DataHandle <TrackRecordCollection> coll;

  StatusCode sc = m_storeGate->retrieve(coll,m_collectionName);

  if (sc.isFailure()) {
    //log << MSG::DEBUG << " EndOfEventAction Cannot retrieve TrackRecordCollection " << endreq;
    //    std::cout<< " G4CosmicOrFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << std::endl;
  }
  //  std::cout<< " G4CosmicOrFilter: EndOfEventAction retrieved TrackRecordCollection ok " << std::endl;

  counterOne = coll->size();
 
  const DataHandle <TrackRecordCollection> coll2;
  
  StatusCode sc2 = m_storeGate->retrieve(coll2,m_collectionName2);
  
  if (sc2.isFailure()) {
    //    log << MSG::WARNING << " EndOfEventAction Cannot retrieve TrackRecordCollection " << endreq;
    //    std::cout<< " G4CosmicOrFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << std::endl;
  }
  //  std::cout<< " G4CosmicOrFilter: EndOfEventAction retrieved TrackRecordCollection ok " << std::endl;
  
  counterTwo = coll2->size();
  
  
  const DataHandle <TrackRecordCollection> coll3;
  
  StatusCode sc3 = m_storeGate->retrieve(coll3,m_collectionName3);
  
  if (sc3.isFailure()) {
    //    log << MSG::WARNING << " EndOfEventAction Cannot retrieve TrackRecordCollection " << endreq;
    //    std::cout<< " G4CosmicOrFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << std::endl;
  }
  //  std::cout<< " G4CosmicOrFilter: EndOfEventAction retrieved TrackRecordCollection ok " << std::endl;
  
  counterThree = coll3->size();
  
  //std::cout << " G4CosmicOrFilter: EndOfEventAction " << counterOne << ", " << counterTwo << ", " << counterThree << std::endl;


  if (counterOne==0 && counterTwo==0  && counterThree==0){
    //    std::cout<< " G4CosmicOrFilter: counter is 0 aborting event ..."<< std::endl;
    
    G4RunManager::GetRunManager()->AbortEvent();
  } else {

    //    std::cout<< " G4CosmicOrFilter: counter is> 0 not aborting event ... passed events "<<m_npass<< std::endl;
    
    m_npass++;}

}
void G4CosmicOrFilter::BeginOfRunAction(const G4Run*)
{
  //  MsgStream log(msgSvc(), "G4CosmicOrFilter");
  //  log << MSG::DEBUG << "BeginOfRunAction " << endreq;

  //std::cout<< " G4CosmicOrFilter: in BeginOfRunAction " << std::endl;

  if(theProperties.find("VolumeName")==theProperties.end()){
    std::cout<<"G4CosmicOrFilter: no VolumeName specified, setting to default (=TRTBarrelEntryLayer)"<<std::endl;
    theProperties["VolumeName"]="TRTBarrelEntryLayer";
  };
  m_collectionName = theProperties["VolumeName"].c_str();

  if(theProperties.find("VolumeName2")==theProperties.end()){
    std::cout<<"G4CosmicOrFilter: no VolumeName2 specified, setting to default (=CaloEntryLayer)"<<std::endl;
    theProperties["VolumeName2"]="CaloEntryLayer";
  };
  m_collectionName2 = theProperties["VolumeName2"].c_str();

  m_collectionName3 = theProperties["VolumeName3"].c_str();

  std::cout<<"G4CosmicOrFilter: using collectionName(s) "<<m_collectionName << " and " <<m_collectionName2 << "and, with OR " << m_collectionName3 << std::endl;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  
  StatusCode status =   svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure())
    {
      //      std::cout<< " G4CosmicOrFilter: BeginOfRunAction could not access StoreGateSvc!"<<std::endl;
      //      log << MSG::WARNING <<"G4CosmicOrFilter::BeginOfRunAction could not access StoreGateSvc!"<<endreq;
    }


}
void G4CosmicOrFilter::EndOfRunAction(const G4Run*)
{
  /*
  MsgStream log(msgSvc(), "G4CosmicOrFilter");
  log << MSG::DEBUG << "BeginOfRunAction " << endreq;
  log << MSG::INFO << " processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter "<<endreq;
  */
  std::cout<<"G4CosmicOrFilter: processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter "<<std::endl;

}

void G4CosmicOrFilter::SteppingAction(const G4Step*)
{

}

