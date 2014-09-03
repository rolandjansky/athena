/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  CaloSamplingData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Component test for CaloSamplingData.
 */

#undef NDEBUG


#include "CaloEvent/CaloSamplingData.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  CaloSamplingData sd1;
  assert (sd1.getNumberOfSamplings() == CaloSampling::Unknown);
  assert (sd1.getNumberOfVariableTypes() == CaloVariableType::NO_OF_TYPES);
  assert (sd1.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 0);
  assert (!sd1.contains (CaloVariableType::ENERGY));
  assert (!sd1.contains (CaloVariableType::PHI));

  const CaloSamplingData& csd1 = sd1;
  std::vector<double> v;
  std::vector<double> v2 (CaloSampling::Unknown, 0);
  assert (csd1.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1) == 0);
  assert (csd1.retrieveData (CaloVariableType::ENERGY, v));
  assert (v == v2);
  assert (!sd1.contains (CaloVariableType::ENERGY));

  assert (sd1.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1) == 0);
  assert (sd1.contains (CaloVariableType::ENERGY));
  assert (sd1.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);

  assert (!sd1.contains (CaloVariableType::PHI));
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 0);
  assert (sd1.storeData (CaloVariableType::PHI, CaloSampling::EMB1, 2));
  assert (sd1.contains (CaloVariableType::PHI));
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 1);
  assert (sd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 2);
  v2[CaloSampling::EMB1] = 2;
  assert (sd1.retrieveData (CaloVariableType::PHI, v));
  assert (v == v2);

  for (size_t i = 0; i < CaloSampling::Unknown; i++)
    v2[i] = i+1;

  assert (!sd1.storeData (CaloVariableType::ETA, std::vector<double>()));
  assert (sd1.storeData (CaloVariableType::ETA, v2));
  assert (sd1.retrieveData (CaloVariableType::ETA, v));
  assert (v == v2);
  assert (sd1.contains (CaloVariableType::ETA));
  assert (sd1.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown);

  assert (sd1.retrieveData (CaloVariableType::ETA, CaloSampling::EMB3) == 4);
  assert (sd1.removeVariable (CaloVariableType::ETA, CaloSampling::EMB3));
  assert (sd1.retrieveData (CaloVariableType::ETA, CaloSampling::EMB3) == 0);
  assert (sd1.retrieveData (CaloVariableType::ETA, CaloSampling::EMB2) == 3);
  assert (sd1.contains (CaloVariableType::ETA));
  assert (sd1.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown-1);

  assert (sd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 2);
  assert (sd1.contains (CaloVariableType::PHI));
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 1);
  assert (sd1.removeVariable (CaloVariableType::PHI));
  assert (sd1.contains (CaloVariableType::PHI));
  assert (csd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 0);
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 0);

  assert (sd1.storeData (CaloVariableType::PHI, CaloSampling::EMB1, 10));
  assert (csd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 10);
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 1);

  assert (sd1.removeVariable (CaloSampling::EMB1));
  assert (csd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 0);
  assert (csd1.retrieveData (CaloVariableType::ETA, CaloSampling::EMB1) == 0);
  assert (sd1.contains (CaloVariableType::PHI));
  assert (sd1.contains (CaloVariableType::ETA));
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 0);
  assert (sd1.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown-2);

  assert (sd1.removeVariable());
  assert (!sd1.contains (CaloVariableType::PHI));
  assert (!sd1.contains (CaloVariableType::ETA));
  assert (!sd1.contains (CaloVariableType::ENERGY));
  assert (sd1.getNumberOfSamplings (CaloVariableType::PHI) == 0);
  assert (sd1.getNumberOfSamplings (CaloVariableType::ETA) == 0);
  assert (sd1.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  std::fill (v2.begin(), v2.end(), 0);
  assert (sd1.retrieveData (CaloVariableType::PHI, v));
  assert (v == v2);
  assert (sd1.retrieveData (CaloVariableType::ETA, v));
  assert (v == v2);
  assert (sd1.retrieveData (CaloVariableType::ENERGY, v));
  assert (v == v2);
  sd1.clearStore();

  CaloSamplingData sd2(CaloVariableType::getVariableBit(CaloVariableType::ETA)|
                       CaloVariableType::getVariableBit(CaloVariableType::PHI));
  const CaloSamplingData& csd2 = sd2;
  assert (!sd2.contains (CaloVariableType::ENERGY));
  assert (sd2.contains (CaloVariableType::ETA));
  assert (sd2.contains (CaloVariableType::PHI));
  assert (sd2.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd2.getNumberOfSamplings (CaloVariableType::ETA) == 0);
  assert (sd2.getNumberOfSamplings (CaloVariableType::PHI) == 0);
  assert (!sd2.storeData (CaloVariableType::ENERGY, CaloSampling::EMB1, 10));
  assert (sd2.storeData (CaloVariableType::ETA, CaloSampling::EMB1, 20));
  assert (sd2.storeData (CaloVariableType::PHI, CaloSampling::EMB1, 30));
  assert (csd2.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1)==0);
  assert (csd2.retrieveData (CaloVariableType::ETA, CaloSampling::EMB1) == 20);
  assert (csd2.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 30);
  assert (!sd2.contains (CaloVariableType::ENERGY));
  assert (sd2.contains (CaloVariableType::ETA));
  assert (sd2.contains (CaloVariableType::PHI));
  assert (sd2.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd2.getNumberOfSamplings (CaloVariableType::ETA) == 1);
  assert (sd2.getNumberOfSamplings (CaloVariableType::PHI) == 1);
  for (size_t i = 0; i < v.size(); i++)
    v[i] = i+1;
  assert (!sd2.storeData (CaloVariableType::ENERGY, v));
  for (size_t i = 0; i < v.size(); i++)
    v[i] = i+2;
  assert (sd2.storeData (CaloVariableType::ETA, v));
  for (size_t i = 0; i < v.size(); i++)
    v[i] = i+3;
  assert (sd2.storeData (CaloVariableType::PHI, v));
  assert (csd2.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1)==0);
  assert (csd2.retrieveData (CaloVariableType::ETA, CaloSampling::EMB1) == 3);
  assert (csd2.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 4);
  assert (!sd2.contains (CaloVariableType::ENERGY));
  assert (sd2.contains (CaloVariableType::ETA));
  assert (sd2.contains (CaloVariableType::PHI));
  assert (sd2.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd2.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown);
  assert (sd2.getNumberOfSamplings (CaloVariableType::PHI) == CaloSampling::Unknown);

  // This behavior smells like a bug to me, but since it's been this
  // way a long time, i'm not going to try to change it now.
  assert (sd2.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1)==0);
  assert (sd2.contains (CaloVariableType::ENERGY));

  CaloSamplingData sd3 (sd2);
  assert (sd3.contains (CaloVariableType::ENERGY));
  assert (sd3.contains (CaloVariableType::ETA));
  assert (sd3.contains (CaloVariableType::PHI));
  assert (sd3.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd3.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown);
  assert (sd3.getNumberOfSamplings (CaloVariableType::PHI) == CaloSampling::Unknown);
  assert (sd3.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1)==0);
  assert (sd3.retrieveData (CaloVariableType::ETA, CaloSampling::EMB1) == 3);
  assert (sd3.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 4);

  CaloSamplingData sd4 (&sd2);
  assert (sd4.contains (CaloVariableType::ENERGY));
  assert (sd4.contains (CaloVariableType::ETA));
  assert (sd4.contains (CaloVariableType::PHI));
  assert (sd4.getNumberOfSamplings (CaloVariableType::ENERGY) == 0);
  assert (sd4.getNumberOfSamplings (CaloVariableType::ETA) == CaloSampling::Unknown);
  assert (sd4.getNumberOfSamplings (CaloVariableType::PHI) == CaloSampling::Unknown);
  assert (sd4.retrieveData (CaloVariableType::ENERGY, CaloSampling::EMB1)==0);
  assert (sd4.retrieveData (CaloVariableType::ETA, CaloSampling::EMB1) == 3);
  assert (sd4.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 4);

  double v3[CaloSampling::Unknown];
  for (int i=0; i < CaloSampling::Unknown; i++)
    v3[i] = i+10;
  assert (sd1.storeData (CaloVariableType::DELTA_ETA, v3, v3+CaloSampling::Unknown));
  assert (sd1.retrieveData (CaloVariableType::DELTA_ETA, CaloSampling::EMB1) == 11);
  for (int i=0; i < CaloSampling::Unknown; i++)
    v3[i] = i+20;
  assert (sd1.storeData (CaloVariableType::DELTA_ETA, v3, v3+5));
  for (int i=0; i < 5; i++)
    assert (sd1.retrieveData (CaloVariableType::DELTA_ETA, (CaloSampling::CaloSample)i) == i+20);
  for (int i=5; i < CaloSampling::Unknown; i++)
    assert (sd1.retrieveData (CaloVariableType::DELTA_ETA, (CaloSampling::CaloSample)i) == i+10);
  std::vector<double> v4 (50, 0);
  assert (sd1.storeData (CaloVariableType::DELTA_ETA, v4));
  for (int i=0; i < CaloSampling::Unknown; i++)
    assert (sd1.retrieveData (CaloVariableType::DELTA_ETA, (CaloSampling::CaloSample)i) == 0);

  sd1.swap (sd3);
  assert (sd1.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 4);
  assert (sd3.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 0);

  sd3 = sd1;
  assert (sd4.retrieveData (CaloVariableType::PHI, CaloSampling::EMB1) == 4);
}


int main()
{
  test1();
  return 0;
}
