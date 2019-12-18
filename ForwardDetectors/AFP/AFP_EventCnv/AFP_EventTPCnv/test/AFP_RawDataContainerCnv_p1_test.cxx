/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AFP_EventTPCnv/test/AFP_RawDataContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for AFP_RawDataContainerCnv_p1.
 */


#undef NDEBUG
#include "AFP_EventTPCnv/AFP_RawDataContainerCnv_p1.h"
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
  //assert ( p1.Get_link_header()       ==  p2.Get_link_header()); // not savd
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


void compare (const AFP_RawDataContainer& p1,
              const AFP_RawDataContainer& p2)
{
  assert ( p1.is_FullEVmarker()    ==  p2.is_FullEVmarker());
  assert ( p1.is_ROBmarker()       ==  p2.is_ROBmarker());
  assert ( p1.is_RODmarker()       ==  p2.is_RODmarker());
  assert ( p1.subdetId()           ==  p2.subdetId());
  assert ( p1.mrodId()             ==  p2.mrodId());
  assert ( p1.lvl1Id()             ==  p2.lvl1Id());
  assert ( p1.ecrId()              ==  p2.ecrId());
  assert ( p1.bcId()               ==  p2.bcId());
  assert ( p1.runNum()             ==  p2.runNum());
  assert ( p1.runType()            ==  p2.runType());
  assert ( p1.trigtypeId()         ==  p2.trigtypeId());
  assert ( p1.DetEventType()       ==  p2.DetEventType());
  assert ( p1.GetTimeStamp()       ==  p2.GetTimeStamp());
  assert ( p1.GetTimeStampns()     ==  p2.GetTimeStampns());
  assert ( p1.GetLumiBlock()       ==  p2.GetLumiBlock());
  assert ( p1.GetLvl1Pattern_POT() ==  p2.GetLvl1Pattern_POT());
  assert ( p1.GetLvl2Pattern_POT() ==  p2.GetLvl2Pattern_POT());
  assert ( p1.GetEFPattern_POT()   ==  p2.GetEFPattern_POT());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); ++i) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const AFP_RawDataContainer& trans1)
{
  MsgStream log (0, "test");
  AFP_RawDataContainerCnv_p1 cnv;
  AFP_RawDataContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  AFP_RawDataContainer trans2;
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


std::unique_ptr<AFP_RawDataCollection> makeRawDataCollection (int i)
{
  int offs = 50*i;
  auto rdc = std::make_unique<AFP_RawDataCollection>();
  rdc->Set_lvl1Id (1 + offs);
  rdc->Set_link_header (2 + offs);
  rdc->Set_flag (3 + offs);
  rdc->Set_pattern_POT (std::vector<bool> {(i&1)==1, (i&2)==2, (i&4)==4});
  rdc->Set_bcid (4 + offs);
  rdc->Set_header (5 + offs);
  rdc->Set_ADC2_POT (6 + offs);
  rdc->Set_robID (7 + offs);
  rdc->SetTrigSyncErr ((i&1) == 1);
  rdc->PushBack_POT (makeRawData (1 + offs));
  rdc->PushBack_POT (makeRawData (2 + offs));
  rdc->push_back (std::make_unique<AFP_RawData> (makeRawData (3 + offs)));
  rdc->push_back (std::make_unique<AFP_RawData> (makeRawData (4 + offs)));
  rdc->push_back (std::make_unique<AFP_RawData> (makeRawData (5 + offs)));
  return rdc;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  AFP_RawDataContainer trans1;
  trans1.SetLvl1Id (1);
  trans1.SetBCId (2);
  trans1.SetTimeStamp (3);
  trans1.SetTimeStampns (4);
  trans1.SetLumiBlock (5);
  trans1.SetLvl1Pattern (std::vector<bool> {true,  false,  true});
  trans1.SetLvl2Pattern (std::vector<bool> {false, false,  true});
  trans1.SetEFPattern   (std::vector<bool> {false,  true, false});
  trans1.push_back (makeRawDataCollection (1));
  trans1.push_back (makeRawDataCollection (2));
  trans1.push_back (makeRawDataCollection (3));

  testit (trans1);
}


int main()
{
  std::cout << "AFP_EventTPCnv/AFP_RawDataContainerCnv_p1_test\n";
  test1();
  return 0;
}
