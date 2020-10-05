/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVKalVrtFitter/src/TrkVKalVrtFitterTestAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Algorithm for testing TrkVKalVrtFitter.
 */


#undef NDEBUG
#include "TrkVKalVrtFitterTestAlg.h"
#include "TrkVKalVrtFitter/IVertexCascadeFitter.h"
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
  std::cout << v.x() << " " << v.y() << " " << v.z() << "\n";
  if (v.isAvailable<short> ("vertexType")) {
    std::cout << "vertexType " << v.vertexType() << "\n";
  }
  std::cout << "chi2/ndof " << v.chiSquared() << " " << v.numberDoF() << "\n";

  std::cout << "cov ";
  for (float f : v.covariance()) {
    std::cout << f << " ";
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
  thresh = std::max (thresh, 2e-5);
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


void comparePerigee (const Trk::TrackParameters* a,
                     const Trk::TrackParameters* b,
                     double thresh = 2e-5)
{
  if (!a && !b) return;
  if (!a || !b) std::abort();
  assertVec3D ("perigee pos", a->position(), b->position(), thresh);
  assertVec3D ("perigee mom", a->momentum(), b->momentum(), thresh);
  assert (a->charge() == b->charge());
  assertVec3D ("perigee surf",
               a->associatedSurface().center(),
               b->associatedSurface().center(), thresh);
}


void compareVertex (const xAOD::Vertex& a, const xAOD::Vertex& b,
                    double thresh = 2e-5)
{
  assertVec3D ("vertex pos", a.position(), b.position(), thresh);
  assert (Athena_test::isEqual (a.chiSquared(), b.chiSquared(),
                                std::max (5e-5, thresh)) );
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
      comparePerigee (avec[i].initialPerigee(), bvec[i].initialPerigee(), thresh);
      comparePerigee (avec[i].perigeeAtVertex(), bvec[i].perigeeAtVertex(), thresh);
      assert (Athena_test::isEqual (avec[i].trackQuality().chiSquared(),
                                    bvec[i].trackQuality().chiSquared(),
                                    3e-2));
      assert (avec[i].trackQuality().numberDoF() ==
              bvec[i].trackQuality().numberDoF());
    }
  }
}


void setRefittedPerigee (xAOD::Vertex& v, unsigned i,
                         float charge,
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

  const Amg::Vector3D& pos = v.position();
  auto p = std::make_unique<Trk::Perigee> (pos, mom, charge, pos,
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
StatusCode TrkVKalVrtFitterTestAlg::initialize()
{
  ATH_CHECK( m_fitter.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode TrkVKalVrtFitterTestAlg::execute()
{
  ATH_MSG_VERBOSE ("execute");

  ATH_CHECK( test1() );
  ATH_CHECK( test2() );
  ATH_CHECK( test3() );

  return StatusCode::SUCCESS;
}



// Neutral, no constraint.
// (Mixed charged+neutral seems not to work.)
StatusCode TrkVKalVrtFitterTestAlg::test1()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({-5.50787, -8.38822, -4.67306});
  exp_v0.setFitQuality (0.531859, 3);
  exp_v0.setCovariance (std::vector<float>
                        {246.136, 373.626, 725.174, 134.985,
                           248.851, 145.765, -29779.7, -5881.23,
                           -2327.52, 4.64884e+13, 10334, -5625.55,
                           6012.6, 1.05714e+14, 2.44703e+14, -11615.6,
                           -22512.2, -39837.2, 1.97188e+13, 4.39981e+13,
                           8.80508e+12 });
  setFitQuality (exp_v0, 0, 0.311, 2);
  setFitQuality (exp_v0, 1, 0.138, 2);
  setFitQuality (exp_v0, 2, 0.082, 2);

  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (std::vector<const Trk::TrackParameters*>(),
                                                   asVec (neutrals)));
  compareVertex (*v1, exp_v0);

  return StatusCode::SUCCESS;
}



// Neutral + Vector3D constraint
StatusCode TrkVKalVrtFitterTestAlg::test2()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({-5.32687, -8.28403, -4.63052});
  exp_v0.setFitQuality (0.525555, 3);
  exp_v0.setCovariance (std::vector<float>
                        { 208.636, 319.448, 638.225, 110.249,
                            209.116, 123.504, -39371.1, -11521.7,
                            11317.9, 4.04178e+14, -21193.4, -70432,
                            1453.88, 4.81939e+14, 6.44741e+14, -18347.9,
                            -34574.9, -35450.7, 2.65866e+14, 3.03215e+14,
                           1.77877e+14 });
  setFitQuality (exp_v0, 0, 0.306, 2);
  setFitQuality (exp_v0, 1, 0.143, 2);
  setFitQuality (exp_v0, 2, 0.076, 2);

  Amg::Vector3D pnt1 (5, 6, -3);

  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (std::vector<const Trk::TrackParameters*>(),
                                                   asVec (neutrals),
                                                   pnt1));
  compareVertex (*v1, exp_v0);

  // ??? This gives a different result due to different reference frame
  //     handling in CvtNeutralParameters vs CvtNeutralParticle
  xAOD::Vertex exp_v1;
  exp_v1.makePrivateStore();
  exp_v1.setPosition ({27.5393, 46.192, 14.3115});
  exp_v1.setFitQuality (0.539225, 3);
  exp_v1.setCovariance (std::vector<float>
                        {31839.2, 49179.6, 78856.4, 18195.7,
                           28852.3, 11897.5, 350173, 364859,
                           121754, 9.62321e+13, 462920, 875224,
                           249969, 1.46679e+14, 2.25898e+14, -21927.8,
                           -28423, 126640, 6.79303e+13, 1.08235e+14,
                           5.74699e+13 });
  setFitQuality (exp_v1, 0, 0.281, 2);
  setFitQuality (exp_v1, 1, 0.010, 2);
  setFitQuality (exp_v1, 2, 0.248, 2);

  xAODNPUVec_t xaodnp = makexAODNP (makeNeutrals1());
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (std::vector<const xAOD::TrackParticle*>(),
                                                   asVec (xaodnp),
                                                   pnt1));
  compareVertex (*v2, exp_v1);

  return StatusCode::SUCCESS;
}


