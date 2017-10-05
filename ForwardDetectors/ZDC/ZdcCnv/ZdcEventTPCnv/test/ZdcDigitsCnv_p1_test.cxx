/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZdcDigitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZdcDigitsCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZdcDigitsCnv_p1.h"
//#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ZdcDigits& p1,
              const ZdcDigits& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.get_digits_gain0_delay0() == p2.get_digits_gain0_delay0());
  assert (p1.get_digits_gain0_delay1() == p2.get_digits_gain0_delay1());
  assert (p1.get_digits_gain1_delay0() == p2.get_digits_gain1_delay0());
  assert (p1.get_digits_gain1_delay1() == p2.get_digits_gain1_delay1());
}


void testit (const ZdcDigits& trans1)
{
  MsgStream log (0, "test");
  ZdcDigitsCnv_p1 cnv;
  ZdcDigits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZdcDigits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  //Athena_test::Leakcheck check;

  ZdcDigits trans1 (Identifier (1234));
  trans1.set_digits_gain0_delay0 (std::vector<int> {1});
  trans1.set_digits_gain0_delay1 (std::vector<int> {2, 3});
  trans1.set_digits_gain1_delay0 (std::vector<int> {4, 5, 6});
  trans1.set_digits_gain1_delay1 (std::vector<int> {7, 8, 9, 10});
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
