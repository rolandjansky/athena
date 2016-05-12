/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  SGTools/test/CurrentEventStore_test.cxx
 * @author scott snyder
 * @date Apr, 2015
 * @brief Hold a pointer to the current event store.
 */

#undef NDEBUG


#include "SGTools/CurrentEventStore.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";


  IProxyDict* s1 = reinterpret_cast<IProxyDict*>(0x1000);

  assert (SG::CurrentEventStore::store() == 0);
  assert (SG::CurrentEventStore::setStore(s1) == 0);
  assert (SG::CurrentEventStore::store() == s1);

  {
    IProxyDict* s2 = reinterpret_cast<IProxyDict*>(0x2000);
    SG::CurrentEventStore::Push p (s2);
    assert (SG::CurrentEventStore::store() == s2);
  }
  assert (SG::CurrentEventStore::store() == s1);
}


int main()
{
  test1();
  return 0;
}
