/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEventAthenaPool/test/TRT_LowLumRawDataContainerCnv_common_test.h
 * @brief Definitions of common methods for regression tests of TRT_LoLumRawDataContainerCnv_pX
 * Based on InDetEventTPCnv/test/InDetSimDataCollectionCnv_common_test.h
 */

#undef NDEBUG

#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"

#include "GaudiKernel/MsgStream.h"

#include <cassert>
#include <iostream>


void compare(const TRT_LoLumRawData& p1,
             const TRT_LoLumRawData& p2)
{
  assert(p1.identify() == p2.identify());
  assert(p1.getWord() == p2.getWord());
  assert(p1.highLevel() == p2.highLevel());
  assert(p1.timeOverThreshold() == p2.timeOverThreshold());
  assert(p1.driftTimeBin() == p2.driftTimeBin());
}


void compare(const TRT_RDO_Collection& p1,
             const TRT_RDO_Collection& p2)
{
  assert(p1.size() == p2.size());
  assert(p1.identify() == p2.identify());
  assert(p1.identifyHash() == p2.identifyHash());
  for (unsigned int i = 0; i < p1.size(); ++i) {
    const TRT_LoLumRawData* chan1 = dynamic_cast<const TRT_LoLumRawData*>(p1[i]);
    const TRT_LoLumRawData* chan2 = dynamic_cast<const TRT_LoLumRawData*>(p2[i]);
    compare(*chan1, *chan2);
  }
}


void compare(const TRT_RDO_Container& p1,
             const TRT_RDO_Container& p2)
{
  assert(p1.numberOfCollections() == p2.numberOfCollections());
  TRT_RDO_Container::const_iterator it1 = p1.begin();
  TRT_RDO_Container::const_iterator it2 = p2.begin();
  for (; it1 != p1.end(); ++it1, ++it2) {
    compare(**it1, **it2);
  }
}


// TCnv: TRT_LoLumRawDataContainerCnv_pX
// T: InDetRawDataContainer_pX
template<typename TCnv, typename T>
void testit(const TRT_RDO_Container& trans1)
{
  MsgStream log(0, "test");
  TCnv cnv;
  T pers;
  cnv.transToPers(&trans1, &pers, log);
  TRT_RDO_Container trans2(trans1.numberOfCollections());
  cnv.persToTrans(&pers, &trans2, log);

  compare(trans1, trans2);
}


// TCnv: TRT_LoLumRawDataContainerCnv_pX
// T: InDetRawDataContainer_pX
template<typename TCnv, typename T>
void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  // Build a TRT_RDO_Container
  const unsigned int containerSize(19008);
  TRT_RDO_Container trans1(containerSize);
  // Creating collection for first example module
  const IdentifierHash elementHash1(10026);
  const Identifier::value_type collIdValue1 = 0x1612280000000000;
  const Identifier collID1 = Identifier(collIdValue1);
  std::unique_ptr<TRT_RDO_Collection> collection1 = std::make_unique<TRT_RDO_Collection>(elementHash1);
  collection1->setIdentifier(collID1);
  //Add a TRT_LoLumRawData object
  const Identifier::value_type idValue1 = 0x1612282000000000;
  const Identifier strawID1 = Identifier(idValue1);
  const unsigned int strawWord1(2147483696);
  std::unique_ptr<TRT_LoLumRawData> rdo1 = std::make_unique<TRT_LoLumRawData>(strawID1,strawWord1);
  collection1->push_back(rdo1.release());
  assert(trans1.addCollection(collection1.get(),elementHash1).isSuccess());
  collection1.release(); // Now owned by trans1
  // Creating collection for second example module
  const IdentifierHash elementHash2(10027);
  const Identifier::value_type collIdValue2 = 0x16122c0000000000;
  const Identifier collID2 = Identifier(collIdValue2);
  std::unique_ptr<TRT_RDO_Collection> collection2 = std::make_unique<TRT_RDO_Collection>(elementHash2);
  collection2->setIdentifier(collID2);
  //Add a TRT_LoLumRawData object
  const Identifier::value_type idValue2 = 0x16122ce000000000;
  const Identifier strawID2 = Identifier(idValue2);
  const unsigned int strawWord2(2147499712);
  std::unique_ptr<TRT_LoLumRawData> rdo2 = std::make_unique<TRT_LoLumRawData>(strawID2,strawWord2);
  collection2->push_back(rdo2.release());
  assert(trans1.addCollection(collection2.get(),elementHash2).isSuccess());
  collection2.release(); // Now owned by trans1
  // Could add further modules/straws in the future, but I don't want to make this method unreadable.
  testit<TCnv, T>(trans1);
}


// TCnv: TRT_LoLumRawDataContainerCnv_pX
// T: InDetRawDataContainer_pX
template<typename TCnv, typename T>
int commonMain()
{
  // Initialize Gaudi
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi("InDetEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run (initGaudi)" << std::endl;
    return 0;
  }

  test1<TCnv, T>();

  return 0;
}
