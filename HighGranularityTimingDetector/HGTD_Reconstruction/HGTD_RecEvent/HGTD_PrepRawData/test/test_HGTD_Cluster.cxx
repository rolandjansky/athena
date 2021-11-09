/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_PrepRawData/test/test_HGTD_Cluster.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Basic test of the HGTD_Cluster class, testing initialisation and all
 *        constructors.
 */

#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "Identifier/Identifier.h"
#include <memory>
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

float dummy_toa = 14.5;
float dummy_toa_res = 0.35;
std::vector<int> dummy_tot = {255};

void compare(const InDet::SiWidth& p1, const InDet::SiWidth& p2) {
  std::cout << "compare SiWidth\n";
  BOOST_CHECK(p1.colRow() == p2.colRow());
  BOOST_CHECK(p1.widthPhiRZ() == p2.widthPhiRZ());
  std::cout << "compare SiWidth done\n";
}

void compare(const Trk::PrepRawData& p1, const Trk::PrepRawData& p2) {
  std::cout << "compare PrepRawData\n";
  BOOST_CHECK(p1.identify() == p2.identify());
  BOOST_CHECK(p1.localPosition() == p2.localPosition());
  BOOST_CHECK(p1.rdoList() == p2.rdoList());
  BOOST_CHECK(p1.localCovariance() == p2.localCovariance());
  std::cout << "compare PrepRawData done\n";
}

void compare(const HGTD_Cluster& p1, const HGTD_Cluster& p2) {
  std::cout << "compare HGTD_Cluster\n";
  compare(static_cast<const Trk::PrepRawData&>(p1),
          static_cast<const Trk::PrepRawData&>(p2));
  compare(p1.width(), p2.width());
  BOOST_CHECK(p1.detectorElement() == p2.detectorElement());
  BOOST_CHECK(p1.time() == p2.time());
  BOOST_CHECK(p1.totList() == p2.totList());
  std::cout << "compare HGTD_Cluster done\n";
}

void testDefaultCtor() {
  HGTD_Cluster cluster;
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

  HGTD_Cluster cluster(Identifier(1234), locpos, std::move(rdoList), width, nullptr,
                       std::move(cov), dummy_toa, dummy_toa_res,
                       dummy_tot);

  std::cout << "createCluster done\n";
  return cluster;
}

void testCopyCtor(const HGTD_Cluster& cluster) {
  std::cout << "testCopyCtor\n";
  HGTD_Cluster copied_cluster(cluster);

  compare(cluster, copied_cluster);
  std::cout << "testCopyCtor done\n";
}

void testAssignment(const HGTD_Cluster& cluster) {
  std::cout << "testAssignment\n";
  HGTD_Cluster copied_cluster;
  copied_cluster = cluster;

  compare(cluster, copied_cluster);
  std::cout << "testAssignment done\n";
}

void testMoveCtor(HGTD_Cluster cluster) {
  std::cout << "testMoveCtor\n";
  HGTD_Cluster copied_cluster(std::move(cluster));

  BOOST_CHECK(cluster.time() == 0.0);
  std::cout << "copied_cluster.time() " << copied_cluster.time() << '\n';
  BOOST_CHECK(copied_cluster.time() == dummy_toa);
  std::cout << "testMoveCtor done\n";
}

void testMoveAssignment(HGTD_Cluster cluster) {
  std::cout << "testMoveAssignment\n";
  HGTD_Cluster move_assign_cluster;
  move_assign_cluster = std::move(cluster);

  BOOST_CHECK(cluster.time() == 0.0);
  BOOST_CHECK(move_assign_cluster.time() == dummy_toa);
  BOOST_CHECK(cluster.time() != move_assign_cluster.time());

  std::cout << "testMoveAssignment done\n";
}

BOOST_AUTO_TEST_CASE(HGTD_Cluster_test) {

  std::cout << "running test_HGTD_Cluster\n";

  testDefaultCtor();

  auto cluster = createCluster();

  BOOST_TEST(cluster.time() == dummy_toa);
  std::cout << "cluster.time() " << cluster.time() << '\n';
  BOOST_TEST(cluster.timeResolution() == dummy_toa_res);
  BOOST_CHECK(cluster.totList() == dummy_tot);

  testCopyCtor(cluster);

  testAssignment(cluster);

  testMoveCtor(cluster);

  testMoveAssignment(cluster);

  std::cout << "running test_HGTD_Cluster done\n";
}
