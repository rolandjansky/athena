/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigSteeringEventTPCnv/test/TrigPassBitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigPassBitsCnv_p1.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigPassBits& p1,
              const TrigPassBits& p2)
{
  assert (p1.size() == p2.size());
  for (unsigned int i = 0; i < p1.size(); i++)
    assert (p1.isPassing(i) == p2.isPassing(i));
}


void testit (const TrigPassBits& trans1)
{
  MsgStream log (0, "test");
  TrigPassBitsCnv_p1 cnv;
  TrigPassBits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigPassBits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigPassBits trans1 (10, nullptr);
  for (int i=0; i < 10; i += 2)
    trans1.markPassing(i, nullptr);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
