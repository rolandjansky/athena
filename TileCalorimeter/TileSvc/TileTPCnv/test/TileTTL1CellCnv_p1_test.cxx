/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TileTPCnv/test/TileTTL1CellCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileTTL1CellCnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileTTL1CellCnv_p1.h"
#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileTTL1Cell& p1,
              const TileTTL1Cell& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.eneTower() == p2.eneTower());
  assert (p1.timeAve() == p2.timeAve());
  assert (p1.corrFactor() == p2.corrFactor());
  assert (p1.qualTower() == p2.qualTower());
}



void testit (const TileTTL1Cell& trans1)
{
  MsgStream log (0, "test");
  TileTTL1CellCnv_p1 cnv;
  TileTTL1Cell_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileTTL1Cell trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TileTTL1Cell trans1 (Identifier (0x1234), 1.5, 2.5, 3.5, 4);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
