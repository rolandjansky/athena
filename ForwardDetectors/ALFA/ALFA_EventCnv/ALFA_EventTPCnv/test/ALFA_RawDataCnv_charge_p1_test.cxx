/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_RawDataCnv_charge_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_RawDataCnv_charge_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_charge_p1.h"
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


void testit (const ALFA_RawData_charge& trans1)
{
  MsgStream log (0, "test");
  ALFA_RawDataCnv_charge_p1 cnv;
  ALFA_RawData_charge_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_RawData_charge trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_RawData_charge trans1 (123);
  trans1.SetChannelNum_PMF (234);
  trans1.SetMBId_PMF (345);
  trans1.SetFiberFirmware_PMF (456);
  trans1.SetEventCount_PMF (567);
  trans1.SetChargeChan_PMF (678);
  std::vector<uint16_t> v;
  for (int i=0; i<64; i++)
    v.push_back (200+i);
  trans1.SetChargeChanVect_PMF (v);
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
