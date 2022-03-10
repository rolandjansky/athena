/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  StoreGate/test/StoreClearedIncident_test.cxx
 * @author scott snyder
 * @date Sep 2008
 * @brief Regression test for StoreClearedIncident.
 */


#undef NDEBUG

#include "StoreGate/StoreClearedIncident.h"
#include <cassert>


void test1()
{
  StoreGateSvc* sg = reinterpret_cast<StoreGateSvc*> (0x1234);
  StoreClearedIncident i1 (sg, "i1");
  StoreClearedIncident i2 (sg, "i2", "inc");
  assert (i1.store() == sg);
  assert (i2.store() == sg);
  assert (i1.source() == "i1");
  assert (i2.source() == "i2");
  assert (i1.type() == "StoreCleared");
  assert (i2.type() == "inc");
}


int main()
{
  std::cerr << "*** StoreClearedIncident_test BEGIN ***" << std::endl;
  test1();
  std::cerr << "*** StoreClearedIncident_test END ***" << std::endl;
}

