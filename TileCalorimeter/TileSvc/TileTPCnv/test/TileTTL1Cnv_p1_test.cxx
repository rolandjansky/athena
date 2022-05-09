/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileTPCnv/test/TileTTL1Cnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileTTL1Cnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileTTL1Cnv_p1.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileTTL1& p1,
              const TileTTL1& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.fsamples() == p2.fsamples());
}



void testit (const TileTTL1& trans1)
{
  MsgStream log (0, "test");
  TileTTL1Cnv_p1 cnv;
  TileTTL1_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileTTL1 trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TileTTL1 trans1 (Identifier (0x1234), std::vector<float> { 1.5, 2.5, 3.5} );
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
