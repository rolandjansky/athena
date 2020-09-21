/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/MetaContDataBucket_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2018
 * @brief Regression tests for MetaContDataBucket.
 */

#undef NDEBUG
#include "AthenaKernel/MetaContDataBucket.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


class TestProxyDict
  : public IProxyDict
{
public:
  virtual const std::string& name() const override { std::abort(); }
  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface( const InterfaceID&, void** ) override { std::abort(); }

  virtual
  sgkey_t stringToKey (const std::string&, CLID) override { std::abort(); }
  virtual
  const std::string* keyToString (sgkey_t) const override { std::abort(); }
  virtual
  const std::string* keyToString (sgkey_t, CLID&) const override { std::abort(); }
  virtual
  void registerKey (sgkey_t, const std::string&, CLID) override { std::abort(); }

  virtual SG::DataProxy* proxy_exact (SG::sgkey_t) const override { std::abort(); }
  virtual SG::DataProxy* proxy(const CLID&,
                               const std::string&) const override { std::abort(); }
  virtual SG::DataProxy* proxy(const void* const) const override { std::abort(); }
  virtual std::vector<const SG::DataProxy*> proxies() const override { std::abort(); }
  virtual StatusCode addToStore (CLID, SG::DataProxy*) override { std::abort(); }
  virtual SG::DataProxy* recordObject (SG::DataObjectSharedPtr<DataObject>,
                                       const std::string&,
                                       bool,
                                       bool) override { std::abort(); }

  virtual SG::SourceID sourceID() const override { return source; }
  std::string source;
};


class TestPayload
{
public:
  virtual ~TestPayload() {}
};

class TestContBase
{
public:
  virtual ~TestContBase() {}
};

class TestCont
  : public TestContBase
{
public:
  typedef TestPayload Payload_t;
  TestPayload myPayload;

  bool find (const SG::SourceID& sid, TestPayload*& payload)
  {
    if (sid == "source") {
      payload = &myPayload;
      return true;
    }
    return false;
  }
};


CLASS_DEF( TestContBase, 98770271, 1 )
CLASS_DEF( TestCont, 86960140, 1 )
CLASS_DEF( TestPayload, 128629149, 1 )

SG_BASES( TestCont, TestContBase );


void test1()
{
  std::cout << "test1\n";

  EventContext ctx;
  TestProxyDict proxyDict;
  ctx.setExtension ( Atlas::ExtendedEventContext( &proxyDict ) );
  Gaudi::Hive::setCurrentContext (ctx);

  SG::MetaContDataBucket<TestCont> bucket (std::make_unique<TestCont>());

  void* p = nullptr;

  p = bucket.cast (ClassID_traits<TestCont>::ID());
  assert (typeid (*reinterpret_cast<TestCont*>(p)) == typeid (TestCont));
  p = bucket.cast (ClassID_traits<TestContBase>::ID());
  assert (typeid (*reinterpret_cast<TestContBase*>(p)) == typeid (TestCont));
  assert (bucket.cast (ClassID_traits<TestPayload>::ID()) == nullptr);

  p = bucket.cast (typeid (TestCont));
  assert (typeid (*reinterpret_cast<TestCont*>(p)) == typeid (TestCont));
  p = bucket.cast (typeid (TestContBase));
  assert (typeid (*reinterpret_cast<TestContBase*>(p)) == typeid (TestCont));
  assert (bucket.cast (typeid (TestPayload)) == nullptr);

  proxyDict.source = "source";
  p = bucket.cast (ClassID_traits<TestPayload>::ID());
  //assert (typeid (*reinterpret_cast<TestPayload*>(p)) == typeid (TestPayload));
  p = bucket.cast (typeid (TestPayload));
  //assert (typeid (*reinterpret_cast<TestPayload*>(p)) == typeid (TestPayload));
}


int main()
{
  std::cout << "MetaContDataBucket_test\n";    
  test1();
  return 0;
}
