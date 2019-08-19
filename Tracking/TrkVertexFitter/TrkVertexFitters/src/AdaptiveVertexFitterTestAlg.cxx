/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
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


typedef std::vector<std::unique_ptr<Trk::Perigee> > PerigeeUVec_t;
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


typedef std::vector<std::unique_ptr<Trk::NeutralPerigee> > NeutralUVec_t;
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


typedef std::vector<std::unique_ptr<Trk::Track> > TrackUVec_t;
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


typedef std::vector<std::unique_ptr<Trk::TrackParticleBase> > TrackParticleUVec_t;
TrackParticleUVec_t makeTrackParticles (PerigeeUVec_t&& perigees)
{
  TrackParticleUVec_t tracks;

  std::vector<const Trk::TrackParameters*> pvec;
  for (std::unique_ptr<Trk::Perigee>& p : perigees) {
    tracks.push_back (std::make_unique<Trk::TrackParticleBase> (nullptr,
                                                                Trk::NoVtx,
                                                                nullptr,
                                                                nullptr,
                                                                pvec,
                                                                p.release(),
                                                                nullptr));
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


typedef std::vector<std::unique_ptr<xAOD::TrackParticle> > xAODTPUVec_t;
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


typedef std::vector<std::unique_ptr<xAOD::NeutralParticle> > xAODNPUVec_t;
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
  ATH_CHECK( test4() );
  ATH_CHECK( test5() );
  ATH_CHECK( test6() );

  return StatusCode::SUCCESS;
}


// Charged, no constraint.
StatusCode AdaptiveVertexFitterTestAlg::test1()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({8.43429, 9.21288, -6.60683});
  exp_v0.setFitQuality (0.235948, 1.83137);
  exp_v0.setCovariance (std::vector<float>
                        {35.7266, 30.7883, 47.0715, 3.14669, 4.52157, 6.74599 });
  setRefittedPerigee (exp_v0, 0, 1,
                      {7.8113830, 9.6359772, -7.1130181},
                      {405.1715584, 596.5212045, 199.9961520},
                      {1239.56, -45.9167, 227.862, -0.121956, -33.6206, -45.9167, 128.234, -8.63731, -11.1976, 1.30089, 227.862, -8.63731, 42.8345, -0.0234628, -6.46787, -0.121956, -11.1976, -0.0234628, 1.00001, 0.00399819, -33.6206, 1.30089, -6.46787, 0.00399819, 1}
                      );
  setFitQuality (exp_v0, 0, 0, 2);
  setRefittedPerigee (exp_v0, 1, -1,
                      {9.4647993, 7.6951354, -5.8295872},
                      {596.5909112, 405.0689507, -199.9960745},
                      {1150.81, -107.955, 215.525, -0.120009, -32.3376, -107.955, 132.06, -20.6979, -10.9741, 3.17765, 215.525, -20.6979, 41.3112, -0.0235635, -6.34888, -0.120009, -10.9741, -0.0235635, 1.00001, 0.00408136, -32.3376, 3.17765, -6.34888, 0.00408136, 1}
                      );
  setFitQuality (exp_v0, 1, 0.175, 2);
  setRefittedPerigee (exp_v0, 2, -1,
                      {3.9422703, 10.5334868, -6.0773998},
                      {294.4712678, 1001.6416761, 100.0031255},
                      {872.333, -76.5329, 170.831, -0.0897349, -27.9026, -76.5329, 102.388, -15.3986, -9.71236, 2.58803, 170.831, -15.3986, 34.3815, -0.0185874, -5.77814, -0.0897349, -9.71236, -0.0185874, 1.00001, 0.00312552, -27.9026, 2.58803, -5.77814, 0.00312552, 1}
                      );
  setFitQuality (exp_v0, 2, 0.027, 2);

  TrackUVec_t tracks = makeTracks (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (asVec (tracks)));
  setInitialPerigees (exp_v0, tracks);
  compareVertex (*v1, exp_v0);

  PerigeeUVec_t perigees = makePerigees1();
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (asVec (perigees)));
  compareVertex (*v2, exp_v0);

  return StatusCode::SUCCESS;
}


// Neutral, no constraint.
StatusCode AdaptiveVertexFitterTestAlg::test2()
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


