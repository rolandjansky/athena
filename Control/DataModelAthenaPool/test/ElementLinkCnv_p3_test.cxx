/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  DataModelAthenaPool/test/ElementLinkCnv_p3_test.cxx
 * @author scott snyder
 * @date Dec, 2009
 * @brief Regression test for @c ElementLinkCnv_p3.
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "AthLinks/ElementLink.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
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


void test1()
{
  std::cout << "test1\n";
  MsgStream log (0, "test");
  ElementLinkCnv_p3<ELI> cnv;
  ELI el1 ("key", 23);
  ElementLinkCnv_p3<ELI>::PersLink_t p1;
  cnv.transToPers (el1, p1, log);
  assert (p1.m_elementIndex == 23);
  assert (p1.m_SGKeyHash == 152280269);
  ELI el2;
  cnv.persToTrans (p1, el2, log);
  assert (el1.index() == el2.index());
  assert (el1.key() == el2.key());
}


int main()
{
  std::cout << "DataModelAthenaPool/ElementLinkCnv_p3_test\n";
  SGTest::initTestStore();
  test1();
  return 0;
}
