/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_RawDataCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_RawDataCollectionCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_p1.h"
#include "CxxUtils/make_unique.h"
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


void compare (const ALFA_RawDataCollection& p1,
              const ALFA_RawDataCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);

  assert (p1.GetMBId_POT() == p2.GetMBId_POT());
  assert (0 == p2.GetMrodId_POT());
  assert (p1.GetEventCount_POT() == p2.GetEventCount_POT());
  assert (p1.Get_pattern_POT() == p2.Get_pattern_POT());
  assert (p1.Get_scaler_POT() == p2.Get_scaler_POT());
  assert (p1.Get_ADC1_POT() == p2.Get_ADC1_POT());
  assert (p1.Get_ADC2_POT() == p2.Get_ADC2_POT());
  assert (p1.GetTrigSyncErr() == p2.GetTrigSyncErr());

#if 0  
  assert (p1.Get_POT_DATA().size() == p2.Get_POT_DATA().size());
  for (size_t i=0; i < p1.Get_POT_DATA().size(); i++)
    compare (p1.Get_POT_DATA()[i], p2.Get_POT_DATA()[i]);
#endif
  assert (p2.Get_POT_DATA().empty());
}


void testit (const ALFA_RawDataCollection& trans1)
{
  MsgStream log (0, "test");
  ALFA_RawDataCollectionCnv_p1 cnv;
  ALFA_RawDataCollection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_RawDataCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void init_RawData (ALFA_RawData& r, int o)
{
  r.SetWordId_PMF (234+o);
  r.SetMBId_PMF (345+o);
  r.SetEventCount_PMF (456+o);
  std::vector<uint16_t> v;
  for (int k=0; k<16; k++)
    v.push_back (4+k+o);
  r.SetMarocChan_PMF (v);
  r.Set_error_bit17 (o%200);
  r.addData (10+o);
  r.addData (11+o);
  r.addData (12+o);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_RawDataCollection trans1 (1123);
  for (int i=0; i < 10; i++) {
    int o = i*100;
    auto p = CxxUtils::make_unique<ALFA_RawData> (123+o);
    init_RawData (*p, o);
    trans1.push_back (std::move(p));
  }

  trans1.SetMrodId_POT (1124);
  trans1.SetEventCount_POT (1125);
  trans1.Set_pattern_POT (std::vector<bool> {true, false, false, true});
  trans1.Set_scaler_POT (1126);
  trans1.Set_ADC1_POT (1127);
  trans1.Set_ADC1_POT (1128);
  trans1.SetTrigSyncErr (1129);

  ALFA_RawData r1 (2001);
  init_RawData (r1, 2000);
  ALFA_RawData r2 (3001);
  init_RawData (r2, 3000);
  trans1.PushBack_POT (r1);
  trans1.PushBack_POT (r2);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
