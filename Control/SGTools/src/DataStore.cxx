/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/DataStore.h"
#include "SGTools/DataProxy.h"
#include "SGTools/exceptions.h"
#include "AthenaKernel/IStringPool.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "SGAudCore/ISGAudSvc.h"
#include "CxxUtils/checker_macros.h"
#include "CxxUtils/AthUnlikelyMacros.h"

using namespace std;
using SG::DataStore;
using SG::DataProxy;
using SG::ProxyMap;
using SG::ProxyIterator;
using SG::ConstProxyIterator;


/**
 * @brief Constructor.
 * @param pool The string pool associated with this store.
 */
DataStore::DataStore (IProxyDict& pool)
  : m_pool (pool),
    m_storeMap(), m_storeID(StoreID::UNKNOWN), m_t2p(), 
    m_pSGAudSvc(0), m_noAudSvc(0), m_pSvcLoc(0)
{
   setSvcLoc().ignore();
}

//Destructor
DataStore::~DataStore()
{
  clearStore(false, true, nullptr);
}

StatusCode DataStore::setSvcLoc(){
  StatusCode sc(StatusCode::FAILURE);
  m_pSvcLoc = Gaudi::svcLocator( );
  if (!m_pSvcLoc) std::cout<<"DataStore::setSvcLoc: WARNING svcLocator not found! "<<std::endl;
  return sc;
}

void DataStore::setSGAudSvc() {
  if (0 == m_pSGAudSvc) {
    //try once to get the service
    const bool DONOTCREATE(false);
    if (!m_noAudSvc) {
      m_noAudSvc = m_pSvcLoc->service("SGAudSvc", m_pSGAudSvc, 
				      DONOTCREATE).isFailure();
    }
  }
  return;
}


//////////////////////////////////////////////////////////////
void DataStore::clearStore(bool force, bool hard, MsgStream* /*pmlog*/)
{
  /// Go through the list of unique proxies, and run requestRelease()
  /// on each.  If that returns true, then we're meant to remove this
  /// proxy from the store.  Be careful that removing a proxy from the
  /// store will modify the m_proxies list, so if the proxy is successfully
  /// removed, we should not bump the index.
  for (size_t i = 0; i < m_proxies.size(); ) {
    SG::DataProxy* dp = m_proxies[i];
    if (ATH_UNLIKELY (dp->requestRelease (force, hard))) {
      if (removeProxy (dp, true, true).isFailure()) {
        ++i;
      }
    }
    else {
      ++i;
    }
  }

  // clear T2PMap
  m_t2p.clear();
}

/////////////////////////////////////////////////////////////
// access all the keys associated with an object: 
// 
void DataStore::keys(const CLID& id, std::vector<std::string>& vkeys,
		     bool includeAlias, bool onlyValid)
{
  vector<string> tkeys;
  ProxyMap& pmap = m_storeMap[id];
  ConstProxyIterator p_iter = pmap.begin();
  for (; p_iter != pmap.end(); p_iter++) {
    bool includeProxy(true);
    if (onlyValid) includeProxy=p_iter->second->isValid();
    if (includeAlias) {
      if (includeProxy) tkeys.push_back(p_iter->first);
    }
    else {
      if (p_iter->first == p_iter->second->name() && includeProxy) 
	tkeys.push_back(p_iter->first);
    }
  }
  vkeys.swap(tkeys);
  return;
}

