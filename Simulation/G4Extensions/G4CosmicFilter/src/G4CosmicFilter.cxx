/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicFilter.h"
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

static G4CosmicFilter ts1("G4CosmicFilter");

void G4CosmicFilter::BeginOfEventAction(const G4Event*)
{
  if (!m_init) ParseProperties();
}
void G4CosmicFilter::EndOfEventAction(const G4Event*)
{
  int counter(0);

  m_ntot++;

  const DataHandle <TrackRecordCollection> coll;

  StatusCode sc = m_storeGate->retrieve(coll,m_collectionName);

  if (sc.isFailure() || !coll) {
    std::cout<< " G4CosmicFilter: EndOfEventAction Cannot retrieve TrackRecordCollection " << m_collectionName << std::endl;
    counter = 0;
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  }
  counter = coll->size();

  if (m_magicID!=0 || m_ptMin>0 || m_ptMax>0){
    counter=0;
    for (unsigned int i=0;i<coll->size();++i){
      if (m_magicID!=0 && m_magicID != fabs(coll->operator[](i)->GetPDGCode())) continue;
      if (m_ptMin>0 && m_ptMin > coll->operator[](i)->GetMomentum().perp() ) continue;
      if (m_ptMax>0 && m_ptMax < coll->operator[](i)->GetMomentum().perp() ) continue;
      counter++;
    }
  }

  //std::cout << "EndOfEventAction counter is "<<counter<<std::endl;
  if (counter==0){
    G4RunManager::GetRunManager()->AbortEvent();
  } else {m_npass++;}

}
void G4CosmicFilter::BeginOfRunAction(const G4Run*)
{
  if (!m_init) ParseProperties();
}

void G4CosmicFilter::ParseProperties(){
  if(theProperties.find("VolumeName")==theProperties.end()){
    std::cout<<"G4CosmicFilter: no VolumeName specified, setting to default (=CaloEntryLayer)"<<std::endl;
    theProperties["VolumeName"]="CaloEntryLayer";
  }
  m_collectionName = theProperties["VolumeName"].c_str();
  std::cout<<"G4CosmicFilter: using collectionName "<<m_collectionName <<std::endl;

  if(theProperties.find("PDG_ID")==theProperties.end()){
    std::cout<<"G4CosmicFilter: no PDG ID specified, setting to default (=none)"<<std::endl;
    theProperties["PDG_ID"]="0";
  }
  m_magicID = atoi(theProperties["PDG_ID"].c_str());
  std::cout<<"G4CosmicFilter: using PDG ID "<<m_magicID<<std::endl;

  if(theProperties.find("pTmin")==theProperties.end()){
    std::cout<<"G4CosmicFilter: no pTmin specified, setting to default (=-1)"<<std::endl;
    theProperties["pTmin"]="-1";
  }
  m_ptMin = atoi(theProperties["pTmin"].c_str());
  std::cout<<"G4CosmicFilter: using pTmin "<<m_ptMin<<std::endl;

  if(theProperties.find("pTmax")==theProperties.end()){
    std::cout<<"G4CosmicFilter: no pTmax specified, setting to default (=-1)"<<std::endl;
    theProperties["pTmax"]="-1";
  }
  m_ptMax = atoi(theProperties["pTmax"].c_str());
  std::cout<<"G4CosmicFilter: using pTmax "<<m_ptMax<<std::endl;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()){
      std::cout<< " G4CosmicFilter: BeginOfRunAction could not access StoreGateSvc!"<<std::endl;
  }

  m_init=true;
}
void G4CosmicFilter::EndOfRunAction(const G4Run*)
{
  std::cout<<"G4CosmicFilter: processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter "<<std::endl;
}

void G4CosmicFilter::SteppingAction(const G4Step*){;}

