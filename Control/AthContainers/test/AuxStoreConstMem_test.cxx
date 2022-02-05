/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/test/AuxStoreConstMem_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2021
 * @brief Regression tests for AuxStoreConstMem
 */

#undef NDEBUG
#include "AthContainers/AuxStoreConstMem.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/exceptions.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cassert>


struct AAux : public SG::AuxElement
{
  using SG::AuxElement::AuxElement;
  int xint() const
  { static const Accessor<int> acc ("xint");  return acc(*this); }
  bool xintAvailable() const
  { static const Accessor<int> acc ("xint");  return acc.isAvailable(*this); }

  float xfloat() const
  { static const Accessor<float> acc ("xfloat");  return acc(*this); }
  bool xfloatAvailable() const
  { static const Accessor<float> acc ("xfloat");  return acc.isAvailable(*this); }
};


void test1()
{
  std::cout << "test1\n";
  SG::AuxStoreConstMem cm (5);
  assert (cm.size() == 5);
  assert (cm.getData (1) == 0);
  assert (cm.getAuxIDs().size() == 0);

  const int d1[] = {1, 2, 3, 4, 5};
  const double d2[] = {1.1, 2.2, 3.3, 4.4, 5.5};

  cm.addBuffer (1, d1);
  cm.addBuffer (2, d2);

  assert (cm.getData(1) == d1);
  assert (cm.getData(2) == d2);
  assert (cm.getData(3) == nullptr);
  assert (cm.getData(0) == nullptr);

  const SG::auxid_set_t& ids = cm.getAuxIDs();
  assert (ids.size() == 2);
  assert (ids.find (1) != ids.end());
  assert (ids.find (2) != ids.end());

  assert (cm.getDecoration (1, 5, 5) == nullptr);
  assert (cm.getDecoration (2, 5, 5) == nullptr);
  assert (!cm.isDecoration (1));
  assert (!cm.clearDecorations());
  cm.lock();
  cm.lockDecoration(1);
}


void test2()
{
  std::cout << "test2\n";

  const size_t N = 5;

  DataVector<AAux> v;
  for (size_t i = 0; i < N; i++) {
    v.push_back (new AAux);
  }

  const int xint[N] = {1, 2, 3, 4, 5};
  const float xfloat[N] = {1.1, 2.2, 3.3, 4.4, 5.5};

  SG::auxid_t xint_typ = SG::AuxTypeRegistry::instance().getAuxID<int> ("xint");
  SG::auxid_t xfloat_typ = SG::AuxTypeRegistry::instance().getAuxID<float> ("xfloat");

  SG::AuxStoreConstMem cm (N);
  cm.addBuffer (xint_typ, xint);
  cm.addBuffer (xfloat_typ, xfloat);

  assert (!v[0]->xintAvailable());
  assert (!v[0]->xfloatAvailable());

  v.setStore (&cm);

  assert (v[0]->xintAvailable());
  assert (v[0]->xfloatAvailable());
  assert (v[0]->xint() == 1);
  assert (Athena_test::isEqual (v[0]->xfloat(), 1.1));
  assert (v[3]->xint() == 4);
  assert (Athena_test::isEqual (v[3]->xfloat(), 4.4));
}


int main()
{
  std::cout << "AthContainers/AuxStoreConstMem_test\n";
  test1();
  test2();
  return 0;
}
