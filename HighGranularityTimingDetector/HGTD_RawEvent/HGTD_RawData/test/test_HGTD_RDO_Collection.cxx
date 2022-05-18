/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/test/test_HGTD_RDO_Collection.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>

 * @brief Unit test of the HGTD_RDO_Collection class.
 */

#include "HGTD_RawData/HGTD_RDO.h"
#include "HGTD_RawData/HGTD_RDO_Collection.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include <memory>
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

std::unique_ptr<HGTD_RDO> createRDO(unsigned int id, float time, unsigned int tot,
                                    unsigned short bcid, unsigned short lv1a, unsigned short lv1id) {
  std::cout << "createRDO\n";

  Identifier identifier(id);

  return std::make_unique<HGTD_RDO>(identifier, time, tot, bcid, lv1a, lv1id);
}

BOOST_AUTO_TEST_CASE(HGTD_RDO_Collection_test) {
  // create a collection
  auto coll = std::make_unique<HGTD_RDO_Collection>(IdentifierHash(2));
  // fill it with RDOs
  for (unsigned int id = 1234; id < 1244; id++) {
    std::unique_ptr<HGTD_RDO> rdo = createRDO(id, 14.8348, 266, 1, 2, 3);
    coll->push_back(std::move(rdo));
  }

  BOOST_CHECK(coll->identifierHash() == IdentifierHash(2));
  BOOST_CHECK(coll->size() == 10);
}
