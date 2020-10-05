/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  DataModelAthenaPool/test/DataLink_p2_test.cxx
 * @author scott snyder, Marcin Nowa
 * @date Jan, 2010
 * @brief Regression test for @c DataLink_p2
 */

// FIXME: This is sadly incomplete.


#undef NDEBUG
#include "AthLinks/DataLink.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"
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



typedef DataLink<MyVI> DLI;


void test1()
{
  MsgStream log (0, "test");
  DataLinkCnv_p2<DLI> cnv;
  DLI dl1 ("key");
  DataLinkCnv_p2<DLI>::PersDLink_t pl;
  cnv.transToPers (dl1, pl, log);
  assert (pl.m_SGKeyHash == 152280269);
  DLI dl2;
  cnv.persToTrans (pl, dl2, log);
  assert (dl1.key() == dl2.key());
}


int main()
{
  std::unique_ptr<SGTest::TestStore> testStore = SGTest::getTestStore();
  test1();
  return 0;
}


