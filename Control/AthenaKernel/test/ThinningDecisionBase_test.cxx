/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ThinningDecisionBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Regression tests for ThinningDecisionBase.
 */

#undef NDEBUG

#include "AthenaKernel/ThinningDecisionBase.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>
#include <stdexcept>


template <class APPLY, class TESTTRUE, class TESTFALSE>
void logicTest (SG::ThinningDecisionBase& dec,
                APPLY apply,
                TESTTRUE testTrue,
                TESTFALSE testFalse)
{
  using Op = SG::ThinningDecisionBase::Op;
  dec.keepAll();

  apply (dec, true, Op::Set);  // false SET true -> true
  testTrue (dec);
  apply (dec, true, Op::Set);  // true SET true -> true
  testTrue (dec);
  apply (dec, false, Op::Set);  // true SET false -> false
  testFalse (dec);
  apply (dec, false, Op::Set);  // false SET false -> false
  testFalse (dec);

  apply (dec, true, Op::And);  // false AND true -> false
  testFalse (dec);
  apply (dec, true, Op::Set);  // false SET true -> true
  testTrue (dec);
  apply (dec, true, Op::And);  // true AND true -> true
  testTrue (dec);
  apply (dec, false, Op::And); // true AND false -> false
  testFalse (dec);
  apply (dec, false, Op::And); // false AND false -> false
  testFalse (dec);

  apply (dec, false, Op::Or);  // false OR false -> false
  testFalse (dec);
  apply (dec, true, Op::Or);  // false OR true -> true
  testTrue (dec);
  apply (dec, true, Op::Or);  // true OR true -> true
  testTrue (dec);
  apply (dec, false, Op::Or);  // true OR false -> true
  testTrue (dec);
}


void checkThinned (const SG::ThinningDecisionBase& dec,
                   const std::vector<bool>& v)
{
  assert (dec.size() == v.size());
  for (size_t i = 0; i < dec.size(); i++) {
    assert (dec.thinned(i) == v[i]);
  }
}


void test1()
{
  std::cout << "test1\n";
  SG::ThinningDecisionBase dec (10);
  assert (dec.size() == 10);
  assert (dec.thinnedSize() == 10);
  assert (!dec.thinned(3));

  dec.resize (12);
  assert (dec.size() == 12);

  dec.thinAll();
  assert (dec.thinnedSize() == 0);
  assert (dec.thinned(3));

  dec.keepAll();
  assert (dec.thinnedSize() == 12);
  assert (!dec.thinned(3));

  dec.thin (3);
  assert (dec.thinned(3));
  assert (!dec.thinned(4));
  assert (dec.thinnedSize() == 11);
  EXPECT_EXCEPTION( std::out_of_range, dec.thin(100) );

  dec.keep (3);
  assert (!dec.thinned(3));
  assert (!dec.thinned(4));
  assert (dec.thinnedSize() == 12);
  EXPECT_EXCEPTION( std::out_of_range, dec.keep(100) );

  using Op = SG::ThinningDecisionBase::Op;

  logicTest (dec,
             [] (SG::ThinningDecisionBase& dec, bool flag, Op op)
             { dec.thin(3, flag, op); },
             [] (SG::ThinningDecisionBase& dec) { assert (dec.thinned(3)); },
             [] (SG::ThinningDecisionBase& dec) { assert (!dec.thinned(3)); }
             );

  logicTest (dec,
             [] (SG::ThinningDecisionBase& dec, bool flag, Op op)
             { dec.keep(3, flag, op); },
             [] (SG::ThinningDecisionBase& dec) { assert (!dec.thinned(3)); },
             [] (SG::ThinningDecisionBase& dec) { assert (dec.thinned(3)); }
             );

  std::vector<bool> v1 (dec.size());
  std::vector<bool> v2 (dec.size());
  std::vector<bool> v3 (dec.size());
  std::vector<bool> v4 (dec.size());
  for (size_t i = 0; i < dec.size(); i++) {
    if ((i%3) == 0 || (i%4) == 0) {
      v1[i] = true;
    }
    else {
      v2[i] = true;
    }
    v4[i] = true;
  }

  logicTest (dec,
             [&] (SG::ThinningDecisionBase& dec, bool flag, Op op)
             { dec.thin(flag ? v1 : v3, op); },
             [&] (SG::ThinningDecisionBase& dec) { checkThinned (dec, v1); },
             [&] (SG::ThinningDecisionBase& dec) { checkThinned (dec, v3); }
             );

  logicTest (dec,
             [&] (SG::ThinningDecisionBase& dec, bool flag, Op op)
             { dec.keep(flag ? v1 : v3, op); },
             [&] (SG::ThinningDecisionBase& dec) { checkThinned (dec, v2); },
             [&] (SG::ThinningDecisionBase& dec) { checkThinned (dec, v4); }
             );

  std::vector<bool> vx (10);
  EXPECT_EXCEPTION (std::out_of_range, dec.thin (vx));
  EXPECT_EXCEPTION (std::out_of_range, dec.keep (vx));

  dec.thin (v1);
  dec.buildIndexMap();
  size_t j = 0;
  for (size_t i = 0; i < dec.size(); i++) {
    if ((i%3) == 0 || (i%4) == 0) {
      assert (dec.index (i) == SG::ThinningDecisionBase::RemovedIdx);
    }
    else {
      assert (dec.index (i) == j);
      ++j;
    }
  }
}


int main()
{
  std::cout << "AthenaKernel/ThinningDecisionBase_test\n";
  test1();
  return 0;
}
