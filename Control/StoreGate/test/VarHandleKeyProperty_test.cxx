/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/VarHandleKeyProperty_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for VarHandleKeyProperty.
 */


#undef NDEBUG
#include "StoreGate/VarHandleKeyProperty.h"
#include "StoreGate/exceptions.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "GaudiKernel/PropertyHolder.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include <cassert>
#include <iostream>
#include <sstream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)
CONDCONT_DEF (MyObj, 293847296);

namespace
{
  const std::string emptyName{};
  /// Helper to allow instantiation of PropertyHolder.
  struct AnonymousPropertyHolder : public PropertyHolder<implements<IProperty, INamedInterface>> {
    const std::string& name() const override { return emptyName; }
  };
}


void test1()
{
  std::cout << "test1\n";

  SG::VarHandleKey k (1234, "", Gaudi::DataHandle::Reader);
  assert (Gaudi::Parsers::parse (k, "aaa").isSuccess());
  assert (k.key() == "aaa");
  assert (k.storeHandle().name() == "StoreGateSvc");

  std::ostringstream ss;
  Gaudi::Utils::toStream (k, ss);
  assert (ss.str() == "'StoreGateSvc+aaa'");

  assert (Gaudi::Parsers::parse (k, "FooSvc+bbb").isSuccess());
  assert (k.key() == "bbb");
  assert (k.storeHandle().name() == "FooSvc");

  assert (Gaudi::Parsers::parse (k, "BarSvc+c/bbb").isFailure());
}


void test2()
{
  std::cout << "test2\n";

  SG::VarHandleKey k (1234, "", Gaudi::DataHandle::Reader);
  SG::VarHandleKeyProperty p ("prop", k);

  assert (p.fromString ("aaa").isSuccess());
  assert (k.key() == "aaa");
  assert (k.storeHandle().name() == "StoreGateSvc");

  assert (p.fromString ("FooSvc+bbb").isSuccess());
  assert (k.key() == "bbb");
  assert (k.storeHandle().name() == "FooSvc");

  assert (p.fromString ("FooSvc+c/bbb").isFailure());

  SG::VarHandleKey k2 (1234, "ccc", Gaudi::DataHandle::Reader, "BarSvc");
  p.setValue (k2);
  assert (k.clid() == 1234);
  assert (k.key() == "ccc");
  assert (k.storeHandle().name() == "BarSvc");

  assert (p.toString() == "'BarSvc+ccc'");
  std::ostringstream ss;
  p.toStream (ss);
  assert (ss.str() == "'BarSvc+ccc'");

  std::unique_ptr<SG::VarHandleKeyProperty> p2 (p.clone());
  assert (p2->value().clid() == 1234);
  assert (p2->value().key() == "ccc");
  assert (p2->value().storeHandle().name() == "BarSvc");

  SG::VarHandleKey k3 (1234, "ddd", Gaudi::DataHandle::Reader, "BazSvc");
  *p2 = k3;
  assert (p2->value().clid() == 1234);
  assert (p2->value().key() == "ddd");
  assert (p2->value().storeHandle().name() == "BazSvc");
  assert (k.clid() == 1234);
  assert (k.key() == "ddd");
  assert (k.storeHandle().name() == "BazSvc");

  SG::VarHandleKey k4 (1234, "", Gaudi::DataHandle::Reader);
  SG::VarHandleKeyProperty p4 ("prop4", k4);
  assert (p.load (p4));
  assert (k4.clid() == 1234);
  assert (k4.key() == "ddd");
  assert (k4.storeHandle().name() == "BazSvc");

  p.setValue (k2);
  assert (p4.assign (p));
  assert (k4.clid() == 1234);
  assert (k4.key() == "ccc");
  assert (k4.storeHandle().name() == "BarSvc");
}


