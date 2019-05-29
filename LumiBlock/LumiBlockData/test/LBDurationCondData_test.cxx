/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/test/LBDurationCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for LBDurationCondData.
 */


#undef NDEBUG
#include "LumiBlockData/LBDurationCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  LBDurationCondData data (123.5);
  assert (data.lbDuration() == 123.5);
}


int main()
{
  std::cout << "LumiBlockComps/LBDurationCondData\n";
  test1();
  return 0;
}
