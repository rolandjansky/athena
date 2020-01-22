/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigFTKPool/test/FTKTestDataCnv_p0_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2020
 * @brief Tests for FTKTestDataCnv_p0.
 */


#undef NDEBUG
#include "../src/FTKTestDataCnv_p0.h"
#include "TrigFTKPool/FTKTestData.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const FTKTestData& p1,
              const FTKTestData& p2)
{
  assert (p1.getValue() == p2.getValue());
}


void testit (const FTKTestData& trans1)
{
  MsgStream log (0, "test");
  FTKTestDataCnv_p0 cnv;
  FTKTestData_p0 pers;
  cnv.transToPers (&trans1, &pers, log);
  FTKTestData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  FTKTestData trans1;
  trans1.setValue (1);

  Athena_test::Leakcheck check;
  testit (trans1);
}


int main()
{
  std::cout << "TrigFTKPool/FTKTestDataCnv_p0_test\n";
  test1();
  return 0;
}
