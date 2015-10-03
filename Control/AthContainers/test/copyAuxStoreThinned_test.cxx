/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/copyAuxStoreThinned_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Regression tests for copyAuxStoreThinned
 */


#undef NDEBUG

#ifndef XAOD_STANDALONE

#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/foreach.h"
#include <vector>
#include <iostream>
#include <cassert>


#include "TestThinningSvc.icc"


class AuxStoreTest
  : public SG::AuxStoreInternal
{
public:
  AuxStoreTest() {}
  void suppress (SG::auxid_t auxid)
  { m_suppressed.insert (auxid); }

  virtual const SG::auxid_set_t& getSelectedAuxIDs() const
  {
    m_selected = getAuxIDs();
    ATHCONTAINERS_FOREACH(SG::auxid_t auxid, m_suppressed)
      m_selected.erase (auxid);
    return m_selected;
  }

private:
  SG::auxid_set_t m_suppressed;
  mutable SG::auxid_set_t m_selected;
};


void compare (const SG::AuxStoreInternal& a,
              const SG::AuxStoreInternal& b,
              bool thinned = false,
              SG::auxid_t suppressed = SG::null_auxid)
{
  if (thinned) {
    assert (a.size()/2 == b.size());
  }
  else {
    assert (a.size() == b.size());
  }

  const SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

  ATHCONTAINERS_FOREACH(SG::auxid_t id, a.getAuxIDs()) {
    if (id == suppressed) {
      assert (b.getAuxIDs().count(id) == 0);
      continue;
    }
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


void test1()
{
  std::cout << "test1\n";
  TestThinningSvc svc;

  AuxStoreTest src;
  SG::AuxStoreInternal dst;

  copyAuxStoreThinned (src, dst, 0);
  compare (src, dst);

  copyAuxStoreThinned (src, dst, &svc);
  compare (src, dst);

  SG::auxid_t ityp = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  SG::auxid_t ftyp = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");

  int* iptr = reinterpret_cast<int*> (src.getData (ityp, 10, 10));
  float* fptr = reinterpret_cast<float*> (src.getData (ftyp, 10, 10));
  for (int i=0; i < 10; i++) {
    iptr[i] = i;
    fptr[i] = 10*i;
  }

  SG::AuxStoreInternal src2;
  svc.remap (&src2, 1, 2);

  copyAuxStoreThinned (src, dst, &svc);
  compare (src, dst);

  for (int i=0, i1=0; i < 10; ++i) {
    if (i%2 == 0) {
      svc.remap (&src, i, i1);
      ++i1;
    }
    else {
      svc.remap (&src, i, IThinningSvc::RemovedIdx);
    }
  }

  copyAuxStoreThinned (src, dst, &svc);
  compare (src, dst, true);

  SG::AuxStoreInternal dst2;
  src.suppress (ftyp);
  copyAuxStoreThinned (src, dst2, &svc);
  compare (src, dst2, true, ftyp);
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