// Neutral + Vertex constraint
StatusCode TrkVKalVrtFitterTestAlg::test3()
{
  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition ({5.04927, 6.19454, -2.81519});
  exp_v0.setFitQuality (2.20339, 6);
  exp_v0.setCovariance (std::vector<float>
                        { 0.955465, 0.0266466, 0.954031, -0.0110269,
                            -0.0101389, 0.953361, 423.461, -446.509,
                            -238.452, 2.88637e+14, -555.742, 861.926,
                            -298.391, 1.63611e+14, 1.14116e+14, 173.422,
                            280.448, 1008.54, -3.86156e+13, -4.35414e+11,
                           2.85245e+13 });
  setFitQuality (exp_v0, 0, 0.396, 2);
  setFitQuality (exp_v0, 1, 0.752, 2);
  setFitQuality (exp_v0, 2, 1.055, 2);

  Amg::Vector3D pnt1 (5, 6, -3);
  xAOD::Vertex pnt2;
  pnt2.makePrivateStore();
  pnt2.setPosition (pnt1);
  AmgSymMatrix(3) pnt2covar;
  pnt2covar.setIdentity();
  pnt2.setCovariancePosition (pnt2covar);

  NeutralUVec_t neutrals = makeNeutrals1();
  std::unique_ptr<xAOD::Vertex> v1 (m_fitter->fit (std::vector<const Trk::TrackParameters*>(),
                                                   asVec (neutrals),
                                                   pnt2));
  compareVertex (*v1, exp_v0);

  // ??? This gives a different result due to different reference frame
  //     handling in CvtNeutralParameters vs CvtNeutralParticle
  xAOD::Vertex exp_v1;
  exp_v1.makePrivateStore();
  exp_v1.setPosition ({5.00841, 6.23496, -2.82754});
  exp_v1.setFitQuality (2.2508, 6);
  exp_v1.setCovariance (std::vector<float>
                        { 0.968676, 0.0228383, 0.951682, -0.0091141,
                            -0.00989197, 0.95025, 380.968, -425.258,
                            -247.262, 3.10985e+14, -404.383, 919.668,
                            -261.107, 1.42949e+14, 9.26041e+13, 173.059,
                            241.234, 1027.67, -2.68628e+13, 1.72792e+13,
                           3.76269e+13 });
  setFitQuality (exp_v1, 0, 0.313, 2);
  setFitQuality (exp_v1, 1, 0.692, 2);
  setFitQuality (exp_v1, 2, 1.246, 2);

  xAODNPUVec_t xaodnp = makexAODNP (makeNeutrals1());
  std::unique_ptr<xAOD::Vertex> v2 (m_fitter->fit (std::vector<const xAOD::TrackParticle*>(),
                                                   asVec (xaodnp),
                                                   pnt2));
  compareVertex (*v2, exp_v1);

  return StatusCode::SUCCESS;
}


} // namespace Trk



