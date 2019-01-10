/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CxxUtils/test/Ring_test.cxx
 * @author scott snyder
 * @date May 2018
 * @brief Regression tests for Ring.
 */


#undef NDEBUG
#include "CxxUtils/Ring.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  CxxUtils::Ring<int> ring;
  assert (ring.getKeysDedup().empty());
  ring.reset (10);
  assert (ring.getKeysDedup().empty());

  ring.push (1);
  ring.push (2);
  ring.push (2);
  ring.push (3);
  // 1 2 2 3*
  assert (ring.getKeysDedup() == (std::vector<int> { 1, 2, 3 }));

  ring.push (4);
  ring.push (5);
  ring.push (5);
  ring.push (5);
  ring.push (6);
  ring.push (7);
  // 1 2 2 3 4 5 5 5 6 7*
  assert (ring.getKeysDedup() == (std::vector<int> { 1, 2, 3, 4, 5, 6, 7 }));

  ring.push (7);
  // 7* 2 2 3 4 5 5 5 6 7
  assert (ring.getKeysDedup() == (std::vector<int> { 2, 3, 4, 5, 6, 7 }));

  ring.push (8);
  // 7 8* 2 3 4 5 5 5 6 7
  assert (ring.getKeysDedup() == (std::vector<int> { 2, 3, 4, 5, 6, 7, 8 }));

  ring.push (9);
  ring.push (10);
  ring.push (11);
  ring.push (12);
  // 7 8 9 10 11 12* 5 5 6 7
  assert (ring.getKeysDedup() == (std::vector<int> { 5, 6, 7, 8, 9, 10, 11, 12 }));
}


int main()
{
  std::cout << "Ring_test\n";
  test1();
  return 0;
}
