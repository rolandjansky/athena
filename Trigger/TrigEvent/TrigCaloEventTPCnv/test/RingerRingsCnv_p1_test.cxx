/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigCaloEventTPCnv/test/RingerRingsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for RingerRingsCnv_p1.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/RingerRingsCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const RingerRings& p1,
              const RingerRings& p2)
{
  assert (p1.rings() == p2.rings());
}


void testit (const RingerRings& trans1)
{
  MsgStream log (0, "test");
  RingerRingsCnv_p1 cnv;
  RingerRings_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  RingerRings trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  RingerRings trans1 (std::vector<float> {1.5, 2.5, 3.5, 4.5});
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
