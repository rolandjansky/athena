/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/CMMRoICnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for CMMRoICnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/CMMRoICnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::CMMRoI& p1,
              const LVL1::CMMRoI& p2)
{
  assert (p1.jetEtRoiWord() == p2.jetEtRoiWord());
  assert (p1.energyRoiWord0() == p2.energyRoiWord0());
  assert (p1.energyRoiWord1() == p2.energyRoiWord1());
  assert (p1.energyRoiWord2() == p2.energyRoiWord2());
}


void testit (const LVL1::CMMRoI& trans1)
{
  MsgStream log (0, "test");
  CMMRoICnv_p1 cnv;
  CMMRoI_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::CMMRoI trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::CMMRoI trans1 (1, 2, 3, 4);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
