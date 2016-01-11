/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicFilter.h"

#include "MCTruth/TrackHelper.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "StoreGate/ReadHandle.h"

G4CosmicFilter::G4CosmicFilter(const std::string& type, const std::string& name, const IInterface* parent)
  : UserActionBase(type,name,parent)
  , m_ntot(0)
  , m_npass(0)
  , m_PDGId(0)
  , m_collectionName("CaloEntryLayer")
  , m_ptMin(-1)
  , m_ptMax(-1)
{
  declareProperty("CollectionName",m_collectionName);
  declareProperty("PDGId",m_PDGId);
  declareProperty("PtMin",m_ptMin);
  declareProperty("PtMax",m_ptMax);
}


void G4CosmicFilter::EndOfEvent(const G4Event*)
{
  int counter(0);

  m_ntot++;

  SG::ReadHandle <TrackRecordCollection> coll(m_collectionName);
  if (! coll.isValid())
    {
      ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_collectionName );
      G4RunManager::GetRunManager()->AbortEvent();
      return;
    }

  counter = coll->size();
  
  if (m_PDGId!=0 || m_ptMin>0 || m_ptMax>0)
    {
      counter=0;
      for (const auto& a_tr : *coll)
        {
          if (m_PDGId!=0 && m_PDGId != fabs(a_tr.GetPDGCode())) continue;
          if (m_ptMin>0 && m_ptMin > a_tr.GetMomentum().perp() ) continue;
          if (m_ptMax>0 && m_ptMax < a_tr.GetMomentum().perp() ) continue;
          counter++;
        }
    }
  
  //std::cout << "EndOfEventAction counter is "<<counter<<std::endl;
  if (counter==0)
    {
      ATH_MSG_INFO("aborting event due to failing filter");
      G4RunManager::GetRunManager()->AbortEvent();
      return;
    }
  
  m_npass++;
  return;
}

StatusCode G4CosmicFilter::initialize()
{

  ATH_MSG_INFO( "using collectionName "<<m_collectionName );
  ATH_MSG_INFO( "using PDG ID "<<m_PDGId );
  ATH_MSG_INFO( "using pTmin "<<m_ptMin );
  ATH_MSG_INFO( "using pTmax "<<m_ptMax );

  return StatusCode::SUCCESS;
}

void G4CosmicFilter::EndOfRun(const G4Run*)
{
  ATH_MSG_INFO( "processed "<< m_ntot <<" events, "<< m_npass<<" events passed filter" );
}

StatusCode G4CosmicFilter::queryInterface(const InterfaceID& riid, void** ppvInterface)
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

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

namespace G4UA{

  G4CosmicFilter::G4CosmicFilter(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc" ),"G4CosmicFilter"),
    m_config(config),m_report(),
    m_evtStore("StoreGateSvc/StoreGateSvc","G4CosmicFilter"),
    m_detStore("StoreGateSvc/DetectorStore","G4CosmicFilter"){
  }


  void G4CosmicFilter::endOfEvent(const G4Event*){;
    
    int counter(0);
    
    m_report.ntot++;
    
    SG::ReadHandle <TrackRecordCollection> coll(m_config.collectionName);
    if (! coll.isValid() )
      {
	ATH_MSG_WARNING( "Cannot retrieve TrackRecordCollection " << m_config.collectionName );
	G4RunManager::GetRunManager()->AbortEvent();
	return;
      }
    
    counter = coll->size();
    
    if (m_config.PDGId!=0 || m_config.ptMin>0 || m_config.ptMax>0)
      {
	counter=0;
	for (const auto& a_tr : *coll)
	  {
	    if (m_config.PDGId!=0 && m_config.PDGId != fabs(a_tr.GetPDGCode())) continue;
	    if (m_config.ptMin>0 && m_config.ptMin > a_tr.GetMomentum().perp() ) continue;
	    if (m_config.ptMax>0 && m_config.ptMax < a_tr.GetMomentum().perp() ) continue;
	    counter++;
	  }
      }
    
    //std::cout << "EndOfEventAction counter is "<<counter<<std::endl;
    if (counter==0)
      {
	ATH_MSG_INFO("aborting event due to failing filter");
	G4RunManager::GetRunManager()->AbortEvent();
	return;
      }
    
    m_report.npass++;
    return;
 
  }

} // namespace G4UA 
