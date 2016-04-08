/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/RODHeaderCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for RODHeaderCnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/RODHeaderCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::RODHeader& p1,
              const LVL1::RODHeader& p2)
{
  assert (p1.version() == p2.version());
  assert (p1.sourceID() == p2.sourceID());
  assert (p1.run() == p2.run());
  assert (p1.extendedL1ID() == p2.extendedL1ID());
  assert (p1.bunchCrossing() == p2.bunchCrossing());
  assert (p1.l1TriggerType() == p2.l1TriggerType());
  assert (p1.detEventType() == p2.detEventType());
  assert (p1.statusWords() == p2.statusWords());
  assert (p1.payloadSize() == p2.payloadSize());
}


void testit (const LVL1::RODHeader& trans1)
{
  MsgStream log (0, "test");
  RODHeaderCnv_p1 cnv;
  RODHeader_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::RODHeader trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::RODHeader trans1 (1, 2, 3, 4, 5, 6, 7,
                           std::vector<unsigned int> {20, 21, 22},
                          13);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
