/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RawData/test/test_HGTD_RDOCont.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date August, 2021
 * @brief Unit test of the HGTD_RDOCollection class.
 */

#include "HGTD_RawData/HGTD_RDO.h"
#include "HGTD_RawData/HGTD_RDOCollection.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"

#include <memory>
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

std::unique_ptr<HGTD_RDO> createRDO(int id, float time, int tot, int bcid,
                                          int lv1a, int lv1id) {
  std::cout << "createRDO\n";

  Identifier identifier(id);

  return std::make_unique<HGTD_RDO>(identifier, time, tot, bcid, lv1a,
                                          lv1id);
}

BOOST_AUTO_TEST_CASE(HGTD_RDOCont) {

  // initialise Gaudi for testing
  ISvcLocator* pSvcLoc;
  BOOST_REQUIRE(Athena_test::initGaudi("test_HGTD_RDOCont.txt", pSvcLoc));

  ISvcLocator* svc_locator = Gaudi::svcLocator();
  // get StoreGate service

  StoreGateSvc* storegate;

  StatusCode sc = svc_locator->service("StoreGateSvc", storegate);
  if (sc.isFailure()) {
    BOOST_TEST(false);
  }

  auto container = std::make_unique<HGTD_RDOContainer>(5);

  for (int hash = 2; hash <= 3; hash++) {
    // create a collection
    auto collection =
        std::make_unique<HGTD_RDOCollection>(IdentifierHash(hash));
    // fill it with RDOs
    for (int id = 1234; id < 1244; id++) {
      std::unique_ptr<HGTD_RDO> rdo =
          createRDO(id, 14.8348, 266, 1, 2, 3);
      collection->push_back(std::move(rdo));
    }

    BOOST_CHECK(collection->identifierHash() == IdentifierHash(hash));

    sc = container->addCollection(collection.release(), hash);
    if (sc.isFailure()) {
      std::cout << "Could not add HGTD_RDOCollection to the HGTD_RDOContainer!\n";
      BOOST_TEST(false);
    }
  }

  // Get the sct helper from the detector store
  sc = storegate->record(container.release(), "HGTD_RDOContainer");
  if (sc.isFailure()) {
    std::cout << "Could not record the HGTD_RDOContainer!\n";
    BOOST_TEST(false);
  }
}
