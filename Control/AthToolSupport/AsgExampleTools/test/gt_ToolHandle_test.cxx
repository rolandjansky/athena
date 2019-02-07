/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/ToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
#include <AsgExampleTools/UnitTestTool1.h>
#include <AsgExampleTools/UnitTestTool2.h>
#include <cmath>
#include <gtest/gtest.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

//
// method implementations
//

using namespace asg::msgUserCode;

namespace asg
{
  TEST (ToolHandleTest, empty)
  {
    ToolHandle<IUnitTestTool1> handle("");
    ASSERT_TRUE (handle.empty());
  }

  TEST (ToolHandleTest, nonempty_name)
  {
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_FALSE (handle.empty());
  }

  TEST (ToolHandleTest, nonempty_tool)
  {
#ifndef ASGTOOL_ATHENA
    // This never worked properly in Athena --- in that case the ToolHandle
    // ctor matches the signature:
    // ToolHandle( const IInterface* parent = nullptr, bool createIf = true )
    // Previously, the ToolHandle would get initialized with the
    // default type name; that's why empty() (which tests the type+name field,
    // not the pointer) was returning false.   But with gaudi v30, the
    // ToolHandle gets initialized with an empty type+name, so empty()
    // returns true.
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle (tool.get());
    ASSERT_FALSE (handle.empty());
#endif
  }



  TEST (ToolHandleTest, retrieve_empty)
  {
    ToolHandle<IUnitTestTool1> handle;
    ASSERT_FAILURE (handle.retrieve());
  }

  TEST (ToolHandleTest, retrieve_name_miss)
  {
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_FAILURE (handle.retrieve());
  }

#ifdef ROOTCORE
  TEST (ToolHandleTest, retrieve_name_hit)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_SUCCESS (handle.retrieve());
  }

  TEST (ToolHandleTest, retrieve_name_type_missmatch)
  {
    std::unique_ptr<IUnitTestTool2> tool (new UnitTestTool2 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_FAILURE (handle.retrieve());
  }

  TEST (ToolHandleTest, retrieve_tool)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle (tool.get());
    ASSERT_SUCCESS (handle.retrieve());
  }
#endif



  TEST (ToolHandleTest, dereference_empty)
  {
    ToolHandle<IUnitTestTool1> handle;
    ASSERT_THROW_REGEX (*handle, "(Couldn't find tool with name)|(Failed to retrieve)");
  }

  TEST (ToolHandleTest, dereference_name_miss)
  {
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_THROW_REGEX (*handle, "(Couldn't find tool with name)|(Failed to retrieve)");
  }

#ifdef ROOTCORE
  TEST (ToolHandleTest, dereference_name_hit)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_EQ (&*handle, tool.get());
  }


  TEST (ToolHandleTest, dereference_name_type_missmatch)
  {
    std::unique_ptr<IUnitTestTool2> tool (new UnitTestTool2 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_THROW_REGEX (*handle, "Couldn't find tool with name");
  }

  TEST (ToolHandleTest, dereference_tool)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle (tool.get());
    ASSERT_EQ (&*handle, tool.get());
  }
#endif


  TEST (ToolHandleTest, arrow_empty)
  {
    ToolHandle<IUnitTestTool1> handle;
    ASSERT_THROW_REGEX (handle.operator->(), "(Couldn't find tool with name)|(Failed to retrieve)");
  }

  TEST (ToolHandleTest, arrow_name_miss)
  {
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_THROW_REGEX (handle.operator->(), "(Couldn't find tool with name)|(Failed to retrieve)");
  }

#ifdef ROOTCORE
  TEST (ToolHandleTest, arrow_name_hit)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_EQ (handle.operator->(), tool.get());
  }

  TEST (ToolHandleTest, arrow_name_type_missmatch)
  {
    std::unique_ptr<IUnitTestTool2> tool (new UnitTestTool2 ("test"));
    ToolHandle<IUnitTestTool1> handle ("test");
    ASSERT_THROW_REGEX (handle.operator->(), "(Couldn't find tool with name)|(Failed to retrieve)");
  }

  TEST (ToolHandleTest, arrow_tool)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle (tool.get());
    ASSERT_EQ (handle.operator->(), tool.get());
  }
#endif


#ifdef ROOTCORE
  TEST (ToolHandleTest, assign)
  {
    std::unique_ptr<IUnitTestTool1> tool (new UnitTestTool1 ("test"));
    ToolHandle<IUnitTestTool1> handle;
    handle = tool.get();
    ASSERT_EQ (&*handle, tool.get());
  }
#endif
}

int main (int argc, char **argv)
{
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS();
}
