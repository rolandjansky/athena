/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/InDetSimDataCollectionCnv_common_test.h
 * @author Susumu Oda <Susumu.Oda@cern.ch>
 * @date 2020-05-22
 * @brief Definitions of common methods for regression tests of InDetSimDataCollectionCnv_pX.
 * Based on MuonEventTPCnv/test/MuonSimDataCollectionCnv_p1_test.cxx
 * and InDetEventTPCnv/test/PixelClusterContainerCnv_p1_test.cxx.
 */

#undef NDEBUG

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "IdDictParser/IdDictParser.h"
#include "SGTools/TestStore.h"
#include "StoreGate/StoreGateSvc.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"

#include "GaudiKernel/MsgStream.h"

#include <cassert>
#include <iostream>


void compare(const HepMcParticleLink& p1,
             const HepMcParticleLink& p2)
{
  assert( p1.isValid() == p2.isValid() );
  assert( p1.barcode() == p2.barcode() );
  assert( p1.eventIndex() == p2.eventIndex() );
  assert( p1.getEventCollectionAsChar() == p2.getEventCollectionAsChar() );
  assert( p1.cptr() == p2.cptr() );
  assert( p1 == p2 );
}


void compare(const InDetSimData& p1,
             const InDetSimData& p2)
{
  assert(p1.word() == p2.word());
  const std::vector<InDetSimData::Deposit>& dep1 = p1.getdeposits();
  const std::vector<InDetSimData::Deposit>& dep2 = p2.getdeposits();
  assert(dep1.size() == dep2.size());
  // In InDetSimData.h, typedef std::pair<HepMcParticleLink, float> Deposit;
  for (size_t i = 0; i < dep1.size(); i++) {
    compare(dep1[i].first, dep2[i].first);
    assert(dep1[i].first == dep2[i].first);
    assert(dep1[i].second == dep2[i].second);
  }
}


void compare(const InDetSimDataCollection& p1,
             const InDetSimDataCollection& p2)
{
  assert(p1.size() == p2.size());
  InDetSimDataCollection::const_iterator it1 = p1.begin();
  InDetSimDataCollection::const_iterator it2 = p2.begin();
  for (; it1 != p1.end(); ++it1, ++it2) {
    assert(it1->first == it2->first);
    compare(it1->second, it2->second);
  }
}


// TCnv: InDetSimDataCollectionCnv_pX
// T: InDetSimDataCollection_pX
template<typename TCnv, typename T>
void testit(const InDetSimDataCollection& trans1)
{
  MsgStream log(0, "test");
  TCnv cnv;
  T pers;
  cnv.transToPers(&trans1, &pers, log);
  InDetSimDataCollection trans2;
  cnv.persToTrans(&pers, &trans2, log);

  compare(trans1, trans2);
}


// TCnv: InDetSimDataCollectionCnv_pX
// T: InDetSimDataCollection_pX
template<typename TCnv, typename T>
void test1(std::vector<HepMC::GenParticle*>& genPartVector)
{
  std::cout << "test1\n";
  const HepMC::GenParticle *particle = genPartVector.at(0);
  // Create HepMcParticleLink outside of leak check.
  HepMcParticleLink dummyHMPL(particle->barcode(), particle->parent_event()->event_number());
  assert(dummyHMPL.cptr()==particle);
  Athena_test::Leakcheck check;

  InDetSimDataCollection trans1;
  // Assumme genPartVector is filled by Athena_test::initMcEventCollection. Do not check the vector size.
  for (int i=0; i<3; i++) {
    std::vector<InDetSimData::Deposit> deps;
    HepMcParticleLink trkLink1(genPartVector.at(0+(3*i))->barcode(), genPartVector.at(0+(3*i))->parent_event()->event_number());
    deps.emplace_back(trkLink1,  2.5+i);
    HepMcParticleLink trkLink2(genPartVector.at(1+(3*i))->barcode(), genPartVector.at(1+(3*i))->parent_event()->event_number());
    deps.emplace_back(trkLink2, 13.5+i);
    HepMcParticleLink trkLink3(genPartVector.at(2+(3*i))->barcode(), genPartVector.at(2+(3*i))->parent_event()->event_number());
    deps.emplace_back(trkLink3, 23.5+i);

    trans1[Identifier(1234+i)] = InDetSimData(deps, 4321+i);
    trans1[Identifier(2345+i)] = InDetSimData(deps);
  }
  testit<TCnv, T>(trans1);
}


void makePixelID(ISvcLocator* pSvcLoc)
{
  auto pix_id = std::make_unique<PixelID>();
  IdDictParser parser;
  parser.register_external_entity("InnerDetector", "IdDictInnerDetector.xml");
  IdDictMgr& idd = parser.parse("IdDictParser/ATLAS_IDS.xml");
  pix_id->initialize_from_dictionary(idd);
 
  StoreGateSvc* detStore = nullptr;
  assert(pSvcLoc->service("DetectorStore", detStore).isSuccess());
  assert(detStore->record(std::move(pix_id), "PixelID"));
}


// TCnv: InDetSimDataCollectionCnv_pX
// T: InDetSimDataCollection_pX
template<typename TCnv, typename T>
int commonMain()
{
  // Initialize Gaudi
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi("InDetEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run (initGaudi)" << std::endl;
    return 0;
  }

  // Make PixelID for InDetSimDataCollectionCnv_pX (TCnv)
  makePixelID(pSvcLoc);

  std::vector<HepMC::GenParticle*> genPartVector;
  // Fill genPartVector
  // false is to skip Athena_test::initGaudi in Athena_test::initMcEventCollection.
  if (!Athena_test::initMcEventCollection(pSvcLoc, genPartVector, false)) {
    std::cerr << "This test can not be run (initMcEventCollection)" << std::endl;
    return 0;
  }

  test1<TCnv, T>(genPartVector);

  return 0;
}
