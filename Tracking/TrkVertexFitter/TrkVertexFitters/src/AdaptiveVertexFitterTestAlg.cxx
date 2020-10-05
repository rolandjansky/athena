/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitters/src/AdaptiveVertexFitterTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing AdaptiveVertexFitter.
 */


#undef NDEBUG
#include "AdaptiveVertexFitterTestAlg.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
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


template <class T>
std::vector<const T*> asVec (const std::vector<std::unique_ptr<T> >& v)
{
  std::vector<const T*> ret;
  for (const std::unique_ptr<T>& p : v) {
    ret.push_back (p.get());
  }
  return ret;
}


std::vector<const Trk::TrackParameters*>
asVec (const std::vector<std::unique_ptr<Trk::Perigee> >& v)
{
  std::vector<const Trk::TrackParameters*> ret;
  for (const std::unique_ptr<Trk::Perigee>& p : v) {
    ret.push_back (p.get());
  }
  return ret;
}


std::vector<const Trk::NeutralParameters*>
asVec (const std::vector<std::unique_ptr<Trk::NeutralPerigee> >& v)
{
  std::vector<const Trk::NeutralParameters*> ret;
  for (const std::unique_ptr<Trk::NeutralPerigee>& p : v) {
    ret.push_back (p.get());
  }
  return ret;
}


std::unique_ptr<AmgSymMatrix(5)> cov5()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setIdentity();
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


using NeutralUVec_t = std::vector<std::unique_ptr<Trk::NeutralPerigee> >;
NeutralUVec_t makeNeutrals1()
{
  Amg::Vector3D pos0 { 0, 0, 0 };

  Amg::Vector3D pos1a { 3*mm, 0.5*mm, -7*mm };
  Amg::Vector3D mom1a { 1000*MeV, 900*MeV, 2000*MeV };
  Amg::Vector3D pos1b { -1*mm, 2.5*mm, 1*mm };
  Amg::Vector3D mom1b { 800*MeV, 1000*MeV, 300*MeV };
  Amg::Vector3D pos1c { -1.5*mm, 1*mm, -3*mm };
  Amg::Vector3D mom1c { 500*MeV, 4000*MeV, 800*MeV };

  NeutralUVec_t ret;

  ret.emplace_back (std::make_unique<Trk::NeutralPerigee>(pos1a, mom1a,  1, pos1a, cov5().release()));
  ret.emplace_back (std::make_unique<Trk::NeutralPerigee>(pos1b, mom1b,  1, pos1b, cov5().release()));
  ret.emplace_back (std::make_unique<Trk::NeutralPerigee>(pos1c, mom1c,  1, pos1c, cov5().release()));

  return ret;
}


using TrackUVec_t = std::vector<std::unique_ptr<Trk::Track> >;
TrackUVec_t makeTracks (PerigeeUVec_t&& perigees)
{
  TrackUVec_t tracks;

  for (std::unique_ptr<Trk::Perigee>& p : perigees) {
    Trk::TrackInfo info (Trk::TrackInfo::Unknown, Trk::undefined);
    auto fqual = std::make_unique<Trk::FitQuality> (0, 0);
    auto tsos = std::make_unique<DataVector<const Trk::TrackStateOnSurface> >();
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    tsos->push_back (std::make_unique<Trk::TrackStateOnSurface>
                     (nullptr, p.release(), nullptr, nullptr, typePattern));
    tracks.push_back (std::make_unique<Trk::Track> (info,
                                                    tsos.release(),
                                                    fqual.release()));
  }

  return tracks;
}



// Copied from TrackParticleCreatorTool.
void setDefiningParameters( xAOD::TrackParticle& tp,
                            const Trk::Perigee& perigee )
{
  tp.setDefiningParameters(perigee.parameters()[Trk::d0],
    perigee.parameters()[Trk::z0],
    perigee.parameters()[Trk::phi0],
    perigee.parameters()[Trk::theta],
    perigee.parameters()[Trk::qOverP]);
  const AmgSymMatrix(5)* covMatrix = perigee.covariance();
  // see https://its.cern.ch/jira/browse/ATLASRECTS-645 for justification to comment out the following line 
  // assert(covMatrix && covMatrix->rows()==5&& covMatrix->cols()==5); 
  std::vector<float> covMatrixVec;
  if( !covMatrix ) ;//ATH_MSG_WARNING("Setting Defining parameters without error matrix");
  else Amg::compress(*covMatrix,covMatrixVec);
  tp.setDefiningParametersCovMatrixVec(covMatrixVec);
  const Amg::Vector3D& surfaceCenter = perigee.associatedSurface().center(); 
  tp.setParametersOrigin(surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z() );
}
void setDefiningParameters( xAOD::NeutralParticle& tp,
                            const Trk::NeutralPerigee& perigee )
{
  tp.setDefiningParameters(perigee.parameters()[Trk::d0],
    perigee.parameters()[Trk::z0],
    perigee.parameters()[Trk::phi0],
    perigee.parameters()[Trk::theta],
    perigee.parameters()[Trk::qOverP]);
  const AmgSymMatrix(5)* covMatrix = perigee.covariance();
  // see https://its.cern.ch/jira/browse/ATLASRECTS-645 for justification to comment out the following line 
  // assert(covMatrix && covMatrix->rows()==5&& covMatrix->cols()==5); 
  std::vector<float> covMatrixVec;
  if( !covMatrix ) ;//ATH_MSG_WARNING("Setting Defining parameters without error matrix");
  else Amg::compress(*covMatrix,covMatrixVec);
  tp.setDefiningParametersCovMatrixVec(covMatrixVec);
  const Amg::Vector3D& surfaceCenter = perigee.associatedSurface().center(); 
  tp.setParametersOrigin(surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z() );
}


