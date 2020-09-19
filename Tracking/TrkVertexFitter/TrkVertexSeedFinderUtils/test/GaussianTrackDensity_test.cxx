/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/GaussianTrackDensity_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Simple unit test for GaussianTrackDensity.
 */


#undef NDEBUG
#include "TrkVertexSeedFinderUtils/GaussianTrackDensity.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/expect_exception.h"
#include "TestTools/random.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
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


// Very basic test.
void test1 (Trk::GaussianTrackDensity& tool)
{
  std::cout << "test1\n";
  Amg::Vector3D pos1a { 2*mm, 1*mm, -10*mm };
  Amg::Vector3D mom1a { 400*MeV, 600*MeV, 200*MeV };
  Amg::Vector3D pos1b { 1*mm, 2*mm, -3*mm };
  Amg::Vector3D mom1b { 600*MeV, 400*MeV, -200*MeV };
  Amg::Vector3D pos1c { 1.2*mm, 1.3*mm, -7*mm };
  Amg::Vector3D mom1c { 300*MeV, 1000*MeV, 100*MeV };

  Amg::Vector3D pos0 { 0, 0, 0 };

  Trk::Perigee p1a (pos1a, mom1a,  1, pos0, cov5().release());
  Trk::Perigee p1b (pos1b, mom1b, -1, pos0, cov5().release());
  Trk::Perigee p1c (pos1c, mom1c, -1, pos0, cov5().release());

  std::vector<const Trk::TrackParameters*> v1 { &p1a, &p1b, &p1c };
  std::unique_ptr<Trk::IVertexTrackDensityEstimator::ITrackDensity> density;
  double globalMaximum = tool.globalMaximum (v1, density);
  assert( Athena_test::isEqual (globalMaximum,  -7.01305, 1e-5) );
  assert( Athena_test::isEqual (density->trackDensity(1),  4.38256e-06, 1e-5) );
  assert( Athena_test::isEqual (density->trackDensity(-1), 0.00176805, 1e-5) );

  double d, d1, d2;
  density->trackDensity (1, d, d1, d2);
  assert( Athena_test::isEqual (d, 4.38256e-06, 1e-5) );
  assert( Athena_test::isEqual (d1, -1.75302e-05, 1e-5) );
  assert( Athena_test::isEqual (d2,  6.57384e-05, 1e-5) );
  density->trackDensity (-1, d, d1, d2);
  assert( Athena_test::isEqual (d, 0.00176805, 1e-5) );
  assert( Athena_test::isEqual (d1, -0.00353611, 1e-5) );
  assert( Athena_test::isEqual (d2,  0.00530417, 1e-5) );
}


// Test a larger set of tracks.
void test2 (Trk::GaussianTrackDensity& tool)
{
  std::cout << "test2\n";

  Amg::Vector3D pos0 { 0, 0, 0 };

  std::vector<std::unique_ptr<Trk::Perigee> > perigees;
  std::vector<const Trk::TrackParameters*> pvec;
  Athena_test::normal_distribution<double> xdist (1*mm, 0.1*mm);
  Athena_test::normal_distribution<double> ydist (-0.7*mm, 0.1*mm);
  Athena_test::normal_distribution<double> z1dist (12*mm, 1*mm);
  Athena_test::normal_distribution<double> z2dist (-3*mm, 0.5*mm);
  Athena_test::uniform_real_distribution<double> ptdist (0.1*GeV, 100*GeV);
  Athena_test::uniform_real_distribution<double> phidist (-M_PI, M_PI);
  Athena_test::uniform_real_distribution<double> etadist (-4, 4);
  Athena_test::URNG rng;
  for (unsigned int i=0; i < 200; i++) {
    double x = xdist(rng);
    double y = ydist(rng);
    double z;
    if ((i%3) == 0) {
      z = z2dist(rng);
    }
    else {
      z = z1dist(rng);
    }
      
    Amg::Vector3D pos { x, y, z };
    double pt = ptdist(rng);
    double phi = phidist(rng);
    double eta = etadist(rng);
    Amg::Vector3D mom { pt*cos(phi), pt*sin(phi), pt*sinh(eta) };
    double charge = etadist(rng) > 0 ? 1 : -1;
    perigees.emplace_back (std::make_unique<Trk::Perigee> (pos, mom, charge, pos0,
                                                           cov5().release()));
    pvec.push_back (perigees.back().get());
  }

  std::unique_ptr<Trk::IVertexTrackDensityEstimator::ITrackDensity> density;
  double globalMaximum = tool.globalMaximum (pvec, density);

  assert( Athena_test::isEqual (globalMaximum,  12.0446, 1e-5) );
  assert( Athena_test::isEqual (density->trackDensity(1),  0.00755785, 1e-5) );
  assert( Athena_test::isEqual (density->trackDensity(-1), 0.927959, 1e-5) );

  double d, d1, d2;
  density->trackDensity (1, d, d1, d2);
  assert( Athena_test::isEqual (d, 0.00755785, 1e-5) );
  assert( Athena_test::isEqual (d1, -0.0244075, 1e-5) );
  assert( Athena_test::isEqual (d2, 0.0724778, 1e-5) );
  density->trackDensity (-1, d, d1, d2);
  assert( Athena_test::isEqual (d, 0.927959, 1e-5) );
  assert( Athena_test::isEqual (d1, -1.47783, 1e-5) );
  assert( Athena_test::isEqual (d2,  1.60754, 1e-5) );
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/GaussianTrackDensity_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);

  ToolHandle<Trk::GaussianTrackDensity> tool ("Trk::GaussianTrackDensity");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);
  test2 (*tool);

  return 0;
}
