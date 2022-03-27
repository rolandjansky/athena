/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RIO_OnTrack/test/test_HGTD_ClusterOnTrack.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date July, 2021
 * @brief Basic test of the HGTD_ClusterOnTrack class, testing initialisation
 *        and all constructors.
 */

#include "HGTD_RIO_OnTrack/HGTD_ClusterOnTrack.h"
#include "Identifier/Identifier.h"
#include <memory>
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

const float dummy_toa = 14.5;
const float dummy_toa_res = 0.35;
const std::vector<int> dummy_tot = {255};

void compare(const Trk::PrepRawData* p1, const Trk::PrepRawData* p2) {
  std::cout << "compare PrepRawData\n";
  if (p1 == nullptr or p2 == nullptr) {
    std::cout << "WARNING: can't do PrepRawData comparison with nullpointers\n";
    return;
  }
  BOOST_CHECK(p1->identify() == p2->identify());
  BOOST_CHECK(p1->localPosition() == p2->localPosition());
  BOOST_CHECK(p1->rdoList() == p2->rdoList());
  BOOST_CHECK(p1->localCovariance() == p2->localCovariance());
  std::cout << "compare PrepRawData done\n";
}
void compare(const HGTD_ClusterOnTrack& p1,
             const HGTD_ClusterOnTrack& p2) {
  std::cout << "compare HGTD_ClusterOnTrack\n";
  BOOST_CHECK(p1.idDE() == p2.idDE());
  compare(p1.prepRawData(), p2.prepRawData());
  BOOST_CHECK(p1.detectorElement() == p2.detectorElement());
  BOOST_CHECK(p1.time() == p2.time());
  BOOST_CHECK(p1.timeResolution() == p2.timeResolution());
  std::cout << "compare HGTD_ClusterOnTrack done\n";
}

void testDefaultCtor() {
  HGTD_ClusterOnTrack cluster;
  BOOST_CHECK(cluster.detectorElement() == nullptr);
}

HGTD_Cluster createCluster() {
  std::cout << "createCluster\n";

  Amg::Vector2D locpos(1.5, 2.5);
  std::vector<Identifier> rdoList{Identifier(1234)};

  InDet::SiWidth width(Amg::Vector2D(1, 2), Amg::Vector2D(3.5, 4.5));

  Amg::MatrixX cov(2, 2);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      cov(i, j) = 100 * (i + 1) * (j + 1);

  HGTD_Cluster cluster(Identifier(1234), locpos, rdoList, width, nullptr,
                       std::move(cov), dummy_toa, dummy_toa_res,
                       dummy_tot);

  std::cout << "createCluster done\n";
  return cluster;
}

HGTD_ClusterOnTrack
createClusterOnTrackWithCluster(const HGTD_Cluster& cluster) {

  Amg::Vector2D locpars(1.5, 2.5);

  Amg::MatrixX cov(2, 2);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      cov(i, j) = 100 * (i + 1) * (j + 1);

  HGTD_ClusterOnTrack cot(&cluster, locpars, cov, dummy_toa,
                          dummy_toa_res, IdentifierHash(1234));

  return cot;
}

void testCopyCtor(const HGTD_ClusterOnTrack& cot) {
  std::cout << "testCopyCtor\n";
  HGTD_ClusterOnTrack copied_cot(cot);

  compare(cot, copied_cot);
  std::cout << "testCopyCtor done\n";
}

void testAssignment(const HGTD_ClusterOnTrack& cot) {
  std::cout << "testAssignment\n";
  HGTD_ClusterOnTrack copied_cot;
  copied_cot = cot;

  compare(cot, copied_cot);
  std::cout << "testAssignment done\n";
}

void testMoveCtor(HGTD_ClusterOnTrack cot) {
  std::cout << "testMoveCtor\n";
  HGTD_ClusterOnTrack copied_cot(std::move(cot));

  compare(cot, copied_cot);
  std::cout << "testMoveCtor done\n";
}

void testMoveAssignment(HGTD_ClusterOnTrack cot) {
  std::cout << "testMoveAssignment\n";
  HGTD_ClusterOnTrack move_assign_cot;
  move_assign_cot = std::move(cot);

  compare(cot, move_assign_cot);
  std::cout << "testMoveAssignment done\n";
}

BOOST_AUTO_TEST_CASE(HGTD_ClusterOnTrack_test) {

  std::cout << "running test_HGTD_ClusterOnTrack\n";

  testDefaultCtor();

  std::cout << "testing with HGTD_Cluster\n";

  HGTD_Cluster cluster = createCluster();
  // cluster needs to be on heap, otherwise ownership issues arrise when COT is
  // deleted
  HGTD_Cluster* cluster_ptr = new HGTD_Cluster(cluster);

  std::cout << "HGTD_Cluster created\n";

  auto cluster_on_track2 = createClusterOnTrackWithCluster(*cluster_ptr);

  std::cout << "HGTD_ClusterOnTrack created\n";

  testCopyCtor(cluster_on_track2);

  testAssignment(cluster_on_track2);

  testMoveCtor(cluster_on_track2);

  testMoveAssignment(cluster_on_track2);

  delete cluster_ptr;
  std::cout << "test_HGTD_ClusterOnTrack done\n";
}
