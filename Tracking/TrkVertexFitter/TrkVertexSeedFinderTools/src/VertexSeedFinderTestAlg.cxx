/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderTools/src/VertexSeedFinderTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Algorithm for doing unit tests of the seed finder tools.
 */


#undef NDEBUG
#include "VertexSeedFinderTestAlg.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "StoreGate/WriteHandle.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/random.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <cassert>
#include <cmath>


using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


namespace {


AmgSymMatrix(5) cov5()
{
  AmgSymMatrix(5) m;
  m.setIdentity();
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


#if 0
std::ostream& printVec3D (const Amg::Vector3D& a)
{
  std::cout << a.x() << " " << a.y() << " " << a.z();
  return std::cout;
}
#endif
void assertVec3D (const char* which,
                  const Amg::Vector3D& a,
                  const std::vector<double>& v,
                  int offs = 0)
{
  if (v.size() - offs < 3) std::abort();
  if ( ! Athena_test::isEqual (a.x(), v[offs+0], 1e-5) ||
       ! Athena_test::isEqual (a.y(), v[offs+1], 1e-5) ||
       ! Athena_test::isEqual (a.z(), v[offs+2], 1e-5) )
  {
    std::cerr << "VertexSeedFinderTestAlg::assertVec3D mismatch " << which
              << ": ["
              << a.x() << ", "
              << a.y() << ", "
              << a.z() << "] / ["
              << v[offs+0] << ", "
              << v[offs+1] << ", "
              << v[offs+2] << "]\n";
    std::abort();
  }
}


void dumpVector (const std::vector<float>& v)
{
  std::cerr << "  [";
  for (float f : v) {
    std::cerr << f << ", ";
  }
  std::cerr << "]\n";
}
void failVector (const char* which,
                 const std::vector<float>& a,
                 const std::vector<float>& b)
{
  std::cerr << "VertexSeedFinderTestAlg::assertVector mismatch " << which
            << "\n";
  dumpVector (a);
  dumpVector (b);
  std::abort();
}
void assertVector (const char* which,
                   const std::vector<float>& a,
                   const std::vector<float>& b)
{
  if (a.size() != b.size()) {
    failVector (which, a, b);
  }
  for (size_t i=0; i < a.size(); i++) {
    if (! Athena_test::isEqual (a[i], b[i], 1e-5) ) {
      failVector (which, a, b);
    }
  }
}


} // anonymous namespace


namespace Trk {


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode VertexSeedFinderTestAlg::initialize()
{
  ATH_CHECK( m_finder.retrieve() );
  ATH_CHECK( m_mcEventCollectionKey.initialize (SG::AllowEmpty));
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode VertexSeedFinderTestAlg::execute()
{
  ATH_MSG_VERBOSE ("execute");
  const EventContext& ctx = Gaudi::Hive::currentContext();

  double vx = 0;
  double vy = 0;
  if (!m_priVert.empty()) {
    if (m_priVert.size() != 2) std::abort();
    vx = m_priVert[0];
    vy = m_priVert[1];
  }

  Amg::Vector3D pos0 { 0, 0, 0 };

  Amg::Vector3D pos1a { 2*mm, 1*mm, -10*mm };
  Amg::Vector3D mom1a { 400*MeV, 600*MeV, 200*MeV };
  Amg::Vector3D pos1b { 1*mm, 2*mm, -3*mm };
  Amg::Vector3D mom1b { 600*MeV, 400*MeV, -200*MeV };
  Amg::Vector3D pos1c { 1.2*mm, 1.3*mm, -7*mm };
  Amg::Vector3D mom1c { 300*MeV, 1000*MeV, 100*MeV };

  auto p1a = std::make_unique<Trk::Perigee>(pos1a, mom1a,  1, pos0, cov5());
  auto p1b = std::make_unique<Trk::Perigee>(pos1b, mom1b, -1, pos0, cov5());
  auto p1c = std::make_unique<Trk::Perigee>(pos1c, mom1c, -1, pos0, cov5());

  std::vector<const Trk::TrackParameters*> v1a { p1a.get(), p1b.get(), p1c.get() };
  std::vector<const Trk::TrackParameters*> v1b { p1c.get(), p1a.get(), p1b.get() };

  if (!m_expected1.empty()) {
    ATH_MSG_VERBOSE ("testing 1");
    Amg::Vector3D p = m_finder->findSeed (vx, vy, v1a);
    assertVec3D ("1a", p,  m_expected1);

    std::unique_ptr<Trk::IMode3dInfo> info;
    p = m_finder->findSeed (vx, vy, info, v1b);
    assertVec3D ("1b", p,  m_expected1);

    if (!m_expected1PhiModes.empty()) {
      std::vector<float> phi;
      std::vector<float> r;
      std::vector<float> z;
      std::vector<float> w;
      size_t sz = info->Modes1d (phi, r, z, w);
      assert (sz == phi.size());
      assert (sz == r.size());
      assert (sz == z.size());
      assert (sz == w.size());
      assertVector ("phiModes", phi, m_expected1PhiModes);
      assertVector ("rModes",     r, m_expected1RModes);
      assertVector ("zModes",     z, m_expected1ZModes);
      assertVector ("weights",    w, m_expected1Weights);
    }

    if (!m_expected1Indices.empty()) {
      std::vector<const Trk::TrackParameters*> p;
      size_t sz = info->perigeesAtSeed (p, v1a);
      assert (sz == p.size());
      std::vector<int> ndx;
      for (const Trk::TrackParameters* pp : p) {
        auto it = std::find (v1a.begin(), v1a.end(), pp);
        assert (it != v1a.end());
        ndx.push_back (it - v1a.begin());
      }
      assert (ndx == m_expected1Indices);
    }

    if (!m_expected1CorrDist.empty()) {
      double cXY = 0;
      double cZ = 0;
      info->getCorrelationDistance (cXY, cZ);
      assert (Athena_test::isEqual (cXY, m_expected1CorrDist[0], 1e-5));
      assert (Athena_test::isEqual (cZ, m_expected1CorrDist[1], 1e-5));
    }
  }

  xAOD::Vertex vert1;
  initVertex (vert1);

  if (!m_expected2.empty()) {
    ATH_MSG_VERBOSE ("testing 2");
    Amg::Vector3D p = m_finder->findSeed (vx, vy, v1a, &vert1);
    assertVec3D ("2a", p, m_expected2);

    p = m_finder->findSeed (vx, vy, v1b, &vert1);
    assertVec3D ("2b", p, m_expected2);
  }

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
  for (unsigned int i=0; i < m_npart3; i++) {
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
                                                           cov5()));
    pvec.push_back (perigees.back().get());
  }

