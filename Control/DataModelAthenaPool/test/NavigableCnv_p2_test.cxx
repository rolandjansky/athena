/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  DataModelAthenaPool/test/NavigableCnv_p2_test.cxx
 * @author scott snyder
 * @date Dec, 2009
 * @brief Regression test for @c NavigableCnv_p2.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "Navigation/Navigable.h"
#include "Navigation/NavigableTerminalNode.h"
#include "DataModelAthenaPool/NavigableCnv_p2.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/ThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <cassert>
#include <iostream>


#include "SGTools/TestStore.h"


class MyVI
  : public DataVector<NavigableTerminalNode>
{
};


CLASS_DEF (MyVI, 104857385, 1)



typedef Navigable<MyVI> NI;
typedef Navigable<MyVI, int> NIpar;


void test1()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");
  NavigableCnv_p2<NI> cnv;
  NI ni1;
  ni1.insertElement (ElementLink<MyVI> ("key", 10));
  ni1.insertElement (ElementLink<MyVI> ("key", 20));
  NavigableCnv_p2<NI>::PersNavigable_t p1;
  cnv.transToPers (ni1, p1, log);
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
  NI ni2;
  cnv.persToTrans (p1, ni2, log);
  assert (ni2.size() == 2);
  NI::object_iter it = ni2.begin();
  assert (it.getElement().index() == 10);
  assert (it.getElement().dataID() == "key");
  ++it;
  assert (it.getElement().index() == 20);
  assert (it.getElement().dataID() == "key");

  SG::ThinningDecisionBase dec;
  dec.resize (50);
  dec.keepAll();
  dec.thin (10);
  dec.buildIndexMap();

  SG::ThinningCache cache;
  cache.addThinning ("key",
                     std::vector<SG::sgkey_t> {ElementLink<MyVI> ("key", 10).key()},
                     &dec);
  cnv.transToPers (ni1, p1, &cache, log);
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 0);
  assert (p1.m_links[0].m_SGKeyHash == 0);
  assert (p1.m_links[1].m_elementIndex == 19);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
}


void test2()
{
  std::cout << "test2\n";
  MsgStream log (0, "test");
  NavigableCnv_p2<NIpar> cnv;
  NIpar ni1;
  ni1.insertElement (ElementLink<MyVI> ("key", 10), 101);
  ni1.insertElement (ElementLink<MyVI> ("key", 20), 102);
  NavigableCnv_p2<NIpar>::PersNavigable_t p1;
  cnv.transToPers (ni1, p1, log);
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
  assert (p1.m_parameters.size() == 2);
  assert (p1.m_parameters[0] == 101);
  assert (p1.m_parameters[1] == 102);
  NIpar ni2;
  cnv.persToTrans (p1, ni2, log);
  assert (ni2.size() == 2);
  NIpar::object_iter it = ni2.begin();
  assert (it.getElement().index() == 10);
  assert (it.getElement().dataID() == "key");
  assert (it.getParameter() == 101);
  ++it;
  assert (it.getElement().index() == 20);
  assert (it.getElement().dataID() == "key");
  assert (it.getParameter() == 102);

  SG::ThinningDecisionBase dec;
  dec.resize (50);
  dec.keepAll();
  dec.thin (10);
  dec.buildIndexMap();

  SG::ThinningCache cache;
  cache.addThinning ("key",
                     std::vector<SG::sgkey_t> {ElementLink<MyVI> ("key", 10).key()},
                     &dec);
  cnv.transToPers (ni1, p1, &cache, log);
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 0);
  assert (p1.m_links[0].m_SGKeyHash == 0);
  assert (p1.m_links[1].m_elementIndex == 19);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
  assert (p1.m_parameters.size() == 2);
  assert (p1.m_parameters[0] == 101);
  assert (p1.m_parameters[1] == 102);
}


int main()
{
  std::cout << "DataModelAthenaPool/NavigableCnv_p2_test\n";
  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1();
  test2();
  return 0;
}
