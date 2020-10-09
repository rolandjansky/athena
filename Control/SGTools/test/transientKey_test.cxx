/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  SGTools/test/trasnsientKey_test.cxx
 * @author scott snyder
 * @date Aug 2020
 * @brief Regression test for transientKey.
 */


#undef NDEBUG

#include "SGTools/transientKey.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

  assert (!SG::isTransientKey (""));
  assert (!SG::isTransientKey ("asd"));
  assert ( SG::isTransientKey ("_asd"));
  assert (SG::transientKey ("asd") == "_asd");
  assert (SG::transientKey ("_asd") == "_asd");
}


int main()
{
  std::cout << "SGtools/transientKey_test\n";
  test1();
  return 0;
}



