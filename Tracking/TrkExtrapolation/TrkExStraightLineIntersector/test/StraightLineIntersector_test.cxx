/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkExStraightLineIntersector/test/StraghtLineIntersector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2019
 * @brief Regression tests for StraightLineIntersector
 */

#undef NDEBUG
#include "TrkExInterfaces/IIntersector.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/ToolHandle.h"
#include <iostream>
#include <cassert>
#include <cmath>


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  FLOAT_EQassert (a.x(), b.x());
  FLOAT_EQassert (a.y(), b.y());
  FLOAT_EQassert (a.z(), b.z());
}


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


void test_plane (Trk::IIntersector& tool)
{
  std::cout << "test_plane\n";
  Amg::Vector3D pos1 { 0, 0, 100 };
  Amg::Vector3D norm1 { 0, 1, 1 };
  Trk::PlaneSurface plane1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 200 };
  Trk::PlaneSurface plane2 (transf (pos2, norm1));
  
  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,1,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (plane1, &isect0, 1));
  assertVec3D (isect1->position(), {50, 50, 50});
  assertVec3D (isect1->direction(), {1/sqrt(3.), 1/sqrt(3.), 1/sqrt(3.)});
  FLOAT_EQassert (isect1->pathlength(), 50*sqrt(3.));

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1a
    (tool.intersectSurface (plane2, isect1.get(), 1));
  assertVec3D (isect1a->position(), {100, 100, 100});
  assertVec3D (isect1a->direction(), {1/sqrt(3.), 1/sqrt(3.), 1/sqrt(3.)});
  FLOAT_EQassert (isect1a->pathlength(), 100*sqrt(3.));

  Trk::TrackSurfaceIntersection isect2
    (Amg::Vector3D{0,0,0}, unit(0,0,-1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3
    (tool.intersectSurface (plane1, &isect2, 1));
  assertVec3D (isect3->position(), {0, 0, 100});
  assertVec3D (isect3->direction(), {0, 0, -1});
  FLOAT_EQassert (isect3->pathlength(), -100);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3a
    (tool.intersectSurface (plane2, isect3.get(), 1));
  assertVec3D (isect3a->position(), {0, 0, 200});
  assertVec3D (isect3a->direction(), {0, 0, -1});
  FLOAT_EQassert (isect3a->pathlength(), -200);
}


void test_line (Trk::IIntersector& tool)
{
  std::cout << "test_line\n";
  Amg::Vector3D pos1 { 0, 0, 100 };
  Amg::Vector3D norm1 { 0, 1, 0 };
  Trk::StraightLineSurface line1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 200 };
  Trk::StraightLineSurface line2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (line1, &isect0, 1));
  assertVec3D (isect1->position(), {50, 0, 50});
  assertVec3D (isect1->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect1->pathlength(), 50*sqrt(2.));

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (line2, isect1.get(), 1));
  assertVec3D (isect2->position(), {100, 0, 100});
  assertVec3D (isect2->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect2->pathlength(), 100*sqrt(2.));
}


void test_cylinder (Trk::IIntersector& tool)
{
  std::cout << "test_cylinder\n";

  Amg::Vector3D pos1 { 0, 0, 0 };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::CylinderSurface cyl1 (transf (pos1, norm1).release(),  50, 100);
  Trk::CylinderSurface cyl2 (transf (pos1, norm1).release(), 200, 100);

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (cyl1, &isect0, 1));
  assertVec3D (isect1->position(), {50, 0, 50});
  assertVec3D (isect1->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect1->pathlength(), 50*sqrt(2.));

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (cyl2, isect1.get(), 1));
  assertVec3D (isect2->position(), {200, 0, 200});
  assertVec3D (isect2->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect2->pathlength(), 200*sqrt(2.));
}


void test_disc (Trk::IIntersector& tool)
{
  std::cout << "test_disc\n";

  Amg::Vector3D pos1 { 0, 0, 75 };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::DiscSurface disc1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 200 };
  Trk::DiscSurface disc2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (disc1, &isect0, 1));
  assertVec3D (isect1->position(), {75, 0, 75});
  assertVec3D (isect1->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect1->pathlength(), 75*sqrt(2.));

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (disc2, isect1.get(), 1));
  assertVec3D (isect2->position(), {200, 0, 200});
  assertVec3D (isect2->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect2->pathlength(), 200*sqrt(2.));
}


void test_perigee (Trk::IIntersector& tool)
{
  std::cout << "test_perigee\n";

  Amg::Vector3D pos1 { 0, 0, 80 };
  Amg::Vector3D norm1 { 0, 1, 0 };
  Trk::PerigeeSurface perigee1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 200 };
  Trk::PerigeeSurface perigee2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (perigee1, &isect0, 1));
  assertVec3D (isect1->position(), {40, 0, 40});
  assertVec3D (isect1->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect1->pathlength(), 40*sqrt(2.));

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (perigee2, isect1.get(), 1));
  assertVec3D (isect2->position(), {100, 0, 100});
  assertVec3D (isect2->direction(), {1/sqrt(2.), 0, 1/sqrt(2.)});
  FLOAT_EQassert (isect2->pathlength(), 100*sqrt(2.));
}


int main()
{
  std::cout << "StraightLineIntersector_test\n";
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi (svcloc);
  ToolHandle<Trk::IIntersector> tool ("Trk::StraightLineIntersector");
  assert( tool.retrieve().isSuccess() );

  test_plane (*tool);
  test_line (*tool);
  test_cylinder (*tool);
  test_disc (*tool);
  test_perigee (*tool);
  return 0;
}
