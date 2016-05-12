/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/ForwardIndexingPolicy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for ForwardIndexingPolicy.
 */


#undef NDEBUG
#include "AthLinks/tools/ForwardIndexingPolicy.h"
#include <vector>
#include <iostream>
#include <cassert>


#include "TestTools/expect_exception.h"


void test1()
{
  std::cout << "test1\n";

  typedef std::vector<int> Cont;
  typedef SG::ForwardIndexingPolicy<Cont> Pol;

  Pol::stored_index_type idx;
  Pol::reset (idx);
  assert ((int)idx == -1);
  assert (!Pol::isValid(idx));
  idx = 1;
  assert (Pol::isValid(idx));
  Pol::index_type idx2 = idx;
  assert (idx2 == 1);

  Cont c;
  c.push_back (0);
  c.push_back (1);
  c.push_back (2);

  Pol::ElementType elt = c[0];
  assert (elt == 0);
  Pol::ElementConstReference r_elt = elt;
  assert (r_elt == 0);
  Pol::ElementConstPointer p_elt = &elt;
  assert (*p_elt == 0);

  Pol::reset (idx);
  assert (Pol::lookup (1, c) == 1);
  EXPECT_EXCEPTION (SG::ExcBadForwardLink, Pol::lookup (10, c));
  EXPECT_EXCEPTION (SG::ExcBadForwardLink, Pol::lookup (idx, c));

  Pol::reverseLookup (c, 2, idx2);
  assert (idx2 == 2);
  EXPECT_EXCEPTION (SG::ExcElementNotFound, Pol::reverseLookup (c, 10, idx2));

  assert (Pol::storedToExternal(0) == 0);
  assert (Pol::storedToExternal(3) == 3);
  assert (Pol::storedToExternal(static_cast<Pol::stored_index_type>(-1)) ==
          static_cast<Pol::index_type>(-1));
}


int main()
{
  test1();
  return 0;
}