using xAODTPUVec_t = std::vector<std::unique_ptr<xAOD::TrackParticle> >;
xAODTPUVec_t makexAODTP (PerigeeUVec_t&& perigees)
{
  xAODTPUVec_t tracks;

  for (std::unique_ptr<Trk::Perigee>& p : perigees) {
    auto tp = std::make_unique<xAOD::TrackParticle>();
    tp->makePrivateStore();
    setDefiningParameters (*tp, *p);
    tracks.push_back (std::move (tp));
  }

  return tracks;
}


using xAODNPUVec_t = std::vector<std::unique_ptr<xAOD::NeutralParticle> >;
xAODNPUVec_t makexAODNP (NeutralUVec_t&& perigees)
{
  xAODNPUVec_t tracks;

  for (std::unique_ptr<Trk::NeutralPerigee>& p : perigees) {
    auto tp = std::make_unique<xAOD::NeutralParticle>();
    tp->makePrivateStore();
    setDefiningParameters (*tp, *p);
    tracks.push_back (std::move (tp));
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


void comparePerigee (const Trk::TrackParameters* a,
                     const Trk::TrackParameters* b)
{
  if (!a && !b) return;
  if (!a || !b) std::abort();
  assertVec3D ("perigee pos", a->position(), b->position());
  assertVec3D ("perigee mom", a->momentum(), b->momentum());
  assert (a->charge() == b->charge());
  assertVec3D ("perigee surf",
               a->associatedSurface().center(),
               b->associatedSurface().center());
}


void compareVertex (const xAOD::Vertex& a, const xAOD::Vertex& b)
{
  assertVec3D ("vertex pos", a.position(), b.position());
  assert (Athena_test::isEqual (a.chiSquared(), b.chiSquared(), 5e-5) );
  assert (Athena_test::isEqual (a.numberDoF(), b.numberDoF(), 1e-5) );

  assert (a.covariance().size() == b.covariance().size());
  for (unsigned int i = 0; i < a.covariance().size(); i++) {
    if (isinf(a.covariance()[i]) && isinf(b.covariance()[i])) continue;
    assert (Athena_test::isEqual (a.covariance()[i], b.covariance()[i], 2e-2) );
  }

  assert (a.vxTrackAtVertexAvailable() == b.vxTrackAtVertexAvailable());
  if (a.vxTrackAtVertexAvailable()) {
    const std::vector< Trk::VxTrackAtVertex >& avec = a.vxTrackAtVertex();
    const std::vector< Trk::VxTrackAtVertex >& bvec = b.vxTrackAtVertex();
    assert (avec.size() == bvec.size());
    for (unsigned int i = 0; i < avec.size(); i++) {
      comparePerigee (avec[i].initialPerigee(), bvec[i].initialPerigee());
      comparePerigee (avec[i].perigeeAtVertex(), bvec[i].perigeeAtVertex());
      assert (Athena_test::isEqual (avec[i].trackQuality().chiSquared(),
                                    bvec[i].trackQuality().chiSquared(),
                                    3e-2));
      assert (avec[i].trackQuality().numberDoF() ==
              bvec[i].trackQuality().numberDoF());
    }
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


void clearInitialPerigees (xAOD::Vertex& v)
{
  for (Trk::VxTrackAtVertex& v : v.vxTrackAtVertex()) {
    v.setInitialPerigee (static_cast<const Trk::Perigee*>(nullptr));
  }
}


void setRefittedPerigee (xAOD::Vertex& v, unsigned i,
                         float charge,
                         const Amg::Vector3D& pos,
                         const Amg::Vector3D& mom,
                         const std::vector<float>& c)
{
  std::vector< Trk::VxTrackAtVertex >& vec = v.vxTrackAtVertex();
  if (vec.size() <= i) vec.resize(i+1);

  std::unique_ptr<AmgSymMatrix(5)> cov = cov5();
  for (int i=0; i < 5; i++) {
    for (int j=0; j < 5; j++) {
      unsigned ipos = i*5 + j;
      (*cov)(i,j) = ipos < c.size() ? c[ipos] : 0;
    }
  }

  const Amg::Vector3D& vpos = v.position();
  auto p = std::make_unique<Trk::Perigee> (pos, mom, charge, vpos,
                                           cov.release());
  vec[i].setPerigeeAtVertex (p.release());
}


void setFitQuality (xAOD::Vertex& v, unsigned i, float chi2, int ndof)
{
  std::vector< Trk::VxTrackAtVertex >& vec = v.vxTrackAtVertex();
  if (vec.size() <= i) vec.resize(i+1);
  vec[i].setTrackQuality (Trk::FitQuality (chi2, ndof));
}


} // anonymous namespace


namespace Trk {


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode AdaptiveVertexFitterTestAlg::initialize()
{
  ATH_CHECK( m_fitter.retrieve() );
  return StatusCode::SUCCESS;
}
/**
 * @brief Standard Gaudi execute method.
 */
StatusCode AdaptiveVertexFitterTestAlg::execute()
{
  ATH_MSG_VERBOSE ("execute");

  ATH_CHECK( test1() );
  ATH_CHECK( test2() );
  ATH_CHECK( test3() );
  return StatusCode::SUCCESS;
}

// Neutral, no constraint.
StatusCode AdaptiveVertexFitterTestAlg::test1()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({4.27611, 4.35683, 1.62467});
  exp_v0.setFitQuality (1.69878, 0.79376);
  exp_v0.setCovariance (std::vector<float>
                        {28.318, 26.913, 37.8531, 17.3323, 17.3041, 23.6152});
  setFitQuality (exp_v0, 0, 0.936, 1);
  setFitQuality (exp_v0, 1, 0.000, 2);
  setFitQuality (exp_v0, 2, 0.000, 2);

  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (std::vector<const Trk::TrackParameters*>(),
                                                   asVec (neutrals)));
  compareVertex (*v1, exp_v0);

  return StatusCode::SUCCESS;
}

// Charged + Neutral + Vector3D constraint
StatusCode AdaptiveVertexFitterTestAlg::test2()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({-0.664619, 1.88247, -4.28452});
  exp_v0.setFitQuality (1.36786, 6.60783);
  exp_v0.setCovariance (std::vector<float>
                        {25.8826, 26.6122, 91.0458, 6.34189, 14.6174, 13.9884});
  setRefittedPerigee(
    exp_v0,
    0,
    1,
    { 1.5885284, 0.3823804, -10.2058300 },
    { 399.6301520, 600.2463141, 200.0002601 },
    { 1.58278, -0.618193,    -0.821391, 0.00010879, -0.171884, -0.618193,
      2.32566, 0.908938,     0.800925,  0.346876,   -0.821391, 0.908938,
      1.20816, -0.000294073, 0.461137,  0.00010879, 0.800925,  -0.000294073,
      1,       -0.000269915, -0.171884, 0.346876,   0.461137,  -0.000269915,
      1 });
  setFitQuality (exp_v0, 0, 0.000, 2);
  setRefittedPerigee(
    exp_v0,
    1,
    -1,
    { -0.2079830, 1.1957289, -2.5975002 },
    { 600.4814296, 399.2766328, -200.0005578 },
    { 3.53014, 0.466334,     -2.04043,  0.000621337, -0.653413, 0.466334,
      3.53405, -0.415368,    1.56192,   -0.206492,   -2.04043,  -0.415368,
      1.81448, -0.000856625, 0.901728,  0.000621337, 1.56192,   -0.000856625,
      1,       -0.000578862, -0.653413, -0.206492,   0.901728,  -0.000578862,
      1 });
  setFitQuality (exp_v0, 1, 0.017, 2);
  setRefittedPerigee(exp_v0,
                     2,
                     -1,
                     { 1.2063615, 1.3212056, -6.9978818 },
                     { 299.9873170, 1000.0038041, 100.0000072 },
                     { 1.00049,
                       0.00413671,
                       0.0221412,
                       -2.7918e-08,
                       -0.000147081,
                       0.00413671,
                       1.03551,
                       0.18734,
                       -0.0223173,
                       -0.00248881,
                       0.0221412,
                       0.18734,
                       1.00242,
                       -1.91988e-06,
                       -0.0133167,
                       -2.7918e-08,
                       -0.0223173,
                       -1.91988e-06,
                       1,
                       7.24261e-06,
                       -0.000147081,
                       -0.00248881,
                       -0.0133167,
                       7.24261e-06,
                       1 });
  setFitQuality (exp_v0, 2, 0.020, 2);
  setFitQuality (exp_v0, 3, 0.136, 2);
  setFitQuality (exp_v0, 4, 0.000, 2);
  setFitQuality (exp_v0, 5, 0.000, 2);

  Amg::Vector3D pnt1 (5, 6, -3);

  TrackUVec_t tracks = makeTracks (makePerigees1());
  setInitialPerigees (exp_v0, tracks);

  PerigeeUVec_t perigees = makePerigees1();
  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (asVec (perigees),
                                                   asVec (neutrals),
                                                   pnt1));
  compareVertex (*v1, exp_v0);

  xAODTPUVec_t xtps = makexAODTP (makePerigees1());
  xAODNPUVec_t xaodnp = makexAODNP (makeNeutrals1());
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (asVec (xtps),
                                                   asVec (xaodnp),
                                                   pnt1));
  clearInitialPerigees (exp_v0);
  compareVertex (*v2, exp_v0);

  return StatusCode::SUCCESS;
}


