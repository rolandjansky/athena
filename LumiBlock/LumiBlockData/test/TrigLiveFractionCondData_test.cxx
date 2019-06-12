/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/test/TrigLiveFractionCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2019
 * @brief Unit test for TrigLiveFractionCondData.
 */


#undef NDEBUG
#include "LumiBlockData/TrigLiveFractionCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  TrigLiveFractionCondData data (123,
                                 std::vector<float> {1, 3.5, 7},
                                 std::vector<float> {10, 3, 1.5, 5},
                                 10.5,
                                 12);

  assert (data.l1LiveFractionVector(false) ==
          (std::vector<float> {1, 3.5, 7}));
  assert (data.l1LiveFractionVector(true) ==
          (std::vector<float> {10, 3, 1.5, 5}));
  assert (data.lhcTurnCounter() == 123);
  assert (data.lbAverageLiveFraction (false) == 10.5);
  assert (data.lbAverageLiveFraction (true) == 12);
}


int main()
{
  std::cout << "LumiBlockComps/TrigLiveFractionCondData\n";
  test1();
  return 0;
}
