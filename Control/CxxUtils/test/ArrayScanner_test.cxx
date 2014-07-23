/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArrayScanner_test.cxx,v 1.1 2009-03-20 20:44:23 ssnyder Exp $
/**
 * @file  ArrayScanner_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Regression tests for ArrayScanner class.
 */


#undef NDEBUG


#include "CxxUtils/ArrayScanner.h"
#include <sstream>
#include <cassert>


using namespace CxxUtils;


void test1()
{
  float f;
  std::istringstream is ("  [ 3.5, 5.5, [7.5], [9.5]]  ");
  ArrayScanner s (is);
  assert (!s.at_end());
  assert (!s.at_close());
  assert (!s.at_num (f));
  assert (s.at_open());

  assert (!s.at_end());
  assert (!s.at_close());
  assert (!s.at_open());
  assert (s.at_num (f));
  assert (f == 3.5);

  assert (s.at_num (f));
  assert (f == 5.5);

  assert (s.at_open());
  assert (s.at_num (f));
  assert (f == 7.5);

  assert (s.at_close());
  assert (s.at_open());
  assert (s.at_num (f));
  assert (f == 9.5);
  assert (!s.at_end());

  assert (s.at_close());
  assert (s.at_close());
  assert (s.at_end());

  std::istringstream is2 (" 3");
  ArrayScanner s2 (is2);
  assert (s2.at_num (f));
  assert (f == 3);
  assert (s2.at_end());
}


int main()
{
  test1();
  return 0;
}
