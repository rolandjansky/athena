/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/FillParamsCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for FillParamsCondData.
 */


#undef NDEBUG
#include "CoolLumiUtilities/FillParamsCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  FillParamsCondData data;

  uint16_t d1[] = {1, 2, 3, 4};
  uint16_t d2[] = {4, 10, 15};
  uint16_t d3[] = {13, 12, 11, 10, 9};

  data.setBeam1Bunches (d1, d1+4);
  data.setBeam2Bunches (d2, d2+3);
  data.setLuminousBunches (d3, d3+5);

  assert (data.beam1Bunches() == (std::vector<unsigned int> {1, 2, 3, 4}));
  assert (data.beam2Bunches() == (std::vector<unsigned int> {4, 10, 15}));
  assert (data.luminousBunches() == (std::vector<unsigned int> {13, 12, 11, 10, 9}));
}


int main()
{
  std::cout << "CoolLumiUtilities/FillParamsCondData\n";
  test1();
  return 0;
}
