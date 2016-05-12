/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/SetIndexingPolicy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for SetIndexingPolicy.
 */


#undef NDEBUG
#include "AthLinks/tools/SetIndexingPolicy.h"
#include <vector>
#include <iostream>
#include <cassert>


#include "TestTools/expect_exception.h"


void test1()
{
  std::cout << "test1\n";

  typedef std::set<int> Cont;
  typedef SG::SetIndexingPolicy<Cont> Pol;

  Pol::stored_index_type idx;
  Pol::reset (idx);
  assert (!Pol::isValid(idx));
  idx = Pol::stored_index_type(1);
  assert (Pol::isValid(idx));
  Pol::index_type idx2 = idx;
  assert (idx2 == 1);
  Pol::reset (idx);
  assert (!Pol::isValid(idx));

  Cont c;
  c.insert (0);
  c.insert (1);
  c.insert (2);

  Pol::ElementType elt = *c.begin();
  assert (elt == 0);
  Pol::ElementConstReference r_elt = elt;
  assert (r_elt == 0);
  Pol::ElementConstPointer p_elt = &elt;
  assert (*p_elt == 0);

  Pol::reset (idx);
  assert (Pol::lookup (1, c) == 1);
  EXPECT_EXCEPTION (SG::ExcIndexNotFound, Pol::lookup (10, c));
  EXPECT_EXCEPTION (SG::ExcInvalidIndex, Pol::lookup (idx, c));

  Pol::reverseLookup (c, 2, idx2);
  assert (idx2 == 2);
  EXPECT_EXCEPTION (SG::ExcElementNotFound, Pol::reverseLookup (c, 10, idx2));

  assert (Pol::storedToExternal (Pol::stored_index_type(3)) == 3);
}


int main()
{
  test1();
  return 0;
}
