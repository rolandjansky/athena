/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/src.TestStore.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief Dummy event store, for regression tests.
 */


#include "SGTools/TestStore.h"
#include "SGTools/DataBucketBase.h"
#include <iostream>
#include <algorithm>


namespace SGTest {


TestStore store;


unsigned long TestStore::addRef()
{
  std::cout << "addRef\n"; std::abort();
}


unsigned long TestStore::release()
{
  std::cout << "release\n"; std::abort();
}


StatusCode TestStore::queryInterface(const InterfaceID &/*ti*/, void** /*pp*/)
{
  std::cout << "queryInterface\n"; std::abort();
}


std::vector<const SG::DataProxy*> TestStore::proxies() const
{
  std::cout << "proxies\n"; std::abort();
}


const std::string* TestStore::keyToString (sgkey_t key) const
{
  return m_stringPool.keyToString (key);
}


void TestStore::registerKey (sgkey_t /*key*/,
                             const std::string& /*str*/,
                             CLID /*clid*/)
{
  std::cout << "registerKey\n"; std::abort();
}


SG::DataProxy* TestStore::recordObject (SG::DataObjectSharedPtr<DataObject> obj,
                                        const std::string& key,
                                        bool allowMods,
                                        bool returnExisting)
{
  const void* raw_ptr = obj.get();
  if (DataBucketBase* bucket = dynamic_cast<DataBucketBase*> (obj.get())) {
    raw_ptr = bucket->object();
  }

  CLID clid = obj->clID();
  SG::DataProxy* proxy = this->proxy (clid, key);
  if (proxy) {
    if (returnExisting)
      return proxy;
    else
      return nullptr;
  }

  proxy = this->proxy (raw_ptr);
  if (proxy) {
    sgkey_t sgkey = stringToKey (key, obj->clID());
    if (!returnExisting)
      return nullptr;
    if (obj->clID() == proxy->clID()) {
      // Alias?
      m_kmap[sgkey] = proxy;
      proxy->addRef();
      proxy->transientAddress()->setAlias (key);
      return proxy;
    }
    if (key == proxy->name()) {
      // Symlink?
      m_kmap[sgkey] = proxy;
      proxy->addRef();
      proxy->transientAddress()->setTransientID (obj->clID());
      return proxy;
    }

    // Error.
    return nullptr;
  }
  
  proxy = record1 (raw_ptr, obj.get(), clid, key);
  if (!allowMods)
    proxy->setConst();
  return proxy;
}


StatusCode TestStore::updatedObject (CLID /*id*/,
                                     const std::string& key)
{
  m_updated.push_back (key);
  if (m_failUpdatedObject)
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


const std::string& TestStore::name() const
{
  static const std::string nm = "TestStore";
  return nm;
}


SG::DataProxy* TestStore::proxy(const void* const pTransient) const
{ 
  tmap_t::const_iterator i = m_tmap.find (pTransient);
  if (i != m_tmap.end())
    return i->second;
  return 0;
}


SG::DataProxy* TestStore::proxy(const CLID& id, const std::string& key) const
{
  sgkey_t sgkey = const_cast<TestStore*>(this)->stringToKey (key, id);
  kmap_t::const_iterator i = m_kmap.find (sgkey);
  if (i != m_kmap.end())
    return i->second;
  m_missedProxies.emplace_back (id, key);
  return 0;
}


SG::DataProxy* TestStore::proxy_exact (SG::sgkey_t sgkey) const 
{
  kmap_t::const_iterator i = m_kmap.find (sgkey);
  if (i != m_kmap.end())
    return i->second;
  return 0;
}


sgkey_t TestStore::stringToKey (const std::string& str, CLID clid)
{
  return m_stringPool.stringToKey (str, clid);
}


const std::string* TestStore::keyToString (sgkey_t key, CLID& clid) const
{
  return m_stringPool.keyToString (key, clid);
}


bool TestStore::tryELRemap (sgkey_t sgkey_in, size_t index_in,
                            sgkey_t& sgkey_out, size_t& index_out)
{
  remap_t::iterator i = m_remap.find (TestStoreRemap (sgkey_in, index_in));
  if (i == m_remap.end()) return false;
  sgkey_out = i->second.key;
  index_out = i->second.index;
  return true;
}


StatusCode TestStore::addToStore (CLID /*id*/, SG::DataProxy* proxy)
{
  proxy->setStore (this);
  m_kmap[proxy->transientAddress()->sgkey()] = proxy;
  proxy->addRef();
  return StatusCode::SUCCESS;
}


void TestStore::boundHandle (IResetable* handle)
{
  m_boundHandles.push_back (handle);
}


void TestStore::unboundHandle (IResetable* handle)
{
  std::vector<IResetable*>::iterator it =
    std::find (m_boundHandles.begin(), m_boundHandles.end(), handle);
  if (it != m_boundHandles.end())
    m_boundHandles.erase (it);
}


SG::DataProxy* TestStore::record1 (const void* p, DataObject* obj,
                                   CLID clid, const std::string& key)
{
  sgkey_t sgkey = stringToKey (key, clid);
  if (m_kmap.find (sgkey) != m_kmap.end()) {
    SG::DataProxy* dp = m_kmap[sgkey];
    dp->setObject (obj);
    if (dp->transientAddress()->clID() == CLID_NULL)
      dp->transientAddress()->setID (clid, key);
    m_tmap[p] = dp;
    return dp;
  }

  SG::TransientAddress* tAddr = new SG::TransientAddress(clid, key);
  SG::DataProxy* dp = new SG::DataProxy(obj, tAddr);
  dp->setStore (this);
  m_tmap[p] = dp;

  m_kmap[sgkey] = dp;
  dp->addRef();
  tAddr->setSGKey (sgkey);
  return dp;
}


void TestStore::remap (sgkey_t sgkey_in, sgkey_t sgkey_out,
                       size_t index_in, size_t index_out)
{
  m_remap[TestStoreRemap(sgkey_in, index_in)] =
    TestStoreRemap(sgkey_out, index_out);
}


void TestStore::alias (SG::DataProxy* proxy,
                       const std::string& newKey)
{
  sgkey_t sgkey = stringToKey (newKey, proxy->clID());
  m_kmap[sgkey] = proxy;
  proxy->addRef();
  proxy->transientAddress()->setAlias (newKey);
}


void initTestStore()
{
  SG::CurrentEventStore::setStore (&store);
}


} // namespace SGTest
