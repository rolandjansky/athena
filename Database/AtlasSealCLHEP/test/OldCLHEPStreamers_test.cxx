/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AtlasSealCLHEP/test/OldCLHEPStreamers_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2016
 * @brief Regression tests for OldCLHEPStreamers
 */


#undef NDEBUG
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AtlasSealCLHEP/OldCLHEPStreamers.h"
#include <cassert>
#include <iostream>


void testVec3d()
{
  std::cout << "testVec3d\n";
  OldHepVector3D v1;
  v1.vec = CLHEP::Hep3Vector (3, 4, 5);
  HepGeom::Vector3D<double> v2;
  CLHEPVec3dStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.x() == 3);
  assert (v2.y() == 4);
  assert (v2.z() == 5);
}


void testPoint3d()
{
  std::cout << "testPoint3d\n";
  OldHepPoint3D v1;
  v1.setX (10);
  v1.setY (11);
  v1.setZ (12);
  HepGeom::Point3D<double> v2;
  CLHEPPoint3dStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.x() == 10);
  assert (v2.y() == 11);
  assert (v2.z() == 12);
}


class MyMatrix
  : public CLHEP::HepMatrix
{
public:
  using CLHEP::HepMatrix::num_size;
};

void testMatrix()
{
  std::cout << "testMatrix\n";
  OldHepMatrix v1;
  v1.nrow = 3;
  v1.ncol = 2;
  v1.size = 6;
  v1.m.assign ({1,2,3,4,5,6});
  MyMatrix v2;
  CLHEPMatrixStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.num_size() == 6);
  assert (v2.num_row() == 2);
  assert (v2.num_col() == 3);
  assert (v2[0][0] == 1);
  assert (v2[0][1] == 2);
  assert (v2[0][2] == 3);
  assert (v2[1][0] == 4);
  assert (v2[1][1] == 5);
  assert (v2[1][2] == 6);
}


void testLorVec()
{
  std::cout << "testLorVec\n";
  OldHepLorentzVector v1;
  v1.ee = 21;
  v1.pp = CLHEP::Hep3Vector (22, 23, 24);
  CLHEP::HepLorentzVector v2;
  CLHEPLorVecStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.e() == 21);
  assert (v2.x() == 22);
  assert (v2.y() == 23);
  assert (v2.z() == 24);
}


void test3Vector()
{
  std::cout << "test3Vector\n";
  OldHep3Vector v1;
  v1.dx = 31;
  v1.dy = 32;
  v1.dz = 33;
  CLHEP::Hep3Vector v2;
  CLHEP3VectorStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.x() == 31);
  assert (v2.y() == 32);
  assert (v2.z() == 33);
}


void testBasicVector()
{
  std::cout << "testBasicVector\n";
  OldBasicVector3D v1;
  v1.vec = CLHEP::Hep3Vector (41, 42, 43);
  HepGeom::BasicVector3D<double> v2 (1, 2, 3);
  CLHEPBasicVectorStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.x() == 41);
  assert (v2.y() == 42);
  assert (v2.z() == 43);
}


void testTransform3DVector()
{
  std::cout << "testTransform3DVector\n";
  OldHepTransform3D v1;
  v1.m1  = 51;
  v1.m2  = 52;
  v1.m3  = 53;
  v1.m4  = 54;
  v1.m5  = 55;
  v1.m6  = 56;
  v1.m7  = 57;
  v1.m8  = 58;
  v1.m9  = 59;
  v1.m10 = 60;
  v1.m11 = 61;
  v1.m12 = 62;
  HepGeom::Transform3D v2;
  CLHEPTransform3DStreamer cnv;
  cnv.CopyOldToNew (v1, v2);
  assert (v2.dx() == 51);
  assert (v2.dy() == 52);
  assert (v2.dz() == 53);
  assert (v2.xx() == 54);
  assert (v2.xy() == 55);
  assert (v2.xz() == 56);
  assert (v2.yx() == 57);
  assert (v2.yy() == 58);
  assert (v2.yz() == 59);
  assert (v2.zx() == 60);
  assert (v2.zy() == 61);
  assert (v2.zz() == 62);
}


int main()
{
  testVec3d();
  testPoint3d();
  testMatrix();
  testLorVec();
  test3Vector();
  testBasicVector();
  testTransform3DVector();
  return 0;
}

