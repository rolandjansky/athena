/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/BunchGroupCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for BunchGroupCondData.
 */


#undef NDEBUG
#include "CoolLumiUtilities/BunchGroupCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  BunchGroupCondData data;

  data.addBCID (3, 0xa1);
  data.addBCID (5, 0x65);
  data.addBCID (7, 0x9c);
  data.shrink();

  assert (data.bunchGroup(0) == (std::vector<unsigned int>{3, 5}));
  assert (data.bunchGroup(1) == (std::vector<unsigned int>{}));
  assert (data.bunchGroup(2) == (std::vector<unsigned int>{5, 7}));
  assert (data.bunchGroup(3) == (std::vector<unsigned int>{7}));
  assert (data.bunchGroup(4) == (std::vector<unsigned int>{7}));
  assert (data.bunchGroup(5) == (std::vector<unsigned int>{3, 5}));
  assert (data.bunchGroup(6) == (std::vector<unsigned int>{5}));
  assert (data.bunchGroup(7) == (std::vector<unsigned int>{3, 7}));
}


int main()
{
  std::cout << "CoolLumiUtilities/BunchGroupCondData\n";
  test1();
  return 0;
}
