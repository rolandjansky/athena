/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileSimEventTPCnv/test/TileHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileHitCnv_p1.
 */


#undef NDEBUG
#include "TileSimEventTPCnv/TileHitCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileHit& p1,
              const TileHit& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.size() == p2.size());
  for (int i = 0; i < p1.size(); i++) {
    assert (p1.energy(i) == p2.energy(i));
    assert (p1.time(i) == p2.time(i));
  }
}


void testit (const TileHit& trans1)
{
  MsgStream log (0, "test");
  TileHitCnv_p1 cnv;
  TileHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TileHit trans1 (Identifier (0x1234), 3.5, 13.5);
  trans1.add (4.5, 14.5);
  trans1.add (5.5, 15.5);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
