/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArSimEventTPCnv/test/LArHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArHitCnv_p1.
 */


#undef NDEBUG
#include "LArSimEventTPCnv/LArHitCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const LArHit& p1,
              const LArHit& p2)
{
  assert (p1.cellID() == p2.cellID());
  assert (p1.energy() == p2.energy());
  assert (p1.time() == p2.time());
}


void testit (const LArHit& trans1)
{
  MsgStream log (0, "test");
  LArHitCnv_p1 cnv;
  LArHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LArHit trans1 (Identifier (0x1234), 10.5, 12.5);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
