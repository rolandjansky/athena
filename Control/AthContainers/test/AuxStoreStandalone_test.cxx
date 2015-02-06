/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/AuxStoreStandalone_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Regression tests for AuxStoreStandalone
 */


#undef NDEBUG
#include "AthContainers/AuxStoreStandalone.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";
  SG::AuxStoreStandalone s;
  assert (s.standalone());

  SG::auxid_t ityp1 = SG::AuxTypeRegistry::instance().getAuxID<int> ("anInt");
  int* i1 = reinterpret_cast<int*> (s.getData(ityp1, 10, 20));
  assert (s.getIOData (ityp1) == i1);
  assert (s.getIOType (ityp1) == &typeid (int));

  SG::auxid_t ftyp1 = SG::AuxTypeRegistry::instance().getAuxID<float> ("aFloat");
  assert (s.getIOData (ftyp1) == 0);
}


int main()
{
#ifndef XAOD_STANDALONE
  errorcheck::ReportMessage::hideErrorLocus();
#endif
  test1();
}
