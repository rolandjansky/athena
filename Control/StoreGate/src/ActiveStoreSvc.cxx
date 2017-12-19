/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/StoreID.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

using namespace SG;
using namespace std;

/// Standard Constructor
ActiveStoreSvc::ActiveStoreSvc(const std::string& name,ISvcLocator* svc) : 
  Service(name,svc),
  p_activeStore(0),
  m_storeName()
{
  declareProperty("StoreName", m_storeName=StoreID::storeName(StoreID::EVENT_STORE));
}


/// Standard Destructor
ActiveStoreSvc::~ActiveStoreSvc()  
{}

//////////////////////////////////////////////////////////////
/// Service initialization
StatusCode ActiveStoreSvc::initialize()    {

  CHECK( Service::initialize() );

  msg() << MSG::VERBOSE << "Initializing " << name() 
      << " - package version " << PACKAGE_VERSION << endmsg ;

  const bool CREATEIF(true);
  CHECK(  service(m_storeName, p_activeStore, CREATEIF) );
  return StatusCode::SUCCESS;
}

///set the active store pointer: used by the event loop mgrs
void ActiveStoreSvc::setStore(StoreGateSvc* s)
{
  if (p_activeStore != s) {
    p_activeStore = s;
    s->makeCurrent();
  }
}


/// get proxy for a given data object address in memory
DataProxy* 
ActiveStoreSvc::proxy(const void* const pTransient) const {
  return activeStore()->proxy(pTransient);
}

/// get proxy with given id and key. Returns 0 to flag failure
DataProxy* 
ActiveStoreSvc::proxy(const CLID& id, const std::string& key) const { 
  return activeStore()->proxy(id,key);
}

SG::DataProxy* ActiveStoreSvc::proxy_exact (SG::sgkey_t sgkey) const
{
  return activeStore()->proxy_exact (sgkey);
}

/// return the list of all current proxies in store
vector<const SG::DataProxy*> 
ActiveStoreSvc::proxies() const {
  return activeStore()->proxies();
}


/// Raw addition of a proxy to the store.
StatusCode ActiveStoreSvc::addToStore (CLID id, SG::DataProxy* proxy)
{
  return activeStore()->addToStore (id, proxy);
}


/**
 * @brief Record an object in the store.
 * @param obj The data object to store.
 * @param key The key as which it should be stored.
 * @param allowMods If false, the object will be recorded as const.
 * @param returnExisting If true, return proxy if this key already exists.
 *
 * Full-blown record.  @c obj should usually be something
 * deriving from @c SG::DataBucket.
 *
 * Returns the proxy for the recorded object; nullptr on failure.
 * If the requested CLID/key combination already exists in the store,
 * the behavior is controlled by @c returnExisting.  If true, then
 * the existing proxy is returned; otherwise, nullptr is returned.
 * In either case, @c obj is destroyed.
 */
SG::DataProxy*
ActiveStoreSvc::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                              const std::string& key,
                              bool allowMods,
                              bool returnExisting)
{
  return activeStore()->recordObject (obj, key, allowMods, returnExisting);
}


/**
 * @brief Inform HIVE that an object has been updated.
 * @param id The CLID of the object.
 * @param key The key of the object.
 */
StatusCode ActiveStoreSvc::updatedObject (CLID id, const std::string& key)
{
  return activeStore()->updatedObject (id, key);
}


/**
 * @brief Find the key for a string/CLID pair.
 * @param str The string to look up.
 * @param clid The CLID associated with the string.
 * @return A key identifying the string.
 *         A given string will always return the same key.
 *         Will abort in case of a hash collision!
 */
sgkey_t ActiveStoreSvc::stringToKey (const std::string& str, CLID clid)
{
  return activeStore()->stringToKey (str, clid);
}


/**
 * @brief Find the string corresponding to a given key.
 * @param key The key to look up.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* ActiveStoreSvc::keyToString (sgkey_t key) const
{
  return activeStore()->keyToString (key);
}


/**
 * @brief Find the string and CLID corresponding to a given key.
 * @param key The key to look up.
 * @param clid[out] The found CLID.
 * @return Pointer to the string found, or null.
 *         We can find keys as long as the corresponding string
 *         was given to either @c stringToKey() or @c registerKey().
 */
const std::string* ActiveStoreSvc::keyToString (sgkey_t key,
                                                CLID& clid) const
{
  return activeStore()->keyToString (key, clid);
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
void ActiveStoreSvc::registerKey (sgkey_t key,
                                  const std::string& str,
                                  CLID clid)
{
  return activeStore()->registerKey (key, str, clid);
}


const InterfaceID& ActiveStoreSvc::interfaceID() { 
  static const InterfaceID IDActiveStoreSvc("ActiveStoreSvc", 1, 0);
  return IDActiveStoreSvc; 
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


StoreGateSvc* ActiveStoreSvc::activeStoreOOL() const
{
  StoreGateSvc* pstore = nullptr;
  const bool CREATEIF(true);
  if (service(m_storeName, pstore, CREATEIF).isSuccess()) {
    return pstore;
  }
  return nullptr;
}
