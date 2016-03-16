/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigInDetTrackCnv_p5_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigInDetTrackCnv_p5.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p5.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp4.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
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


#if 0
void compare (const Trk::PrepRawData& p1,
              const Trk::PrepRawData& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.localPosition() == p2.localPosition());
  assert (p1.rdoList() == p2.rdoList());
  assert (p1.localCovariance() == p2.localCovariance());
}


void compare (const InDet::SiCluster& p1,
              const InDet::SiCluster& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  compare (p1.width(), p2.width());
  assert (p1.globalPosition() == p2.globalPosition());  // ???
  assert (p1.gangedPixel() == p2.gangedPixel());        // ???
  assert (p1.detectorElement() == p2.detectorElement());
}


void compare (const TrigSiSpacePoint& p1,
              const TrigSiSpacePoint& p2)
{
  assert (p1.identify() == p2.identify());
  compare (*p1.clusters().first, *p2.clusters().first);
  compare (*p1.clusters().second, *p2.clusters().second);
}


void compare (const InDet::TRT_DriftCircle& p1,
              const InDet::TRT_DriftCircle& p2)
{
  compare (static_cast<const Trk::PrepRawData&> (p1),
           static_cast<const Trk::PrepRawData&> (p2));
  assert (p1.getWord() == p2.getWord());
}
#endif


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
  assert (p1.rdoList() == p2.rdoList());
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


void testit (const TrigInDetTrack& trans1)
{
  MsgStream log (0, "test");
  TrigInDetTrackCnv_p5 cnv;
  TrigInDetTrackCollectionCnv_tlp4 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigInDetTrack_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigInDetTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  std::vector<double> cov1
  { 201.5*201.5, 202.5, 203.5, 204.5, 205.5,
    206.5*206.5, 207.5, 208.5, 209.5,
    210.5*210.5, 211.5, 212.5,
      213.5*213.5, 214.5,
      215.5*215.5 };
  std::vector<double> cov2
    { 301.5*301.5, 302.5, 303.5, 304.5, 305.5,
      306.5*306.5, 307.5, 308.5, 309.5,
      310.5*310.5, 311.5, 312.5,
      313.5*313.5, 314.5,
      315.5*315.5 };
  auto param = CxxUtils::make_unique<TrigInDetTrackFitPar>
    (2.5, 3.5, 4.5, 5.5, 6.5, TrigInDetTrackFitPar::PERIGEE, 7.5,
    new std::vector<double> (cov1));
  auto endParam = CxxUtils::make_unique<TrigInDetTrackFitPar>
    (102.5, 103.5, 104.5, 105.5, 106.5, TrigInDetTrackFitPar::BARREL, 107.5,
    new std::vector<double> (cov2));
  TrigInDetTrack trans1 (param.release(), endParam.release(), 1.5);
  trans1.algorithmId (TrigInDetTrack::IDSCANID);
  trans1.StrawHits (1);
  trans1.Straw (2);
  trans1.StrawTime (3);
  trans1.TRHits (4);
  trans1.NPixelSpacePoints (5);
  trans1.NSCT_SpacePoints (6);
  trans1.HitPattern (7);
  std::vector<Identifier> rdolist {Identifier(123), Identifier(234)};
  trans1.rdoList (std::move(rdolist));

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
