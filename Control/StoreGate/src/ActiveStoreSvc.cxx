/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

using namespace SG;
using namespace std;

/// Standard Constructor
ActiveStoreSvc::ActiveStoreSvc(const std::string& name,ISvcLocator* svc) : 
  Service(name,svc),
  p_activeStore(0)
{}


/// Standard Destructor
ActiveStoreSvc::~ActiveStoreSvc()  
{}

//////////////////////////////////////////////////////////////
/// Service initialisation
StatusCode ActiveStoreSvc::initialize()    {

  // Initialize service:
  if(!(Service::initialize()).isSuccess()) return StatusCode::FAILURE;

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "Initializing " << name() 
      << " - package version " << PACKAGE_VERSION << endreq ;

  // set up the incident service:
  static const bool CREATEIF(true);
  StatusCode sc = service("StoreGateSvc", 
			  p_activeStore, 
			  CREATEIF);
  if ( !sc.isSuccess() ) 
  {
      log << MSG::ERROR 
	  << "Could not locate default store"
          << endreq;
      return sc;
  }

  return StatusCode::SUCCESS;

}


///set the active store pointer: used by the event loop mgrs
void ActiveStoreSvc::setStore(StoreGateSvc* s)
{
  p_activeStore = s;
  s->makeCurrent();
}


/// get proxy for a given data object address in memory,
/// but performs a deep search among all possible 'symlinked' containers
DataProxy* 
ActiveStoreSvc::deep_proxy(const void* const pTransient) const {
  return p_activeStore->deep_proxy(pTransient);
}

/// get proxy for a given data object address in memory
DataProxy* 
ActiveStoreSvc::proxy(const void* const pTransient) const {
  return p_activeStore->proxy(pTransient);
}

/// get default proxy with given id. Returns 0 to flag failure
DataProxy* 
ActiveStoreSvc::proxy(const CLID& id) const {
  return p_activeStore->proxy(id);
}

/// get proxy with given id and key. Returns 0 to flag failure
DataProxy* 
ActiveStoreSvc::proxy(const CLID& id, const std::string& key) const { 
  return p_activeStore->proxy(id,key);
}

SG::DataProxy* ActiveStoreSvc::proxy_exact (SG::sgkey_t sgkey) const
{
  return p_activeStore->proxy_exact (sgkey);
}

/// return the list of all current proxies in store
vector<const SG::DataProxy*> 
ActiveStoreSvc::proxies() const {
  return p_activeStore->proxies();
}


/// Raw addition of a proxy to the store.
StatusCode ActiveStoreSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  return p_activeStore->addToStore (id, proxy);
}



const InterfaceID& ActiveStoreSvc::interfaceID() { 
  static const InterfaceID _IDActiveStoreSvc("ActiveStoreSvc", 1, 0);
  return _IDActiveStoreSvc; 
}
StatusCode ActiveStoreSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = (ActiveStoreSvc*)this;
  }
  else if ( IProxyDict::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IProxyDict*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


