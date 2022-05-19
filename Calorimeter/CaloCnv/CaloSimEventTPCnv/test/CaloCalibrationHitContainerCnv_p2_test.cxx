/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloSimEventTPCnv/test/CaloCalibrationHitContainerCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p2.h"
#include "TestTools/leakcheck.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CaloCalibrationHit& h1,
              const CaloCalibrationHit& h2)
{
  assert (h1.Equals (&h2));
  for (int i=0; i<4; i++)
    assert (h1.energy(i) == h2.energy(i));
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");

  // Don't run leakcheck over this; it writes log messages.
  CaloCalibrationHitContainer trans1;
  trans1.push_back (new CaloCalibrationHit (Identifier(987),
                                            45.5,
                                            55.5,
                                            65.5,
                                            75.5));
  trans1.push_back (new CaloCalibrationHit (Identifier(234),
                                            145.5,
                                            155.5,
                                            165.5,
                                            175.5));
  trans1.push_back (new CaloCalibrationHit (Identifier(345),
                                            245.5,
                                            255.5,
                                            265.5,
                                            275.5));

  Athena_test::Leakcheck check;

  CaloCalibrationHitContainerCnv_p2 cnv;
  CaloCalibrationHitContainer_p2 pers;
  cnv.transToPers (&trans1, &pers, log);

  CaloCalibrationHitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1.size() == trans2.size());
  compare (*trans1[0], *trans2[2]);
  compare (*trans1[1], *trans2[0]);
  compare (*trans1[2], *trans2[1]);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
