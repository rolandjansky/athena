/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TileTPCnv/test/TileTTL1Cnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileTTL1Cnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileTTL1Cnv_p1.h"
#include "TestTools/initGaudi.h"
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


void test1()
{
  std::cout << "test1\n";

  TileTTL1 trans1 (Identifier (0x1234), std::vector<float> { 1.5, 2.5, 3.5} );
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
