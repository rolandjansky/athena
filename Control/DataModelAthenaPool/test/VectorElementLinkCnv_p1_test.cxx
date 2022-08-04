/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  DataModelAthenaPool/test/VectorElementLinkCnv_p1_test.cxx
 * @author scott snyder
 * @date Sep, 2019
 * @brief Regression test for @c VectorElementLinkCnv_p1.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <cassert>
#include <iostream>


#include "SGTools/TestStore.h"


class MyVI
  : public std::vector<int>
{
};


CLASS_DEF (MyVI, 104857385, 1)



typedef ElementLink<MyVI> ELI;
typedef std::vector<ELI> ELVI;


void test1 (SGTest::TestStore& store)
{
  std::cout << "test1\n";
  MsgStream log (0, "test");
  VectorElementLinkCnv_p1<ELVI> cnv;
  ELVI elv1;
  elv1.push_back (ELI ("key1", 10));
  elv1.push_back (ELI ("key2", 20));
  elv1.push_back (ELI ("key1", 30));
  elv1.push_back (ELI ("key3", 40));
  elv1.push_back (ELI ("key2", 50));
  VectorElementLinkCnv_p1<ELVI>::Pers_t p1;
  cnv.transToPers (elv1, p1, log);

  SG::sgkey_t sgkey1 = store.stringToKey ("key1", ClassID_traits<MyVI>::ID());
  SG::sgkey_t sgkey2 = store.stringToKey ("key2", ClassID_traits<MyVI>::ID());
  SG::sgkey_t sgkey3 = store.stringToKey ("key3", ClassID_traits<MyVI>::ID());

  assert (p1.size() == 5);
  assert (p1[0].m_elementIndex == 10);
  assert (p1[1].m_elementIndex == 20);
  assert (p1[2].m_elementIndex == 30);
  assert (p1[3].m_elementIndex == 40);
  assert (p1[4].m_elementIndex == 50);
  assert (p1[0].m_SGKeyHash == sgkey1);
  assert (p1[1].m_SGKeyHash == sgkey2);
  assert (p1[2].m_SGKeyHash == sgkey1);
  assert (p1[3].m_SGKeyHash == sgkey3);
  assert (p1[4].m_SGKeyHash == sgkey2);

  ELVI elv2;
  cnv.persToTrans (p1, elv2, log);
  assert (elv1.size() == elv2.size());
  for (size_t i = 0; i < elv1.size(); i++) {
    assert (elv1[i].index() == elv2[i].index());
    assert (elv1[i].key() == elv2[i].key());
  }

  SG::ThinningDecisionBase dec;
  dec.resize (60);
  dec.keepAll();
  dec.thin (10);
  dec.buildIndexMap();

  SG::ThinningCache cache;
  cache.addThinning ("key1",
                     std::vector<SG::sgkey_t> {elv1[0].key()},
                     &dec);

  cnv.transToPers (elv1, p1, &cache, log);
  assert (p1.size() == 5);
  assert (p1[0].m_elementIndex == 0);
  assert (p1[1].m_elementIndex == 20);
  assert (p1[2].m_elementIndex == 29);
  assert (p1[3].m_elementIndex == 40);
  assert (p1[4].m_elementIndex == 50);
  assert (p1[0].m_SGKeyHash == 0);
  assert (p1[1].m_SGKeyHash == sgkey2);
  assert (p1[2].m_SGKeyHash == sgkey1);
  assert (p1[3].m_SGKeyHash == sgkey3);
  assert (p1[4].m_SGKeyHash == sgkey2);
}


int main()
{
  std::cout << "DataModelAthenaPool/VectorElementLinkCnv_p1_test\n";
  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1 (*testStore);
  return 0;
}
