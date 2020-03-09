/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/DepositInCaloCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/DepositInCaloCnv_p1.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "muonEvent/DepositInCalo.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const DepositInCalo& p1,
              const DepositInCalo& p2)
{
  assert (p1.subCaloId() == p2.subCaloId());
  assert (p1.energyDeposited() == p2.energyDeposited());
  assert (p1.muonEnergyLoss() == p2.muonEnergyLoss());
  // Not saved in _p1.
  //assert (p1.etDeposited() == p2.etDeposited());
}


void testit (const DepositInCalo& trans1, const DepositInCalo_p1& pers)
{
  MsgStream log (0, "test");
  DepositInCaloCnv_p1 cnv;
  DepositInCalo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  DepositInCalo trans1 (CaloCell_ID::EMB2, 10.5, 11.5, 12.5);
  DepositInCalo_p1 pers1;
  pers1.m_subCaloId = trans1.subCaloId();
  pers1.m_energyDeposited = trans1.energyDeposited();
  pers1.m_muonEnergyLoss = trans1.muonEnergyLoss();
  testit (trans1, pers1);
}


int main()
{
  std::cout << "RecTPCnv/DepositInCaloCnv_p1\n";
  test1();
  return 0;
}
