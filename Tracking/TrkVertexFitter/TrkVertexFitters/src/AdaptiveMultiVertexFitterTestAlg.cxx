/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitters/src/AdaptiveMultiVertexFitterTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing AdaptiveMultiVertexFitter.
 */


#undef NDEBUG
#include "AdaptiveMultiVertexFitterTestAlg.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <cassert>


#include "CLHEP/Vector/LorentzVector.h"

using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


namespace {


std::unique_ptr<AmgSymMatrix(5)> cov5()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setIdentity();
  return m;
}


std::unique_ptr<AmgSymMatrix(5)> cov5a()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setZero();
  for (int i=0; i < 5; i++) {
    (*m)(i,i) = 1e-2;
  }
  (*m)(1,1)=1;
  return m;
}


using PerigeeUVec_t = std::vector<std::unique_ptr<Trk::Perigee> >;
PerigeeUVec_t makePerigees1()
{
  Amg::Vector3D pos0 { 0, 0, 0 };

  Amg::Vector3D pos1a { 2*mm, 1*mm, -10*mm };
  Amg::Vector3D mom1a { 400*MeV, 600*MeV, 200*MeV };
  Amg::Vector3D pos1b { 1*mm, 2*mm, -3*mm };
  Amg::Vector3D mom1b { 600*MeV, 400*MeV, -200*MeV };
  Amg::Vector3D pos1c { 1.2*mm, 1.3*mm, -7*mm };
  Amg::Vector3D mom1c { 300*MeV, 1000*MeV, 100*MeV };

  PerigeeUVec_t ret;

  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1a, mom1a,  1, pos1a, cov5().release()));
  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1b, mom1b, -1, pos1b, cov5().release()));
  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1c, mom1c, -1, pos1c, cov5().release()));

  return ret;
}


using PerigeeUVec_t = std::vector<std::unique_ptr<Trk::Perigee> >;
PerigeeUVec_t makePerigees2()
{
  Amg::Vector3D pos1a { 10*mm,   0*mm, -5*mm };
  Amg::Vector3D mom1a { 1000*MeV, 0*MeV, 0*MeV };
  Amg::Vector3D pos1b { 10.5*mm, 0.5*mm, -5.5*mm };
  Amg::Vector3D mom1b { 800*MeV, 200*MeV, 200*MeV };
  Amg::Vector3D pos1c { 9.5*mm, -0.5*mm, -4.5*mm };
  Amg::Vector3D mom1c { 700*MeV, -300*MeV, -200*MeV };

  PerigeeUVec_t ret;

  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1a, mom1a,  1, pos1a, cov5a().release()));
  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1b, mom1b, -1, pos1a, cov5a().release()));
  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1c, mom1c, -1, pos1a, cov5a().release()));

  return ret;
}


using TrackUVec_t = std::vector<std::unique_ptr<Trk::Track> >;
TrackUVec_t makeTracks (const PerigeeUVec_t& perigees)
{
  TrackUVec_t tracks;

  for (const std::unique_ptr<Trk::Perigee>& p : perigees) {
    Trk::TrackInfo info (Trk::TrackInfo::Unknown, Trk::undefined);
    auto fqual = std::make_unique<Trk::FitQuality> (0, 0);
    auto tsos = std::make_unique<DataVector<const Trk::TrackStateOnSurface> >();
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    tsos->push_back (std::make_unique<Trk::TrackStateOnSurface>
                     (nullptr, p->clone(), nullptr, nullptr, typePattern));
    tracks.push_back (std::make_unique<Trk::Track> (info,
                                                    tsos.release(),
                                                    fqual.release()));
  }

  return tracks;
}


void dumpCovariance (const AmgSymMatrix(5)& m)
{
  for (int i=0; i < 5; i++) {
    for (int j=0; j < 5; j++) {
      std::cout << m(i,j) << ", ";
    }
  }
}


