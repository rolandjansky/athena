/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"



G4CosmicAndFilter::G4CosmicAndFilter(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent)
  , ntracks(0)
  , m_ntot(0)
  , m_npass(0)
  , m_volumeName("TRTBarrelEntryLayer")
  , m_volumeName2("CaloEntryLayer")
{
  declareProperty("VolumeName",m_volumeName);
  declareProperty("VolumeName2",m_volumeName2);
}

void G4CosmicAndFilter::EndOfEvent(const G4Event*)
{

  m_ntot++;
  int counter(0);
  const DataHandle <TrackRecordCollection> coll;
  if (evtStore()->retrieve(coll,m_collectionName).isFailure())
    {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_collectionName);
    }
  else
    {
      counter = coll->size();
    }

  if (counter==0)
    {
      ATH_MSG_INFO("aborting event due to failing AND filter");
      G4RunManager::GetRunManager()->AbortEvent();
      return;
    }

  const DataHandle <TrackRecordCollection> coll2;
  if (evtStore()->retrieve(coll2,m_collectionName2).isFailure())
    {
      ATH_MSG_INFO( "Cannot retrieve TrackRecordCollection " << m_collectionName2 );
    }
  else
    {
      counter = coll2->size();
    }

  if (counter==0)
    {
      ATH_MSG_INFO("aborting event due to failing AND filter");
      G4RunManager::GetRunManager()->AbortEvent();
      return;
    }

    m_npass++;
    return;
}

StatusCode G4CosmicAndFilter::initialize()
{
  m_collectionName=m_volumeName;
  m_collectionName2=m_volumeName2;

  ATH_MSG_INFO( "using collectionName "<<m_collectionName << " and " <<m_collectionName2 );

  return StatusCode::SUCCESS;
}

void G4CosmicAndFilter::EndOfRun(const G4Run*)
{
  ATH_MSG_INFO( "processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter " );
}


StatusCode G4CosmicAndFilter::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) )
    {
      *ppvInterface = dynamic_cast<IUserAction*>(this);
      addRef();
    }
  else
    {
      // Interface is not directly available : try out a base class
      return UserActionBase::queryInterface(riid, ppvInterface);
    }
  return StatusCode::SUCCESS;
}
