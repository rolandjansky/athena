/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/LocalPositionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MsgStream log (0, "test");
  Amg::Vector2D trans1;
  trans1[0] = 1.5;
  trans1[1] = 2.5;
  LocalPositionCnv_p1 cnv;
  Trk::LocalPosition_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Amg::Vector2D trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1 == trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