[[maybe_unused]]
void dumpVertex (const xAOD::Vertex& v)
{
  std::cout << "vvv\n";
  std::cout << v.x() << ", " << v.y() << ", " << v.z() << "\n";
  if (v.isAvailable<short> ("vertexType")) {
    std::cout << "vertexType " << v.vertexType() << "\n";
  }
  std::cout << "chi2/ndof " << v.chiSquared() << ", " << v.numberDoF() << "\n";

  std::cout << "cov ";
  for (float f : v.covariance()) {
    std::cout << f << ", ";
  }
  std::cout << "\n";

  if (v.isAvailable<std::vector<ElementLink<xAOD::TrackParticleContainer> > > ("trackParticleLinks")) {
    std::cout << "tplinks ";
    for (const ElementLink< xAOD::TrackParticleContainer >& l : v.trackParticleLinks()) {
      std::cout << l.dataID() << "/" << l.index() << " ";
    }
    std::cout << "\n";
  }

  if (v.isAvailable<std::vector<float> > ("trackWeights")) {
    std::cout << "wt ";
    for (float f : v.trackWeights()) {
      std::cout << f << " ";
    }
    std::cout << "\n";
  }

  if (v.isAvailable<std::vector<ElementLink<xAOD::NeutralParticleContainer> > > ("neutralParticleLinks")) {
    std::cout << "nplinks ";
    for (const ElementLink< xAOD::NeutralParticleContainer >& l : v.neutralParticleLinks()) {
      std::cout << l.dataID() << "/" << l.index() << " ";
    }
    std::cout << "\n";
  }

  if (v.isAvailable<std::vector<float> > ("neutralWeights")) {
    std::cout << "wt ";
    for (float f : v.neutralWeights()) {
      std::cout << f << " ";
    }
    std::cout << "\n";
  }

  std::cout << v.vxTrackAtVertexAvailable() << "\n";
  for (const Trk::VxTrackAtVertex& vv : v.vxTrackAtVertex()) {
    vv.dump (std::cout);
    std::cout << "cov ";
    if (vv.perigeeAtVertex()) {
      dumpCovariance (*vv.perigeeAtVertex()->covariance());
    }
    else {
      std::cout << "(null)";
    }
    std::cout << "\n";
  }
}


