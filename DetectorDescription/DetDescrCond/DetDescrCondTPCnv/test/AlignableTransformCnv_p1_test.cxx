/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DetDescrCondTPCnv/test/AlignableTransformCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "DetDescrCondTPCnv/AlignableTransformCnv_p1.h"
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
  AlignableTransform trans1 ("tag");

  HepGeom::Transform3D xform1 = makexform(1,2,3,1,2,3);
  HepGeom::Transform3D xform2 = makexform(2,3,4,3,2,1);
  HepGeom::Transform3D xform3 = makexform(3,4,5,2,3,1);

  trans1.add (Identifier(1234), xform1);
  trans1.add (Identifier(2345), xform2);
  trans1.add (Identifier(3456), xform3);

  AlignableTransformCnv_p1 cnv;
  AlignableTransform_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  AlignableTransform trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1.size() == 3);
  assert (trans2.size() == 3);
  assert (trans2.tag() == "tag");

  AlignableTransform::AlignTransMem_citr it;
  it = trans2.findIdent (Identifier(2345));
  assert (it->transform().isNear (xform2));
  assert (!it->transform().isNear (xform1));

  it = trans2.findIdent (Identifier(1234));
  assert (it->transform().isNear (xform1));

  it = trans2.findIdent (Identifier(9876));
  assert (it == trans2.end());
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