namespace {


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


PerigeeUVec_t makePerigees3()
{
  Amg::Vector3D pos1a { 5*mm,   0*mm, -2.5*mm };
  Amg::Vector3D mom1a { 600*MeV, 400*MeV, 200*MeV };
  Amg::Vector3D pos1b { 5.1*mm, 0.3*mm, -2.6*mm };
  Amg::Vector3D mom1b { 700*MeV, -300*MeV, -150*MeV };

  PerigeeUVec_t ret;

  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1a, mom1a,  1, pos1a, cov5a().release()));
  ret.emplace_back (std::make_unique<Trk::Perigee>(pos1b, mom1b, -1, pos1a, cov5a().release()));

  return ret;
}


Amg::Vector3D exp_mom (const double moms[][4], int imom)
{
  return {moms[imom][0], moms[imom][1], moms[imom][2]};
}


} // anonymous namespace


namespace Trk {


// Simple cascade fitter test.
StatusCode TrkVKalVrtFitterTestAlg::test4()
{
  Trk::IVertexCascadeFitter* fitter =
    dynamic_cast<Trk::IVertexCascadeFitter*> (m_fitter.get());

  xAODTPUVec_t tracks1 = makexAODTP (makePerigees2());

  std::unique_ptr<IVKalState> state (fitter->makeState());
  Trk::VertexID v1 = fitter->startVertex (asVec (tracks1),
                                          std::vector<double> {100*MeV, 150*MeV, 200*MeV},
                                          *state,
                                          930*MeV);

  xAODTPUVec_t tracks2 = makexAODTP (makePerigees3());

  fitter->nextVertex (asVec (tracks2),
                      std::vector<double> {130*MeV, 160*MeV},
                      std::vector<Trk::VertexID> {v1},
                      *state,
                      2000*MeV);

  std::unique_ptr<Trk::VxCascadeInfo> info1 (fitter->fitCascade(*state));
  info1->setSVOwnership (true);

#if 0
  std::cout << info1->fitChi2() << " " << info1->nDoF() << "\n";
  for (const std::vector<TLorentzVector>& vv : info1->getParticleMoms()) {
    std::cout << "===\n";
    for (const TLorentzVector& vvv : vv) {
      std::cout << vvv.X() << " " << vvv.Y() << " " << vvv.Z() << " " << vvv.E() << "\n";
    }
  }
  std::cout << "=== vertices\n";
  for (const xAOD::Vertex* v : info1->vertices()) {
    dumpVertex (*v);
  }
#endif

  // Some of the comparison thresholds here have to be quite large,
  // because the fit is unstable against small changes in the rounding
  // of, eg, trig functions.  The results we get can vary depending
  // on the libc version used as well as on the exact hardware used.

  assert (Athena_test::isEqual (info1->fitChi2(), 8.69008, 1e-5));
  assert (info1->nDoF() == 8);


  const double exp_moms0[][4] =
    {
     {65.8357, -2.03326, -1.46405, 119.752},
     {755.228,  239.515,  134.648, 817.537},
     {900.997, -348.825, -292.857, 1029.19},
     {1013.68,  681.319,  331.188, 1272.13},
     {522.571, -222.398, -113.273, 600.81},
     {1719.34, -112.854, -155.908, 1964.27},
    };
  const size_t nmoms0 = std::distance (std::begin(exp_moms0), std::end(exp_moms0));
  size_t imoms0 = 0;
  for (const std::vector<TLorentzVector>& vv : info1->getParticleMoms()) {
    for (const TLorentzVector& vvv : vv) {
      assert (imoms0 < nmoms0);
      assert( Athena_test::isEqual (vvv.X(), exp_moms0[imoms0][0], 0.1) );
      assert( Athena_test::isEqual (vvv.Y(), exp_moms0[imoms0][1], 0.1) );
      assert( Athena_test::isEqual (vvv.Z(), exp_moms0[imoms0][2], 0.1) );
      assert( Athena_test::isEqual (vvv.E(), exp_moms0[imoms0][3], 0.1) );
      ++imoms0;
    }
  }
  assert (imoms0 == nmoms0);

  assert (info1->vertices().size() == 2);

  xAOD::Vertex exp_v0;
  exp_v0.makePrivateStore();
  exp_v0.setPosition({ 7.89827, 0.0514449, -4.04121 });
  exp_v0.setFitQuality(5.34877, 8);
  exp_v0.setCovariance(std::vector<float>{
    0.218298, -0.00769266, 0.0194589, -0.0118989, 0.0107223, 0.208922 });
  setRefittedPerigee(
    exp_v0,
    0,
    1,
    exp_mom(exp_moms0, 0),
    { 0.209404,    -4.58753,  -0.00519457,  0.00377293,   0.00105397,
      -4.58753,    154483,    32.603,       -0.117209,    -35.6007,
      -0.00519457, 32.603,    0.0113951,    -0.000116443, -0.00751125,
      0.00377293,  -0.117209, -0.000116443, 0.009643,     2.37152e-05,
      0.00105397,  -35.6007,  -0.00751125,  2.37152e-05,  0.0082042 });
  setFitQuality(exp_v0, 0, 0.926, 2);
  setRefittedPerigee(
    exp_v0,
    1,
    -1,
    exp_mom(exp_moms0, 1),
    { 0.185428,     0.807536,  -0.00324979,  -0.000237823, 1.99968e-05,
      0.807536,     154490,    6.26553,      -0.238515,    0.168369,
      -0.00324979,  6.26553,   0.00856011,   -0.00036866,  -2.23097e-05,
      -0.000237823, -0.238515, -0.00036866,  0.00933191,   7.51824e-06,
      1.99968e-05,  0.168369,  -2.23097e-05, 7.51824e-06,  3.74483e-07 });
  setFitQuality(exp_v0, 1, 4.142, 2);
  setRefittedPerigee(
    exp_v0,
    2,
    -1,
    exp_mom(exp_moms0, 2),
    { 0.191446,     -9.49834,  -0.00495436,  -0.00130953,  -4.72358e-05,
      -9.49834,     154476,    5.42258,      -0.231539,    0.236084,
      -0.00495436,  5.42258,   0.00860893,   -0.000425575, 3.61158e-05,
      -0.00130953,  -0.231539, -0.000425575, 0.00920703,   -5.93187e-06,
      -4.72358e-05, 0.236084,  3.61158e-05,  -5.93187e-06, 4.8944e-07 });
  setFitQuality(exp_v0, 2, 0.281, 2);

  compareVertex(*info1->vertices()[0], exp_v0, 0.1);

  xAOD::Vertex exp_v1;
  exp_v1.makePrivateStore();
  exp_v1.setPosition({ 5.31046, 0.22012, -3.80093 });
  exp_v1.setFitQuality(3.34131, 8);
  exp_v1.setCovariance(std::vector<float>{
    0.0239352, 0.000977903, 0.00708136, 4.16975e-05, 0.00295894, 0.2431 });
  setRefittedPerigee(
    exp_v1,
    0,
    1,
    exp_mom(exp_moms0, 3),
    { 0.166917,     -16.9685,    -0.000309914, -0.000819924, 1.31935e-05,
      -16.9685,     6.35682e+08, -139.016,     9.70413,      -495.474,
      -0.000309914, -139.016,    0.0100214,    -7.71225e-06, 0.000103046,
      -0.000819924, 9.70413,     -7.71225e-06, 0.00998731,   -5.42066e-06,
      1.31935e-05,  -495.474,    0.000103046,  -5.42066e-06, 0.000386195 });
  setFitQuality(exp_v1, 0, 1.986, 2);
  setRefittedPerigee(
    exp_v1,
    1,
    -1,
    exp_mom(exp_moms0, 4),
    { 0.20904,      -15.3143,    0.00086181,  -0.000463146, -2.83922e-05,
      -15.3143,     6.35682e+08, 306.365,     -3.09595,     -2470.81,
      0.00086181,   306.365,     0.0101467,   -2.3178e-06,  -0.00116433,
      -0.000463146, -3.09595,    -2.3178e-06, 0.00999678,   8.90989e-07,
      -2.83922e-05, -2470.81,    -0.00116433, 8.90989e-07,  0.00960596 });
  setFitQuality(exp_v1, 1, 1.356, 2);

  compareVertex (*info1->vertices()[1], exp_v1, 0.1);

  return StatusCode::SUCCESS;
}


} // namespace Trk
