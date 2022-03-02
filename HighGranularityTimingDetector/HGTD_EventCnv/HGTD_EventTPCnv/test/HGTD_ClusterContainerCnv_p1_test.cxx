/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_EventTPCnv/test/HGTD_ClusterContainerCnv_p1_test.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @date Apr, 2021
 * @brief Tests the functionality of the HGTD_Cluster container converters by
 *        converting them from T to P and then revert the process and compare.
 */

#include "GaudiKernel/MsgStream.h"
#include "HGTD_EventTPCnv/HGTD_ClusterContainerCnv_p1.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "IdDictParser/IdDictParser.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include <cassert>
#include <iostream>
#include <memory>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "HGTD_EventTPCnv_testfunctions.cxx"


void compare(const HGTD_ClusterContainer& p1,
             const HGTD_ClusterContainer& p2) {
  HGTD_ClusterContainer::const_iterator it1 = p1.begin();
  HGTD_ClusterContainer::const_iterator it1e = p1.end();
  HGTD_ClusterContainer::const_iterator it2 = p2.begin();
  HGTD_ClusterContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    BOOST_CHECK(it1.hashId() == it2.hashId());
    const HGTD_ClusterCollection& coll1 = **it1;
    const HGTD_ClusterCollection& coll2 = **it2;
    BOOST_CHECK(coll1.size() == coll2.size());
    for (size_t j = 0; j < coll1.size(); j++) {
      HGTDtest::compare(*coll1.at(j), *coll2.at(j));
    }
    ++it1;
    ++it2;
  }
  BOOST_CHECK(it1 == it1e && it2 == it2e);
}

std::unique_ptr<HGTD_Cluster>
createCluster(int endcap, int layer, int phi_module, int eta_module, float locx,
              float locy, float colx, float coly, float phi, float rz,
              float toa, const std::vector<int>& tot,
              const HGTD_ID* hgtd_idhelper) {

  Identifier id =
      hgtd_idhelper->wafer_id(endcap, layer, phi_module, eta_module);
  Amg::Vector2D locpos(locx, locy);
  std::vector<Identifier> rdoList{Identifier(id)};
  InDet::SiWidth width(Amg::Vector2D(colx, coly), Amg::Vector2D(phi, rz));

  Amg::MatrixX cov(2, 2);
  float sig = 1.3 / std::sqrt(12.);
  cov(0, 0) = sig * sig;
  cov(1, 1) = sig * sig;
  cov(0, 1) = 0;
  cov(1, 0) = 0;

  return std::make_unique<HGTD_Cluster>(
      Identifier(id), locpos, std::move(rdoList), width, nullptr, std::move(cov),
      toa, 0.035, tot);
}

std::unique_ptr<const HGTD_ClusterContainer> makeClusters(HGTD_ID* hgtd_idhelper) {
  auto cont = std::make_unique<HGTD_ClusterContainer>(5);

  std::vector<int> n_coll_per_layer = {2, 1, 3};

  for (int hash = 2; hash <= 3; hash++) {
    auto coll =
        std::make_unique<HGTD_ClusterCollection>(IdentifierHash(hash));

    for (int layer_i = 0; layer_i < 3; layer_i++) {
      for (int i_clus = 0; i_clus < n_coll_per_layer.at(layer_i); i_clus++) {
        // being very verbose about the dummy arguments for better control
        int endcap = 2;
        int layer = layer_i;
        int phi_module = layer + i_clus;
        int eta_module = i_clus + 2;
        float locx = 1.3 * i_clus;
        float locy = 1.3 * i_clus;
        float colx = static_cast<int>(locx / 1.3) + 15;
        float coly = static_cast<int>(locy / 1.3) + 7;
        float phi = 0;  // dummy
        float rz = 400; // dummy
        float toa = 14.5397;
        std::vector<int> tot = {187}; // 0 - 255 in ASIC
        // for valid ID values look at
        // https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/InnerDetector/InDetDetDescr/InDetIdDictFiles/data/IdDictInnerDetector_ITK_HGTD_23.xml?v=21.9
        std::cout << "endcap " << endcap << '\n';
        std::cout << "layer " << layer << '\n';
        std::cout << "phi_module " << phi_module << '\n';
        std::cout << "eta_module " << eta_module << '\n';
        std::cout << "locx " << locx << '\n';
        std::cout << "locy " << locy << '\n';
        std::cout << "colx " << colx << '\n';
        std::cout << "coly " << coly << '\n';
        std::cout << "phi " << phi << '\n';
        std::cout << "rz " << rz << '\n';
        std::cout << "toa " << toa << '\n';
        std::cout << "tot " << tot.at(0) << '\n';
        auto cluster = createCluster(endcap, layer, phi_module, eta_module,
                                     locx, locy, colx, coly, phi, rz, toa, tot,
                                     hgtd_idhelper);
        coll->push_back(std::move(cluster));
      }
    }
    assert( cont->addCollection (coll.release(), hash).isSuccess() );
  }
  return cont;
}

void registerIDHelperAndDetManager() {
  IdDictParser parser;
  parser.register_external_entity(
      "InnerDetector", "InDetIdDictFiles/IdDictInnerDetector_ITK_HGTD_24.xml");
  IdDictMgr& idd = parser.parse("IdDictParser/ATLAS_IDS.xml");

  auto hgtd_id{std::make_unique<HGTD_ID>()};
  hgtd_id->initialize_from_dictionary(idd);

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StoreGateSvc* sg = nullptr;
  BOOST_CHECK(svcLoc->service("DetectorStore", sg).isSuccess());
  BOOST_CHECK(sg->record(std::move(hgtd_id), "HGTD_ID"));

  auto hgtd_det_mgr{std::make_unique<HGTD_DetectorManager>(sg)};
  BOOST_CHECK(sg->record(std::move(hgtd_det_mgr), "HGTD"));
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

BOOST_AUTO_TEST_CASE(HGTD_ClusterContainerCnv_p1_test) {

  //initialise Gaudi for testing
  ISvcLocator* pSvcLoc;
  BOOST_REQUIRE(Athena_test::initGaudi("HGTD_EventTPCnv_test.txt", pSvcLoc));

  //register the relevant helper classe
  registerIDHelperAndDetManager();

  HGTD_ID* hgtd_idhelper = retrieve();
  BOOST_CHECK(hgtd_idhelper != nullptr);

  hgtd_idhelper->set_do_checks(true);

  Identifier id = hgtd_idhelper->wafer_id(2, 1, 5, 10);
  Identifier id2 = hgtd_idhelper->wafer_id(-2, 5, 1, 1);

  IdentifierHash id_hash = hgtd_idhelper->wafer_hash(id);
  //
  std::cout << "id " << id.get_compact() << '\n';
  std::cout << "id_hash " << id_hash << '\n';

  std::cout << "id2 " << id2.get_compact() << '\n';
  std::cout << "id2 " << hgtd_idhelper->wafer_hash(id2) << '\n';

  std::unique_ptr<const HGTD_ClusterContainer> trans1 = makeClusters(hgtd_idhelper);
  BOOST_REQUIRE(trans1->size() > 0); // otherwise there is nothing to test
  //
  MsgStream log(0, "test");
  HGTD_ClusterContainerCnv_p1 cnv;

  HGTD_ClusterContainer_p1 pers;

  cnv.transToPers(trans1.get(), &pers, log);

  std::unique_ptr<HGTD_ClusterContainer> trans2(
      cnv.createTransient(&pers, log));

  compare(*trans1, *trans2);
}
