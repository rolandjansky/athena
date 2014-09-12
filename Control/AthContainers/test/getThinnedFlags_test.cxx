/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/getThinnedFlags_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief Regression tests for getThinnedFlags
 */

#undef NDEBUG

#ifndef XAOD_STANDALONE

#include "AthContainers/tools/getThinnedFlags.h"
#include <vector>
#include <iostream>
#include <cassert>


#include "TestThinningSvc.icc"


void test1()
{
  std::cout << "test1\n";
  TestThinningSvc svc;

  std::vector<int> c;
  for (int i=0; i < 10; i++) c.push_back(i);

  std::vector<int> c2;

  size_t n = 0;
  std::vector<unsigned char> flags;
  assert (SG::getThinnedFlags (0, c, n, flags) == false);
  assert (n == 10);
  assert (flags.empty());

  n = 0;
  assert (SG::getThinnedFlags (&svc, c, n, flags) == false);
  assert (n == 10);
  assert (flags.empty());

  svc.remap (&c2, 1, 2);
  n = 0;
  assert (SG::getThinnedFlags (&svc, c, n, flags) == false);
  assert (n == 10);
  assert (flags.empty());

  for (int i=0, i1=0; i < 10; ++i) {
    if (i%2 == 0) {
      svc.remap (&c, i, i1);
      ++i1;
    }
    else {
      svc.remap (&c, i, IThinningSvc::RemovedIdx);
    }
  }

  n = 0;
  assert (SG::getThinnedFlags (&svc, c, n, flags) == true);
  assert (n == 5);
  assert (flags.size() == 10);
  for (int i=0; i < 10; ++i) {
    assert (flags[i] == (i%2 == 1));
  }
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
