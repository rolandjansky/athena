/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TransientAddress_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Regression tests for TransientAddress.
 */

#undef NDEBUG
#include "SGTools/TransientAddress.h"
#include "AthenaKernel/IAddressProvider.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include <iostream>
#include <cstdlib>
#include <cassert>


class TestAddress
  : public IOpaqueAddress
{
public:
  TestAddress (int x) : m_x (x), m_ref(0) {}
  /// Add reference to object
  virtual unsigned long        addRef     ();
  /// release reference to object
  virtual unsigned long        release    ();
  /// Retrieve class information from link
  virtual const CLID&          clID       () const { std::abort(); }
  /// Retrieve service type
  virtual long                 svcType    () const { std::abort(); }
  /// Update branch name
  virtual IRegistry*           registry   () const { std::abort(); }
  /// Update directory pointer
  virtual void                 setRegistry(IRegistry* /*r*/) { std::abort(); }
  /// Retrieve String parameters
  virtual const std::string*   par        () const { std::abort(); }
  /// Access to generic link parameters
  virtual const unsigned long* ipar       () const { std::abort(); }

  int m_x;
  int m_ref;
};


class TestProvider
  : public IAddressProvider
{
public:
  virtual StatusCode updateAddress(StoreID::type /*storeID*/,
				   SG::TransientAddress* /*pTAd*/,
                                   const EventContext& /*ctx*/) override
  { return StatusCode::SUCCESS; }
};


unsigned long TestAddress::addRef()
{
  ++m_ref;
  std::cout << "addRef " << m_x << " " << m_ref << "\n";
  return m_ref;
}


unsigned long TestAddress::release()
{
  --m_ref;
  std::cout << "release " << m_x << " " << m_ref << "\n";
  return m_ref;
}


void test1()
{
  std::cout << "test1\n";

  SG::TransientAddress tad1;
  assert (tad1.clID() == CLID_NULL);
  assert (tad1.name() == "");
  assert (tad1.transientID().size() == 0);
  assert (tad1.alias().size() == 0);
  assert (!tad1.isValid(nullptr));
  assert (tad1.provider() == 0);
  assert (tad1.storeID() == StoreID::UNKNOWN);
  assert (tad1.address() == 0);
  assert (tad1.sgkey() == 0);
  tad1.setID (123, "key");
  assert (tad1.name() == "key");
  assert (tad1.clID() == 123);
  assert (tad1.transientID().size() == 1);
  assert (tad1.alias().size() == 0);

  SG::TransientAddress tad2 (123, "key");
  assert (tad2.name() == "key");
  assert (tad2.clID() == 123);
  assert (tad2.transientID().size() == 1);
  assert (tad2.alias().size() == 0);
  assert (!tad2.isValid(nullptr));
  assert (tad2.provider() == 0);
  assert (tad2.storeID() == StoreID::UNKNOWN);
  assert (tad2.address() == 0);
  assert (tad2.sgkey() == 0);

  tad2.setSGKey (876);
  assert (tad2.sgkey() == 876);

  assert (tad2.transientID (123));
  assert (!tad2.transientID (124));
  tad2.setTransientID (124);
  assert (tad2.transientID (124));
  assert (tad2.transientID().size() == 2);

  tad2.setAlias ("key2");
  assert (tad2.alias().size() == 1);
  std::set<std::string> a;
  a.insert ("key3");
  a.insert ("key4");
  tad2.setAlias (a);
  assert (tad2.alias().size() == 2);
  assert (tad2.alias().count ("key2") == 0);
  assert (tad2.alias().count ("key3") == 1);
  tad2.removeAlias ("key3");
  assert (tad2.alias().size() == 1);
  assert (tad2.alias().count ("key3") == 0);

  TestAddress ad1(1);
  SG::TransientAddress tad3 (123, "key", &ad1);
  assert (tad3.name() == "key");
  assert (tad3.clID() == 123);
  assert (tad3.isValid(nullptr));
  assert (tad3.provider() == 0);
  assert (tad3.storeID() == StoreID::UNKNOWN);
  assert (tad3.address() == &ad1);

  TestAddress ad2(2);
  tad3.setAddress (&ad2);
  assert (tad3.isValid(nullptr));
  assert (tad3.address() == &ad2);
  tad3.reset();
  assert (!tad3.isValid(nullptr));
  assert (tad3.address() == 0);

  tad3.setAddress (&ad2);
  tad3.clearAddress (false);
  tad3.reset();
  assert (tad3.isValid(nullptr));
  assert (tad3.address() == &ad2);
  tad3.clearAddress (true);
  tad3.reset();
  assert (!tad3.isValid(nullptr));
  assert (tad3.address() == 0);

  TestProvider tp;
  tad3.setProvider (&tp, StoreID::SPARE_STORE);
  assert (tad3.provider() == &tp);
  assert (tad3.storeID() == StoreID::SPARE_STORE);
  assert (tad3.isValid(nullptr));
  tad3.consultProvider (false);
  assert (!tad3.isValid(nullptr));
  tad3.consultProvider (true);
  assert (tad3.isValid(nullptr));

  SG::TransientAddress tad4(0, "");
  assert (tad4.clID() == CLID_NULL);
  assert (tad4.name() == "");
  assert (tad4.transientID().size() == 0);
  tad4.setID (123, "key");
  assert (tad4.name() == "key");
  assert (tad4.clID() == 123);
  assert (tad4.transientID().size() == 1);
  assert (tad4.alias().size() == 0);
}


int main()
{
  test1();
  return 0;
}
