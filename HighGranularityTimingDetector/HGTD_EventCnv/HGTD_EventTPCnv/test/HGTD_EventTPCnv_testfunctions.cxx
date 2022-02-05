/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/test/HGTD_EventTPCnv_testfunctions.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Provides a number of functions run in several of the tests to avoid
 *        code duplication
 */

namespace HGTDtest {

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
  BOOST_CHECK(p1.timeResolution() == p2.timeResolution());
  BOOST_CHECK(p1.totList() == p2.totList());
  std::cout << "compare HGTD_Cluster done\n";
}

} // namespace HGTDtest
