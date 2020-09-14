/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DetDescrCondTPCnv/test/DetCondKeyTransCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "DetDescrCondTPCnv/DetCondKeyTransCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


HepGeom::Transform3D makexform (float dx, float dy, float dz,
                                float rx, float ry, float rz)
{
  return  HepGeom::Translate3D(dx,dy,dz) *
    HepGeom::RotateX3D(rx) * HepGeom::RotateY3D(ry) * HepGeom::RotateZ3D(rz);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  MsgStream log (0, "test");
  DetCondKeyTrans trans1;

  trans1.setTransform ("aaa", makexform (1, 2, 3, 3, 2, 1));
  trans1.setTransform ("bbb", makexform (3, 2, 1, 1, 2, 3));

  DetCondKeyTransCnv_p1 cnv;
  DetCondKeyTrans_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  DetCondKeyTrans trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans2.exists ("aaa"));
  assert (trans2.exists ("bbb"));
  assert (!trans2.exists ("ccc"));

  HepGeom::Transform3D t;
  assert (trans2.getTransform ("aaa", t));
  assert (t.isNear (makexform (1, 2, 3, 3, 2, 1)));

  assert (trans2.getTransform ("bbb", t));
  assert (t.isNear (makexform (3, 2, 1, 1, 2, 3)));
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
