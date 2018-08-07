/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZdcDigitsCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZdcDigitsCollectionCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZdcDigitsCollectionCnv_p1.h"
#include "CxxUtils/make_unique.h"
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


void compare (const ZdcDigitsCollection& p1,
              const ZdcDigitsCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const ZdcDigitsCollection& trans1)
{
  MsgStream log (0, "test");
  ZdcDigitsCollectionCnv_p1 cnv;
  ZdcDigitsCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZdcDigitsCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  //Athena_test::Leakcheck check;

  ZdcDigitsCollection trans1;
  for (int i = 0; i < 10; i++) {
    int o = i*100;
    auto p = CxxUtils::make_unique<ZdcDigits> (Identifier (1234+o));
    p->set_digits_gain0_delay0 (std::vector<int> {1+o});
    p->set_digits_gain0_delay1 (std::vector<int> {2+o, 3+o});
    p->set_digits_gain1_delay0 (std::vector<int> {4+o, 5+o, 6+o});
    p->set_digits_gain1_delay1 (std::vector<int> {7+o, 8+o, 9+o, 10+o});
    trans1.push_back (std::move(p));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
