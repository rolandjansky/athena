/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ALFA_EventTPCnv/test/ALFA_ODHitCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ALFA_ODHitCnv_p1.
 */


#undef NDEBUG
#include "ALFA_EventTPCnv/ALFA_ODHitCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const ALFA_ODHit& p1,
              const ALFA_ODHit& p2)
{
  assert (p1.GetHitID() == p2.GetHitID());
  assert (p1.GetParticleEncoding() == p2.GetParticleEncoding());
  assert (p1.GetKineticEnergy() == p2.GetKineticEnergy());
  assert (p1.GetEnergyDeposit() == p2.GetEnergyDeposit());
  assert (p1.GetPreStepX() == p2.GetPreStepX());
  assert (p1.GetPreStepY() == p2.GetPreStepY());
  assert (p1.GetPreStepZ() == p2.GetPreStepZ());
  assert (p1.GetPostStepX() == p2.GetPostStepX());
  assert (p1.GetPostStepY() == p2.GetPostStepY());
  assert (p1.GetPostStepZ() == p2.GetPostStepZ());
  assert (p1.GetGlobalTime() == p2.GetGlobalTime());
  assert (p1.GetSignFiber() == p2.GetSignFiber());
  assert (p1.GetPlateNumber() == p2.GetPlateNumber());
  assert (p1.GetFiberNumber() == p2.GetFiberNumber());
  assert (p1.GetStationNumber() == p2.GetStationNumber());
  assert (p1.GetODSide() == p2.GetODSide());
}


void testit (const ALFA_ODHit& trans1)
{
  MsgStream log (0, "test");
  ALFA_ODHitCnv_p1 cnv;
  ALFA_ODHit_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ALFA_ODHit trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ALFA_ODHit trans1 (123, 234, 345,
                     10.5, 11.5,
                     12.5, 13.5, 14.5,
                     15.5, 16.5, 17.7,
                     18.5,
                     456, 1, 567, 678, 789);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
