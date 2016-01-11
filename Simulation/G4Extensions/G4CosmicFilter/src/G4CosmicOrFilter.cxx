/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilter.h"
#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "StoreGate/ReadHandle.h"

G4CosmicOrFilter::G4CosmicOrFilter(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent)
  , m_ntot(0)
  , m_npass(0)
  , m_collectionName("TRTBarrelEntryLayer")
  , m_collectionName2("CaloEntryLayer")
  , m_collectionName3("TRTBarrelEntryLayer")
{

  declareProperty("CollectionName", m_collectionName);
  declareProperty("CollectionName2",m_collectionName2);
  declareProperty("CollectionName3",m_collectionName3);

}

void G4CosmicOrFilter::EndOfEvent(const G4Event*)
{
  
  int counterOne(0), counterTwo(0), counterThree(0);
  //need way to get "and" or "or" in
  m_ntot++;
  
  SG::ReadHandle <TrackRecordCollection> coll(m_collectionName);
  if (! coll.isValid())
    {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
    }
  else
    {
      counterOne = coll->size();
    }
  
  SG::ReadHandle <TrackRecordCollection> coll2(m_collectionName2);
  if (!coll2.isValid())
    {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
    }
  else
    {
      counterTwo = coll2->size();
    }
  
  SG::ReadHandle <TrackRecordCollection> coll3(m_collectionName3);
  if (! coll3.isValid())
    {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection" );
    }
  else
    {
      counterThree = coll3->size();
    }
  
  if (counterOne==0 && counterTwo==0 && counterThree==0)
    {
      ATH_MSG_INFO("aborting event due to failing OR filter");
      G4RunManager::GetRunManager()->AbortEvent();
    }
  else
    {
      m_npass++;
    }
  return;
}

StatusCode G4CosmicOrFilter::initialize()
{
  
  ATH_MSG_INFO( "G4CosmicOrFilter: using collectionName(s) "<<m_collectionName << " and " <<m_collectionName2 << "and, with OR " << m_collectionName3 );
  
  return StatusCode::SUCCESS;
}

void G4CosmicOrFilter::EndOfRun(const G4Run*)
{
  ATH_MSG_INFO( "Processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter " );
}

StatusCode G4CosmicOrFilter::queryInterface(const InterfaceID& riid, void** ppvInterface)
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


#include "G4CosmicFilter/G4CosmicFilter.h"


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{
  
  
  G4CosmicOrFilter::G4CosmicOrFilter(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"G4CosmicOrFilter"),
    m_config(config),m_report(),
    m_evtStore("StoreGateSvc/StoreGateSvc","G4CosmicOrFilter"),
    m_detStore("StoreGateSvc/DetectorStore","G4CosmicOrFilter"){;
  }
  
  void G4CosmicOrFilter::endOfEvent(const G4Event*){;
    
    int counterOne(0), counterTwo(0), counterThree(0);
    //need way to get "and" or "or" in
    m_report.ntot++;
    
    SG::ReadHandle <TrackRecordCollection> coll(m_config.collectionName);
    if (! coll.isValid())
      {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
    }
  else
    {
      counterOne = coll->size();
    }
  
    SG::ReadHandle <TrackRecordCollection> coll2(m_config.collectionName2);
    if (!coll2.isValid())
      {
	ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " );
      }
    else
      {
	counterTwo = coll2->size();
      }
    
    SG::ReadHandle <TrackRecordCollection> coll3(m_config.collectionName3);
    if (! coll3.isValid())
      {
	ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection" );
      }
    else
      {
	counterThree = coll3->size();
      }
    
    if (counterOne==0 && counterTwo==0 && counterThree==0)
      {
	ATH_MSG_INFO("aborting event due to failing OR filter");
	G4RunManager::GetRunManager()->AbortEvent();
      }
    else
      {
	m_report.npass++;
    }
  return;
 
  }





} // namespace G4UA 
