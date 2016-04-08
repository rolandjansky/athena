/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_RawDataCollectionCnv_charge_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_RawDataCollectionCnv_charge_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_charge_p1.h"
#include "CxxUtils/make_unique.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_RawData_charge& p1,
              const ALFA_RawData_charge& p2)
{
  assert (p1.GetChannelNum_PMF() == p2.GetChannelNum_PMF());
  assert (p1.GetPMFId_PMF() == p2.GetPMFId_PMF());
  assert (p1.GetMBId_PMF() == p2.GetMBId_PMF());
  assert (0 == p2.GetFiberFirmware_PMF());
  assert (p1.GetEventCount_PMF() == p2.GetEventCount_PMF());
  //assert (p1.GetChargeHit() == p2.GetChargeHit());
  assert (p1.Get_bit12() == p2.Get_bit12());
  assert (p1.ChargeChanVect_number() == p2.ChargeChanVect_number());
  assert (*p1.dataWords() == *p2.dataWords());
}


void compare (const ALFA_RawDataCollection_charge& p1,
              const ALFA_RawDataCollection_charge& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);

  assert (p1.GetMBId_POT() == p2.GetMBId_POT());
  assert (0 == p2.GetMrodId_POT());
  assert (p1.GetEventCount_POT() == p2.GetEventCount_POT());
  assert (p1.GetTimeStamp_POT() == p2.GetTimeStamp_POT());
  assert (p1.GetBCId_POT() == p2.GetBCId_POT());

#if 0
  assert (p1.Get_POT_DATA().size() == p2.Get_POT_DATA().size());
  for (size_t i=0; i < p1.Get_POT_DATA().size(); i++)
    compare (p1.Get_POT_DATA()[i], p2.Get_POT_DATA()[i]);
#endif
  assert (p2.Get_POT_DATA().empty());
}


void testit (const ALFA_RawDataCollection_charge& trans1)
{
  MsgStream log (0, "test");
  ALFA_RawDataCollectionCnv_charge_p1 cnv;
  ALFA_RawDataCollection_charge_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_RawDataCollection_charge trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void init_RawData (ALFA_RawData_charge& r, int o)
{
  r.SetChannelNum_PMF (234+o);
  r.SetMBId_PMF (345+o);
  r.SetFiberFirmware_PMF (456+o);
  r.SetEventCount_PMF (567+o);
  r.SetChargeChan_PMF (678+o);
  std::vector<uint16_t> v;
  for (int i=0; i<64; i++)
    v.push_back (200+i+o);
  r.SetChargeChanVect_PMF (v);
  r.addData (10+o);
  r.addData (11+o);
  r.addData (12+o);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_RawDataCollection_charge trans1 (1124, 1123);
  for (int i=0; i < 10; i++) {
    int o = i*100;
    auto p = CxxUtils::make_unique<ALFA_RawData_charge> (123+o);
    init_RawData (*p, o);
    trans1.push_back (std::move(p));
  }

  trans1.SetEventCount_POT (1125);
  trans1.SetTimeStamp_POT (1126);
  trans1.SetBCId_POT (1127);

  ALFA_RawData_charge r1 (2001);
  init_RawData (r1, 2000);
  ALFA_RawData_charge r2 (3001);
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
