/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigationThinningSvc.h"



TrigNavigationThinningSvc::TrigNavigationThinningSvc( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthService(name, pSvcLocator),
    m_workerThinning("ThinningSvc", name),
    m_slimmingTool("TrigNavigationThinningTool/TrigNavigationThinningTool/")
{
  declareProperty("WorkerThinningSvc", m_workerThinning, "Service which deals wiht all requests except the navigation");
  declareProperty("SlimmingTool", m_slimmingTool, "Tool responsible for the actual thinning");
}


StatusCode TrigNavigationThinningSvc::initialize() {
  CHECK( m_workerThinning.retrieve() );

  CHECK( m_slimmingTool.retrieve() );
  return StatusCode::SUCCESS;
}
StatusCode TrigNavigationThinningSvc::reinitialize() {
  return StatusCode::SUCCESS;
}
StatusCode TrigNavigationThinningSvc::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode TrigNavigationThinningSvc::queryInterface(const InterfaceID& rrid, void** ppvInterface) {
  if ( IThinningSvc::interfaceID().versionMatch(rrid)) {
    *ppvInterface = (IThinningSvc*)this;
  } else if ( ITrigNavigationThinningSvc::interfaceID().versionMatch(rrid) ) {
    *ppvInterface = (ITrigNavigationThinningSvc*)this;
  } else {
    return Service::queryInterface(rrid, ppvInterface);
  }

  return StatusCode::SUCCESS;
}





// forwards of ThinningSvc
bool TrigNavigationThinningSvc::thinningOccurred() const  {
    return m_workerThinning->thinningOccurred(); 
  }
  
  
StatusCode TrigNavigationThinningSvc::register_slimmer (Athena::ISlimmingHdlr *handler)  { 
  return m_workerThinning->register_slimmer(handler);
}

Athena::IThinningHdlr* TrigNavigationThinningSvc::handler( SG::DataProxy* proxy )  {
  return m_workerThinning->handler(proxy);
}

StatusCode TrigNavigationThinningSvc::filter_impl( Athena::IThinningHdlr* handler,
	       SG::DataProxy* proxy,
	       const IThinningSvc::Filter_t& filter,
	       const IThinningSvc::Operator::Type op)  {
  return m_workerThinning->filter_impl(handler, proxy, filter, op);
}


   
StatusCode TrigNavigationThinningSvc::commit()  {
  return m_workerThinning->commit();
}

   
StatusCode TrigNavigationThinningSvc::rollback()  {
  return m_workerThinning->rollback();
}


   
std::size_t TrigNavigationThinningSvc::index_impl( const SG::DataProxy* objProxy, 
						   std::size_t idx ) const  {
  return m_workerThinning->index_impl(objProxy, idx);
}


  
bool TrigNavigationThinningSvc::is_thinned_impl(const SG::DataProxy* p) const  {
  return m_workerThinning->is_thinned_impl(p);
}

// IIncidentListener
void TrigNavigationThinningSvc::handle( const Incident& incident )  { 
  IIncidentListener *incidentListener(0);
  if( m_workerThinning->queryInterface(IIncidentListener::interfaceID(), (void**)&incidentListener).isFailure() ) {
    ATH_MSG_WARNING("Worker ThinningSvc " << m_workerThinning.type()
		    <<"/" << m_workerThinning.name()
		    <<" can not be querried for IncidentListerner interface");
    
  }
  if ( incidentListener ) 
    incidentListener->handle(incident); 
  else
    ATH_MSG_WARNING("Worker ThinningSvc " << m_workerThinning.type()
		    <<"/" << m_workerThinning.name()
		    <<" does not implement IncidentListener interface, request can not be handled");
}



  ///@{ @c IProxyDict interface
  /// get proxy for a given data object address in memory,
  /// but performs a deep search among all possible 'symlinked' containers
SG::DataProxy* TrigNavigationThinningSvc::deep_proxy(const void* const pTransient) const   { 
  return m_workerThinning->deep_proxy(pTransient);
}

  /// get proxy for a given data object address in memory
SG::DataProxy* TrigNavigationThinningSvc::proxy( const void* const pTransient ) const   { 
  return m_workerThinning->proxy(pTransient); 
}

  /// get default proxy with given id. Returns 0 to flag failure
SG::DataProxy* TrigNavigationThinningSvc::proxy( const CLID& id ) const  { 
  return m_workerThinning->proxy(id); 
}

/// get proxy with given id and key. Returns 0 to flag failure
SG::DataProxy* TrigNavigationThinningSvc::proxy( const CLID& id, const std::string& key ) const  { 
  return m_workerThinning->proxy(id, key); 
}

/// Get proxy given a hashed key+clid.
/// Find an exact match; no handling of aliases, etc.
/// Returns 0 to flag failure.
SG::DataProxy* TrigNavigationThinningSvc::proxy_exact (SG::sgkey_t sgkey) const  {
  return m_workerThinning->proxy_exact(sgkey); 
}

/// return the list of all current proxies in store
std::vector<const SG::DataProxy*> TrigNavigationThinningSvc::proxies() const  { 
  return m_workerThinning->proxies(); 
}

/// Add a new proxy to the store.
StatusCode TrigNavigationThinningSvc::addToStore (CLID id, SG::DataProxy* proxy)  {
  return m_workerThinning->addToStore(id, proxy);
}

