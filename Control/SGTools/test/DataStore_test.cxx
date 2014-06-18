/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataStore_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Regression tests for DataStore.
 */

#undef NDEBUG
#include "SGTools/DataStore.h"
#include "SGTools/StringPool.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/IProxyDictWithPool.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/IAddressProvider.h"
#include <iostream>
#include <cstdlib>
#include <cassert>


class TestProvider
  : public IAddressProvider
{
public:
  virtual StatusCode updateAddress(StoreID::type /*storeID*/,
				   SG::TransientAddress* /*pTAd*/)
  { return StatusCode::SUCCESS; }
};


class TestPool
  : public IProxyDictWithPool
{
public:
  virtual
  sgkey_t stringToKey (const std::string& str, CLID clid)
  { return m_pool.stringToKey (str, clid); }
  virtual
  const std::string* keyToString (sgkey_t key) const 
  { return m_pool.keyToString (key); }
  virtual
  const std::string* keyToString (sgkey_t key,
                                  CLID& clid) const
  { return m_pool.keyToString (key, clid); }
  virtual
  void registerKey (sgkey_t key,
                    const std::string& str,
                    CLID clid)
  { m_pool.registerKey (key, str, clid); }


  virtual unsigned long addRef() { std::abort(); }
  virtual unsigned long release() { std::abort(); }
  virtual const std::string& name() const { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &, void** )  { std::abort(); }
  virtual SG::DataProxy* proxy(const CLID&) const { std::abort(); }
  virtual StatusCode addToStore (CLID , SG::DataProxy*) { std::abort(); }
  virtual SG::DataProxy* deep_proxy(const void* const) const { std::abort(); }
  virtual SG::DataProxy* proxy(const void* const) const { std::abort(); }
  virtual SG::DataProxy* proxy_exact (SG::sgkey_t) const { std::abort(); }
  virtual std::vector<const SG::DataProxy*> proxies() const { std::abort(); }
  virtual SG::DataProxy* proxy(const CLID&, const std::string&) const { std::abort(); }

  SG::StringPool m_pool;
};


SG::DataProxy* make_proxy (CLID clid, const std::string& name)
{
  SG::TransientAddress* tad = new SG::TransientAddress (clid, name);
  return new SG::DataProxy (tad, (IConversionSvc*)0);
}


void test_ctor()
{
  std::cout << "test_ctor\n";
  TestPool pool;
  SG::DataStore store (pool);
  assert (store.storeID() == StoreID::UNKNOWN);
  store.setStoreID (StoreID::SPARE_STORE);
  assert (store.storeID() == StoreID::SPARE_STORE);
}


void test_addToStore()
{
  std::cout << "test_addToStore\n";

  TestPool pool;
  SG::DataStore store (pool);
  SG::StringPool::sgkey_t sgkey = pool.stringToKey ("dp1", 123);

  assert (store.addToStore (123, 0).isFailure());
  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (dp1->refCount() == 0);
  assert (store.addToStore (123, dp1).isSuccess());
  assert (dp1->store() == &pool);
  assert (dp1->refCount() == 1);
  assert (dp1->transientAddress()->sgkey() == sgkey);
  assert (store.proxy_exact (sgkey) == dp1);
  assert (store.proxy (123, "dp1") == dp1);

  int trans1;
  dp1->registerTransient (&trans1);
  assert (store.locatePersistent (&trans1) == dp1);

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (124, dp2).isSuccess());
  SG::StringPool::sgkey_t sgkey2a = pool.stringToKey ("dp2", 123);
  SG::StringPool::sgkey_t sgkey2b = pool.stringToKey ("dp2", 124);
  assert (store.proxy_exact (sgkey2b) == dp2);
  assert (store.proxy_exact (sgkey2a) == 0);
  assert (dp2->transientAddress()->sgkey() == sgkey2a);
}


