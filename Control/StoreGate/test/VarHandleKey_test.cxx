/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file StoreGate/test/VarHandleKey_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for VarHandleKey.
 */


#undef NDEBUG
#include "StoreGate/VarHandleKey.h"
#include "StoreGate/exceptions.h"
#include "AthenaKernel/errorcheck.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include <cassert>
#include <iostream>


namespace SG {
class VarHandleBase
{
public:
  void setOwner_test (VarHandleKey& key) { key.setOwningHandle (this); }
};
}


void test1()
{
  std::cout << "test1\n";

  SG::VarHandleKey k1 (1234, "aaa", Gaudi::DataHandle::Reader);
  assert (k1.clid() == 1234);
  assert (k1.key() == "aaa");
  assert (k1.mode() == Gaudi::DataHandle::Reader);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (!k1.storeHandle().isSet());
  assert (k1.initialize().isSuccess());
  assert (k1.storeHandle().isSet());
  assert (k1.start().isSuccess());
  assert (!k1.isCondition());
  assert (!k1.empty());

  k1 = "aab";
  assert (k1.clid() == 1234);
  assert (k1.key() == "aab");
  assert (k1.mode() == Gaudi::DataHandle::Reader);
  assert (k1.storeHandle().name() == "StoreGateSvc");
  assert (k1.storeHandle().isSet());

  assert (k1.assign ("FeeSvc+aac").isSuccess());
  assert (k1.clid() == 1234);
  assert (k1.key() == "aac");
  assert (k1.mode() == Gaudi::DataHandle::Reader);
  assert (k1.storeHandle().name() == "FeeSvc");
  assert (!k1.storeHandle().isSet());

  assert (k1.assign ("ConditionStore+Feedir/aac").isSuccess());
  assert (k1.clid() == 1234);
  assert (k1.key() == "Feedir/aac");
  assert (k1.mode() == Gaudi::DataHandle::Reader);
  assert (k1.storeHandle().name() == "ConditionStore");
  assert (!k1.storeHandle().isSet());

  assert (k1.assign ("FeeSvc+foo/aac").isFailure());
  EXPECT_EXCEPTION (SG::ExcBadHandleKey,
                    k1 = "FeeSvc+foo/aac");
  
  SG::VarHandleKey k2 (1235, "bbb", Gaudi::DataHandle::Writer, "FooSvc", true);
  assert (k2.clid() == 1235);
  assert (k2.key() == "bbb");
  assert (k2.mode() == Gaudi::DataHandle::Writer);
  assert (k2.storeHandle().name() == "FooSvc");
  assert (!k2.storeHandle().isSet());
  assert (k2.initialize().isFailure());
  assert (!k2.storeHandle().isSet());
  assert (k2.isCondition());

  SG::VarHandleKey k3 (1236, "BarSvc+ccc", Gaudi::DataHandle::Updater, "FooSvc");
  assert (k3.clid() == 1236);
  assert (k3.key() == "ccc");
  assert (k3.mode() == Gaudi::DataHandle::Updater);
  assert (k3.storeHandle().name() == "BarSvc");
  assert (!k3.storeHandle().isSet());

  SG::VarHandleKey k4 (1237, "", Gaudi::DataHandle::Updater);
  assert (k4.clid() == 1237);
  assert (k4.key() == "");
  assert (k4.mode() == Gaudi::DataHandle::Updater);
  assert (k4.storeHandle().name() == "StoreGateSvc");
  assert (!k4.storeHandle().isSet());
  assert (k4.initialize().isFailure());
  assert (k4.initialize(false).isSuccess());
  assert (k4.initialize(SG::AllowEmpty).isSuccess());
  assert (k4.empty());

  EXPECT_EXCEPTION (SG::ExcBadHandleKey,
                    SG::VarHandleKey (1237, "a/b/c", Gaudi::DataHandle::Updater));

  SG::VarHandleKey k5 (1236, "BarSvc+ccc", Gaudi::DataHandle::Updater, "FooSvc");
  assert (k5.key() == "ccc");
  assert (k5.initialize(false).isSuccess());
  assert (k5.key() == "");

  assert (k5.owningHandle() == nullptr);
  SG::VarHandleBase base;
  base.setOwner_test (k5);
  assert (k5.owningHandle() == &base);
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* pDum;
  Athena_test::initGaudi(pDum); //need MessageSvc

  test1();
}
