/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/MissingETCompositionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/MissingETCompositionCnv_p2.h"
#include "MissingETEvent/MissingETComposition.h"
#include "SGTools/TestStore.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


void compare (const MissingETComposition& p1,
              const MissingETComposition& p2)
{
  assert (p1.size() == p2.size());
  MissingETComposition::object_iter i1 = p1.begin();
  MissingETComposition::object_iter i1e = p1.end();
  MissingETComposition::object_iter i2 = p2.begin();
  MissingETComposition::object_iter i2e = p2.end();
  while (i1 != i1e && i2 != i2e) {
    assert (i1.getParameter() == i2.getParameter());
    assert (i1.getInternalIterator()->first == i2.getInternalIterator()->first);
    ++i1;
    ++i2;
  }
  assert (i1 == i1e && i2 == i2e);
}


void testit (const MissingETComposition& trans1)
{
  MsgStream log (0, "test");
  MissingETCompositionCnv_p2 cnv;
  MissingETComposition_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  MissingETComposition trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  (void)Gaudi::Hive::currentContext();
  ElementLink<INavigable4MomentumCollection> el1 ("foo", 1);
  ElementLink<INavigable4MomentumCollection> el2 ("bar", 2);
  Athena_test::Leakcheck check;

  MissingETComposition trans1;
  MissingETHandlers::Weight w1 (10.5, 11.5, 12.5, 2);
  trans1.insertElement (el1, w1);
  MissingETHandlers::Weight w2 (20.5, 21.5, 22.5, 3);
  trans1.insertElement (el2, w2);
  testit (trans1);
}


int main()
{
  std::cout << "RecTPCnv/MissingETCompositionCnv_p2\n";
  SGTest::initTestStore();
  test1();
  return 0;
}
