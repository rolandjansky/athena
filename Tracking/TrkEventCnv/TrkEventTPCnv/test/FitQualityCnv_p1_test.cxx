/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkEventTPCnv/test/FitQualityCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/FitQualityCnv_p1.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void test1()
{
  std::cout << "test1\n";

  MsgStream log (0, "test");
  Trk::FitQuality trans1 (1.5, 2.5);
  FitQualityCnv_p1 cnv;
  Trk::FitQuality_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::FitQuality trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


int main()
{
  test1();
  return 0;
}
