/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilter.h"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"


G4CosmicOrFilter::G4CosmicOrFilter(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent),m_storeGate(0),
                                                                                                                m_ntot(0),m_npass(0),
                                                                                                                m_volumeName("TRTBarrelEntryLayer"),
                                                                                                                m_volumeName2("CaloEntryLayer"),
                                                                                                                m_volumeName3("TRTBarrelEntryLayer")
{

  declareProperty("VolumeName",m_volumeName);
  declareProperty("VolumeName2",m_volumeName2);
  declareProperty("VolumeName3",m_volumeName3);

}

void G4CosmicOrFilter::EndOfEvent(const G4Event*)
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
    ATH_MSG_INFO("aborting event due to failing OR filter");
    G4RunManager::GetRunManager()->AbortEvent();
  } else {

    m_npass++;
  }

}

StatusCode G4CosmicOrFilter::initialize(){

  m_collectionName=m_volumeName;
  m_collectionName2=m_volumeName2;
  m_collectionName3=m_volumeName3;

  ATH_MSG_INFO( "G4CosmicOrFilter: using collectionName(s) "<<m_collectionName << " and " <<m_collectionName2 << "and, with OR " << m_collectionName3 );


  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status =   svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()){
    ATH_MSG_WARNING( "Could not access StoreGateSvc!" );
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;

}

void G4CosmicOrFilter::EndOfRun(const G4Run*)
{
  ATH_MSG_INFO( "Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter " );
}



StatusCode G4CosmicOrFilter::queryInterface(const InterfaceID& riid, void** ppvInterface)
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
