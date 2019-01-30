// $Id: gt_asgtools_toolhandle_test.cxx 743773 2016-04-28 21:21:41Z krumnack $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTesting/UnitTest.h"

using namespace testing;

#ifdef ASGTOOL_STANDALONE

struct ToolHandleTest : public Test
{
  ToolHandleTest ()
    : tool1( "dummyTool1" ),
      tool2( "dummyTool2" )
  {}

  xAOD::TEvent event;

  // Create some dummy tools on the stack:
  asg::AsgTool tool1;
  asg::AsgMetadataTool tool2;
};

TEST_F (ToolHandleTest, handle1)
{
   // Construct tool handles in a number of different ways:
  ToolHandle< asg::IAsgTool > handle1( static_cast<asg::IAsgTool*>(nullptr) );

   // And now test if they behave as they should:
   EXPECT_TRUE( handle1.empty() == true );
   EXPECT_TRUE( handle1.typeAndName() == "" );
   EXPECT_TRUE( handle1.type() == "" );
   EXPECT_TRUE( handle1.name() == "" );
}

TEST_F (ToolHandleTest, handle2)
{
   // Construct tool handles in a number of different ways:
   ToolHandle< asg::IAsgTool > handle2( &tool1 );

   // And now test if they behave as they should:
   EXPECT_TRUE( handle2.operator->() == &tool1 );
   EXPECT_TRUE( handle2.name() == tool1.name() );
}

TEST_F (ToolHandleTest, handle3)
{
   // Construct tool handles in a number of different ways:
   ToolHandle< asg::IAsgTool > handle3( "dummyTool2" );

   // And now test if they behave as they should:
   EXPECT_TRUE( handle3.operator->() == &tool2 );
   EXPECT_TRUE( handle3.name() == "dummyTool2" );
}

TEST_F (ToolHandleTest, handle4)
{
   // Construct tool handles in a number of different ways:
   ToolHandle< asg::IAsgTool > handle4( "asg::AsgTool/dummyTool1" );

   // And now test if they behave as they should:
   EXPECT_TRUE( handle4.operator->() == &tool1 );
   EXPECT_TRUE( handle4.type() == "asg::AsgTool" );
   EXPECT_TRUE( handle4.name() == "dummyTool1" );
}

TEST_F (ToolHandleTest, DISABLED_handle5)
{
   // Construct tool handles in a number of different ways:
   ToolHandle< asg::IAsgTool > handle5( "asg::AsgMetadataTool" );

   // And now test if they behave as they should:
   EXPECT_TRUE( handle5.operator->() == 0 );
   EXPECT_TRUE( handle5.type() == "asg::AsgMetadataTool" );
   EXPECT_TRUE( handle5.name() == "asg::AsgMetadataTool" );
}

#endif

ATLAS_GOOGLE_TEST_MAIN
