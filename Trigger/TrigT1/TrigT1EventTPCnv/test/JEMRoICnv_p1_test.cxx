/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigT1EventTPCnv/test/JEMRoICnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for JEMRoICnv_p1.
 */


#undef NDEBUG
#include "TrigT1EventTPCnv/JEMRoICnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LVL1::JEMRoI& p1,
              const LVL1::JEMRoI& p2)
{
  assert (p1.roiWord() == p2.roiWord());
}


void testit (const LVL1::JEMRoI& trans1)
{
  MsgStream log (0, "test");
  JEMRoICnv_p1 cnv;
  JEMRoI_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LVL1::JEMRoI trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LVL1::JEMRoI trans1 (123345);
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
