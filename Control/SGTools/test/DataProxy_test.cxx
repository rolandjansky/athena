/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataProxy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Regression tests for DataProxy.
 */

#undef NDEBUG
#include "SGTools/DataProxy.h"
#include "SGTools/IProxyDictWithPool.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/CLASS_DEF.h"
#include "AthenaKernel/ILockable.h"
#include <iostream>
#include <cstdlib>
#include <cassert>


struct X1
{
  ~X1() {}
};
CLASS_DEF(X1, 8011, 1)

class XLock : public ILockable
{
public:
  XLock() : m_locked (false) {}
  void lock() { m_locked = true; std::cout << "lock\n"; }
  bool m_locked;
};
CLASS_DEF(XLock, 8114, 1)

class TestStore
  : public IProxyDictWithPool
{
public:
  unsigned long addRef()
  { std::cout << "addRef\n"; std::abort(); }
  unsigned long release()
  { std::cout << "release\n"; std::abort(); }
  StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/)
  { std::cout << "queryInterface\n"; std::abort(); }
  const std::string& name() const
  { std::cout << "name\n"; std::abort(); }


  SG::DataProxy* deep_proxy(const void* const /*pTransient*/) const
  { std::cout << "deep_proxy\n"; std::abort(); }
  SG::DataProxy* proxy(const void* const /*pTransient*/) const
  { std::cout << "proxy1\n"; std::abort(); }
  SG::DataProxy* proxy(const CLID& /*id*/) const
  { std::cout << "proxy2\n"; std::abort(); }
  SG::DataProxy* proxy(const CLID& /*id*/, const std::string& /*key*/) const
  { std::cout << "proxy3\n"; std::abort(); }
  SG::DataProxy* proxy_exact (SG::sgkey_t /*sgkey*/) const
  { std::cout << "proxy_exact1\n"; std::abort(); }
  std::vector<const SG::DataProxy*> proxies() const
  { std::cout << "proxies\n"; std::abort(); }
  StatusCode addToStore (CLID /*id*/, SG::DataProxy* /*proxy*/)
  { std::cout << "addToStore\n";  std::abort(); }

  sgkey_t stringToKey (const std::string& /*str*/, CLID /*clid*/)
  { std::cout << "stringToKey\n"; std::abort(); }
  const std::string* keyToString (sgkey_t /*key*/) const
  { std::cout << "keyToString1\n"; std::abort(); }
  const std::string* keyToString (sgkey_t /*key*/, CLID& /*clid*/) const
  { std::cout << "keyToString2\n"; std::abort(); }
  void registerKey (sgkey_t /*key*/, const std::string& /*str*/, CLID /*clid*/)
  { std::cout << "registerKey\n"; std::abort(); }
};


void test1()
{
  std::cout << "test1\n";
  TestStore store;

  SG::DataProxy dp;
  assert (dp.store() == 0);
  dp.setStore (&store);
  assert (dp.store() == &store);
}


// Test setConst()
void test2()
{
  std::cout << "test2\n";

  X1* x1 = new X1;
  DataObject* b1 = SG::asStorable (x1);
  SG::DataProxy dp1;
  dp1.setObject (b1);
  assert (!dp1.isConst());
  dp1.setConst();
  assert (dp1.isConst());

  XLock* xlock = new XLock;
  DataObject* b2 = SG::asStorable (xlock);
  SG::DataProxy dp2;
  dp2.setObject (b2);
  assert (!dp2.isConst());
  assert (!xlock->m_locked);
  dp2.setConst();
  assert (dp2.isConst());
  assert (xlock->m_locked);

  XLock* xlock_a = new XLock;
  DataObject* b2a = SG::asStorable (xlock_a);
  assert (!xlock_a->m_locked);
  dp2.setObject (b2a);
  assert (xlock_a->m_locked);
  assert (dp2.isConst());
}


int main()
{
  test1();
  test2();

  // FIXME: INCOMPLETE!

  return 0;
}


