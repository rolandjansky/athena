/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/LocalDirectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/LocalDirectionCnv_p1.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::LocalDirection& p1,
              const Trk::LocalDirection& p2)
{
  assert (p1.angleXZ() == p2.angleXZ());
  assert (p1.angleYZ() == p2.angleYZ());
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MsgStream log (0, "test");
  Trk::LocalDirection trans1 (1.5, 2.5);
  LocalDirectionCnv_p1 cnv;
  Trk::LocalDirection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::LocalDirection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