  if (!m_mcEventCollectionKey.empty()) {
    ATH_CHECK( makeMcEventCollection(ctx) );
  }

  if (!m_expected3.empty()) {
    ATH_MSG_VERBOSE ("testing 3");
    if (m_expected3.size() == 3) {
      Amg::Vector3D p = m_finder->findSeed (vx, vy, pvec);
      assertVec3D ("3a", p, m_expected3);
    }
    else {
      std::vector<Amg::Vector3D> seeds = m_finder->findMultiSeeds (pvec);
      if (seeds.size() != m_expected3.size()/3) {
        std::cerr << "VertexSeedFinderTestAlg size mismatch 3b "
                  << seeds.size() << " / " <<  m_expected3.size()/3 << "\n";
        std::abort();
      }
      int i = 0;
      for (const auto& seed : seeds) {
        assertVec3D ("3b", seed, m_expected3, i);
        i += 3;
      }
    }
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Make a test McEventCollection.
 */
StatusCode
VertexSeedFinderTestAlg::makeMcEventCollection (const EventContext& ctx) const
{
  auto evt1 = new HepMC::GenEvent();
  auto evt2 = new  HepMC::GenEvent();
  auto evt3 = new HepMC::GenEvent();

  HepMC::set_signal_process_vertex (evt1,HepMC::newGenVertexPtr(HepMC::FourVector{1*mm,   2*mm,  12*mm,0.0}));
  HepMC::set_signal_process_vertex (evt2,HepMC::newGenVertexPtr(HepMC::FourVector{0.3*mm,-0.7*mm,-3*mm,0.0}));
  HepMC::set_signal_process_vertex (evt3,HepMC::newGenVertexPtr(HepMC::FourVector{0.6*mm, 0.2*mm, 7*mm,0.0}));
 
  Athena_test::uniform_real_distribution<double> ptdist (0.1*GeV, 100*GeV);
  Athena_test::uniform_real_distribution<double> phidist (-M_PI, M_PI);
  Athena_test::uniform_real_distribution<double> etadist (-4, 4);
  Athena_test::URNG rng (123);
  for (unsigned int i = 0; i < 1000; i++) {
    double pt = ptdist(rng);
    double phi = phidist(rng);
    double eta = etadist(rng);
    double e = pt*sinh(eta);
    double charge = etadist(rng) > 0 ? 1 : -1;
    HepMC::FourVector mom {pt*cos(phi), pt*sin(phi), e, e };
    auto p = HepMC::newGenParticlePtr ( mom, charge * 11, 1 );

    double vrand = etadist(rng);
    if (vrand < 0) {
      HepMC::signal_process_vertex(evt1)->add_particle_out (p);
    }
    else if (vrand < 3) {
      HepMC::signal_process_vertex(evt2)->add_particle_out (p);
    }
    else {
      HepMC::signal_process_vertex(evt3)->add_particle_out (p);
    }
  }

  auto evtcoll = std::make_unique<McEventCollection>();
  evtcoll->push_back (std::move (evt1));
  evtcoll->push_back (std::move (evt2));
  evtcoll->push_back (std::move (evt3));

  SG::WriteHandle<McEventCollection> h (m_mcEventCollectionKey, ctx);
  ATH_CHECK( h.record (std::move (evtcoll)) );
  return StatusCode::SUCCESS;
}


} // namespace Trk
