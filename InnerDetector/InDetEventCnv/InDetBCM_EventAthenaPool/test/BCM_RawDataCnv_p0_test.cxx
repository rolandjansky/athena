/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetBCM_EventAthenaPool/test/BCM_RawDataCnv_p0_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for BCM_RawDataCnv_p0.
 */


#undef NDEBUG
#include "../src/BCM_RawDataCnv_p0.h"
#include <cassert>
#include <iostream>


void compare (const BCM_RawData& p1,
              const BCM_RawData& p2)
{
  assert (p1.getWord1() == p2.getWord1());
  assert (p1.getWord2() == p2.getWord2());
}


void testit (const BCM_RawData& trans1)
{
  MsgStream log (nullptr, "test");
  BCM_RawDataCnv_p0 cnv;
  BCM_RawData_p0 pers;
  cnv.transToPers (&trans1, &pers, log);
  BCM_RawData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  BCM_RawData trans1 (123, 654);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
