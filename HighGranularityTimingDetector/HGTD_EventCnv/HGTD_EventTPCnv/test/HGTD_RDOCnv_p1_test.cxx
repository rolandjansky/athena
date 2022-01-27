/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/test/HGTD_RDOCnv_p1_test.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Tests the conversion between transient and persistent version of the
 *        HGTD_RDO class.
 */

#include "GaudiKernel/MsgStream.h"
#include "HGTD_EventTPCnv/HGTD_RDOCnv_p1.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

HGTD_RDO createRDO(int id, float time, int tot, int bcid, int lv1a,
                         int lv1id) {
  std::cout << "createRDO\n";

  Identifier identifier(id);

  return HGTD_RDO(identifier, time, tot, bcid, lv1a, lv1id);
}

void compare(const HGTD_RDO& p1, const HGTD_RDO& p2) {
  std::cout << "compare HGTD_RDO\n";
  BOOST_CHECK(p1.identify() == p2.identify());
  BOOST_CHECK(p1.getTOA() == p2.getTOA());
  BOOST_CHECK(p1.getTOT() == p2.getTOT());
  BOOST_CHECK(p1.getBCID() == p2.getBCID());
  BOOST_CHECK(p1.getL1ID() == p2.getL1ID());
  BOOST_CHECK(p1.getL1A() == p2.getL1A());
  std::cout << "compare HGTD_RDO done\n";
}

void convertAndBack(const HGTD_RDO& trans1) {

  std::cout << "convertAndBack\n";

  MsgStream log(0, "test");
  HGTD_RDOCnv_p1 cnv;
  HGTD_RDO_p1 pers;

  cnv.transToPers(&trans1, &pers, log);

  HGTD_RDO trans2;
  cnv.persToTrans(&pers, &trans2, log);

  compare(trans1, trans2);
  std::cout << "convertAndBack done\n";
}

BOOST_AUTO_TEST_CASE(HGTD_RDOCnv_p1_test) {

  std::cout << "start test\n";

  HGTD_RDO rdo = createRDO(1234, 14.8348, 266, 1, 2, 3);

  convertAndBack(rdo);
}
