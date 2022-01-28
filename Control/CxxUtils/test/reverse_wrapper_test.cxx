/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CxxUtils/tests/reverse_wrapper_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2022
 * @brief Unit tests for reverse_wrapper.
 */


#undef NDEBUG
#include "CxxUtils/reverse_wrapper.h"
#include <cassert>
#include <iostream>
#include <vector>


void test1()
{
  std::cout << "test1\n";
  std::vector<int> v { 1, 2, 3 };
  int acc = 0;
  for (int i : CxxUtils::make_reverse_wrapper (v)) {
    acc = acc*2 + i;
  }
  assert (acc == 17);
}


int main()
{
  std::cout << "CxxUtils/reverse_wrapper_test.cxx\n";
  test1();
  return 0;
}
