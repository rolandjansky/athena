/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/test/HGTD_ClusterCnv_p1_test.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Tests the conversion between transient and persistent version of the
 *        HGTD_Cluster class.
 *
 *        HGTD_Cluster is converted to HGTD_Cluster_p1 and the reverse
 *        conversion is carried out afterwards. To check if this process works
 *        as expected, the individual data members are compared for equality.
 *        This is also done for the base class of HGTD_Cluster.
 */

#undef NDEBUG
#include "GaudiKernel/MsgStream.h"
#include "HGTD_EventTPCnv/HGTD_ClusterCnv_p1.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "IdDictParser/IdDictParser.h"
#include <cassert>
#include <iostream>
#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "HGTD_EventTPCnv_testfunctions.cxx"

void convertAndBack(const HGTD_Cluster& trans1) {
  std::cout << "convertAndBack\n";
  Identifier wafer_id = trans1.identify();
  std::cout << "Transient wafer ID: " << wafer_id << '\n';
  MsgStream log(0, "test");
  HGTD_ClusterCnv_p1 cnv;
  HGTD_Cluster_p1 pers;
  cnv.transToPers(&trans1, &pers, log);
  HGTD_Cluster trans2;
  cnv.persToTrans(&pers, &trans2, log);

  HGTDtest::compare(trans1, trans2);
  std::cout << "convertAndBack done\n";
}

HGTD_Cluster setupTransientCluster() {
  std::cout << "setupTransientCluster\n";

  Amg::Vector2D locpos(1.5, 2.5);
  std::vector<Identifier> rdoList{Identifier(1234)};

  InDet::SiWidth width(Amg::Vector2D(1, 2), Amg::Vector2D(3.5, 4.5));

  Amg::MatrixX cov(2, 2);
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      cov(i, j) = 100 * (i + 1) * (j + 1);

  HGTD_Cluster trans_cluster(Identifier(1234), locpos, std::move(rdoList), width,
                             nullptr, std::move(cov), 14.5, 0.35,
                             {145});

  std::cout << "setupTransientCluster done\n";
  return trans_cluster;
}

BOOST_AUTO_TEST_CASE(HGTD_ClusterCnv_p1_test) {

  std::cout << "start test\n";

  HGTD_Cluster cluster = setupTransientCluster();

  convertAndBack(cluster);

  std::cout << "finish test\n";
}
