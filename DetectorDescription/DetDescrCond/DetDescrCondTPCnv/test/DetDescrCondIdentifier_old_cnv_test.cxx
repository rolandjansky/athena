/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DetDescrCondTPCnv/test/DetDescrCondIdentifier_old_cnv_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "DetDescrCondTPCnv/DetDescrCondIdentifier_old_cnv.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/ubsan_suppress.h"
#include "TInterpreter.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";
  DetDescrCondIdentifier_old_cnv cnv;
  Athena_test::Leakcheck check;

  DetDescrCondIdentifier_old oldobj;
  oldobj.m_id = 1234;

  Identifier newobj;
  cnv.Convert (&newobj, &oldobj);
  assert (newobj.get_identifier32().get_compact() == 1234);
}


int main()
{
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  test1();
  return 0;
}
