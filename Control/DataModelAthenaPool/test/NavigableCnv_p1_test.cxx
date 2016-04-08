/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelAthenaPool/test/NavigableCnv_p1_test.cxx
 * @author scott snyder
 * @date Dec, 2009
 * @brief Regression test for @c NavigableCnv_p1.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "Navigation/Navigable.h"
#include "Navigation/NavigableTerminalNode.h"
#include "DataModelAthenaPool/NavigableCnv_p1.h"
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
typedef Navigable<MyVI, int> NIpar;


void test1()
{
  MsgStream log (0, "test");
  NavigableCnv_p1<NI> cnv;
  NI ni1;
  ni1.insertElement (ElementLink<MyVI> ("key", 10));
  ni1.insertElement (ElementLink<MyVI> ("key", 20));
  NavigableCnv_p1<NI>::PersNavigable_t p1;
  cnv.transToPers (ni1, p1, log);
#if 0
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
#endif
  NI ni2;
  cnv.persToTrans (p1, ni2, log);
  assert (ni2.size() == 2);
  NI::object_iter it = ni2.begin();
  assert (it.getElement().index() == 10);
  assert (it.getElement().dataID() == "key");
  ++it;
  assert (it.getElement().index() == 20);
  assert (it.getElement().dataID() == "key");
}


void test2()
{
  MsgStream log (0, "test");
  NavigableCnv_p1<NIpar> cnv;
  NIpar ni1;
  ni1.insertElement (ElementLink<MyVI> ("key", 10), 101);
  ni1.insertElement (ElementLink<MyVI> ("key", 20), 102);
  NavigableCnv_p1<NIpar>::PersNavigable_t p1;
  cnv.transToPers (ni1, p1, log);
#if 0
  assert (p1.m_links.size() == 2);
  assert (p1.m_links[0].m_elementIndex == 10);
  assert (p1.m_links[0].m_SGKeyHash == 152280269);
  assert (p1.m_links[1].m_elementIndex == 20);
  assert (p1.m_links[1].m_SGKeyHash == 152280269);
  assert (p1.m_parameters.size() == 2);
  assert (p1.m_parameters[0] == 101);
  assert (p1.m_parameters[1] == 102);
#endif
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
}


int main()
{
  SGTest::initTestStore();
  test1();
  test2();
  return 0;
}