// Charged + Vector3D constraint.
StatusCode AdaptiveVertexFitterTestAlg::test3()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({8.82019, 9.90325, -6.4356});
  exp_v0.setFitQuality (0.0430536, 1.85853);
  exp_v0.setCovariance (std::vector<float>
                        {198.998, 111.025, 256.182, 7.89219, 18.6016, 25.5699});
  setRefittedPerigee (exp_v0, 0, 1,
                      {8.2545569, 10.2879887, -6.8943690},
                      {405.5620128, 596.2559088, 199.9958625},
                      {1640.25, -51.1682, 281.924, -0.152864, -38.9104, -51.1682, 147.868, -8.97462, -12.0465, 1.26021, 281.924, -8.97462, 49.4107, -0.0273351, -6.9577, -0.152864, -12.0465, -0.0273351, 1.00002, 0.00429935, -38.9104, 1.26021, -6.9577, 0.00429935, 1}
                      );
  setFitQuality (exp_v0, 0, 0.000, 2);
  setRefittedPerigee (exp_v0, 1, -1,
                      {10.0507183, 8.0932102, -6.0260413},
                      {596.3526270, 405.4198090, -199.9958068},
                      {1487.84, -156.737, 262.111, -0.147597, -36.9838, -156.737, 155.885, -28.189, -11.7322, 4.05867, 262.111, -28.189, 47.1302, -0.0271073, -6.79166, -0.147597, -11.7322, -0.0271073, 1.00002, 0.00436106, -36.9838, 4.05867, -6.79166, 0.00436106, 1 }
                      );
  setFitQuality (exp_v0, 1, 0.017, 2);
  setRefittedPerigee (exp_v0, 2, -1,
                      {4.1590180, 11.2713536, -6.0037370},
                      {294.0294524, 1001.7714343, 100.0033730},
                      {1167.78, -99.1794, 213.359, -0.112662, -32.5361, -99.1794, 119.804, -18.5507, -10.4868, 2.89953, 213.359, -18.5507, 39.9175, -0.0216083, -6.23884, -0.112662, -10.4868, -0.0216083, 1.00001, 0.00337217, -32.5361, 2.89953, -6.23884, 0.00337217, 1 }
                      );
  setFitQuality (exp_v0, 2, 0.020, 2);

  Amg::Vector3D pnt1 (5, 6, -3);

  TrackUVec_t tracks = makeTracks (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (asVec (tracks), pnt1));
  setInitialPerigees (exp_v0, tracks);
  compareVertex (*v1, exp_v0);

  PerigeeUVec_t perigees = makePerigees1();
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (asVec (perigees), pnt1));
  compareVertex (*v2, exp_v0);

  TrackParticleUVec_t tps = makeTrackParticles (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v3 (m_fitter->fit (asVec (tps), pnt1));
  compareVertex (*v3, exp_v0);

  xAODTPUVec_t xtps = makexAODTP (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v4 (m_fitter->fit (asVec (xtps), pnt1));
  clearInitialPerigees (exp_v0);
  compareVertex (*v4, exp_v0);

  return StatusCode::SUCCESS;
}


// Charged + Vertex constraint.
StatusCode AdaptiveVertexFitterTestAlg::test4()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({5.00265, 6.02993, -3.2627});
  exp_v0.setFitQuality (0.904414, 4.84469);
  exp_v0.setCovariance (std::vector<float>
                        {1.24242, 0.00898474, 1.25033,
                           -0.0018625, 0.00503271, 1.16608});
  setRefittedPerigee (exp_v0, 0, 1,
                      {5.2563912, 5.8589792, -8.3777169},
                      {402.9097324, 598.0506755, 199.9978428},
                      { 148.334, 3.90191, 44.5024, -0.019887, -10.635, 3.90191, 40.8102, 1.24424, -6.30148, -0.32025, 44.5024, 1.24424, 14.2092, -0.00679499, -3.63453, -0.019887, -6.30148, -0.00679499, 1, 0.00224026, -10.635, -0.32025, -3.63453, 0.00224026, 1 }
                      );
  setFitQuality (exp_v0, 0, 0.682, 2);
  setRefittedPerigee (exp_v0, 1, -1,
                      {5.6281930, 5.1009479, -4.5451087},
                      {598.1437842, 402.7714914, -199.9978479},
                      {124.605, -11.9636, 38.8859, -0.0179296, -9.62427, -11.9636, 38.1524, -3.99608, -5.99156, 1.06341, 38.8859, -3.99608, 12.9843, -0.00645023, -3.46157, -0.0179296, -5.99156, -0.00645023, 1, 0.00223385, -9.62427, 1.06341, -3.46157, 0.00223385, 1 }
                      );
  setFitQuality (exp_v0, 1, 0.061, 2);
  setRefittedPerigee (exp_v0, 2, -1,
                      {2.8050119, 6.6815112, -6.4621037},
                      {296.7777220, 1000.9608467, 100.0018360},
                      { 123.052, -8.25619, 37.7378, -0.0183804, -9.50958, -8.25619, 33.7962, -2.7124, -5.67336, 0.738344, 37.7378, -2.7124, 12.4073, -0.0065315, -3.37786, -0.0183804, -5.67336, -0.0065315, 1, 0.0018425, -9.50958, 0.738344, -3.37786, 0.0018425, 1 }
                      );
  setFitQuality (exp_v0, 2, 0.352, 2);

  Amg::Vector3D pnt1 (5, 6, -3);
  xAOD::Vertex pnt2;
  pnt2.makePrivateStore();
  pnt2.setPosition (pnt1);
  AmgSymMatrix(3) pnt2covar;
  pnt2covar.setIdentity();
  pnt2.setCovariancePosition (pnt2covar);

  TrackUVec_t tracks = makeTracks (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (asVec (tracks), pnt2));
  setInitialPerigees (exp_v0, tracks);
  compareVertex (*v1, exp_v0);

  PerigeeUVec_t perigees = makePerigees1();
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (asVec (perigees), pnt2));
  compareVertex (*v2, exp_v0);

  TrackParticleUVec_t tps = makeTrackParticles (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v3 (m_fitter->fit (asVec (tps), pnt2));
  compareVertex (*v3, exp_v0);

  xAODTPUVec_t xtps = makexAODTP (makePerigees1());
  std::unique_ptr<xAOD::Vertex> v4 (m_fitter->fit (asVec (xtps), pnt2));
  clearInitialPerigees (exp_v0);
  compareVertex (*v4, exp_v0);

  return StatusCode::SUCCESS;
}


