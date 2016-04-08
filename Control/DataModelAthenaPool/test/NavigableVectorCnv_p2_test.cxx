/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelAthenaPool/test/NavigableVectorCnv_p2_test.cxx
 * @author scott snyder
 * @date Dec, 2009
 * @brief Regression test for @c NavigableVectorCnv_p2.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "Navigation/Navigable.h"
#include "Navigation/NavigableTerminalNode.h"
#include "DataModelAthenaPool/NavigableVectorCnv_p2.h"
#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
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
typedef std::vector<NI> VNI;
typedef Navigable<MyVI, int> NIpar;
typedef std::vector<NIpar> VNIpar;


void test1()
{
  MsgStream log (0, "test");
  NavigableVectorCnv_p2<VNI> cnv;
  VNI vni1;
  vni1.resize(2);
  vni1[0].insertElement (ElementLink<MyVI> ("key",  10));
  vni1[0].insertElement (ElementLink<MyVI> ("key2", 20));
  vni1[1].insertElement (ElementLink<MyVI> ("key",  11));
  vni1[1].insertElement (ElementLink<MyVI> ("key2", 21));
  vni1[1].insertElement (ElementLink<MyVI> ("key3", 31));
  NavigableVectorCnv_p2<VNI>::PersNavigableVector_t p1;
  cnv.transToPers (vni1, p1, log);

  assert (p1.size() == 2);
  assert (p1.m_navigableSizes[0] == 2);
  assert (p1.m_navigableSizes[1] == 3);
  assert (p1.m_links.size() == 5);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 793080788);
  assert (p1.m_links[2].m_elementIndex == 11);
  assert (p1.m_links[2].m_SGKeyHash == 152280269);
  assert (p1.m_links[3].m_elementIndex == 21);
  assert (p1.m_links[3].m_SGKeyHash == 793080788);
  assert (p1.m_links[4].m_elementIndex == 31);
  assert (p1.m_links[4].m_SGKeyHash == 887336650);

  VNI vni2;
  cnv.persToTrans (p1, vni2, log);
  assert (vni2.size() == 2);

  assert (vni2[0].size() == 2);
  NI::object_iter it = vni2[0].begin();
  assert (it.getElement().index() == 10);
  assert (it.getElement().dataID() == "key");
  ++it;
  assert (it.getElement().index() == 20);
  assert (it.getElement().dataID() == "key2");

  assert (vni2[1].size() == 3);
  it = vni2[1].begin();
  assert (it.getElement().index() == 11);
  assert (it.getElement().dataID() == "key");
  ++it;
  assert (it.getElement().index() == 21);
  assert (it.getElement().dataID() == "key2");
  ++it;
  assert (it.getElement().index() == 31);
  assert (it.getElement().dataID() == "key3");
}


void test2()
{
  MsgStream log (0, "test");
  NavigableVectorCnv_p2<VNIpar> cnv;
  VNIpar vni1;
  vni1.resize(2);
  vni1[0].insertElement (ElementLink<MyVI> ("key",  10), 101);
  vni1[0].insertElement (ElementLink<MyVI> ("key2", 20), 102);
  vni1[1].insertElement (ElementLink<MyVI> ("key",  11), 103);
  vni1[1].insertElement (ElementLink<MyVI> ("key2", 21), 104);
  vni1[1].insertElement (ElementLink<MyVI> ("key3", 31), 105);
  NavigableVectorCnv_p2<VNIpar>::PersNavigableVector_t p1;
  cnv.transToPers (vni1, p1, log);

  assert (p1.size() == 2);
  assert (p1.m_navigableSizes[0] == 2);
  assert (p1.m_navigableSizes[1] == 3);
  assert (p1.m_links.size() == 5);
  assert (p1.m_parameters.size() == 5);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 793080788);
  assert (p1.m_links[2].m_elementIndex == 11);
  assert (p1.m_links[2].m_SGKeyHash == 152280269);
  assert (p1.m_links[3].m_elementIndex == 21);
  assert (p1.m_links[3].m_SGKeyHash == 793080788);
  assert (p1.m_links[4].m_elementIndex == 31);
  assert (p1.m_links[4].m_SGKeyHash == 887336650);
  assert (p1.m_parameters[0] == 101);
  assert (p1.m_parameters[1] == 102);
  assert (p1.m_parameters[2] == 103);
  assert (p1.m_parameters[3] == 104);
  assert (p1.m_parameters[4] == 105);

  VNIpar vni2;
  cnv.persToTrans (p1, vni2, log);
  assert (vni2.size() == 2);

  assert (vni2[0].size() == 2);
  NIpar::object_iter it = vni2[0].begin();
  assert (it.getElement().index() == 10);
  assert (it.getElement().dataID() == "key");
  assert (it.getParameter() == 101);
  ++it;
  assert (it.getElement().index() == 20);
  assert (it.getElement().dataID() == "key2");
  assert (it.getParameter() == 102);

  assert (vni2[1].size() == 3);
  it = vni2[1].begin();
  assert (it.getElement().index() == 11);
  assert (it.getElement().dataID() == "key");
  assert (it.getParameter() == 103);
  ++it;
  assert (it.getElement().index() == 21);
  assert (it.getElement().dataID() == "key2");
  assert (it.getParameter() == 104);
  ++it;
  assert (it.getElement().index() == 31);
  assert (it.getElement().dataID() == "key3");
  assert (it.getParameter() == 105);
}


int main()
{
  SGTest::initTestStore();
  test1();
  test2();
  return 0;
}
