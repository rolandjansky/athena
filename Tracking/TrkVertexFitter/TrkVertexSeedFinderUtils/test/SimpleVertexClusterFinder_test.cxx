/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/SimpleVertexClusterFinder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Simple unit test for SimpleVertexClusterFinder.
 */


#undef NDEBUG
#include "TrkVertexSeedFinderUtils/SimpleVertexClusterFinder.h"
#include "TrkVertexSeedFinderUtils/VertexImageMaker.h"
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


void initVertex (xAOD::Vertex& v)
{
  v.makePrivateStore();
  v.setX (1.7);
  v.setY (1.3);
  v.setZ (-6);

  AmgSymMatrix(3) cov;
  cov.setIdentity();
  v.setCovariancePosition (cov);
}


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x(), 1e-5) );
  assert( Athena_test::isEqual (a.y(), b.y(), 1e-5) );
  assert( Athena_test::isEqual (a.z(), b.z(), 1e-5) );
}


#if 0
std::ostream& printVec3D (const Amg::Vector3D& a)
{
  std::cout << a.x() << " " << a.y() << " " << a.z();
  return std::cout;
}
#endif
void test1 (Trk::VertexImageMaker& tool1, Trk::SimpleVertexClusterFinder& tool2)
{
  std::cout << "test1\n";

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

  xAOD::Vertex vert1;
  initVertex (vert1);

  std::unique_ptr<Trk::VertexImage> image = tool1.makeVertexImage(pvec, &vert1);
  std::vector<Amg::Vector3D> verts = tool2.findVertexClusters (*image);
  assert (verts.size() == 4);
  assertVec3D (verts[0], { -0.778228, -1.80779, 17.8691  });
  assertVec3D (verts[1], { -0.721533, -1.79632,  2.88162 });
  assertVec3D (verts[2], { -0.756713,  1.85018, 17.9267  });
  assertVec3D (verts[3], { -0.788483,  1.85138,  2.82426 });
#if 0
  for (const auto& vv : verts) {
    printVec3D (vv) << "\n";
  }
#endif
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/SimpleVertexClusterFinder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);

  ToolHandle<Trk::VertexImageMaker> tool1 ("Trk::VertexImageMaker");
  assert( tool1.retrieve().isSuccess() );
  ToolHandle<Trk::SimpleVertexClusterFinder> tool2 ("Trk::SimpleVertexClusterFinder");
  assert( tool2.retrieve().isSuccess() );

  test1 (*tool1, *tool2);

  return 0;
}
