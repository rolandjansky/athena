/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file  DataModelAthenaPool/test/ElementLinkVectorCnv_p1_test.cxx
 * @author scott snyder
 * @date Dec, 2009
 * @brief Regression test for @c ElementLinkVectorCnv_p1.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "AthLinks/ElementLinkVector.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "CLIDSvc/CLASS_DEF.h"
#include <vector>
#include <cassert>
#include <iostream>


#include "SGTools/TestStore.h"


class MyVI
  : public std::vector<int>
{
};


CLASS_DEF (MyVI, 104857385, 1)



typedef ElementLinkVector<MyVI> ELVI;
typedef ElementLink<MyVI> ELI;


void test1()
{
  MsgStream log (0, "test");
  ElementLinkVectorCnv_p1<ELVI> cnv;
  ELVI elv1;
  elv1.push_back (ELI ("key1", 10));
  elv1.push_back (ELI ("key2", 20));
  elv1.push_back (ELI ("key1", 30));
  elv1.push_back (ELI ("key3", 40));
  elv1.push_back (ELI ("key2", 50));
  ElementLinkVectorCnv_p1<ELVI>::PersLinkVect_t p1;
  cnv.transToPers (elv1, p1, log);

  assert (p1.m_links.size() == 3);
  assert (p1.m_links[0].m_SGKeyHash == 61740278);
  assert (p1.m_links[1].m_SGKeyHash == 793080788);
  assert (p1.m_links[2].m_SGKeyHash == 887336650);
  assert (p1.m_elementRefs.size() == 5);
  assert (p1.m_elementRefs[0].m_elementIndex == 10);
  assert (p1.m_elementRefs[1].m_elementIndex == 20);
  assert (p1.m_elementRefs[2].m_elementIndex == 30);
  assert (p1.m_elementRefs[3].m_elementIndex == 40);
  assert (p1.m_elementRefs[4].m_elementIndex == 50);
  assert (p1.m_elementRefs[0].m_nameIndex == 0);
  assert (p1.m_elementRefs[1].m_nameIndex == 1);
  assert (p1.m_elementRefs[2].m_nameIndex == 0);
  assert (p1.m_elementRefs[3].m_nameIndex == 2);
  assert (p1.m_elementRefs[4].m_nameIndex == 1);

  ELVI elv2;
  cnv.persToTrans (p1, elv2, log);
  assert (elv1.size() == elv2.size());
  for (size_t i = 0; i < elv1.size(); i++) {
    assert (elv1[i].index() == elv2[i].index());
    assert (elv1[i].key() == elv2[i].key());
  }
}


int main()
{
  SGTest::initTestStore();
  test1();
  return 0;
}
