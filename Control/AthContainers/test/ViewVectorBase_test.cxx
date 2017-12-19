/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/ViewVectorBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Tests for ViewVectorBase.
 */

// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "AthContainers/ViewVectorBase.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "SGTools/TestStore.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cassert>


class X
  : public SG::AuxElement
{
public:
  X(int x) : m_x(x) {}
  int m_x;
};
static const CLID x_clid = 8324723;
CLASS_DEF (DataVector<X>, x_clid, 1)


class Y {};


class ViewVectorBaseTest
  : public SG::ViewVectorBase
{
public:
  virtual void toPersistent() override {}
  virtual void toTransient() override {}

  void testToPersistent (DataVector<X>& dv)
  {
    doToPersistent (dv);
  }

  template <class DV>
  void testToTransient (DV& dv)
  {
    doToTransient (dv);
  }

  void checkELV (unsigned int);

  void checkClear()
  {
    clearPersistent();
    assert (m_persKey.size() == 0);
    assert (m_persIndex.size() == 0);
  }
};



void ViewVectorBaseTest::checkELV (unsigned int sgkey)
{
  assert (m_persKey.size() == 10);
  assert (m_persIndex.size() == 10);
  for (unsigned int i=0; i < 10; i++) {
    assert (m_persKey[i] == sgkey);
    assert (m_persIndex[i] == 9-i);
  }
}


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";
  ViewVectorBaseTest vvb1;

  DataVector<X>* dv = new DataVector<X>;
  for (int i = 0; i < 10; i++)
    dv->push_back (new X(i));
  store.record (dv, "dv");
  unsigned int sgkey = store.stringToKey ("dv", x_clid);

  DataVector<X> vv (SG::VIEW_ELEMENTS);
  for (int i = 0; i < 10; i++)
    vv.push_back ((*dv)[9-i]);

  vvb1.testToPersistent (vv);
  vvb1.checkELV (sgkey);
  assert (vv.size() == 10);

  vvb1.setClearOnPersistent();
  vvb1.testToPersistent (vv);
  vvb1.checkELV (sgkey);
  assert (vv.size() == 0);

  vvb1.testToTransient (vv);
  assert (vv.size() == 10);
  for (int i = 0; i < 10; i++)
    assert (vv[i] == (*dv)[9-i]);

  
  vvb1.checkELV (sgkey);
  vvb1.checkClear();

  for (int i = 0; i < 10; i++)
    store.remap (sgkey, 12345, i, i);
  vvb1.testToPersistent (vv);
  vvb1.checkELV (12345);

  DataVector<Y> vy (SG::VIEW_ELEMENTS);
  vvb1.testToTransient (vy);
  assert (vy.empty());
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SGTest::initTestStore();
  test1 (SGTest::store);
  return 0;
}

#else

int main()
{
  return 0;
}

#endif