void assertVec3D (const char* which,
                  const Amg::Vector3D& a,
                  const Amg::Vector3D& b,
                  double thresh = 2e-5)
{
  if ( ! Athena_test::isEqual (a.x(), b.x(), thresh) ||
       ! Athena_test::isEqual (a.y(), b.y(), thresh) ||
       ! Athena_test::isEqual (a.z(), b.z(), thresh) )
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


void compareVertex (const xAOD::Vertex& a, const xAOD::Vertex& b)
{
  assertVec3D ("vertex pos", a.position(), b.position(), 5e-4);
  assert (Athena_test::isEqual (a.chiSquared(), b.chiSquared(), 5e-3) );
  assert (Athena_test::isEqual (a.numberDoF(), b.numberDoF(), 5e-3) );

  assert (a.covariance().size() == b.covariance().size());
  for (unsigned int i = 0; i < a.covariance().size(); i++) {
    if (isinf(a.covariance()[i]) && isinf(b.covariance()[i])) continue;
    assert (Athena_test::isEqual (a.covariance()[i], b.covariance()[i], 2e-2) );
  }
}


void setInitialPerigee (xAOD::Vertex& v, unsigned i, const Trk::Perigee* p)
{
  std::vector< Trk::VxTrackAtVertex >& vec = v.vxTrackAtVertex();
  if (vec.size() <= i) vec.resize(i+1);
  vec[i].setInitialPerigee (p);
}


void setInitialPerigees (xAOD::Vertex& v, const TrackUVec_t& tracks)
{
  int i = 0;
  for (const std::unique_ptr<Trk::Track>& t : tracks) {
    setInitialPerigee (v, i, t->perigeeParameters());
    ++i;
  }
}


struct VertexInfo
{
  xAOD::Vertex v;
  TrackUVec_t tracks;
  PerigeeUVec_t perigees;
  std::unique_ptr<Trk::MvfFitInfo> fi;
  std::vector<Trk::VxTrackAtVertex*> vtracks;
  std::vector<Trk::TrackToVtxLink> links;
};


void initVertex (VertexInfo& vi,
                 const Amg::Vector3D& pos,
                 PerigeeUVec_t&& perigees)
{
  xAOD::Vertex::Decorator< Trk::MvfFitInfo* > MvfFitInfo("MvfFitInfo");
  xAOD::Vertex::Decorator< bool > isInitialized("isInitialized");
  xAOD::Vertex::Decorator< std::vector<Trk::VxTrackAtVertex*> > VTAV("VTAV");

  vi.perigees = std::move (perigees);

  vi.v.makePrivateStore();
  vi.v.setPosition (pos);

  vi.tracks = makeTracks (vi.perigees);
  setInitialPerigees (vi.v, vi.tracks);

  auto cv = std::make_unique<xAOD::Vertex>();
  cv->makePrivateStore();
  cv->setPosition (pos);
  Amg::MatrixX looseConstraintCovariance(3, 3);
  looseConstraintCovariance.setIdentity();
  looseConstraintCovariance = looseConstraintCovariance * 1e+8;
  cv->setCovariancePosition (looseConstraintCovariance);
  cv->setFitQuality (0, -3);
  vi.fi = std::make_unique<Trk::MvfFitInfo>  (cv.release(),
                                              new Amg::Vector3D(pos),
                                              new Amg::Vector3D(pos));
  MvfFitInfo(vi.v) = vi.fi.get();
  isInitialized(vi.v) = false;
  
  vi.links.reserve (vi.perigees.size());
  for (std::unique_ptr<Trk::Perigee>& p : vi.perigees) {
    vi.links.push_back (new std::vector<xAOD::Vertex*> {&vi.v});
    auto tav = std::make_unique<Trk::MVFVxTrackAtVertex>(1.5, p->clone(), p.get());
    tav->setLinkToVertices (&vi.links.back());
    vi.vtracks.push_back (tav.release());
  }
  VTAV(vi.v) = vi.vtracks;
}


} // anonymous namespace


namespace Trk {


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode AdaptiveMultiVertexFitterTestAlg::initialize()
{
  ATH_CHECK( m_fitter.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode AdaptiveMultiVertexFitterTestAlg::execute()
{
  ATH_MSG_VERBOSE ("execute");

  ATH_CHECK( test1() );

  return StatusCode::SUCCESS;
}


StatusCode AdaptiveMultiVertexFitterTestAlg::test1()
{
  VertexInfo v1;
  initVertex (v1, {1.5*mm, 1.7*mm, -6*mm}, makePerigees1());

                       
  VertexInfo v2;
  initVertex (v2, {9.8*mm, 0.2*mm, -4.8*mm}, makePerigees2());

  std::vector<xAOD::Vertex*> verts {&v1.v, &v2.v};
  m_fitter->fit (verts);

  xAOD::Vertex exp_v1;
  exp_v1.makePrivateStore();
  exp_v1.setPosition ({8.67978, 10.3569, -6.33368});
  exp_v1.setFitQuality (0.0141012, 0.797808);
  exp_v1.setCovariance (std::vector<float>
                        {751.563, 142.738, 950.953,
                           -7.62904, 47.5311, 67.4557});

  xAOD::Vertex exp_v2;
  exp_v2.makePrivateStore();
  exp_v2.setPosition ({7.97404, 0.106101, -4.97333});
  exp_v2.setFitQuality (1.74029, 1.6738);
  exp_v2.setCovariance (std::vector<float>
                        {0.348487, -0.0194744, 0.0290909,
                           -0.00362647, 0.000706365, 0.445415});

  compareVertex (v1.v, exp_v1);
  compareVertex (v2.v, exp_v2);

  return StatusCode::SUCCESS;
}


} // namespace Trk
