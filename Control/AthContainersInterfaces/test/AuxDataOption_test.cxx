/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersInterfaces/test/AuxDataOption_traits_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2014
 * @brief Regression test for AuxDataOption_traits.
 */


#undef NDEBUG
#include "AthContainersInterfaces/AuxDataOption.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  SG::AuxDataOption o1 ("o1", static_cast<int> (1));
  assert (o1.name() == "o1");
  assert (o1.intVal() == 1);
  assert (o1.floatVal() == 1);

  SG::AuxDataOption o2 ("o2", static_cast<float> (2.5));
  assert (o2.name() == "o2");
  assert (o2.intVal() == 2);
  assert (o2.floatVal() == 2.5);

  SG::AuxDataOption o3 ("o3", static_cast<double> (3.5));
  assert (o3.name() == "o3");
  assert (o3.intVal() == 3);
  assert (o3.floatVal() == 3.5);
}


int main()
{
  test1();
  return 0;
}

