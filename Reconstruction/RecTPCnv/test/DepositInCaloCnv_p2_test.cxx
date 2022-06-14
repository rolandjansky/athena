/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file RecTPCnv/test/DepositInCaloCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "RecTPCnv/DepositInCaloCnv_p2.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "muonEvent/DepositInCalo.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const DepositInCalo& p1,
              const DepositInCalo& p2)
{
  assert (p1.subCaloId() == p2.subCaloId());
  assert (p1.energyDeposited() == p2.energyDeposited());
  assert (p1.muonEnergyLoss() == p2.muonEnergyLoss());
  assert (p1.etDeposited() == p2.etDeposited());
}


void testit (const DepositInCalo& trans1)
{
  MsgStream log (0, "test");
  DepositInCaloCnv_p2 cnv;
  DepositInCalo_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  DepositInCalo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  DepositInCalo trans1 (CaloCell_ID::EMB2, 10.5, 11.5, 12.5);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE()
{
  std::cout << "RecTPCnv/DepositInCaloCnv_p2\n";
  test1();
  return 0;
}
