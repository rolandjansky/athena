/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/test/test_HGTD_RDO.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Unit test of the HGTD_RDO class, testing initialisation
 * and all constructors.
 */

#include "HGTD_RawData/HGTD_RDO.h"
#include "Identifier/Identifier.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

HGTD_RDO createRDO() {
  std::cout << "createRDO\n";

  Identifier id(5678);

  HGTD_RDO rdo(id, 14.537, 212, 1, 2, 3);

  std::cout << "createRDO done\n";
  return rdo;
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

void testCopyCtor(const HGTD_RDO& rdo) {
  std::cout << "testCopyCtor\n";
  HGTD_RDO copied_rdo(rdo);

  compare(rdo, copied_rdo);
  std::cout << "testCopyCtor done\n";
}

void testAssignment(const HGTD_RDO& rdo) {
  std::cout << "testAssignment\n";
  HGTD_RDO assigned_rdo = rdo;

  compare(rdo, assigned_rdo);
  std::cout << "testAssignment done\n";
}

void testMoveCtor(HGTD_RDO rdo) {
  std::cout << "testMoveCtor\n";
  HGTD_RDO copied_rdo(std::move(rdo));

  compare(rdo, copied_rdo);
  std::cout << "testMoveCtor done\n";
}

void testMoveAssignment(HGTD_RDO rdo) {
  std::cout << "testMoveAssignment\n";
  HGTD_RDO move_assign_rdo = std::move(rdo);

  compare(rdo, move_assign_rdo);

  std::cout << "testMoveAssignment done\n";
}

BOOST_AUTO_TEST_CASE(HGTD_RDO_test, *boost::unit_test::tolerance(1e-10)) {

  std::cout << "running test_HGTD_RDO\n";

  // don't bother with default ctor, should not be used!

  Identifier id(1234);

  float toa = 14.8348;

  HGTD_RDO rdo(id, toa, 266, 1, 2, 3);

  BOOST_CHECK(rdo.identify() == id);
  BOOST_CHECK(rdo.getTOA() == toa);
  BOOST_CHECK(rdo.getTOT() == 266);
  BOOST_CHECK(rdo.getBCID() == 1);
  BOOST_CHECK(rdo.getL1ID() == 2);
  BOOST_CHECK(rdo.getL1A() == 3);

  HGTD_RDO rdo2 = createRDO();

  testCopyCtor(rdo2);

  testAssignment(rdo2);

  testMoveCtor(rdo2);

  testMoveAssignment(rdo2);

  std::cout << "running test_HGTD_RDO done\n";
}
