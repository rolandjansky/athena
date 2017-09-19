/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGComps/test/AddressRemappingSvc_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2016
 * @brief Regression tests for AddressRemappingSvc
 */

// FIXME: only renaming is tested.

#undef NDEBUG
#include "SGTools/CLASS_DEF.h"
#include "SGTools/ClassID_traits.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/StringPool.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IInputRename.h"
#include "AthenaKernel/IAddressProvider.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include <iostream>
#include <cassert>


namespace AddressRemappingSvc_test {
  class Foo {};
}
CLASS_DEF (AddressRemappingSvc_test::Foo, 874546632, 0)

using AddressRemappingSvc_test::Foo;

class TestAddress
  : public IOpaqueAddress
{
public:
  virtual unsigned long        addRef     () override { return ++m_ref; }
  virtual unsigned long        release    () override { return --m_ref; }
  virtual const CLID&          clID       () const override { std::abort(); }
  virtual long                 svcType    () const override { std::abort(); }
  virtual IRegistry*           registry   () const override { std::abort(); }
  virtual void                 setRegistry(IRegistry* /*r*/) override { std::abort(); }
  virtual const std::string*   par        () const override { std::abort(); }
  virtual const unsigned long* ipar       () const override { std::abort(); }

private:
  int m_ref = 0;
};


struct Addrs
{
  TestAddress addr1;
  TestAddress addr2;
  TestAddress addr3;
};


void fillTADList (IAddressProvider::tadList& tads,
                  Addrs& addrs)
{
  CLID fooclid = ClassID_traits<Foo>::ID();

  {
    auto tad = std::make_unique<SG::TransientAddress>
      (fooclid, "foo1", &addrs.addr1, true);
    tads.push_back (tad.release());
  }

  {
    auto tad = std::make_unique<SG::TransientAddress>
      (fooclid, "foo2", &addrs.addr2, false);
    tad->setTransientID (321);
    tad->setAlias ("foo2.d1");
    tad->setAlias ("foo2.d2");
    tad->setAlias ("foo2.d3");
    tads.push_back (tad.release());
  }

  {
    auto tad = std::make_unique<SG::TransientAddress>
      (fooclid, "foo3", &addrs.addr3, false);
    tad->setAlias ("foo3.d1");
    tad->setAlias ("foo3.d2");
    tads.push_back (tad.release());
  }
}


void checkTADList (const IAddressProvider::tadList& tads,
                   const Addrs& addrs)
{
  CLID fooclid = ClassID_traits<Foo>::ID();

  assert (tads.size() == 3);
  size_t i = 0;
  for (const SG::TransientAddress* tad : tads) {
    if (i == 0) {
      assert (tad->clID() == fooclid);
      assert (tad->name() == "bar1");
      assert (tad->address() == &addrs.addr1);
      assert (tad->clearAddress() == true);
      assert (tad->transientID() ==
              SG::TransientAddress::TransientClidSet { fooclid });
      assert (tad->alias().empty());
    }
    else if (i == 1) {
      assert (tad->clID() == fooclid);
      assert (tad->name() == "bar2");
      assert (tad->address() == &addrs.addr2);
      assert (tad->clearAddress() == false);
      assert (tad->transientID() ==
              (SG::TransientAddress::TransientClidSet { 321, fooclid }));
      assert (tad->alias() ==
              (SG::TransientAddress::TransientAliasSet { "bar2.d1",
                                                         "bar2.d2",
                                                         "bar2.x3" }));
    }
    else if (i == 2) {
      assert (tad->clID() == fooclid);
      assert (tad->name() == "foo3");
      assert (tad->address() == &addrs.addr3);
      assert (tad->clearAddress() == false);
      assert (tad->transientID() ==
              SG::TransientAddress::TransientClidSet { fooclid });
      assert (tad->alias() ==
              (SG::TransientAddress::TransientAliasSet { "foo3.x1",
                                                         "foo3.d2" }));
    }
    ++i;
  }
}


void test1 (Athena::IInputRename& svc,
            IAddressProvider& prov)
{
  std::cout << "test1\n";

  Addrs addrs;

  {
    IAddressProvider::tadList tads;
    fillTADList (tads, addrs);
    assert( prov.preLoadAddresses (StoreID::EVENT_STORE, tads).isSuccess() );
    checkTADList (tads, addrs);
  }
  {
    IAddressProvider::tadList tads;
    fillTADList (tads, addrs);
    assert( prov.loadAddresses (StoreID::EVENT_STORE, tads).isSuccess() );
    checkTADList (tads, addrs);
  }

  Athena::RCURead<Athena::IInputRename::InputRenameMap_t> r
    (*svc.inputRenameMap());
  assert (r->size() == 5);

  CLID fooclid = ClassID_traits<Foo>::ID();
  SG::StringPool sp;
  Athena::IInputRename::InputRenameMap_t::const_iterator it;

  it = r->find (sp.stringToKey ("foo1", fooclid));
  assert (it != r->end());
  assert (it->second == sp.stringToKey ("bar1", fooclid));

  it = r->find (sp.stringToKey ("foo2", fooclid));
  assert (it != r->end());
  assert (it->second == sp.stringToKey ("bar2", fooclid));

  it = r->find (sp.stringToKey ("foo2", 321));
  assert (it != r->end());
  assert (it->second == sp.stringToKey ("bar2", 321));

  it = r->find (sp.stringToKey ("foo3.d1", fooclid));
  assert (it != r->end());
  assert (it->second == sp.stringToKey ("foo3.x1", fooclid));

  it = r->find (sp.stringToKey ("foo2.d3", fooclid));
  assert (it != r->end());
  assert (it->second == sp.stringToKey ("bar2.x3", fooclid));
}


// Test ARS inputRenameMap() with no renaming.
void test2()
{
  std::cout << "test2\n";
  
  ServiceHandle<Athena::IInputRename> svc ("ARSTest2", "test");
  if (svc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve service." << std::endl;
  }
  assert (svc->inputRenameMap() == nullptr);
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc = 0;
  if (!Athena_test::initGaudi("AddressRemappingSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }  
  assert(svcloc);

  ServiceHandle<Athena::IInputRename> svc ("AddressRemappingSvc", "test");
  if (svc.retrieve().isFailure()) {
    std::cerr << "Can't retrieve service." << std::endl;
    return 1;
  }

  IAddressProvider* prov = dynamic_cast<IAddressProvider*>(&*svc);
  if (!prov) std::abort();

  test1(*svc, *prov);
  test2();
  return 0;
}
