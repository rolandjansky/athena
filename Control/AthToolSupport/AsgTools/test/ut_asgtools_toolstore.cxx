// $Id: ut_asgtools_toolstore.cxx 671353 2015-06-01 13:33:51Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/ToolStore.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"

/// Helper macro to test some statements
#define SIMPLE_ASSERT( EXP )                                         \
   do {                                                              \
      const bool result = EXP;                                       \
      if( ! result ) {                                               \
         std::cerr << "Failed to evaluate: " << #EXP << std::endl;   \
            return 1;                                                \
      }                                                              \
   } while( 0 )

int main() {

   // Create some dummy tools on the stack:
   asg::AsgTool tool1( "dummyTool1" );
   asg::AsgMetadataTool tool2( "dummyTool2" );

   // Check that they are found by the store now:
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool1" ) != 0 );
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool2" ) != 0 );
   SIMPLE_ASSERT( asg::ToolStore::contains< asg::AsgMetadataTool >( "dummyTool1" ) ==
                  false );
   SIMPLE_ASSERT( asg::ToolStore::contains< asg::AsgTool >( "dummyTool1" ) ==
                  true );
   SIMPLE_ASSERT( asg::ToolStore::contains< asg::AsgMetadataTool >( "dummyTool2" ) ==
                  true );
   SIMPLE_ASSERT( asg::ToolStore::contains< asg::AsgTool >( "dummyTool2" ) ==
                  true );

   // Remove both tools from the store now:
   asg::ToolStore::remove( "dummyTool1" ).ignore();
   asg::ToolStore::remove( &tool2 ).ignore();

   // Check that they are indeed gone from the store:
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool1" ) == 0 );
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool2" ) == 0 );

   // Create two tools on the heap:
   asg::AsgTool* tool3 = new asg::AsgTool( "dummyTool3" );
   asg::AsgMetadataTool* tool4 = new asg::AsgMetadataTool( "dummyTool4" );

   // Check that these are also in the store:
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool3" ) != 0 );
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool4" ) != 0 );

   // Delete the two tools:
   delete tool3; delete tool4;

   // Check that the tools are no longer in the store:
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool3" ) == 0 );
   SIMPLE_ASSERT( asg::ToolStore::get( "dummyTool4" ) == 0 );

   // Retrun gracefully:
   return 0;
}
