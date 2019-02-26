/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkSurfaces/test/CylinderSurface_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2019
 * @brief Regression tests for CylinderSurface.  Incomplete!
 */


#undef NDEBUG
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cassert>
#include <cmath>


Amg::Vector3D unit (double x, double y, double z)
{
  return Amg::Vector3D (x, y, z).unit();
}


std::unique_ptr<Amg::Transform3D> transf (const Amg::Vector3D& pos,
                                          const Amg::Vector3D& norm)
{
  Trk::CurvilinearUVT c (norm);
  Amg::RotationMatrix3D curvilinearRotation;
  curvilinearRotation.col(0) = c.curvU();
  curvilinearRotation.col(1) = c.curvV();
  curvilinearRotation.col(2) = c.curvT();
  auto transf = std::make_unique<Amg::Transform3D>();
  *transf = curvilinearRotation;
  transf->pretranslate(pos);
  return transf;
}


void try_intersection (const Trk::CylinderSurface& cyl,
                       const Amg::Vector3D& pos,
                       const Amg::Vector3D& dir)
{
  double R = cyl.bounds().r();

  Trk::Intersection isect =
    cyl.straightLineIntersection (pos, dir, false, false);

  if (dir.x() == 0 && dir.y() == 0) {
    assert (!isect.valid);
    return;
  }

  assert (isect.valid);
  assert (isect.distance == 0);

  const Amg::Vector3D& ipos = isect.position;

  // Check that intersection is on the cylinder surface.
  assert (Athena_test::isEqual (R, ipos.perp()));

  // Check that intersection is on the line.
  assert (Athena_test::isEqual ((ipos-pos).unit().dot (dir), 1));

  // Check distance to intersection.
  assert (Athena_test::isEqual ((ipos-pos).norm(), isect.pathLength));
}


void test_intersect()
{
  std::cout << "test_intersect\n";

  Amg::Vector3D pos1 { 0, 0, 0 };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::CylinderSurface cyl1 (transf (pos1, norm1).release(), 2000, 10000);

  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  0,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  0,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0,  1,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  1,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  1,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0, -1,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1, -1,  0));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1, -1,  0));

  try_intersection (cyl1, {0, 1000, 0}, unit( 0,  0,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  0,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  0,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0,  1,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  1,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  1,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0, -1,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1, -1,  1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1, -1,  1));

  try_intersection (cyl1, {0, 1000, 0}, unit( 0,  0,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  0,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  0,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0,  1,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1,  1,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1,  1,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 0, -1,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit( 1, -1,  -1));
  try_intersection (cyl1, {0, 1000, 0}, unit(-1, -1,  -1));
}


int main()
{
  std::cout << "CylinderSurface_test\n";
  test_intersect();
  return 0;
}
