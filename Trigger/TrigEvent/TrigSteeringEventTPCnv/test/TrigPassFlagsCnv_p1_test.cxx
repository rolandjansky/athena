/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigSteeringEventTPCnv/test/TrigPassFlagsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigPassFlagsCnv_p1.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"
#include "TestTools/random.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigPassFlags& p1,
              const TrigPassFlags& p2)
{
  assert (p1.size() == p2.size());
  for (unsigned int i = 0; i < p1.size(); i++)
    assert (p1.getFlag(i) == p2.getFlag(i));
}


void testit (const TrigPassFlags& trans1)
{
  MsgStream log (0, "test");
  TrigPassFlagsCnv_p1 cnv;
  TrigPassFlags_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigPassFlags trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigPassFlags trans1 (10, 8, nullptr);
  for (int i=0; i < 10; i++) {
    unsigned int x = Athena_test::randi(256);
    for (int j=0; j < 8; j++) {
      if (x&1) trans1.setFlagBit (i, j, true, nullptr);
      x = x>>1;
    }
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
