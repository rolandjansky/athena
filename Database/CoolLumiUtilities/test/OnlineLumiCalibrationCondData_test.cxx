/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/OnlineLumiCalibrationCondData_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2018
 * @brief Unit test for OnlineLumiCalibrationCondData.
 */


#undef NDEBUG

// Disable checking due to use of AttributeList.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "CoolLumiUtilities/OnlineLumiCalibrationCondData.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include <iostream>
#include <cassert>


OnlineLumiCalibrator makeCalibrator (float muToLumi,
                                     float p0, float p1, float p2)
{
  coral::AttributeList attr;
  attr.extend ("NumOfParameters", "unsigned int");
  attr.extend ("Function", "string");
  attr.extend ("MuToLumi", "float");
  attr.extend ("Parameters", "blob");

  attr["MuToLumi"].setValue (muToLumi);
  attr["Function"].setValue (std::string ("Polynomial"));

  coral::Blob blob (9 * sizeof(float));
  float* p = static_cast<float*> (blob.startingAddress());
  p[0] = 1;
  p[1] = 0;
  p[2] = 100;
  p[3] = p0;
  p[4] = p1;
  p[5] = p2;
  p[6] = 0;
  p[7] = 0;
  p[8] = 0;
  attr["Parameters"].setValue (blob);
  attr["NumOfParameters"].setValue (9u);

  OnlineLumiCalibrator lc;
  assert (lc.setCalibration (attr));
  return lc;
}


void test1()
{
  std::cout << "test1\n";
  OnlineLumiCalibrationCondData data;

  data.set (1, makeCalibrator (10.5, 1, 2, 3));
  data.set (7, makeCalibrator (13.5, 3, 2, 1));

  assert (data.getMuToLumi(1) == 10.5);
  assert (data.getMuToLumi(7) == 13.5);
  assert (data.getMuToLumi(5) == -1);

  float x = 0;
  assert (data.calibrateMu(1, 2, x));
  assert (x == (1 + 2*2 + 3*4));
  assert (data.calibrateMu(7, 2, x));
  assert (x == (3 + 2*2 + 1*4));
  assert (!data.calibrateMu(5, 2, x));

  assert (data.calibrateLumi(1, 2, x));
  assert (x == (1 + 2*2 + 3*4) * 10.5);
  assert (data.calibrateLumi(7, 2, x));
  assert (x == (3 + 2*2 + 1*4) * 13.5);
  assert (!data.calibrateLumi(5, 2, x));
}


int main()
{
  std::cout << "CoolLumiUtilities/OnlineLumiCalibrationCondData\n";
  test1();
  return 0;
}
