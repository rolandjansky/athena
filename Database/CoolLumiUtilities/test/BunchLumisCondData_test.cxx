/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/BunchLumisCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for BunchLumisCondData.
 */


#undef NDEBUG
#include "CoolLumiUtilities/BunchLumisCondData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  BunchLumisCondData data;

  data.addChannel (0, std::vector<float> { 3, 4.5, 6} );
  data.addChannel (4, std::vector<float> { 5, 2.5 } );

  assert( data.rawLuminosity(0) == (std::vector<float> { 3, 4.5, 6}) );
  assert( data.rawLuminosity(4) == (std::vector<float> { 5, 2.5}) );
  assert( data.rawLuminosity(1).empty());
}


int main()
{
  std::cout << "CoolLumiUtilities/BunchLumisCondData\n";
  test1();
  return 0;
}
