/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloSimEventTPCnv/test/CaloCalibrationHitContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "CaloSimEventTPCnv/CaloCalibrationHitContainerCnv_p1.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
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


void test1()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");

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
  CaloCalibrationHitContainerCnv_p1 cnv;
  CaloCalibrationHitContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  CaloCalibrationHitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1.size() == trans2.size());
  for (size_t i = 0; i < trans1.size(); i++)
    compare (*trans1[i], *trans2[i]);
}


int main()
{
  test1();
  return 0;
}
