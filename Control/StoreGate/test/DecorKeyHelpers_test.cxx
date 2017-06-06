/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file StoreGate/test/DecorKeyHelpers_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Tests for DecorKeyHelpers.
 */


#undef NDEBUG
#include "StoreGate/tools/DecorKeyHelpers.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";
  assert (SG::contKeyFromKey ("a.b") == "a");
  assert (SG::contKeyFromKey ("a") == "a");
  assert (SG::contKeyFromKey ("a.") == "a");
  assert (SG::contKeyFromKey ("") == "");
  assert (SG::contKeyFromKey (".b") == "");

  assert (SG::decorKeyFromKey ("a.b") == "b");
  assert (SG::decorKeyFromKey ("a") == "");
  assert (SG::decorKeyFromKey ("a.") == "");
  assert (SG::decorKeyFromKey ("") == "");
  assert (SG::decorKeyFromKey (".b") == "b");
}


int main()
{
  test1();
  return 0;
}
