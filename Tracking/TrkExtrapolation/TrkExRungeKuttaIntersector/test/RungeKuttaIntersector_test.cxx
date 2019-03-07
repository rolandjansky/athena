/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkExRungeKuttaIntersector/test/RungeKuttaIntersector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2019
 * @brief Regression tests for RungeKuttaIntersector
 */

#undef NDEBUG
#include "TrkExInterfaces/IIntersector.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "AthenaKernel/Units.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


using Athena::Units::meter;
using Athena::Units::GeV;


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x()) );
  assert( Athena_test::isEqual (a.y(), b.y()) );
  assert( Athena_test::isEqual (a.z(), b.z()) );
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
  Amg::Vector3D pos1 { 0, 0, 5*meter };
  Amg::Vector3D norm1 { 0, 1, 1 };
  Trk::PlaneSurface plane1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 10*meter };
  Trk::PlaneSurface plane2 (transf (pos2, norm1));
  
  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,1,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (plane1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {2773.12, 2402.05, 2597.95});
  assertVec3D (isect1->direction(), {0.620947, 0.527586, 0.579722});
  assert( Athena_test::isEqual (isect1->pathlength(), 4496.37) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1a
    (tool.intersectSurface (plane2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect1a->position(), {5497.63, 4749.85, 5250.15});
  assertVec3D (isect1a->direction(), {0.596962, 0.513091, 0.616745});
  assert( Athena_test::isEqual (isect1a->pathlength(), 8965.55) );

  Trk::TrackSurfaceIntersection isect2
    (Amg::Vector3D{0,0,0}, unit(0,0,-1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3
    (tool.intersectSurface (plane1, &isect2, 1/(10*GeV)));
  assertVec3D (isect3->position(), {-0.1490986, -0.287689, 5000.29});
  assertVec3D (isect3->direction(), {2.288634e-05, 0.000105063, -1});
  assert( Athena_test::isEqual (isect3->pathlength(), -5000.29) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3a
    (tool.intersectSurface (plane2, isect3.get(), 1/(10*GeV)));
  assertVec3D (isect3a->position(), {-0.35816, -0.796257, 10000.8});
  assertVec3D (isect3a->direction(), {5.16553e-05, 0.0001023313, -1});
  assert( Athena_test::isEqual (isect3a->pathlength(), -10000.8) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect4
    (Amg::Vector3D{0,0,0}, unit(1,0,-1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4a
    (tool.intersectSurface (plane1, &isect4, 1/(0.01*GeV)));
  assert (!isect4a);

  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(1,0,-1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (plane2, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


void test_line (Trk::IIntersector& tool)
{
  std::cout << "test_line\n";
  Amg::Vector3D pos1 { 0, 0, 5*meter };
  Amg::Vector3D norm1 { 0, 1, 0 };
  Trk::StraightLineSurface line1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 10*meter };
  Trk::StraightLineSurface line2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (line1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {2502.7, -185.58, 2511.46});
  assertVec3D (isect1->direction(), { 0.703713, -0.0626336, 0.707718});
  assert( Athena_test::isEqual (isect1->pathlength(), 3551.23) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (line2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {5059.02, -413.306, 5102.03});
  assertVec3D (isect2->direction(), { 0.693974, -0.0678801, 0.716793});
  assert( Athena_test::isEqual (isect2->pathlength(), 7197.76) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (line2, &isect3, 1/(0.01*GeV)));
  assert (!isect4);

  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (line2, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


void test_cylinder (Trk::IIntersector& tool)
{
  std::cout << "test_cylinder\n";

  Amg::Vector3D pos1 { 0, 0, 0 };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::CylinderSurface cyl1 (transf (pos1, norm1).release(), 2*meter, 10*meter);
  Trk::CylinderSurface cyl2 (transf (pos1, norm1).release(), 5*meter, 10*meter);

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (cyl1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {1995.16, -139.11, 2001.34});
  assertVec3D (isect1->direction(), {0.703789, -0.0663505, 0.707304});
  assert( Athena_test::isEqual (isect1->pathlength(), 2830.13) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (cyl2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {4983.49, -405.934, 5024.11});
  assertVec3D (isect2->direction(), {0.694755, -0.0676002, 0.716063});
  assert( Athena_test::isEqual (isect2->pathlength(), 7089.10) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (cyl1, &isect3, 1/(0.01*GeV)));
  assert (!isect4);

  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (cyl1, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


void test_disc (Trk::IIntersector& tool)
{
  std::cout << "test_disc\n";

  Amg::Vector3D pos1 { 0, 0, 3*meter };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::DiscSurface disc1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 6*meter };
  Trk::DiscSurface disc2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (disc1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {2988.08, -227.624, 3000});
  assertVec3D (isect1->direction(), { 0.703426, -0.0598764, 0.708242});
  assert( Athena_test::isEqual (isect1->pathlength(), 4241.17) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (disc2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {5915.1, -499.253, 6000});
  assertVec3D (isect2->direction(), {0.682016, -0.0699434, 0.727985});
  assert( Athena_test::isEqual (isect2->pathlength(), 8441.51) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (disc1, &isect3, 1/(0.01*GeV)));
  assert (!isect4);

  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(100,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (disc2, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


void test_perigee (Trk::IIntersector& tool)
{
  std::cout << "test_perigee\n";

  Amg::Vector3D pos1 { 0, 0, 4*meter };
  Amg::Vector3D norm1 { 0, 1, 0 };
  Trk::PerigeeSurface perigee1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 8*meter };
  Trk::PerigeeSurface perigee2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (perigee1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {2001.86, -139.7423, 2008.08});
  assertVec3D (isect1->direction(), { 0.703793, -0.0662967, 0.707305});
  assert( Athena_test::isEqual (isect1->pathlength(), 2839.65) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (perigee2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {4011.21, -314.772, 4031.51});
  assertVec3D (isect2->direction(), {0.70198, -0.0614824, 0.709537});
  assert( Athena_test::isEqual (isect2->pathlength(), 5696.65) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (perigee1, &isect3, 1/(0.01*GeV)));
  assert (!isect4);

  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (perigee1, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


int main()
{
  std::cout << "RungeKuttaIntersector_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("RungeKuttaIntersector_test.txt", svcloc);
  ToolHandle<Trk::IIntersector> tool ("Trk::RungeKuttaIntersector");
  assert( tool.retrieve().isSuccess() );
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc);
  
  test_plane (*tool);
  test_line (*tool);
  test_cylinder (*tool);
  test_disc (*tool);
  test_perigee (*tool);
  return 0;
}
