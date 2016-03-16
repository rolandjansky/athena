/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigHisto2DCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigHisto2DCnv_p1.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigHisto2DCnv_p1.h"
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


void compare (const TrigHisto2D& p1,
              const TrigHisto2D& p2)
{
  compare (static_cast<const TrigHisto&>(p1),
           static_cast<const TrigHisto&>(p2));
  
  assert (p1.nbins_y() == p2.nbins_y());
  assert (p1.min_y() == p2.min_y());
  assert (p1.max_y() == p2.max_y());

  assert (p1.sumEntries (5, 4, TrigHistoCutType::BELOW_X_BELOW_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::BELOW_X_BELOW_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_BELOW_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_BELOW_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::BELOW_X_ABOVE_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::BELOW_X_ABOVE_Y));
  assert (p1.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_ABOVE_Y) ==
          p2.sumEntries (5, 4, TrigHistoCutType::ABOVE_X_ABOVE_Y));
}


void testit (const TrigHisto2D& trans1)
{
  MsgStream log (0, "test");
  TrigHisto2DCnv_p1 cnv;
  TrigHisto2D_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigHisto2D trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigHisto2D trans1 (20, 0, 10, 18, 1, 9);
  for (int i=0; i < 1000; i++)
    trans1.fill (randf(12, -2), randf(10), randf(3));
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