void test_addAlias()
{
  std::cout << "test_addAlias\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());

  assert (store.addAlias ("dp1a", dp1).isSuccess());
  assert (store.addAlias ("dp1b", dp1).isSuccess());
  assert (dp1->refCount() == 3);

  assert (store.proxy (123, "dp1a") == dp1);
  assert (store.proxy_exact (pool.stringToKey ("dp1a", 123)) == dp1);

  assert (dp1->transientAddress()->alias().count ("dp1a") == 1);

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());
  assert (store.addAlias ("dpx", dp2).isSuccess());
  assert (dp2->refCount() == 2);
  assert (dp2->transientAddress()->alias().count ("dpx") == 1);

  assert (store.addAlias ("dpx", dp1).isSuccess());
  assert (dp1->refCount() == 4);
  assert (dp2->refCount() == 1);
  assert (dp2->transientAddress()->alias().count ("dpx") == 0);
  assert (dp1->transientAddress()->alias().count ("dpx") == 1);
  assert (store.addAlias ("dpx", dp1).isSuccess());
  assert (dp1->refCount() == 4);
  assert (dp2->refCount() == 1);
}


void test_addSymLink()
{
  std::cout << "test_addSymLink\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  assert (store.proxy_exact (123, "dp1") == dp1);
  assert (store.proxy_exact (124, "dp1") == dp1);
  assert (dp1->transientAddress()->transientID(123));
  assert (dp1->transientAddress()->transientID(124));

  assert (store.addSymLink (124, dp1).isSuccess());
  assert (store.proxy_exact (124, "dp1") == dp1);

  SG::DataProxy* dp2 = make_proxy (125, "dp1");
  assert (store.addToStore (125, dp2).isSuccess());
  assert (store.addSymLink (125, dp1).isFailure());
  assert (!dp1->transientAddress()->transientID(125));
}


void test_proxy_exact()
{
  std::cout << "test_proxy_exact\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());

  assert (store.proxy_exact (123, "dp1") == dp1);
  assert (store.proxy_exact (124, "dp1") == 0);
  assert (store.proxy_exact (123, "dp2") == 0);
  SG::StringPool::sgkey_t sgkey = pool.stringToKey ("dp1", 123);
  assert (store.proxy_exact (sgkey) == dp1);
  assert (store.proxy_exact (sgkey+1) == 0);
}


void test_proxy()
{
  std::cout << "test_proxy\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());

  assert (store.proxy (123, "dp1") == dp1);
  assert (store.proxy (123, "") == dp1);

  assert (store.proxy (new SG::TransientAddress (123, "dp1")) == dp1);

  assert (store.addAlias ("dp1a", dp1).isSuccess());
  assert (store.addAlias ("dp1b", dp1).isSuccess());
  assert (store.proxy (123, "") == dp1);

  SG::DataProxy* dp2 = make_proxy (124, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());
  assert (store.proxy (123, "") == dp1);

  SG::DataProxy* dp3 = make_proxy (123, "dp3");
  assert (store.addToStore (123, dp3).isSuccess());
  assert (store.proxy (123, "") == 0);
  assert (store.proxy (123, "dp3") == dp3);
}


void test_typeCount()
{
  std::cout << "test_typeCount\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());

  assert (store.typeCount (123) == 2);
  assert (store.typeCount (124) == 1);
  assert (store.typeCount (125) == 0);
}


void test_tRange()
{
  std::cout << "test_tRange\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());

  SG::DataStore::ConstStoreIterator tbeg;
  SG::DataStore::ConstStoreIterator tend;
  assert (store.tRange (tbeg, tend).isSuccess());

  assert (tbeg->first == 123);
  assert (tbeg->second.size() == 2);
  ++tbeg;
  assert (tbeg->first == 124);
  assert (tbeg->second.size() == 1);
  ++tbeg;
  assert (tbeg == tend);
}


