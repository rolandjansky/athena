/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
/**
 * @file CxxUtils/test/fpcompare_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2008
 * @brief Regression tests for the fpcompare header.
 */


#include "CxxUtils/fpcompare.h"
#include <cmath>
#include <cassert>
#include <stdio.h>


struct tester
{
  tester (double x) : m_x (x) {}
  double pt() const;
  double ptf() const { return pt(); }
  double m_x;
};


double tester::pt() const
{
  return m_x * (1. / std::cosh(1.9025873924597605));
}


void test1 (const tester* a, const tester* b, const tester* c)
{
  using namespace CxxUtils::fpcompare;

  assert (   equal (a->pt(), a->pt()) );
  assert ( ! equal (a->pt(), c->pt()) );

  assert ( ! greater (a->pt(), b->pt()) ); // FAILX
  assert (   greater (c->pt(), b->pt()) );
  assert ( ! greater (b->pt(), c->pt()) );
  
  assert ( ! less (a->pt(), b->pt()) );
  assert (   less (a->pt(), c->pt()) );
  assert ( ! less (c->pt(), a->pt()) );

  assert (   greater_equal (a->pt(), b->pt()) );
  assert (   greater_equal (c->pt(), b->pt()) );
  assert ( ! greater_equal (b->pt(), c->pt()) );

  assert (   less_equal (a->pt(), b->pt()) ); // FAILX
  assert (   less_equal (a->pt(), c->pt()) );
  assert ( ! less_equal (c->pt(), a->pt()) );

  //********************************************************

  assert (   equal (a->ptf(), a->ptf()) );
  assert ( ! equal (a->ptf(), c->ptf()) );

  assert ( ! greater (a->ptf(), b->ptf()) );
  assert (   greater (c->ptf(), b->ptf()) );
  assert ( ! greater (b->ptf(), c->ptf()) );
  
  assert ( ! less (a->ptf(), b->ptf()) );
  assert (   less (a->ptf(), c->ptf()) );
  assert ( ! less (c->ptf(), a->ptf()) );

  assert (   greater_equal (a->ptf(), b->ptf()) );
  assert (   greater_equal (c->ptf(), b->ptf()) );
  assert ( ! greater_equal (b->ptf(), c->ptf()) );

  assert (   less_equal (a->ptf(), b->ptf()) );
  assert (   less_equal (a->ptf(), c->ptf()) );
  assert ( ! less_equal (c->ptf(), a->ptf()) );
}


void test2 (const tester* a, const tester* b, const tester* c)
{
  using namespace CxxUtils::fpcompare_fn;

  equal eq;
  assert (   eq (a->pt(), a->pt()) );
  assert ( ! eq (a->pt(), c->pt()) );

  greater gt;
  assert ( ! gt (a->pt(), b->pt()) ); // FAILX
  assert (   gt (c->pt(), b->pt()) );
  assert ( ! gt (b->pt(), c->pt()) );

  less lt;
  assert ( ! lt (a->pt(), b->pt()) );
  assert (   lt (a->pt(), c->pt()) );
  assert ( ! lt (c->pt(), a->pt()) );

  greater_equal ge;
  assert (   ge (a->pt(), b->pt()) );
  assert (   ge (c->pt(), b->pt()) );
  assert ( ! ge (b->pt(), c->pt()) );

  less_equal le;
  assert (   le (a->pt(), b->pt()) ); // FAILX
  assert (   le (a->pt(), c->pt()) );
  assert ( ! le (c->pt(), a->pt()) );

  //********************************************************

  equalf eqf;
  assert (   eqf (a->ptf(), a->ptf()) );
  assert ( ! eqf (a->ptf(), c->ptf()) );

  greaterf gtf;
  assert ( ! gtf (a->ptf(), b->ptf()) );
  assert (   gtf (c->ptf(), b->ptf()) );
  assert ( ! gtf (b->ptf(), c->ptf()) );

  lessf ltf;
  assert ( ! ltf (a->ptf(), b->ptf()) );
  assert (   ltf (a->ptf(), c->ptf()) );
  assert ( ! ltf (c->ptf(), a->ptf()) );

  greater_equalf gef;
  assert (   gef (a->ptf(), b->ptf()) );
  assert (   gef (c->ptf(), b->ptf()) );
  assert ( ! gef (b->ptf(), c->ptf()) );

  less_equalf lef;
  assert (   lef (a->ptf(), b->ptf()) );
  assert (   lef (a->ptf(), c->ptf()) );
  assert ( ! lef (c->ptf(), a->ptf()) );
}


int main()
{
  // chosen so that a->pt() > a->pt is true when compiled
  // with optimization using x87 instructions.
  // The assertions marked with FAILX above fail in that case
  // if the workaround is not operative.
  tester a (441849.03125);
  tester c (841849.03125);
  test1 (&a, &a, &c);
  test2 (&a, &a, &c);
  return 0;
}
