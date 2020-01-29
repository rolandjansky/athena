/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_RawDataCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_RawDataCollectionCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_RawDataCollectionCnv_p1.h"
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


void compare (const AFP_RawDataCollection& p1,
              const AFP_RawDataCollection& p2)
{
  assert ( p1.Get_lvl1Id()            ==  p2.Get_lvl1Id());
  //assert ( p1.Get_link_header()       ==  p2.Get_link_header()); // not saved
  assert ( p1.Get_flag()              ==  p2.Get_flag());
  assert ( p1.Get_pattern_POT()       ==  p2.Get_pattern_POT());
  assert ( p1.Get_bcid()              ==  p2.Get_bcid());
  assert ( p1.Get_header_number_POT() ==  p2.Get_header_number_POT());
  assert ( p1.Get_ADC2_POT()          ==  p2.Get_ADC2_POT());
  //assert ( p1.Get_robID()             ==  p2.Get_robID()); // not saved
  assert ( p1.GetTrigSyncErr()        ==  p2.GetTrigSyncErr());

  // not saved
  //assert (p1.Get_POT_DATA().size() == p2.Get_POT_DATA().size());
  //for (size_t i = 0; i < p1.Get_POT_DATA().size(); ++i) {
  //  compare (p1.Get_POT_DATA()[i], p2.Get_POT_DATA()[i]);
  //}

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); ++i) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const AFP_RawDataCollection& trans1)
{
  MsgStream log (0, "test");
  AFP_RawDataCollectionCnv_p1 cnv;
  AFP_RawDataCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_RawDataCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


AFP_RawData makeRawData (int i)
{
  int offs = i*100;

  AFP_RawData rd;
  rd.Set_DiscConf (1 + offs);
  rd.Set_link (2 + offs);
  rd.Set_column (3 + offs);
  rd.Set_row (4 + offs);
  rd.Set_ToT (5 + offs);
  rd.Set_error_bit17 ((i&1) == 1);
  rd.Set_lvl1 (6 + offs);
  rd.addData (7 + offs);
  rd.addData (8 + offs);
  rd.addData (9 + offs);
  return rd;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_RawDataCollection trans1;
  trans1.Set_lvl1Id (1);
  trans1.Set_link_header (2);
  trans1.Set_flag (3);
  trans1.Set_pattern_POT (std::vector<bool> {true, false, true});
  trans1.Set_bcid (4);
  trans1.Set_header (5);
  trans1.Set_ADC2_POT (6);
  trans1.Set_robID (7);
  trans1.SetTrigSyncErr (true);
  trans1.PushBack_POT (makeRawData (1));
  trans1.PushBack_POT (makeRawData (2));
  trans1.push_back (std::make_unique<AFP_RawData> (makeRawData (3)));
  trans1.push_back (std::make_unique<AFP_RawData> (makeRawData (4)));
  trans1.push_back (std::make_unique<AFP_RawData> (makeRawData (5)));

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_RawDataCollectionCnv_p1_test\n";
  test1();
  return 0;
}