void test_pRange()
{
  std::cout << "test_pRange\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());

  SG::ConstProxyIterator pbeg;
  SG::ConstProxyIterator pend;

  assert (store.pRange (123, pbeg, pend).isSuccess());
  assert (pbeg->first == "dp1");
  assert (pbeg->second == dp1);
  ++pbeg;
  assert (pbeg->first == "dp2");
  assert (pbeg->second == dp2);
  ++pbeg;
  assert (pbeg == pend);

  assert (store.pRange (124, pbeg, pend).isSuccess());
  assert (pbeg->first == "dp1");
  assert (pbeg->second == dp1);
  ++pbeg;
  assert (pbeg == pend);

  assert (store.pRange (125, pbeg, pend).isFailure());
  assert (pbeg == pend);
}


void test_proxyList()
{
  std::cout << "test_proxyList\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());

  std::list<SG::DataProxy*> pl;
  assert (store.proxyList (pl).isSuccess());
  assert (pl.size() == 3);

  std::vector<SG::DataProxy*> pv (pl.begin(), pl.end());
  assert (pv[0] == dp1);
  assert (pv[1] == dp2);
  assert (pv[2] == dp1);
}


void test_keys()
{
  std::cout << "test_keys\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());

  std::vector<std::string> v;
  store.keys (123, v, true, false);
  assert (v.size() == 2);
  assert (v[0] == "dp1");
  assert (v[1] == "dp2");

  store.keys (124, v, true, false);
  assert (v.size() == 1);
  assert (v[0] == "dp1");

  assert (store.addAlias ("dp1a", dp1).isSuccess());
  store.keys (123, v, false, false);
  assert (v.size() == 2);
  assert (v[0] == "dp1");
  assert (v[1] == "dp2");

  store.keys (123, v, true, false);
  assert (v.size() == 3);
  assert (v[0] == "dp1");
  assert (v[1] == "dp1a");
  assert (v[2] == "dp2");

  SG::DataProxy* dp3 = make_proxy (125, "dp3");
  SG::DataProxy* dp4 = make_proxy (125, "dp4");
  assert (store.addToStore (125, dp3).isSuccess());
  assert (store.addToStore (125, dp4).isSuccess());
  TestProvider prov;
  dp4->transientAddress()->setProvider (&prov, StoreID::SPARE_STORE);

  store.keys (125, v, true, false);
  assert (v.size() == 2);
  assert (v[0] == "dp3");
  assert (v[1] == "dp4");

  store.keys (125, v, true, true);
  assert (v.size() == 1);
  assert (v[0] == "dp4");
}


void test_removeProxy()
{
  std::cout << "test_removeProxy\n";

  TestPool pool;
  SG::DataStore store (pool);

  assert (store.removeProxy (0, false).isFailure());

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  dp1->addRef();
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());
  assert (store.addAlias ("dp1x", dp1).isSuccess());

  assert (store.proxy_exact (pool.stringToKey ("dp1", 123)) == dp1);
  assert (store.proxy_exact (pool.stringToKey ("dp1", 124)) == dp1);
  assert (store.proxy_exact (pool.stringToKey ("dp1x", 123)) == dp1);
  assert (store.proxy (123, "dp1") == dp1);
  assert (store.proxy (124, "dp1") == dp1);
  assert (store.proxy (123, "dp1x") == dp1);

  assert (dp1->refCount() == 4);

  dp1->resetOnly (true);
  assert (store.removeProxy (dp1, false).isSuccess());
  assert (dp1->refCount() == 4);

  dp1->resetOnly (false);
  assert (store.removeProxy (dp1, false).isSuccess());
  assert (dp1->refCount() == 1);
  assert (store.proxy_exact (pool.stringToKey ("dp1", 123)) == 0);
  assert (store.proxy_exact (pool.stringToKey ("dp1", 124)) == 0);
  assert (store.proxy_exact (pool.stringToKey ("dp1x", 123)) == 0);
  assert (store.proxy (123, "dp1") == 0);
  assert (store.proxy (124, "dp1") == 0);
  assert (store.proxy (123, "dp1x") == 0);

}


