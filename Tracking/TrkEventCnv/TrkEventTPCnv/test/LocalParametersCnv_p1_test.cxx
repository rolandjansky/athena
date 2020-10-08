/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/LocalParametersCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void compare (const Trk::LocalParameters& p1,
              const Trk::LocalParameters& p2)
{
  assert (p1.parameterKey() == p2.parameterKey());
  assert (p1.dimension() == p2.dimension());
  for (int i = 0; i < p1.dimension(); i++) {
    Trk::ParamDefs ii = static_cast<Trk::ParamDefs>(i);
    assert (p1[ii] == p2[ii]);
  }
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MsgStream log (0, "test");
  Trk::LocalParameters trans1 (1.5, 2.5, 3.5, 4.5, 5.5);
  LocalParametersCnv_p1 cnv;
  Trk::LocalParameters_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::LocalParameters trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
