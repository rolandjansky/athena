/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicAndFilter.h"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"


#include "StoreGate/ReadHandle.h"

G4CosmicAndFilter::G4CosmicAndFilter(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent)
  , ntracks(0)
  , m_ntot(0)
  , m_npass(0)
  , m_collectionName("TRTBarrelEntryLayer")
  , m_collectionName2("CaloEntryLayer")
{
  declareProperty("CollectionName",m_collectionName);
  declareProperty("CollectionName2",m_collectionName2);
}

void G4CosmicAndFilter::EndOfEvent(const G4Event*)
{

  m_ntot++;
  int counter(0);
  SG::ReadHandle <TrackRecordCollection> coll(m_collectionName);
  if (! coll.isValid())
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

  SG::ReadHandle <TrackRecordCollection> coll2(m_collectionName2);
  if (! coll2.isValid())
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




#include "G4CosmicFilter/G4CosmicAndFilter.h"


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{


  G4CosmicAndFilter::G4CosmicAndFilter(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"G4CosmicAndFilter"),
    m_config(config),m_report(),
    m_evtStore("StoreGateSvc/StoreGateSvc","G4CosmicAndFilter"),
    m_detStore("StoreGateSvc/DetectorStore","G4CosmicAndFilter"){;
  }

  void G4CosmicAndFilter::endOfEvent(const G4Event*){;

    m_report.ntot++;
    int counter(0);
    SG::ReadHandle <TrackRecordCollection> coll(m_config.collectionName);
    if (! coll.isValid())
      {
	ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_config.collectionName);
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
    
    SG::ReadHandle <TrackRecordCollection> coll2(m_config.collectionName2);
    if (! coll2.isValid())
    {
      ATH_MSG_INFO( "Cannot retrieve TrackRecordCollection " << m_config.collectionName2 );
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
    
    m_report.npass++;
    return;

  }


} // namespace G4UA 