//////////////////////////////////////////////////////////////
//---------------------------------------------------------------//
// record the proxy in StoreGate
StatusCode 
DataStore::addToStore(const CLID& id, DataProxy* dp)
{
  if (!dp) {
    return StatusCode::FAILURE;
  }

  /// If this proxy has not yet been added to the store, then make
  /// an entry for it in m_proxies.  Remember the index in that list.
  int index = -1;
  if (dp->store() == nullptr) {
    index = m_proxies.size();
    m_proxies.push_back (dp);
  }

  if (id == 0 && dp->clID() == 0 && dp->sgkey() != 0) {
    // Handle a dummied proxy.
    m_keyMap[dp->sgkey()] = std::make_pair (index, dp);
  }
  else {
    ProxyMap& pmap = m_storeMap[id];

    // Set the primary key.
    sgkey_t primary_sgkey = m_pool.stringToKey (dp->name(), dp->clID());
    sgkey_t sgkey = primary_sgkey;
    if (id != dp->clID()) {
      sgkey = m_pool.stringToKey (dp->name(), id);
    }
    if (dp->sgkey() == 0) {
      dp->setSGKey (sgkey);
    }

    if (!m_keyMap.emplace (sgkey, std::make_pair (index, dp)).second) {
      if (index != -1) {
        m_proxies.pop_back();
      }
      return StatusCode::FAILURE;
    }

    pmap.insert(ProxyMap::value_type(dp->name(), dp));
  }

  // Note : No checking if proxy already exists in store because it
  // is already checked in StoreGateSvc.  
  //  if (pmap.find(dp->name()) == pmap.end()) {
  dp->addRef();   // The store now owns this
  dp->setT2p(&m_t2p);
  dp->setStore(&m_pool);

  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////
//---------------------------------------------------------------//
// if Proxy is a resettable proxy only then reset it, otherwise
// delete it and remove from proxy map.
StatusCode
DataStore::removeProxy(DataProxy* proxy, bool forceRemove, bool hard)
{
  StatusCode sc(StatusCode::FAILURE);
  if (0 == proxy) return sc;

  if (!forceRemove && proxy->isResetOnly()) {
    proxy->reset (hard);
    sc =  StatusCode::SUCCESS;
  } else {

    proxy->setStore (nullptr);
    
    SG::DataProxy::CLIDCont_t clids = proxy->transientID();
    std::string name = proxy->name();
    sgkey_t primary_sgkey = proxy->sgkey();

    CLID clid = proxy->clID();
    ProxyMap* pmap = nullptr;
    StoreIterator storeIter = m_storeMap.find(clid);
    if (storeIter != m_storeMap.end()) {
      pmap = &m_storeMap[clid];
    }

    // first remove the alias key:
    SG::DataProxy::AliasCont_t alias_set = proxy->alias();
    for (SG::DataProxy::AliasCont_t::const_iterator i = alias_set.begin();
         i != alias_set.end(); ++i)
    {
      m_keyMap.erase (m_pool.stringToKey (*i, clid));
      if (pmap && 1 == pmap->erase(*i)) proxy->release();
    }
      
    // then remove the primary key
    KeyMap_t::iterator it = m_keyMap.find (primary_sgkey);
    if (it == m_keyMap.end()) {
      return StatusCode::FAILURE;
    }
    // Remember the index of this proxy in m_proxies.
    int index = it->second.first;
    sc = StatusCode::SUCCESS;

    m_keyMap.erase (it);
    if (storeIter != m_storeMap.end()) {
      if (1 == storeIter->second.erase(name)) {
        proxy->release();
      }
    }

    // Remove all symlinks too.
    for (SG::DataProxy::CLIDCont_t::const_iterator i = clids.begin();
         i != clids.end(); ++i)
    {
      if (*i == clid) continue;
      m_keyMap.erase (m_pool.stringToKey (name, *i));
      storeIter = m_storeMap.find(*i);
      if (storeIter != m_storeMap.end()) {
        SG::ProxyIterator it = storeIter->second.find (name);
        if (it != storeIter->second.end() && it->second == proxy) {
          storeIter->second.erase (it);
          proxy->release();
        }
      }
    } //symlinks loop

    if (index != -1 && !m_proxies.empty()) {
      // Remove the proxy from m_proxies.  If it's not at the end, then
      // move the proxy at the end to this proxy's index (and update the
      // index for the other proxy stored in m_keyMap).
      if (index != (int)m_proxies.size() - 1) {
        SG::DataProxy* dp = m_proxies.back();
        m_proxies[index] = dp;
        m_keyMap[dp->sgkey()].first = index;
      }
      m_proxies.pop_back();
    }
  } //reset only
  return sc;
}

//////////////////////////////////////////////////////////////
//---------------------------------------------------------------//
// record the symlink in StoreGate
StatusCode 
DataStore::addSymLink(const CLID& linkid, DataProxy* dp)
{
  // Make sure the symlink doesn't already exist.
  DataProxy* exist = proxy_exact (linkid, dp->name());
  if (exist == dp) {
    // Entry already exists pointing at the desired proxy.
    return StatusCode::SUCCESS;
  }
  else if (exist != 0) {
    // Entry already exists pointing at another proxy.
    // Don't change the existing entry.
    return StatusCode::FAILURE;
  }

  dp->setTransientID(linkid); 
  return addToStore(linkid, dp);
}
//---------------------------------------------------------------//
// record the alias in StoreGate
StatusCode
DataStore::addAlias(const std::string& aliasKey, DataProxy* dp)
{
  // locate proxy map and add alias to proxymap
  ProxyMap& pmap = m_storeMap[dp->clID()];

  // check if another proxy for the same type caries the same alias.
  // if yes, then remove that alias from that proxy and establish the
  // alias in the new proxy.
  // pmap.insert will overwrite, associate alias with new proxy.
  ConstProxyIterator p_iter = pmap.find(aliasKey);
  if (p_iter != pmap.end() && dp->clID() == p_iter->second->clID()) {
    if (dp->name() == p_iter->second->name()) return StatusCode::SUCCESS;
    p_iter->second->removeAlias(aliasKey);
    p_iter->second->release();
  }
  // set alias in proxy
  dp->setAlias(aliasKey);
  dp->addRef();
  pmap[aliasKey] = dp;

  m_keyMap[m_pool.stringToKey (aliasKey, dp->clID())] = std::make_pair (-1, dp);
  //  pmap.insert(ProxyMap::value_type(aliasKey, dp));
  // use [] as it overwrites, .insert ignores second entry

  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------//
// Count instances of TYPE in store
int DataStore::typeCount(const CLID& id) const
{
  ConstStoreIterator storeIter = m_storeMap.find(id);
  if (storeIter == m_storeMap.end()) return 0;
  return (storeIter->second).size();
}

//---------------------------------------------------------------//
// Return proxy for a given Transient Address
DataProxy* DataStore::proxy(const TransientAddress* tAddr) const
{
  return proxy(tAddr->clID(), tAddr->name());
}

// Return proxy for a given Transient ID (TYPE and KEY) 
DataProxy* DataStore::proxy(const CLID& id, const std::string& key) const
{	 

  // The logic here: if a key is specified, we locate it.
  // If we don't find it and the default key (DEFAULTKEY) is specified,
  // then we return any existing proxy for this type as long as there
  // is exactly one, not counting aliases.  More than one would be ambiguous
	  
  ConstStoreIterator siter = m_storeMap.find(id);
  DataProxy *p(0);
  if (siter != m_storeMap.end()) 
  {
    const ProxyMap& pmap = siter->second;
    ConstProxyIterator p_iter = pmap.find(key);
    if (p_iter != pmap.end()) {
      p=p_iter->second;

    } else if (key == SG::DEFAULTKEY && !pmap.empty()) {
      // we did not find the object using key.
      // Now check for default object.
      // Simple case first --- single object.
      if (pmap.size() == 1) {
        p = pmap.begin()->second;
      } else {
        // Otherwise, match only the exact type requested.
        ConstProxyIterator p_match = pmap.end();
        size_t nmatch = 0;
        for (p_iter = pmap.begin(); p_iter != pmap.end(); ++p_iter) {
          if (p_iter->second->clID() == id) {
            ++nmatch;
            if (p_match == pmap.end()) p_match = p_iter;
          }
        }

        // We must have matched only one object, not counting its aliases.
        // Notice: we test that there are less than two matches: symlinked objects
        // may carry aliases from the concrete class. In that case nmatch
        // may be equal to or even smaller than the number of aliases
        if (nmatch > 0 &&
            (int(nmatch - p_match->second->alias().size()) < 2))
        {
          p = pmap.begin()->second;
        }
      }
    }
    else {
      // Ok since access to DataStore is serialized by StoreGate.
      DataStore* nc_store ATLAS_THREAD_SAFE = const_cast<DataStore*> (this);
      p = nc_store->findDummy (id, key);
    }
  }
  else {
    // Ok since access to DataStore is serialized by StoreGate.
    DataStore* nc_store ATLAS_THREAD_SAFE = const_cast<DataStore*> (this);
    p = nc_store->findDummy (id, key);
  }

  if (p && m_pSGAudSvc) 
    m_pSGAudSvc->SGAudit(p->name(), id, 0, m_storeID);

  return p;
}


/**
 * @brief Look for (and convert) a matching dummy proxy.
 * @param id The CLID for which to search.
 * @param key The key for which to search.
 *
 * In some cases, we may enter into the store a `dummy' proxy,
 * which is identified only by the hashed CLID/key pair.
 * (This can happen when we have a link to an object that's not
 * listed in the DataHeader; in this case, we know the only hashed key
 * and not the CLID or key.)
 *
 * This function is called after we fail to find a proxy by CLID/key.
 * It additionally checks to see if there exists a dummy proxy with
 * a hash matching this CLID/key.  If so, the CLID/key are filled
 * in in the proxy, and the proxy is entered in m_storeMap.
 *
 * Returns either the matching proxy or 0.
 */
DataProxy* DataStore::findDummy (CLID id, const std::string& key)
{
  sgkey_t sgkey = m_pool.stringToKey (key, id);
  DataProxy* p = proxy_exact (sgkey);
  if (p) {
    p->setID (id, key);
    ProxyMap& pmap = m_storeMap[id];
    if (!pmap.insert(ProxyMap::value_type(key, p)).second) {
      // This shouldn't happen.
      DataProxy* p2 = pmap[key];
      throw SG::ExcProxyCollision (id, key, p2->clID(), p2->name());
    }
  }
  return p;
}


/// get proxy with given key. Returns 0 to flag failure
/// the key must match exactly (no wild carding for the default key)
DataProxy* DataStore::proxy_exact(sgkey_t sgkey) const
{
  if (m_pSGAudSvc) {
    CLID clid;
    const std::string* strkey = m_pool.keyToString (sgkey, clid);
    if (strkey)
      m_pSGAudSvc->SGAudit(*strkey, clid, 0, m_storeID);
  }
  KeyMap_t::const_iterator i = m_keyMap.find (sgkey);
  if (i != m_keyMap.end())
    return i->second.second;
  return 0;
}


/// get proxy with given key. Returns 0 to flag failure
/// the key must match exactly (no wild carding for the default key)
DataProxy* DataStore::proxy_exact(const CLID& id,
                                  const std::string& key) const
{
  return proxy_exact (m_pool.stringToKey (key, id));
}


//---------------------------------------------------------------//
// Return an iterator over proxies for a given CLID:
StatusCode DataStore::pRange(const CLID& id, ConstProxyIterator& pf,
			                     ConstProxyIterator& pe) const
{
  static const ProxyMap emptyMap;
  StatusCode sc(StatusCode::FAILURE);

  ConstStoreIterator storeIter = m_storeMap.find(id);
  if (storeIter != m_storeMap.end()) 
  {
    const ProxyMap& pmap = storeIter->second;
    pf = pmap.begin();
    pe = pmap.end();
    if (pmap.size() > 0) sc = StatusCode::SUCCESS;
  } else {
    //keep valgrind happy
    pf = emptyMap.end();
    pe = emptyMap.end();
  }
  return sc;
}
//---------------------------------------------------------------//
// Return an iterator over the Store Map
StatusCode DataStore::tRange(ConstStoreIterator& tf,
			     ConstStoreIterator& te) const
{
  tf = m_storeMap.begin();
  te = m_storeMap.end();
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------//
// Build a list of proxies that are in the Store:
StatusCode DataStore::proxyList(std::list<DataProxy*>& plist) const
{
  ConstProxyIterator pf, pe;
  ConstStoreIterator sf, se;
  (tRange(sf, se)).ignore();
  for (; sf!=se; sf++) 
  {
    (pRange(sf->first, pf, pe)).ignore();
    for (; pf!=pe; pf++) {
      // FIXME : Check validity of proxy.
      plist.push_back(pf->second);
    }
  }
  return StatusCode::SUCCESS;
}
//---------------------------------------------------------------//
// locate Persistent Representation of a Transient Object
//
DataProxy* DataStore::locatePersistent(const void* const pTransient) const
{
  return m_t2p.locatePersistent(pTransient);
}

//////////////////////////////////////////////////////////////////
StatusCode
DataStore::t2pRegister(const void* const pTrans, DataProxy* const pPers)
{
    std::string name=pPers->name();
    int i=name.find("/", 0);
    name=name.erase(0,i+1);  

  if (doAudit()) m_pSGAudSvc->SGAudit(name, pPers->clID(), 1, m_storeID);

  return (m_t2p.t2pRegister(pTrans, pPers)) ? 
			StatusCode::SUCCESS :
			StatusCode::FAILURE; 
}







