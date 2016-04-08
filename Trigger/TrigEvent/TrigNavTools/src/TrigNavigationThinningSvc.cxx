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



  /// get proxy for a given data object address in memory
SG::DataProxy* TrigNavigationThinningSvc::proxy( const void* const pTransient ) const   { 
  return m_workerThinning->proxy(pTransient); 
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

/// Record an object in the store.
SG::DataProxy*
TrigNavigationThinningSvc::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                         const std::string& key,
                                         bool allowMods,
                                         bool returnExisting)
{
  return m_workerThinning->recordObject (std::move(obj), key,
                                         allowMods, returnExisting);
}


/// Inform HIVE that an object has been updated.
StatusCode
TrigNavigationThinningSvc::updatedObject (CLID id, const std::string& key)
{
  return m_workerThinning->updatedObject (id, key);
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
SG::sgkey_t
TrigNavigationThinningSvc::stringToKey (const std::string& str, CLID clid)
{
  return m_workerThinning->stringToKey (str, clid);
}

/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string*
TrigNavigationThinningSvc::keyToString (SG::sgkey_t key) const
{
  return m_workerThinning->keyToString (key);
}

/**
 * @brief Find the string and CLID corresponding to a given key.
 * @param key The key to look up.
 * @param clid[out] The found CLID.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string*
TrigNavigationThinningSvc::keyToString (SG::sgkey_t key,
                                        CLID& clid) const
{
  return m_workerThinning->keyToString (key, clid);
}

/**
 * @brief Remember an additional mapping from key to string/CLID.
 * @param key The key to enter.
 * @param str The string to enter.
 * @param clid The CLID associated with the string.
 * @return True if successful; false if the @c key already
 *         corresponds to a different string.
 *
 * This registers an additional mapping from a key to a string;
 * it can be found later through @c lookup() on the string.
 * Logs an error if @c key already corresponds to a different string.
 */
void TrigNavigationThinningSvc::registerKey (SG::sgkey_t key,
                                             const std::string& str,
                                             CLID clid)
{
  return m_workerThinning->registerKey (key, str, clid);
}

