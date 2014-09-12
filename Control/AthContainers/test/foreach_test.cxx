/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/foreach_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression test for foreach.h
 */


#undef NDEBUG
#include "AthContainers/tools/foreach.h"
#include <vector>
#include <iostream>


void test1()
{
  std::cout << "test1\n";
  std::vector<int> v;
  for (int i=0; i < 10; i++)
    v.push_back (i+10);
  ATHCONTAINERS_FOREACH (int x, v)
    std::cout << x << " ";
  std::cout << "\n";
}


int main()
{
  test1();
  return 0;
}
