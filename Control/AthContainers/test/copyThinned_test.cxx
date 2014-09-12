/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/copyThinned_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Regression tests for copyThinned
 */


#undef NDEBUG

#ifndef XAOD_STANDALONE

#include "AthContainers/tools/copyThinned.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include <vector>
#include <iostream>
#include <cassert>


#include "TestThinningSvc.icc"


void compare (const SG::AuxStoreInternal& a,
              const SG::AuxStoreInternal& b,
              bool thinned = false)
{
  if (thinned) {
    assert (a.size()/2 == b.size());
  }
  else {
    assert (a.size() == b.size());
  }

  const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

  for (SG::auxid_t id : a.getAuxIDs()) {
    const char* aptr = reinterpret_cast<const char*>(a.getData(id));
    const char* bptr = reinterpret_cast<const char*>(b.getData(id));
    assert (aptr != 0 && bptr != 0);

    size_t sz = reg.getEltSize(id);
    for (size_t ia=0, ib=0; ia < a.size(); ++ia) {
      if (!thinned || ia%2 == 0) {
        assert (memcmp (aptr + ia*sz , bptr + ib*sz, sz) == 0);
        ++ib;
      }
    }
  }
}


void compare (const DataVector<int>& a,
              const DataVector<int>& b,
              bool thinned = false)
{
  if (thinned) {
    assert (a.size()/2 == b.size());
  }
  else {
    assert (a.size() == b.size());
  }

  for (size_t ia=0, ib=0; ia < a.size(); ++ia) {
    if (!thinned || ia%2 == 0) {
      assert (*a[ia] == *b[ib]);
      ++ib;
    }
  }
}


void compare (const std::vector<int>& a,
              const std::vector<int>& b,
              bool /*thinned*/ = false)
{
  assert (a.size() == b.size());
  for (size_t i = 0; i < a.size(); i++)
    assert (a[i] == b[i]);
}


template <class CONTAINER>
void tryit (CONTAINER& cont, IThinningSvc* svc, bool thinned = false)
{
  const CONTAINER* newcont = SG::copyThinned (cont, svc);
  compare (cont, *newcont, thinned);
  delete newcont;
}


void test1()
{
  std::cout << "test1\n";
  TestThinningSvc svc;

  SG::AuxStoreInternal store;
  DataVector<int> dv;
  std::vector<int> v;

  tryit (store, 0);
  tryit (dv, 0);
  tryit (v, 0);

  tryit (store, &svc);
  tryit (dv, &svc);
  tryit (v, &svc);

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");

  int* iptr = reinterpret_cast<int*> (store.getData (ityp, 10, 10));
  float* fptr = reinterpret_cast<float*> (store.getData (ftyp, 10, 10));
  for (int i=0; i < 10; i++) {
    iptr[i] = i;
    fptr[i] = 10*i;

    v.push_back(i);
    dv.push_back (new int(i));
  }

  SG::AuxStoreInternal store2;
  DataVector<int> dv2;
  std::vector<int> v2;

  svc.remap (&store2, 1, 2);
  svc.remap (&dv2, 1, 2);
  svc.remap (&v2, 1, 2);

  tryit (store, &svc);
  tryit (dv, &svc);
  tryit (v, &svc);

  for (int i=0, i1=0; i < 10; ++i) {
    if (i%2 == 0) {
      svc.remap (&store, i, i1);
      svc.remap (&dv, i, i1);
      svc.remap (&v, i, i1);
      ++i1;
    }
    else {
      svc.remap (&store, i, IThinningSvc::RemovedIdx);
      svc.remap (&dv, i, IThinningSvc::RemovedIdx);
      svc.remap (&v, i, IThinningSvc::RemovedIdx);
    }
  }

  tryit (store, &svc, true);
  tryit (dv, &svc, true);
  tryit (v, &svc, true);
}


int main()
{
  test1();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // XAOD_STANDALONE
