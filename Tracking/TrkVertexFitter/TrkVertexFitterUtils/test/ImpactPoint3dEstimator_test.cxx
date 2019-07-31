/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitterUtils/test/ImpactPoint3dEstimator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for ImpactPoint3dEstimator.
 */


#undef NDEBUG
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


std::unique_ptr<AmgSymMatrix(5)> cov5()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setIdentity();
  return m;
}


[[maybe_unused]]
void dumpPlaneSurface (const Trk::PlaneSurface& s)
{
  std::cout << "{" << s.center().x() << ", "
            << s.center().y() << ", "
            << s.center().z() << "} ";
  std::cout << "{" << s.normal().x() << ", "
            << s.normal().y() << ", "
            << s.normal().z() << "}\n";
}


void assertVec3D (const char* which,
                  const Amg::Vector3D& a,
                  const Amg::Vector3D& b)
{
  if ( ! Athena_test::isEqual (a.x(), b.x(), 2e-5) ||
       ! Athena_test::isEqual (a.y(), b.y(), 2e-5) ||
       ! Athena_test::isEqual (a.z(), b.z(), 2e-5) )
  {
    std::cerr << "TrkVKalVrtFitterTestAlg::assertVec3D mismatch " << which
              << ": ["
              << a.x() << ", "
              << a.y() << ", "
              << a.z() << "] / ["
              << b.x() << ", "
              << b.y() << ", "
              << b.z() << "]\n";
    std::abort();
  }
}


void checkPlaneSurface (const Trk::PlaneSurface& s,
                        const Amg::Vector3D& pos,
                        const Amg::Vector3D& norm)
{
  assertVec3D ("pos", s.center(), pos);
  assertVec3D ("normal", s.normal(), norm);
}


void test1 (Trk::IImpactPoint3dEstimator& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos1 { 2*mm, 1*mm, -10*mm };
  Amg::Vector3D mom1 { 400*MeV, 600*MeV, 200*MeV };
  Amg::Vector3D vert { 1.2*mm, 0.8*mm, -7*mm };

  Trk::Perigee p1 (pos1, mom1, 1, pos1, cov5().release());
  std::unique_ptr<Trk::PlaneSurface> s1 (tool.Estimate3dIP (&p1, &vert));
  checkPlaneSurface (*s1, vert, {0.53466, 0.801692, 0.267261});
  assert( Athena_test::isEqual (tool.getDistance(), 3.10391, 1e-5) );
  
  Trk::NeutralPerigee p2 (pos1, mom1, 1, pos1, cov5().release());
  std::unique_ptr<Trk::PlaneSurface> s2 (tool.Estimate3dIP (&p2, &vert));
  checkPlaneSurface (*s2, vert, {0.534522, 0.801784, 0.267261});
  assert( Athena_test::isEqual (tool.getDistance(), 3.10391, 1e-5) );

  // Other methods require full Extrapolator / TrackingGeometry machinery;
  // not tested here.
}


int main()
{
  std::cout << "TrkVertexFitterUtils/ImpactPoint3dEstimator_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexFitterUtils/TrkVertexFitterUtils_tests.txt", svcloc);

  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  ToolHandle<Trk::IImpactPoint3dEstimator> tool ("Trk::ImpactPoint3dEstimator");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
