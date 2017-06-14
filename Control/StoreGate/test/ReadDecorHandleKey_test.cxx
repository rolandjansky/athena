/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */

// $Id$
/**
 * @file StoreGate/test/ReadDecorHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2017
 * @brief Tests for ReadDecorHandleKey.
 */


#undef NDEBUG
#include "StoreGate/ReadDecorHandleKey.h"
#include "TestTools/initGaudi.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (MyObj, 293847295, 1)


void test1()
{
  std::cout << "test1\n";

  SG::ReadDecorHandleKey<MyObj> k1 ("aaa.dec");
  assert (k1.clid() == 293847295);
  assert (k1.key() == "aaa.dec");
  assert (k1.mode() == Gaudi::DataHandle::Reader);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (!k1.storeHandle().isSet());
  assert (k1.initialize().isSuccess());
  assert (k1.storeHandle().isSet());

  k1 = "bbb.foo";
  assert (k1.key() == "bbb.foo");

  k1.assign ("ccc.fee");
  assert (k1.key() == "ccc.fee");
}


int main()
{
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test1();
  return 0;
}
