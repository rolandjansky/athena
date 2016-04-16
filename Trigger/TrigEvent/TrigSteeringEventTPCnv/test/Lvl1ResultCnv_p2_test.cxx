/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigSteeringEventTPCnv/test/Lvl1ResultCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for Lvl1ResultCnv_p2.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/Lvl1ResultCnv_p2.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1CTP::Lvl1Result& p1,
              const LVL1CTP::Lvl1Result& p2)
{
  assert (p1.isConfigured() == p2.isConfigured());
  assert (p1.itemsPassed() == p2.itemsPassed());
  assert (p1.itemsBeforePrescale() == p2.itemsBeforePrescale());
  assert (p1.itemsAfterPrescale() == p2.itemsAfterPrescale());
}


void testit (const LVL1CTP::Lvl1Result& trans1)
{
  MsgStream log (0, "test");
  LVL1CTP::Lvl1ResultCnv_p2 cnv;
  LVL1CTP::Lvl1Result_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1CTP::Lvl1Result trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1CTP::Lvl1Result trans1 (true);
  trans1.itemsPassed() = std::vector<uint32_t> { 1, 2 };
  trans1.itemsBeforePrescale() = std::vector<uint32_t> { 3, 4, 5 };
  trans1.itemsAfterPrescale() = std::vector<uint32_t> { 6, 7, 8, 9 };

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
