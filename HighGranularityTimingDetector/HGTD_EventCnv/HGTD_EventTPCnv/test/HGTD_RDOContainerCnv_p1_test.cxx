/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/test/HGTD_RDOContainerCnv_p1_test.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief
 */

#include "GaudiKernel/MsgStream.h"
#include "HGTD_EventTPCnv/HGTD_RDOContainerCnv_p1.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_RawData/HGTD_RDOContainer.h"
#include "IdDictParser/IdDictParser.h"
#include "Identifier/Identifier.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"

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

void compare(const HGTD_RDOContainer& p1,
             const HGTD_RDOContainer& p2) {
  std::cout << "start HGTD_RDOContainer comparison\n";
  HGTD_RDOContainer::const_iterator it1 = p1.begin();
  HGTD_RDOContainer::const_iterator it1e = p1.end();
  HGTD_RDOContainer::const_iterator it2 = p2.begin();
  HGTD_RDOContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    BOOST_CHECK(it1.hashId() == it2.hashId());
    const HGTD_RDOCollection& coll1 = **it1;
    const HGTD_RDOCollection& coll2 = **it2;
    BOOST_CHECK(coll1.size() == coll2.size());
    for (size_t j = 0; j < coll1.size(); j++) {
      compare(*coll1.at(j), *coll2.at(j));
    }
    ++it1;
    ++it2;
  }
  BOOST_CHECK(it1 == it1e && it2 == it2e);
}

std::unique_ptr<const HGTD_RDOContainer> makeRDOContainer(const HGTD_ID* hgtd_idhelper) {
  auto container = std::make_unique<HGTD_RDOContainer>(5);

  for (int hash = 2; hash <= 3; hash++) {
    // create a collection
    auto collection =
        std::make_unique<HGTD_RDOCollection>(IdentifierHash(hash));
    // fill it with RDOs
    for (int i = 1; i < 10; i++) {

      Identifier id = hgtd_idhelper->wafer_id(2, 1, hash, i);

      std::unique_ptr<HGTD_RDO> rdo =
          createRDO(id.get_compact(), 14.8348, 266, 1, 2, 3);
      collection->push_back(std::move(rdo));
    }

    BOOST_CHECK(collection->identifierHash() == IdentifierHash(hash));

    BOOST_CHECK(container->addCollection(collection.release(), hash).isSuccess());
  }
  return container;
}

void registerIDHelperAndDetManager() {
  IdDictParser parser;
  parser.register_external_entity(
      "InnerDetector", "InDetIdDictFiles/IdDictInnerDetector_ITK_HGTD_23.xml");
  IdDictMgr& idd = parser.parse("IdDictParser/ATLAS_IDS.xml");

  auto hgtd_id{std::make_unique<HGTD_ID>()};
  hgtd_id->initialize_from_dictionary(idd);

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = nullptr;
  BOOST_CHECK(svcLoc->service("DetectorStore", sg).isSuccess());
  BOOST_CHECK(sg->record(std::move(hgtd_id), "HGTD_ID"));
}

HGTD_ID* retrieve() {
  std::cout << "retrieve \n";

  // Get Storegate, ID helpers, and so on
  ISvcLocator* svc_locator = Gaudi::svcLocator();
  // get StoreGate service

  StoreGateSvc* storegate;

  StatusCode sc = svc_locator->service("StoreGateSvc", storegate);
  if (sc.isFailure()) {
    std::cout << "StoreGate service not found !\n";
    return nullptr;
  }

  // get DetectorStore service
  StoreGateSvc* detStore;
  sc = svc_locator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    std::cout << "DetectorStore service not found !\n";
    return nullptr;
  }

  // Get the sct helper from the detector store
  HGTD_ID* hgtd_idhelper = nullptr;
  sc = detStore->retrieve(hgtd_idhelper, "HGTD_ID");
  if (sc.isFailure()) {
    std::cout << "Could not get HGTD_ID helper !\n";
    return nullptr;
  } else {
    std::cout << "[retrieve] HGTD_ID initialised\n";
  }

  return hgtd_idhelper;
}

BOOST_AUTO_TEST_CASE(HGTD_RDOContainerCnv_p1_test) {

  std::cout << "start HGTD_RDOContainerCnv_p1_test\n";

  // initialise Gaudi for testing
  ISvcLocator* pSvcLoc;
  BOOST_REQUIRE(Athena_test::initGaudi("HGTD_EventTPCnv_test.txt", pSvcLoc));

  registerIDHelperAndDetManager();

  HGTD_ID* hgtd_idhelper = retrieve();
  BOOST_CHECK(hgtd_idhelper != nullptr);

  hgtd_idhelper->set_do_checks(true);

  std::unique_ptr<const HGTD_RDOContainer> trans_container =
      makeRDOContainer(hgtd_idhelper);
  std::cout << "HGTD_RDOContainer created\n";
  // otherwise there is nothing to test
  BOOST_REQUIRE(trans_container->size() > 0);

  MsgStream log(0, "test");
  HGTD_RDOContainerCnv_p1 cnv;

  HGTD_RDOContainer_p1 pers;

  std::cout << "HGTD_RDOContainer transToPers\n";
  cnv.transToPers(trans_container.get(), &pers, log);
  std::cout << "HGTD_RDOContainer transToPers done\n";

  HGTD_RDOContainer trans(hgtd_idhelper->wafer_hash_max());

  std::cout << "HGTD_RDOContainer persToTrans\n";
  cnv.persToTrans(&pers, &trans, log);
  std::cout << "HGTD_RDOContainer persToTrans done\n";

  compare(*trans_container, trans);

  std::cout << "done\n";
}