// Charged + Neutral + Vector3D constraint
StatusCode AdaptiveVertexFitterTestAlg::test5()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({-0.664619, 1.88247, -4.28452});
  exp_v0.setFitQuality (1.36786, 6.60783);
  exp_v0.setCovariance (std::vector<float>
                        {25.8826, 26.6122, 91.0458, 6.34189, 14.6174, 13.9884});
  setRefittedPerigee (exp_v0, 0, 1,
                      {1.5885284, 0.3823804, -10.2058300},
                      {399.6301520, 600.2463141, 200.0002601},
                      { 1.58278, -0.618193, -0.821391, 0.00010879, -0.171884, -0.618193, 2.32566, 0.908938, 0.800925, 0.346876, -0.821391, 0.908938, 1.20816, -0.000294073, 0.461137, 0.00010879, 0.800925, -0.000294073, 1, -0.000269915, -0.171884, 0.346876, 0.461137, -0.000269915, 1 }
                      );
  setFitQuality (exp_v0, 0, 0.000, 2);
  setRefittedPerigee (exp_v0, 1, -1,
                      {-0.2079830, 1.1957289, -2.5975002},
                      {600.4814296, 399.2766328, -200.0005578},
                      { 3.53014, 0.466334, -2.04043, 0.000621337, -0.653413, 0.466334, 3.53405, -0.415368, 1.56192, -0.206492, -2.04043, -0.415368, 1.81448, -0.000856625, 0.901728, 0.000621337, 1.56192, -0.000856625, 1, -0.000578862, -0.653413, -0.206492, 0.901728, -0.000578862, 1 }
                      );
  setFitQuality (exp_v0, 1, 0.017, 2);
  setRefittedPerigee (exp_v0, 2, -1,
                      {1.2063615, 1.3212056, -6.9978818},
                      {299.9873170, 1000.0038041, 100.0000072},
                      { 1.00049, 0.00413671, 0.0221412, -2.7918e-08, -0.000147081, 0.00413671, 1.03551, 0.18734, -0.0223173, -0.00248881, 0.0221412, 0.18734, 1.00242, -1.91988e-06, -0.0133167, -2.7918e-08, -0.0223173, -1.91988e-06, 1, 7.24261e-06, -0.000147081, -0.00248881, -0.0133167, 7.24261e-06, 1 }
                      );
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
StatusCode AdaptiveVertexFitterTestAlg::test6()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({4.85215, 5.94893, -3.13472});
  exp_v0.setFitQuality (2.38431, 8.54327);
  exp_v0.setCovariance (std::vector<float>
                        {1.183, 0.0323074, 1.21271,
                           0.00903037, 0.0167373, 1.12584});
  setRefittedPerigee (exp_v0, 0, 1,
                      {5.1719083, 5.7335618, -8.4196568},
                      {402.8346276, 598.1012479, 199.9979000},
                      { 135.368, 4.54292, 41.4349, -0.0182748, -10.0936, 4.54292, 38.8427, 1.48244, -6.13913, -0.389733, 41.4349, 1.48244, 13.5349, -0.00640884, -3.54057, -0.0182748, -6.13913, -0.00640884, 1, 0.00218082, -10.0936, -0.389733, -3.54057, 0.00218082, 1 }
                      );
  setFitQuality (exp_v0, 0, 0.682, 2);

  setRefittedPerigee (exp_v0, 1, -1,
                      {5.4869777, 5.0058724, -4.4978936},
                      {598.2006961, 402.6869274, -199.9979142},
                      { 111.922, -11.1715, 35.7532, -0.0162277, -9.04482, -11.1715, 35.9577, -3.83303, -5.80842, 1.04702, 35.7532, -3.83303, 12.2632, -0.0060222, -3.35579, -0.0162277, -5.80842, -0.0060222, 1, 0.00216502, -9.04482, 1.04702, -3.35579, 0.00216502, 1 }
                      );
  setFitQuality (exp_v0, 1, 0.061, 2);

  setRefittedPerigee (exp_v0, 2, -1,
                      {2.7708142, 6.5661849, -6.4736255},
                      {296.8467752, 1000.9403742, 100.0017963},
                      { 114.181, -7.37341, 35.593, -0.0172367, -9.10529, -7.37341, 32.345, -2.46816, -5.55155, 0.684107, 35.593, -2.46816, 11.9239, -0.00626001, -3.30551, -0.0172367, -5.55155, -0.00626001, 1, 0.00180269, -9.10529, 0.684107, -3.30551, 0.00180269, 1 }
                      );
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
