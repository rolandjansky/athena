/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockData/test/LuminosityCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for LuminosityCondData.
 */


#undef NDEBUG
#include "LumiBlockData/LuminosityCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  LuminosityCondData data;

  assert (data.lbAverageLuminosity() == 0);
  assert (data.lbAverageInteractionsPerCrossing() == 0);
  assert (data.lbAverageValid() == 0xffffffff);
  assert (data.muToLumi() == 0);
  assert (data.lbLuminosityPerBCIDVector().size() == LuminosityCondData::TOTAL_LHC_BCIDS);

  data.setLbAverageLuminosity (10.5);
  data.setLbAverageInteractionsPerCrossing (11.5);
  data.setLbAverageValid (12);
  data.setMuToLumi (14.5);
  data.setLbLuminosityPerBCIDVector({1.5, 2, 3.5});

  assert (data.lbAverageLuminosity() == 10.5);
  assert (data.lbAverageInteractionsPerCrossing() == 11.5);
  assert (data.lbAverageValid() == 12);
  assert (data.muToLumi() == 14.5);
  assert (data.lbLuminosityPerBCIDVector() == (std::vector<float>{1.5, 2, 3.5}));
}


int main()
{
  std::cout << "LumiBlockComps/LuminosityCondData\n";
  test1();
  return 0;
}
