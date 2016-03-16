/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigTrtCountsCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigTrtCountsCnv_p2.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigTrtCountsCnv_p2.h"
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


void compare (const TrigTrtCounts& p1,
              const TrigTrtCounts& p2)
{
  compare (p1.z0_pt(), p2.z0_pt());
  compare (p1.eta_phi(), p2.eta_phi());
}


void testit (const TrigTrtCounts& trans1)
{
  MsgStream log (0, "test");
  TrigTrtCountsCnv_p2 cnv;
  TrigTrtCounts_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigTrtCounts trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigHisto2D z0_pt (20, 0, 10, 20, 0, 10);
  for (int i=0; i < 1000; i++)
    z0_pt.fill (randf(10), randf(10), randf(3));

  TrigHisto2D eta_phi (20, 0, 10, 20, 0, 10);
  for (int i=0; i < 1000; i++)
    eta_phi.fill (randf(10), randf(10), randf(3));

  TrigTrtCounts trans1 (z0_pt, eta_phi);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
