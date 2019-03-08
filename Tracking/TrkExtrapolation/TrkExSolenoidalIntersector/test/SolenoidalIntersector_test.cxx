/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkExSolenoidalIntersector/test/SolenoidalIntersector_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2019
 * @brief Regression tests for SolenoidalIntersector
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
  Amg::Vector3D pos1 { 0, 0, 1*meter };
  Amg::Vector3D norm1 { 0, 1, 1 };
  Trk::PlaneSurface plane1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 2*meter };
  Trk::PlaneSurface plane2 (transf (pos2, norm1));
  
  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,1,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (plane1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), {519.822, 493.238, 506.762});
  assertVec3D (isect1->direction(), {0.6068, 0.546315, 0.57735});
  assert( Athena_test::isEqual (isect1->pathlength(), 877.738) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1a
    (tool.intersectSurface (plane2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect1a->position(), {1080.723, 971.811, 1028.19});
  assertVec3D (isect1a->direction(), {0.634899, 0.513391, 0.57735});
  assert( Athena_test::isEqual (isect1a->pathlength(), 1780.876) );

  // SolenoidalIntersector fails for trajectories exactly along the Z axis.
  Trk::TrackSurfaceIntersection isect2
    (Amg::Vector3D{0,0,0}, unit(0,1e-3,-1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3
    (tool.intersectSurface (plane1, &isect2, 1/(10*GeV)));
  assertVec3D (isect3->position(), {0.0297868, -1.00041, 1001});
  assertVec3D (isect3->direction(), {-5.95611e-05, 0.000998224, -1});
  assert( Athena_test::isEqual (isect3->pathlength(), -1001) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect3a
    (tool.intersectSurface (plane2, isect3.get(), 1/(10*GeV)));
  assertVec3D (isect3a->position(), {0.1207914, -1.997095, 2002});
  assertVec3D (isect3a->direction(), {-0.000124302, 0.000992244, -1});
  assert( Athena_test::isEqual (isect3a->pathlength(), -2002) );

  // Test fallback to RK intersector.
  Amg::Vector3D pos3 { 0, 0, 5*meter };
  Trk::PlaneSurface plane3 (transf (pos3, norm1));
  Trk::TrackSurfaceIntersection isect4
    (Amg::Vector3D{0,0,0}, unit(1,1,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect5
    (tool.intersectSurface (plane3, &isect4, 1/(10*GeV)));
  assertVec3D (isect5->position(), {2773.12, 2402.05, 2597.95});
  assertVec3D (isect5->direction(), {0.620947, 0.527586, 0.579722});
  assert( Athena_test::isEqual (isect5->pathlength(), 4496.37) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect6
    (Amg::Vector3D{0,0,0}, unit(0,1,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect7
    (tool.intersectSurface (plane2, &isect6, 1/(0.03*GeV)));
  assert (!isect7);
}


void test_line (Trk::IIntersector& tool)
{
  // This always delegates to RungeKuttaIntersector.

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
  Trk::CylinderSurface cyl1 (transf (pos1, norm1).release(), 0.2*meter, 10*meter);
  Trk::CylinderSurface cyl2 (transf (pos1, norm1).release(), 0.5*meter, 10*meter);

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (cyl1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), { 199.993, -1.69206, 200.002});
  assertVec3D (isect1->direction(), {0.707006, -0.01196054, 0.707107});
  assert( Athena_test::isEqual (isect1->pathlength(), 282.846) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (cyl2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {499.889, -10.55783, 500.037});
  assertVec3D (isect2->direction(), { 0.706479, -0.0297955, 0.707107});
  assert( Athena_test::isEqual (isect2->pathlength(), 707.159) );

  // Test fallback to RK intersector.
  Trk::CylinderSurface cyl3 (transf (pos1, norm1).release(), 2*meter, 10*meter);
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (cyl3, &isect3, 1/(10*GeV)));
  assertVec3D (isect4->position(), {1995.16, -139.11, 2001.34});
  assertVec3D (isect4->direction(), {0.703789, -0.0663505, 0.707304});
  assert( Athena_test::isEqual (isect4->pathlength(), 2830.13) );

  // SolenoidalIntersector doesn't really handle loopers for cylinders.
}


void test_disc (Trk::IIntersector& tool)
{
  std::cout << "test_disc\n";

  Amg::Vector3D pos1 { 0, 0, 1*meter };
  Amg::Vector3D norm1 { 0, 0, 1 };
  Trk::DiscSurface disc1 (transf (pos1, norm1));
  Amg::Vector3D pos2 { 0, 0, 2*meter };
  Trk::DiscSurface disc2 (transf (pos2, norm1));

  Trk::TrackSurfaceIntersection isect0
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect1
    (tool.intersectSurface (disc1, &isect0, 1/(10*GeV)));
  assertVec3D (isect1->position(), { 998.826, -41.9437, 1000});
  assertVec3D (isect1->direction(), { 0.70466, -0.0587754, 0.707107});
  assert( Athena_test::isEqual (isect1->pathlength(), 1414.214) );

  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect2
    (tool.intersectSurface (disc2, isect1.get(), 1/(10*GeV)));
  assertVec3D (isect2->position(), {1991.24, -162.928, 2000});
  assertVec3D (isect2->direction(), {0.698425, -0.1104626, 0.707107});
  assert( Athena_test::isEqual (isect2->pathlength(), 2828.43) );

  // Test fallback to RK intersector.
  Amg::Vector3D pos3 { 0, 0, 3*meter };
  Trk::DiscSurface disc3 (transf (pos3, norm1));
  Trk::TrackSurfaceIntersection isect3
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect4
    (tool.intersectSurface (disc3, &isect3, 1/(10*GeV)));
  assertVec3D (isect4->position(), {2988.08, -227.624, 3000});
  assertVec3D (isect4->direction(), { 0.703426, -0.0598764, 0.708242});
  assert( Athena_test::isEqual (isect4->pathlength(), 4241.17) );

  // Loopers.
  Trk::TrackSurfaceIntersection isect5
    (Amg::Vector3D{0,0,0}, unit(1,0,1), 0);
  std::unique_ptr<const Trk::TrackSurfaceIntersection> isect6
    (tool.intersectSurface (disc2, &isect5, 1/(0.03*GeV)));
  assert (!isect6);
}


void test_perigee (Trk::IIntersector& tool)
{
  // This always delegates to RungeKuttaIntersector.

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
  std::cout << "SolenoidalIntersector_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("SolenoidalIntersector_test.txt", svcloc);
  ToolHandle<Trk::IIntersector> tool ("Trk::SolenoidalIntersector");
  assert( tool.retrieve().isSuccess() );
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);
  
  test_plane (*tool);
  test_line (*tool);
  test_cylinder (*tool);
  test_disc (*tool);
  test_perigee (*tool);
  return 0;
}
