/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicFilter.h"
#include <stdexcept>
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackRecorderSD.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"

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
    ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_collectionName );
    counter = 0;
    G4RunManager::GetRunManager()->AbortEvent();
    return;
  }
  counter = coll->size();

  if (m_magicID!=0 || m_ptMin>0 || m_ptMax>0){
    counter=0;
    for (const auto& a_tr : *coll){
      if (m_magicID!=0 && m_magicID != fabs(a_tr.GetPDGCode())) continue;
      if (m_ptMin>0 && m_ptMin > a_tr.GetMomentum().perp() ) continue;
      if (m_ptMax>0 && m_ptMax < a_tr.GetMomentum().perp() ) continue;
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
    ATH_MSG_INFO( "no VolumeName specified, setting to default (=CaloEntryLayer)" );
    theProperties["VolumeName"]="CaloEntryLayer";
  }
  m_collectionName = theProperties["VolumeName"].c_str();
  ATH_MSG_INFO( "using collectionName "<<m_collectionName );

  if(theProperties.find("PDG_ID")==theProperties.end()){
    ATH_MSG_INFO( "G4CosmicFilter: no PDG ID specified, setting to default (=none)" );
    theProperties["PDG_ID"]="0";
  }
  char *endptr; 
  m_magicID = strtol(theProperties["PDG_ID"].c_str(), &endptr, 0);
  if (endptr[0] != '\0') {  
    throw std::invalid_argument("Could not convert string to int: " + theProperties["PDG_ID"]);
  }
  ATH_MSG_INFO( "using PDG ID "<<m_magicID );

  if(theProperties.find("pTmin")==theProperties.end()){
    ATH_MSG_INFO( "no pTmin specified, setting to default (=-1)" );
    theProperties["pTmin"]="-1";
  }
  m_ptMin = strtol(theProperties["pTmin"].c_str(), &endptr, 0);
  if (endptr[0] != '\0') {  
    throw std::invalid_argument("Could not convert string to int: " + theProperties["pTmin"]);
  }

  ATH_MSG_INFO( "using pTmin "<<m_ptMin );

  if(theProperties.find("pTmax")==theProperties.end()){
    ATH_MSG_INFO( "G4CosmicFilter: no pTmax specified, setting to default (=-1)" );
    theProperties["pTmax"]="-1";
  }
  m_ptMax = strtol(theProperties["pTmax"].c_str(), &endptr, 0);
  if (endptr[0] != '\0') {  
    throw std::invalid_argument("Could not convert string to int: " + theProperties["pTmax"]);
  }
  ATH_MSG_INFO( "using pTmax "<<m_ptMax );

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()){
      ATH_MSG_WARNING( "Could not access StoreGateSvc!" );
  }

  m_init=true;
}
void G4CosmicFilter::EndOfRunAction(const G4Run*)
{
  ATH_MSG_INFO( "processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter" );
}

void G4CosmicFilter::SteppingAction(const G4Step*){;}

