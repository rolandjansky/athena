/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigTrtHitCountsCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigTrtHitCountsCnv_p2.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCnv_p2.h"
#include "TestTools/random.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


using Athena_test::randf;


void compare (const TrigHisto& p1,
              const TrigHisto& p2)
{
  assert (p1.contents() == p2.contents());
  assert (p1.nbins_x() == p2.nbins_x());
  assert (p1.min_x() == p2.min_x());
  assert (p1.max_x() == p2.max_x());
}


void compare (const TrigHisto1D& p1,
              const TrigHisto1D& p2)
{
  compare (static_cast<const TrigHisto&>(p1),
           static_cast<const TrigHisto&>(p2));
  
  assert (p1.sumEntries (5, TrigHistoCutType::BELOW_X) ==
          p2.sumEntries (5, TrigHistoCutType::BELOW_X));
  assert (p1.sumEntries (5, TrigHistoCutType::ABOVE_X) ==
          p2.sumEntries (5, TrigHistoCutType::ABOVE_X));
}


void compare (const TrigTrtHitCounts& p1,
              const TrigTrtHitCounts& p2)
{
  compare (p1.endcapC(), p2.endcapC());
  compare (p1.endcapA(), p2.endcapA());
  compare (p1.barrel(), p2.barrel());
}


void testit (const TrigTrtHitCounts& trans1)
{
  MsgStream log (0, "test");
  TrigTrtHitCountsCnv_p2 cnv;
  TrigTrtHitCounts_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTrtHitCounts trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigHisto1D endcapC (20, 0, 10);
  for (int i=0; i < 1000; i++)
    endcapC.fill (randf(10), randf(3));

  TrigHisto1D endcapA (20, 0, 10);
  for (int i=0; i < 1000; i++)
    endcapA.fill (randf(10), randf(3));

  TrigHisto1D barrel (20, 0, 10);
  for (int i=0; i < 1000; i++)
    barrel.fill (randf(10), randf(3));

  TrigTrtHitCounts trans1 (endcapC, barrel, endcapA);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
