/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZdcRawChannelCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZdcRawChannelCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZdcRawChannelCnv_p1.h"
//#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ZdcRawChannel& p1,
              const ZdcRawChannel& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.getSize() == p2.getSize());
  for (size_t i = 0; i < p1.getSize(); i++) {
    assert (p1.getEnergy(i) == p2.getEnergy(i));
    assert (p1.getTime(i) == p2.getTime(i));
    assert (p1.getChi(i) == p2.getChi(i));
  }
}


void testit (const ZdcRawChannel& trans1)
{
  MsgStream log (0, "test");
  ZdcRawChannelCnv_p1 cnv;
  ZdcRawChannel_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZdcRawChannel trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  //Athena_test::Leakcheck check;

  ZdcRawChannel trans1 (Identifier (1234));
  trans1.setSize (3);
  for (int k=0; k < 3; k++) {
    trans1.setEnergy (k, 12345.5 + k*10);
    trans1.setTime (k, 100.5 + k*10);
    trans1.setChi (k, 20.5 + k*10);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