void test_clearStore()
{
  std::cout << "test_clearStore\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  DataObject* o1 = new DataObject;
  dp1->setObject (o1);
  dp1->addRef();
  assert (store.addToStore (123, dp1).isSuccess());
  assert (store.addSymLink (124, dp1).isSuccess());
  dp1->resetOnly (false);

  SG::DataProxy* dp2 = make_proxy (123, "dp2");
  assert (store.addToStore (123, dp2).isSuccess());
  DataObject* o2 = new DataObject;
  dp2->setObject (o2);
  dp2->addRef();

  assert (dp1->refCount() == 3);
  assert (dp2->refCount() == 2);

  int trans1;
  dp1->registerTransient (&trans1);
  assert (store.locatePersistent (&trans1) == dp1);
  int trans2;
  dp2->registerTransient (&trans2);
  assert (store.locatePersistent (&trans2) == dp2);

  store.clearStore();

  assert (store.locatePersistent (&trans1) == 0);
  assert (store.locatePersistent (&trans2) == 0);

  assert (dp1->refCount() == 1);
  assert (dp1->object() == o1);
  assert (store.proxy (123, "dp1") == 0);
  assert (store.proxy (124, "dp1") == 0);
  assert (store.proxy_exact (pool.stringToKey ("dp1", 123)) == 0);
  assert (store.proxy_exact (pool.stringToKey ("dp1", 124)) == 0);

  assert (dp2->refCount() == 2);
  assert (dp2->object() == 0);
  assert (store.proxy (123, "dp2") == dp2);
  assert (store.proxy_exact (pool.stringToKey ("dp2", 123)) == dp2);

  store.clearStore(true);
  assert (dp2->refCount() == 1);
  assert (store.proxy (123, "dp2") == 0);
  assert (store.proxy_exact (pool.stringToKey ("dp2", 123)) == 0);
}


void test_t2p()
{
  std::cout << "test_t2p\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::DataProxy* dp1 = make_proxy (123, "dp1");
  assert (store.addToStore (123, dp1).isSuccess());

  int trans1;
  assert (store.locatePersistent (&trans1) == 0);
  assert (store.t2pRegister (&trans1, dp1).isSuccess());
  assert (store.locatePersistent (&trans1) == dp1);
  assert (store.locatePersistent (&trans1) == dp1);
  assert (store.t2pRegister (&trans1, dp1).isFailure());
  store.t2pRemove (&trans1);
  assert (store.locatePersistent (&trans1) == 0);
}


void test_dummy()
{
  std::cout << "test_dummy\n";

  TestPool pool;
  SG::DataStore store (pool);

  SG::StringPool::sgkey_t sgkey = pool.stringToKey ("dp1", 456);

  SG::DataProxy* dp1 = make_proxy (0, "");
  dp1->transientAddress()->setSGKey (sgkey);
  assert (store.addToStore (0, dp1).isSuccess());
  assert (dp1->refCount() == 1);
  assert (store.proxy_exact (sgkey) == dp1);

  assert (store.proxy (456, "dp1") == dp1);
  assert (dp1->clID() == 456);
  assert (dp1->name() == "dp1");
  assert (dp1->refCount() == 1);

  SG::StringPool::sgkey_t sgkey2 = pool.stringToKey ("dp2", 456);
  SG::DataProxy* dp2 = make_proxy (0, "");
  dp2->transientAddress()->setSGKey (sgkey2);
  assert (store.addToStore (0, dp2).isSuccess());
  assert (dp2->refCount() == 1);
  assert (store.proxy_exact (sgkey2) == dp2);

  assert (store.proxy (456, "dp2") == dp2);
  assert (dp2->clID() == 456);
  assert (dp2->name() == "dp2");
  assert (dp2->refCount() == 1);
}


int main()
{
  Athena::getMessageSvcQuiet = true;
  test_ctor();
  test_addToStore();
  test_addAlias();
  test_addSymLink();
  test_proxy_exact();
  test_proxy();
  test_typeCount();
  test_tRange();
  test_pRange();
  test_proxyList();
  test_keys();
  test_removeProxy();
  test_clearStore();
  test_t2p();
  // pac routines not tested.
  test_dummy();
  return 0;
}


// handle retrieval of dummied dp
// insert dummied dp
