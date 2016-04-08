/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_RawDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_RawDataCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_RawData& p1,
              const ALFA_RawData& p2)
{
  assert (p1.GetWordId_PMF() == p2.GetWordId_PMF());
  assert (p1.GetPMFId_PMF() == p2.GetPMFId_PMF());
  assert (p1.GetMBId_PMF() == p2.GetMBId_PMF());
  assert (p1.GetEventCount_PMF() == p2.GetEventCount_PMF());
  assert (p1.GetMarocHit() == p2.GetMarocHit());
  assert (p1.Get_bit16() == p2.Get_bit16());
  assert (p1.Get_bit18() == p2.Get_bit18());
  assert (p1.Get_error_bit17() == p2.Get_error_bit17());
  assert (p1.Get_bit26_27() == p2.Get_bit26_27());
  assert (p1.Get_bit24_27() == p2.Get_bit24_27());
  assert (*p1.dataWords() == *p2.dataWords());
}


void testit (const ALFA_RawData& trans1)
{
  MsgStream log (0, "test");
  ALFA_RawDataCnv_p1 cnv;
  ALFA_RawData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_RawData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_RawData trans1 (123);
  trans1.SetWordId_PMF (234);
  trans1.SetMBId_PMF (345);
  trans1.SetEventCount_PMF (456);
  trans1.SetMarocChan_PMF (std::vector<uint16_t> {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
  trans1.Set_error_bit17 (true);
  trans1.addData (10);
  trans1.addData (11);
  trans1.addData (12);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
