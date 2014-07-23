/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: sincos_test.cxx,v 1.1 2008-11-24 04:34:07 ssnyder Exp $
/**
 * @file CxxUtils/test/sincos_test.cxx
 * @author scott snyder
 * @date Nov 2008
 * @brief Regression tests for sincos.
 */


#undef NDEBUG


#include "CxxUtils/sincos.h"
#include <cassert>
#include <cmath>

using namespace std;


void fptest (double a, double b)
{
  double den = abs(a) + abs(b);
  if (den == 0) den = 1;
  double frac = abs(a-b)/den;
  assert (frac < 1e-9);
}


void test1()
{
  CxxUtils::sincos a (0.1);
  fptest (sin(0.1), a.sn);
  fptest (cos(0.1), a.cs);
  fptest (2*sin(0.1) + 3*cos(0.1), a.apply(2,3));
  fptest (2*sin(0.1)*sin(0.1) + 3*sin(0.1)*cos(0.1) + 4*cos(0.1)*cos(0.1),
          a.apply2(2,3,4));
}


int main()
{
  test1();
  return 0;
}
