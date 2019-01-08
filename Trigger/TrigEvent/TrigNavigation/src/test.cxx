/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TrigNavigation/src/test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Unit test helper.
 */

#include "TrigNavigation/TestTypesB.h"


// Make sure this library includes typeinfo information for TestBContainer.
// Otherwise, we can get ubsan warnings from the unit tests.
void trigNavigationTestRefs()
{
  TrigNavTest::TestBContainer cont;
}
