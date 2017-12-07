/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CompetingMuonClustersOnTrackCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrackCnv_p2.h"
#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp2.h"
#include "TrkEventTPCnv/TrackCollectionCnv_tlp5.h"
#include "TestTools/leakcheck.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/initGaudi.h"
#include "TPTools/AthenaConverterTLPExtension.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


#include "make_dd.icc"


void compare (const Trk::Surface& s1,
              const Trk::Surface& s2)
{
  if (s1.name() == "Trk::ConeSurface") {
    assert (s2.name() == "Trk::PlaneSurface");
  }
  else {
    assert (s1.name() == s2.name());
    if (s1.name() == "Trk::PerigeeSurface" ||
        s1.name() == "Trk::PlaneSurface" ||
        s1.name() == "Trk::StraightLineSurface")
    {
      for (int i=0; i < 4; i++)
        for (int j=0; j < 4; j++)
          assert (Athena_test::isEqual (s1.transform()(i,j), s2.transform()(i,j)));
    }
    else {
      std::cout << "Unknown surface type: " << s1.name() << "\n";
      std::abort();
    }
  }
}


void compare (const Trk::CompetingRIOsOnTrack& p1,
              const Trk::CompetingRIOsOnTrack& p2)
{
  assert (p1.localParameters().size() == p2.localParameters().size());
  for (int i = 0; i < p1.localParameters().size(); i++) {
    Trk::ParamDefs ii = static_cast<Trk::ParamDefs>(i);
    assert (Athena_test::isEqual (p1.localParameters()[ii], p2.localParameters()[ii]));
  }
  assert (p1.localCovariance() == p2.localCovariance());

  assert (p1.indexOfMaxAssignProb() == p2.indexOfMaxAssignProb());
  assert (p1.numberOfContainedROTs() == p2.numberOfContainedROTs());
  for (unsigned int i=0; i < p1.numberOfContainedROTs(); i++)
    assert (p1.assignmentProbability(i) == p2.assignmentProbability(i));
}


void compare (const Muon::CompetingMuonClustersOnTrack& p1,
              const Muon::CompetingMuonClustersOnTrack& p2)
{
  compare (static_cast<const Trk::CompetingRIOsOnTrack&>(p1),
           static_cast<const Trk::CompetingRIOsOnTrack&>(p2));
  assert (p1.globalPosition() == p2.globalPosition());
  compare (p1.associatedSurface(), p2.associatedSurface());

  const std::vector<const Muon::MuonClusterOnTrack*>& rots1 = p1.containedROTs();
  const std::vector<const Muon::MuonClusterOnTrack*>& rots2 = p2.containedROTs();
  assert (rots1.size() == rots2.size());
}


void testit (const Muon::CompetingMuonClustersOnTrack& trans1)
{
  MsgStream log (0, "test");
  CompetingMuonClustersOnTrackCnv_p2 cnv;
  TrackCollectionCnv_tlp5 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  MuonMeasurementsCnv_tlp2 exttlcnv;
  exttlcnv.addTPConvertersTo (&tlcnv);
  Muon::CompetingMuonClustersOnTrack_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Muon::CompetingMuonClustersOnTrack trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


Muon::CscClusterOnTrack*
make_csccluster (const MuonGM::MuonDetectorManager& muo_dd)
{
  Amg::Vector2D locpos (1.5, 2.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  ElementLinkToIDC_CSC_Container rio (333, 222);

  // 50 == CSS
  Identifier clusId  = muo_dd.cscIdHelper()->channelID (50, 1, 2, 1, 1, 0, 1);

  return new Muon::CscClusterOnTrack (rio,
                                      locpos,
                                      cov,
                                      clusId,
                                      muo_dd.getCscReadoutElement (clusId),
                                      2.5,
                                      Muon::CscStatusSimple,
                                      Muon::CscTimeLate,
                                      5.5);
}


Muon::RpcClusterOnTrack*
make_rpccluster (const MuonGM::MuonDetectorManager& muo_dd)
{
  Amg::Vector2D locpos (1.5, 2.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  ElementLinkToIDC_RPC_Container rio (333, 222);

  // 4 == BOL
  Identifier clusId = muo_dd.rpcIdHelper()->channelID(4, 1, 2, 1, 1, 2, 1, 0, 1);

  return new Muon::RpcClusterOnTrack (rio,
                                      locpos,
                                      cov,
                                      clusId,
                                      muo_dd.getRpcReadoutElement (clusId),
                                      2.5,
                                      5.5);
}


void test1 (const MuonGM::MuonDetectorManager& muo_dd)
{
  std::cout << "test1\n";

  Trk::LocalParameters locpars (1.5, 2.5, 3.5, 4.5, 5.5);

  Amg::MatrixX cov(2,2);
  for (int i=0; i < 2; i++)
    for (int j=0; j < 2; j++)
      cov(i,j) = 100*(i+1)*(j+1);

  std::vector<double> probs { 5, 3, 10.5, 3, 1 };

  std::vector<const Muon::MuonClusterOnTrack*>* childrots =
    new std::vector<const Muon::MuonClusterOnTrack*>;

  childrots->push_back (make_csccluster (muo_dd));
  childrots->push_back (make_rpccluster (muo_dd));

  Muon::CompetingMuonClustersOnTrack trans1 (locpars,
                                             cov,
                                             new Trk::PerigeeSurface(Amg::Vector3D (50, 100, 150)),
                                             childrots,
                                             new std::vector<double> (probs));

  testit (trans1);
  Athena_test::Leakcheck check;
  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  const MuonGM::MuonDetectorManager& muo_dd = make_dd();

  test1 (muo_dd);
  return 0;
}
