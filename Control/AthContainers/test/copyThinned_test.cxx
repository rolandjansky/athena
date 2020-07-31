/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthContainers/test/copyThinned_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Regression tests for copyThinned
 */


#undef NDEBUG

#include "AthContainers/tools/copyThinned.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include <vector>
#include <iostream>
#include <cassert>


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
void tryit (CONTAINER& cont, const SG::ThinningDecisionBase* dec,
            bool thinned = false)
{
  SG::ThinningInfo info;
  info.m_decision = dec;
  std::unique_ptr<const CONTAINER> newcont = SG::copyThinned (cont, &info);
  compare (cont, *newcont, thinned);
}


template <class CONTAINER>
void tryitConst (CONTAINER& cont, const SG::ThinningDecisionBase* dec,
                 bool thinned = false)
{
  SG::ThinningInfo info;
  info.m_decision = dec;
  std::unique_ptr<const CONTAINER> newcont = SG::copyThinnedConst (cont, &info);
  compare (cont, *newcont, thinned);
}


void test1()
{
  std::cout << "test1\n";
  SG::ThinningDecisionBase dec;

  SG::AuxStoreInternal store;
  DataVector<int> dv;
  std::vector<int> v;

  tryit (store, static_cast<const SG::ThinningDecisionBase*>(nullptr));
  tryitConst (dv, static_cast<const SG::ThinningDecisionBase*>(nullptr));
  tryit (v, static_cast<const SG::ThinningDecisionBase*>(nullptr));

  tryit (store, &dec);
  tryitConst (dv, &dec);
  tryit (v, &dec);

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

  dec.resize (10);

  SG::AuxStoreInternal store2;
  DataVector<int> dv2;
  std::vector<int> v2;

  tryit (store, &dec);
  tryitConst (dv, &dec);
  tryit (v, &dec);

  for (int i=0; i < 10; ++i) {
    if (i%2 != 0) {
      dec.thin (i);
    }
  }

  dec.buildIndexMap();

  tryit (store, &dec, true);
  tryitConst (dv, &dec, true);
  tryit (v, &dec, true);
}


int main()
{
  test1();
  return 0;
}
