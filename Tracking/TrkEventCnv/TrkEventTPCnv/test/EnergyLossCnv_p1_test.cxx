/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkEventTPCnv/test/EnergyLossCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::EnergyLoss& p1,
              const Trk::EnergyLoss& p2)
{
  assert (p1.deltaE() == p2.deltaE());
  assert (p1.sigmaDeltaE() == p2.sigmaDeltaE());
  assert (p1.sigmaMinusDeltaE() == p2.sigmaMinusDeltaE());
  assert (p1.sigmaPlusDeltaE() == p2.sigmaPlusDeltaE());
}


void test1()
{
  std::cout << "test1\n";

  MsgStream log (0, "test");
  Trk::EnergyLoss trans1 (1.5, 2.5, 3.5, 4.5);
  EnergyLossCnv_p1 cnv;
  Trk::EnergyLoss_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::EnergyLoss trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  test1();
  return 0;
}
