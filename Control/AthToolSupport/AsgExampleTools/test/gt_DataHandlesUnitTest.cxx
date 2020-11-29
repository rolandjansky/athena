/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <AsgDataHandles/ReadDecorHandle.h>
#include <AsgDataHandles/ReadDecorHandleKey.h>
#include <AsgDataHandles/ReadHandle.h>
#include <AsgDataHandles/ReadHandleKey.h>
#include <AsgDataHandles/WriteDecorHandle.h>
#include <AsgDataHandles/WriteDecorHandleKey.h>
#include <AsgDataHandles/WriteHandle.h>
#include <AsgDataHandles/WriteHandleKey.h>
#include <AsgTesting/UnitTest.h>
#include <AsgTools/CurrentContext.h>
#include <xAODCore/CLASS_DEF.h>
#include <xAODRootAccess/TEvent.h>
#include <gtest/gtest.h>


class MyObj
{
public:
  MyObj(int x=0) : x(x) {}
  int x;
};
CLASS_DEF (MyObj, 293847295, 1)


namespace asg
{
  struct DataHandlesUnitTest : public ::testing::Test
  {
    virtual void SetUp () override
    {
    }

    xAOD::TEvent event;
  };

  // ReadHandle
  TEST_F (DataHandlesUnitTest, ReadHandle)
  {
    SG::ReadHandleKey<MyObj> k1 ("aaa");
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aaa");
    ASSERT_SUCCESS (k1.initialize());

    k1 = "aab";
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aab");

    SG::ReadHandleKey<MyObj> k2 ("asd");
    ASSERT_SUCCESS (k2.initialize());
    SG::ReadHandle<MyObj> h2 (k2);
    // ASSERT_EQ (h2.clid(), 293847295);
    ASSERT_EQ (h2.key(), "asd");

    const EventContext &ctx3 = Gaudi::Hive::currentContext();
    SG::ReadHandle<MyObj> h3 (k2, ctx3);
    // ASSERT_EQ (h3.clid(), 293847295);
    ASSERT_EQ (h3.key(), "asd");
  }

  // WriteHandle
  TEST_F (DataHandlesUnitTest, WriteHandle)
  {
    SG::WriteHandleKey<MyObj> k1 ("aaa");
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aaa");
    ASSERT_SUCCESS (k1.initialize());

    k1 = "aab";
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aab");

    SG::WriteHandleKey<MyObj> k2 ("asd");
    ASSERT_SUCCESS (k2.initialize());
    SG::WriteHandle<MyObj> h2 (k2);
    // ASSERT_EQ (h2.clid(), 293847295);
    ASSERT_EQ (h2.key(), "asd");

    const EventContext &ctx3 = Gaudi::Hive::currentContext();
    SG::WriteHandle<MyObj> h3 (k2, ctx3);
    // ASSERT_EQ (h3.clid(), 293847295);
    ASSERT_EQ (h3.key(), "asd");
  }


  // ReadDecorHandle
  TEST_F (DataHandlesUnitTest, ReadDecorHandle)
  {
    SG::ReadDecorHandleKey<MyObj> k1 ("aaa.dec");
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aaa.dec");
    ASSERT_SUCCESS (k1.initialize());

    // ASSERT_EQ (k1.contHandleKey().clid(), 293847295);
    ASSERT_EQ (k1.contHandleKey().key(), "aaa");

    k1 = "bbb.foo";
    ASSERT_EQ (k1.key(), "bbb.foo");
    ASSERT_EQ (k1.contHandleKey().key(), "bbb");

    ASSERT_SUCCESS (k1.assign ("ccc.fee"));
    ASSERT_EQ (k1.key(), "ccc.fee");
    ASSERT_EQ (k1.contHandleKey().key(), "ccc");

    SG::ReadDecorHandleKey<MyObj> k2 ("asd.aaa");
    ASSERT_SUCCESS (k2.initialize());

    SG::ReadDecorHandle<MyObj, int> h1 (k2);
    // ASSERT_EQ (h1.clid(), 293847295);
    ASSERT_EQ (h1.key(), "asd");
    ASSERT_EQ (h1.decorKey(), "asd.aaa");
    ASSERT_FALSE (h1.isPresent());

    const EventContext &ctx2 = Gaudi::Hive::currentContext();
    SG::ReadDecorHandle<MyObj, int> h2 (k2, ctx2);
    // ASSERT_EQ (h2.clid(), 293847295);
    ASSERT_EQ (h2.key(), "asd");
    ASSERT_EQ (h2.decorKey(), "asd.aaa");
  }

  // WriteDecorHandle
  TEST_F (DataHandlesUnitTest, WriteDecorHandle)
  {
    SG::WriteDecorHandleKey<MyObj> k1 ("aaa.dec");
    // ASSERT_EQ (k1.clid(), 293847295);
    ASSERT_EQ (k1.key(), "aaa.dec");
    ASSERT_SUCCESS (k1.initialize());

    // ASSERT_EQ (k1.contHandleKey().clid(), 293847295);
    ASSERT_EQ (k1.contHandleKey().key(), "aaa");

    k1 = "bbb.foo";
    ASSERT_EQ (k1.key(), "bbb.foo");
    ASSERT_EQ (k1.contHandleKey().key(), "bbb");

    ASSERT_SUCCESS (k1.assign ("ccc.fee"));
    ASSERT_EQ (k1.key(), "ccc.fee");
    ASSERT_EQ (k1.contHandleKey().key(), "ccc");

    SG::WriteDecorHandleKey<MyObj> k2 ("asd.aaa");
    ASSERT_SUCCESS (k2.initialize());

    SG::WriteDecorHandle<MyObj, int> h1 (k2);
    // ASSERT_EQ (h1.clid(), 293847295);
    ASSERT_EQ (h1.key(), "asd");
    ASSERT_EQ (h1.decorKey(), "asd.aaa");
    ASSERT_FALSE (h1.isPresent());

    const EventContext &ctx2 = Gaudi::Hive::currentContext();
    SG::WriteDecorHandle<MyObj, int> h2 (k2, ctx2);
    // ASSERT_EQ (h2.clid(), 293847295);
    ASSERT_EQ (h2.key(), "asd");
    ASSERT_EQ (h2.decorKey(), "asd.aaa");
  }

}

ATLAS_GOOGLE_TEST_MAIN
