/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloGeoHelpers/test/CaloSampling_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Unit test for CaloSampling.
 */

#undef NDEBUG

#include "CaloGeoHelpers/CaloSampling.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  assert (CaloSampling::getNumberOfSamplings() == 28);
  assert (CaloSampling::getSamplingPattern (CaloSampling::EME1) == 0x20);
  assert (CaloSampling::barrelPattern() == 0x01ff00f);
  assert (CaloSampling::endcapPattern() == 0xfe00ff0);
  assert (CaloSampling::getSamplingName (CaloSampling::EME1) == "EME1");
  assert (CaloSampling::getSamplingName (5) == "EME1");
  assert (CaloSampling::getSampling ("EME1") == CaloSampling::EME1);
  assert (CaloSampling::getSampling ("foo") == CaloSampling::Unknown);
}


int main()
{
  std::cout << "CaloGeoHelpers/CaloSampling_test\n";
  test1();
  return 0;
}
