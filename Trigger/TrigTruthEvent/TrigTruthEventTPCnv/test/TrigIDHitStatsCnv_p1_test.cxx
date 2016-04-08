/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigTruthEventTPCnv/test/TrigIDHitStatsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for TrigIDHitStatsCnv_p1.
 */


#undef NDEBUG
#include "TrigTruthEventTPCnv/TrigIDHitStatsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigIDHitStats& p1,
              const TrigIDHitStats& p2)
{
  assert (p1.pixhits() == p2.pixhits());
  assert (p1.scthits() == p2.scthits());
  assert (p1.trthits() == p2.trthits());
}


void testit (const TrigIDHitStats& trans1)
{
  MsgStream log (0, "test");
  TrigIDHitStatsCnv_p1 cnv;
  TrigIDHitStats_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigIDHitStats trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigIDHitStats trans1;
  trans1[TrigIDHitStats::PIX] = 12;
  trans1[TrigIDHitStats::SCT] = 13;
  trans1[TrigIDHitStats::TRT] = 14;
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