void test3()
{
  std::cout << "test3\n";

  SG::VarHandleKey k1 (1234, "", Gaudi::DataHandle::Reader);
  SimplePropertyRef<SG::VarHandleKey > p1 ("p1", k1);
  assert (p1.fromString ("FeeSvc+aaa").isSuccess());
  assert (k1.clid() == 1234);
  assert (k1.key() == "aaa");
  assert (k1.storeHandle().name() == "FeeSvc");

  SG::ReadHandleKey<MyObj> k2;
  SimplePropertyRef<SG::ReadHandleKey<MyObj> > p2 ("p2", k2);
  assert (p2.fromString ("FooSvc+bbb").isSuccess());
  assert (k2.clid() == 293847295);
  assert (k2.key() == "bbb");
  assert (k2.storeHandle().name() == "FooSvc");

  SG::WriteHandleKey<MyObj> k3;
  SimplePropertyRef<SG::WriteHandleKey<MyObj> > p3 ("p3", k3);
  assert (p3.fromString ("BarSvc+ccc").isSuccess());
  assert (k3.clid() == 293847295);
  assert (k3.key() == "ccc");
  assert (k3.storeHandle().name() == "BarSvc");

  SG::UpdateHandleKey<MyObj> k4;
  SimplePropertyRef<SG::UpdateHandleKey<MyObj> > p4 ("p4", k4);
  assert (p4.fromString ("BazSvc+ddd").isSuccess());
  assert (k4.clid() == 293847295);
  assert (k4.key() == "ddd");
  assert (k4.storeHandle().name() == "BazSvc");

  SG::ReadDecorHandleKey<MyObj> k5;
  SimplePropertyRef<SG::ReadDecorHandleKey<MyObj> > p5 ("p5.zzz", k5);
  assert (p5.fromString ("FooSvc+bbb.zzz").isSuccess());
  assert (k5.clid() == 293847295);
  assert (k5.key() == "bbb.zzz");
  assert (k5.storeHandle().name() == "FooSvc");

  SG::WriteDecorHandleKey<MyObj> k6;
  SimplePropertyRef<SG::WriteDecorHandleKey<MyObj> > p6 ("p6.zzz", k6);
  assert (p6.fromString ("FooSvc+bbb.zzz").isSuccess());
  assert (k6.clid() == 293847295);
  assert (k6.key() == "bbb.zzz");
  assert (k6.storeHandle().name() == "FooSvc");

  SG::ReadCondHandleKey<MyObj> k7 ("p7");
  SimplePropertyRef<SG::ReadCondHandleKey<MyObj> > p7 ("p7", k7);
  assert (p7.fromString ("FooSvc+bbb").isSuccess());
  assert (k7.clid() == 293847295);
  assert (k7.key() == "bbb");
  assert (k7.storeHandle().name() == "FooSvc");

  SG::WriteCondHandleKey<MyObj> k8 ("p8", "dbkey");
  SimplePropertyRef<SG::WriteCondHandleKey<MyObj> > p8 ("p8", k8);
  assert (p8.fromString ("FooSvc+bbb").isSuccess());
  assert (k8.clid() == 293847295);
  assert (k8.key() == "bbb");
  assert (k8.storeHandle().name() == "FooSvc");
}


class PropTest
  : public IProperty
{
public:
  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual unsigned long refCount() const override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override
  { std::abort(); }

  virtual StatusCode setProperty( const Property& p ) override
  { return mgr.setProperty(p); }
  virtual StatusCode setProperty( const std::string& s ) override
  { return mgr.setProperty(s); }
  virtual StatusCode setProperty( const std::string& n, const std::string& v ) override
  { return mgr.setProperty(n, v); }
  virtual StatusCode getProperty( Property* p ) const override
  { return mgr.getProperty (p); }
  virtual const Property& getProperty( const std::string& name) const override
  { return mgr.getProperty (name); }
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const override
  { return mgr.getProperty (n, v); }
  virtual const std::vector<Property*>& getProperties( ) const override
  { return mgr.getProperties(); }
  virtual bool hasProperty(const std::string& name) const override
  { return mgr.hasProperty(name); }

  AnonymousPropertyHolder mgr;
};


void test4()
{
  std::cout << "test4\n";

  PropTest ptest;

  SG::ReadHandleKey<MyObj> k1;
  ptest.mgr.declareProperty ("k1", k1);
  SG::WriteHandleKey<MyObj> k2;
  ptest.mgr.declareProperty ("k2", k2);
  SG::UpdateHandleKey<MyObj> k3;
  ptest.mgr.declareProperty ("k3", k3);
  SG::VarHandleKey k4 (1234, "", Gaudi::DataHandle::Reader);
  ptest.mgr.declareProperty ("k4", k4);

  ServiceHandle<IJobOptionsSvc> jo ("JobOptionsSvc", "test");
  assert (jo.retrieve().isSuccess());
  assert (jo->setMyProperties ("test4", &ptest).isSuccess());

  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa");
  assert (k1.storeHandle().name() == "FooSvc");

  assert (k2.clid() == 293847295);
  assert (k2.key() == "bbb");
  assert (k2.storeHandle().name() == "StoreGateSvc");

  assert (k3.clid() == 293847295);
  assert (k3.key() == "ccc");
  assert (k3.storeHandle().name() == "BarSvc");

  assert (k4.clid() == 1234);
  assert (k4.key() == "ddd");
  assert (k4.storeHandle().name() == "BazSvc");
}


void test5()
{
  std::cout << "test5\n";

  PropTest ptest;

  SG::ReadHandleKey<MyObj> k1;
  ptest.mgr.declareProperty ("k1", k1);

  ServiceHandle<IJobOptionsSvc> jo ("JobOptionsSvc", "test");
  assert (jo.retrieve().isSuccess());
  assert (jo->setMyProperties ("test5", &ptest).isFailure());
}


int main()
{
  ISvcLocator* pDum;
  if (!Athena_test::initGaudi("VarHandleKeyProperty_test.txt", pDum)) {
    return 1;
  }

  test1();
  test2();
  test3();
  test4();
  test5();
}