// Charged+Neutral + Vertex constraint
StatusCode AdaptiveVertexFitterTestAlg::test3()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({4.85215, 5.94893, -3.13472});
  exp_v0.setFitQuality (2.38431, 8.54327);
  exp_v0.setCovariance (std::vector<float>
                        {1.183, 0.0323074, 1.21271,
                           0.00903037, 0.0167373, 1.12584});
  setRefittedPerigee(
    exp_v0,
    0,
    1,
    { 5.1719083, 5.7335618, -8.4196568 },
    { 402.8346276, 598.1012479, 199.9979000 },
    { 135.368,    4.54292,   41.4349,     -0.0182748,  -10.0936,
      4.54292,    38.8427,   1.48244,     -6.13913,    -0.389733,
      41.4349,    1.48244,   13.5349,     -0.00640884, -3.54057,
      -0.0182748, -6.13913,  -0.00640884, 1,           0.00218082,
      -10.0936,   -0.389733, -3.54057,    0.00218082,  1 });
  setFitQuality (exp_v0, 0, 0.682, 2);

  setRefittedPerigee(exp_v0,
                     1,
                     -1,
                     { 5.4869777, 5.0058724, -4.4978936 },
                     { 598.2006961, 402.6869274, -199.9979142 },
                     { 111.922,    -11.1715, 35.7532,    -0.0162277, -9.04482,
                       -11.1715,   35.9577,  -3.83303,   -5.80842,   1.04702,
                       35.7532,    -3.83303, 12.2632,    -0.0060222, -3.35579,
                       -0.0162277, -5.80842, -0.0060222, 1,          0.00216502,
                       -9.04482,   1.04702,  -3.35579,   0.00216502, 1 });
  setFitQuality (exp_v0, 1, 0.061, 2);

  setRefittedPerigee(
    exp_v0,
    2,
    -1,
    { 2.7708142, 6.5661849, -6.4736255 },
    { 296.8467752, 1000.9403742, 100.0017963 },
    { 114.181,    -7.37341, 35.593,      -0.0172367,  -9.10529,
      -7.37341,   32.345,   -2.46816,    -5.55155,    0.684107,
      35.593,     -2.46816, 11.9239,     -0.00626001, -3.30551,
      -0.0172367, -5.55155, -0.00626001, 1,           0.00180269,
      -9.10529,   0.684107, -3.30551,    0.00180269,  1 });
  setFitQuality (exp_v0, 2, 0.352, 2);
  setFitQuality (exp_v0, 3, 1.119, 1);
  setFitQuality (exp_v0, 4, 0.000, 2);
  setFitQuality (exp_v0, 5, 0.000, 2);

  Amg::Vector3D pnt1 (5, 6, -3);
  xAOD::Vertex pnt2;
  pnt2.makePrivateStore();
  pnt2.setPosition (pnt1);
  AmgSymMatrix(3) pnt2covar;
  pnt2covar.setIdentity();
  pnt2.setCovariancePosition (pnt2covar);

  TrackUVec_t tracks = makeTracks (makePerigees1());
  setInitialPerigees (exp_v0, tracks);

  PerigeeUVec_t perigees = makePerigees1();
  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (asVec (perigees),
                                                   asVec (neutrals),
                                                   pnt2));
  compareVertex (*v1, exp_v0);

  xAODTPUVec_t xtps = makexAODTP (makePerigees1());
  xAODNPUVec_t xaodnp = makexAODNP (makeNeutrals1());
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (asVec (xtps),
                                                   asVec (xaodnp),
                                                   pnt2));
  clearInitialPerigees (exp_v0);
  compareVertex (*v2, exp_v0);

  return StatusCode::SUCCESS;
}


} // namespace Trk
