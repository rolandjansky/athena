/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigVertexCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigVertexCnv_p2.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigVertexCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp2.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/make_unique.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigInDetTrackFitPar& p1,
              const TrigInDetTrackFitPar& p2)
{
  assert (p1.a0() == p2.a0());
  assert (p1.phi0() == p2.phi0());
  assert (p1.z0() == p2.z0());
  assert (p1.eta() == p2.eta());
  assert (p1.pT() == p2.pT());
  assert (Athena_test::isEqual (p1.ea0(), p2.ea0()));
  assert (Athena_test::isEqual (p1.ephi0(), p2.ephi0()));
  assert (Athena_test::isEqual (p1.ez0(), p2.ez0()));
  assert (Athena_test::isEqual (p1.eeta(), p2.eeta()));
  assert (Athena_test::isEqual (p1.epT(), p2.epT()));
  assert (p1.surfaceType() == p2.surfaceType());
  assert (p1.surfaceCoordinate() == p2.surfaceCoordinate());
  const std::vector<double>& cov1 = *p1.cov();
  const std::vector<double>& cov2 = *p2.cov();
  assert (cov1.size() == cov2.size());
  for (size_t i = 0; i < cov1.size(); i++)
    assert (Athena_test::isEqual (cov1[i], cov2[i]));
}


void compare (const TrigInDetTrack& p1,
              const TrigInDetTrack& p2)
{
  assert (p1.algorithmId() == p2.algorithmId());
  assert (p1.chi2() == p2.chi2());
  assert (p1.NStrawHits() == p2.NStrawHits());
  assert (p1.NStraw() == p2.NStraw());
  assert (p1.NStrawTime() == p2.NStrawTime());
  assert (p1.NTRHits() == p2.NTRHits());
  assert (p1.NPixelSpacePoints() == p2.NPixelSpacePoints());
  assert (p1.NSCT_SpacePoints() == p2.NSCT_SpacePoints());
  assert (p1.HitPattern() == p2.HitPattern());
  //assert (p1.rdoList() == p2.rdoList());
  compare (*p1.param(),  *p2.param());
  compare (*p1.endParam(), *p2.endParam());

  // These aren't saved.
  assert (nullptr == p2.siSpacePoints());
  assert (nullptr == p2.trtDriftCircles());
#if 0
  const std::vector <const TrigSiSpacePoint*>& sp1 = *p1.siSpacePoints();
  const std::vector <const TrigSiSpacePoint*>& sp2 = *p2.siSpacePoints();
  assert (sp1.size() == sp2.size());
  for (size_t i = 0; i < sp1.size(); i++)
    compare (*sp1[i], *sp2[i]);

  const std::vector<const InDet::TRT_DriftCircle*>& tc1 = *p1.trtDriftCircles();
  const std::vector<const InDet::TRT_DriftCircle*>& tc2 = *p2.trtDriftCircles();
  assert (tc1.size() == tc2.size());
  for (size_t i = 0; i < tc1.size(); i++)
    compare (*tc1[i], *tc2[i]);
#endif
}


void compare (const TrackInVertexList& p1,
              const TrackInVertexList& p2)
{
  assert (p1.size() == p2.size());
  TrackInVertexList::const_iterator it1 = p1.begin();
  TrackInVertexList::const_iterator it2 = p2.begin();
  while (it1 != p1.end()) {
    compare (**it1, **it2);
    ++it1;
    ++it2;
  }
}


void compare (const TrigVertex& p1,
              const TrigVertex& p2)
{
  assert (p1.x() == p2.x());
  assert (p1.y() == p2.y());
  assert (p1.z() == p2.z());
  assert (p1.mass() == p2.mass());
  assert (p1.massVariance() == p2.massVariance());
  const double* cov1 = p1.cov();
  const double* cov2 = p2.cov();
  for (int i=0; i < 6; i++)
    assert (cov1[i] == cov2[i]);
  assert (p1.energyFraction() == p2.energyFraction());
  assert (p1.nTwoTracksSecVtx() == p2.nTwoTracksSecVtx());
  //assert (p1.decayLength() == p2.decayLength());
  assert (0 == p2.decayLength());
  //assert (p1.decayLengthSignificance() == p2.decayLengthSignificance());
  assert (0 == p2.decayLengthSignificance());
  //assert (p1.position() == p2.position());
  assert (p1.chi2() == p2.chi2());
  assert (p1.ndof() == p2.ndof());
  assert (p1.algorithmId() == p2.algorithmId());
  //compare (*p1.getMotherTrack(), *p2.getMotherTrack());
  assert (nullptr == p2.getMotherTrack());
  compare (*p1.tracks(), *p2.tracks());
}


void testit (const TrigVertex& trans1)
{
  MsgStream log (0, "test");
  TrigVertexCnv_p2 cnv;
  TrigVertexCollectionCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigVertex_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigVertex trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


TrigInDetTrack makeTrack (int offs)
{
  std::vector<double> cov1
  { 201.5*201.5+offs, 202.5+offs, 203.5+offs, 204.5+offs, 205.5+offs,
    206.5*206.5+offs, 207.5+offs, 208.5+offs, 209.5+offs,
    210.5*210.5+offs, 211.5+offs, 212.5+offs,
      213.5*213.5+offs, 214.5+offs,
      215.5*215.5+offs };
  std::vector<double> cov2
    { 301.5*301.5+offs, 302.5+offs, 303.5+offs, 304.5+offs, 305.5+offs,
      306.5*306.5+offs, 307.5+offs, 308.5+offs, 309.5+offs,
      310.5*310.5+offs, 311.5+offs, 312.5+offs,
      313.5*313.5+offs, 314.5+offs,
      315.5*315.5+offs };
  auto param = CxxUtils::make_unique<TrigInDetTrackFitPar>
    (2.5+offs, 3.5+offs, 4.5+offs, 5.5+offs, 6.5+offs, TrigInDetTrackFitPar::PERIGEE, 7.5+offs,
    new std::vector<double> (cov1));
  auto endParam = CxxUtils::make_unique<TrigInDetTrackFitPar>
    (102.5+offs, 103.5+offs, 104.5+offs, 105.5+offs, 106.5+offs, TrigInDetTrackFitPar::BARREL, 107.5+offs,
    new std::vector<double> (cov2));
  TrigInDetTrack trans1 (param.release(), endParam.release(), 1.5+offs);
  trans1.algorithmId (TrigInDetTrack::IDSCANID);
  trans1.StrawHits (1+offs);
  trans1.Straw (2+offs);
  trans1.StrawTime (3+offs);
  trans1.TRHits (4+offs);
  trans1.NPixelSpacePoints (5+offs);
  trans1.NSCT_SpacePoints (6+offs);
  trans1.HitPattern (7+offs);
  std::vector<Identifier> rdolist {Identifier(123+offs), Identifier(234+offs)};
  trans1.rdoList (std::move(rdolist));
  return trans1;
}


void test1()
{
  std::cout << "test1\n";

  auto tracks = CxxUtils::make_unique<TrackInVertexList>();
  TrigInDetTrack track1 = makeTrack(0);
  TrigInDetTrack track2 = makeTrack(500);
  tracks->push_back (&track1);
  tracks->push_back (&track2);

  double cov[6] = {101.5, 102.5, 103.5, 104.5, 105.5, 106};
  TrigVertex trans1 (1.5, 2.5, 3.5, cov, 4.5, 5, tracks.release(),
                     6.5, 7.5, 8, 9.5, 10.5,
                     TrigVertex::IDSCANID);
  trans1.setMassVariance (18.5);
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
