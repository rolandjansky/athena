// $Id: ut_asgtools_toolstore.cxx 671353 2015-06-01 13:33:51Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/ToolStore.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTesting/UnitTest.h"

namespace asg
{
  TEST (ToolStoreTest, base_tests)
  {
    // Create some dummy tools on the stack:
    AsgTool tool1( "dummyTool1" );
    AsgMetadataTool tool2( "dummyTool2" );

    // Check that they are found by the store now:
    EXPECT_EQ( ToolStore::get( "dummyTool1" ), &tool1 );
    EXPECT_EQ( ToolStore::get( "dummyTool2" ), &tool2 );
    EXPECT_FALSE( ToolStore::contains< AsgMetadataTool >( "dummyTool1" ) );
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "dummyTool1" ) );
    EXPECT_TRUE( ToolStore::contains< AsgMetadataTool >( "dummyTool2" ));
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "dummyTool2" ) );

    // Remove both tools from the store now:
    EXPECT_SUCCESS (ToolStore::remove( "dummyTool1" ));
    EXPECT_SUCCESS (ToolStore::remove( &tool2 ));

    // Check that they are indeed gone from the store:
    EXPECT_EQ( ToolStore::get( "dummyTool1" ), nullptr );
    EXPECT_EQ( ToolStore::get( "dummyTool2" ), nullptr );

    // Create two tools on the heap:
    auto tool3 = std::make_unique<AsgTool>( "ToolSvc.dummyTool3" );
    auto tool4 = std::make_unique<AsgMetadataTool>( "ToolSvc.dummyTool4" );

    // Check that these are also in the store:
    EXPECT_EQ ( ToolStore::get( "dummyTool3" ), tool3.get() );
    EXPECT_EQ ( ToolStore::get( "dummyTool4" ), tool4.get() );
    EXPECT_EQ ( ToolStore::get( "ToolSvc.dummyTool3" ), tool3.get() );
    EXPECT_EQ ( ToolStore::get( "ToolSvc.dummyTool4" ), tool4.get() );
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "dummyTool3" ) );
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "dummyTool4" ) );
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "ToolSvc.dummyTool3" ) );
    EXPECT_TRUE( ToolStore::contains< AsgTool >( "ToolSvc.dummyTool4" ) );

    // Delete the two tools:
    tool3.reset ();
    tool4.reset ();

    // Check that the tools are no longer in the store:
    EXPECT_EQ( ToolStore::get( "dummyTool3" ), nullptr );
    EXPECT_EQ( ToolStore::get( "dummyTool4" ), nullptr );
  }
}

ATLAS_GOOGLE_TEST_MAIN
