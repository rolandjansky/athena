/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_RawDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_RawDataCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_RawDataCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const AFP_RawData& p1,
              const AFP_RawData& p2)
{
  assert ( p1.Get_DiscConf()    ==  p2.Get_DiscConf());
  assert ( p1.Get_link()        ==  p2.Get_link());
  assert ( p1.Get_column()      ==  p2.Get_column());
  assert ( p1.Get_row()         ==  p2.Get_row());
  assert ( p1.Get_ToT()         ==  p2.Get_ToT());
  assert ( p1.Get_bit16()       ==  p2.Get_bit16());
  assert ( p1.Get_bit18()       ==  p2.Get_bit18());
  assert ( p1.Get_error_bit17() ==  p2.Get_error_bit17());
  assert ( p1.Get_bit26_27()    ==  p2.Get_bit26_27());
  assert ( p1.Get_bit24_27()    ==  p2.Get_bit24_27());
  assert ( p1.Get_lvl1()        ==  p2.Get_lvl1());
  assert (*p1.dataWords()       == *p2.dataWords());
}


void testit (const AFP_RawData& trans1)
{
  MsgStream log (0, "test");
  AFP_RawDataCnv_p1 cnv;
  AFP_RawData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_RawData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_RawData trans1;
  trans1.Set_DiscConf (1);
  trans1.Set_link (2);
  trans1.Set_column (3);
  trans1.Set_row (4);
  trans1.Set_ToT (5);
  trans1.Set_error_bit17 (true);
  trans1.Set_lvl1 (6);
  trans1.addData (7);
  trans1.addData (8);
  trans1.addData (9);

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_RawDataCnv_p1_test\n";
  test1();
  return 0;
}
