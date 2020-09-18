/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/VarHandleProperty_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for VarHandlerPoperty specializations.
 */


#undef NDEBUG
#include "StoreGate/VarHandleProperty.h"
#include "StoreGate/exceptions.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/PropertyHolder.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ServiceHandle.h"
#include <cassert>
#include <iostream>
#include <sstream>


class MyObj
{
public:
  typedef std::true_type thread_safe;
};
CLASS_DEF (MyObj, 293847295, 1)

namespace
{
  const std::string emptyName{};
  /// Helper to allow instantiation of PropertyHolder.
  struct AnonymousPropertyHolder : public PropertyHolder<implements<IProperty, INamedInterface>> {
    const std::string& name() const override { return emptyName; }
  };
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

  virtual StatusCode setProperty( const std::string& s ) override
  { return mgr.setProperty(s); }
  virtual StatusCode setProperty( const std::string& n, const Gaudi::Details::PropertyBase& p ) override
  { return mgr.setProperty(n, p); }
  virtual StatusCode setPropertyRepr( const std::string& n, const std::string& r ) override
  { return mgr.setPropertyRepr(n,r); }
  virtual StatusCode getProperty( Gaudi::Details::PropertyBase* p ) const override
  { return mgr.getProperty (p); }
  virtual const Gaudi::Details::PropertyBase& getProperty( const std::string& name) const override
  { return mgr.getProperty (name); }
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const override
  { return mgr.getProperty (n, v); }
  virtual const std::vector<Gaudi::Details::PropertyBase*>& getProperties( ) const override
  { return mgr.getProperties(); }
  virtual bool hasProperty(const std::string& name) const override
  { return mgr.hasProperty(name); }

  AnonymousPropertyHolder mgr;
};


void test1()
{
  std::cout << "test1\n";

  PropTest ptest;
  std::vector<Gaudi::Details::PropertyBase*> props;

  SG::ReadHandle<MyObj> k1;
  props.push_back(ptest.mgr.declareProperty ("k1", k1));
  SG::WriteHandle<MyObj> k2;
  props.push_back(ptest.mgr.declareProperty ("k2", k2));
  SG::UpdateHandle<MyObj> k3;
  props.push_back(ptest.mgr.declareProperty ("k3", k3));
  //SG::VarHandleBase k4 (1234, "", Gaudi::DataHandle::Reader);
  //props.push_back(ptest.mgr.declareProperty ("k4", k4));

  ServiceHandle<Gaudi::Interfaces::IOptionsSvc> jo ("JobOptionsSvc", "test");
  assert (jo.retrieve().isSuccess());
  for (Gaudi::Details::PropertyBase* p : props) {
    jo->bind ("test1", p);
  }

  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa");
  assert (k1.storeHandle().name() == "FooSvc");

  assert (k2.clid() == 293847295);
  assert (k2.key() == "bbb");
  assert (k2.storeHandle().name() == "StoreGateSvc");

  assert (k3.clid() == 293847295);
  assert (k3.key() == "ccc");
  assert (k3.storeHandle().name() == "BarSvc");

  //assert (k4.clid() == 1234);
  //assert (k4.key() == "ddd");
  //assert (k4.storeHandle().name() == "BazSvc");
}


int main()
{
  ISvcLocator* pDum;
  if (!Athena_test::initGaudi("StoreGate/VarHandleProperty_test.txt", pDum)) {
    return 1;
  }

  test1();
}
