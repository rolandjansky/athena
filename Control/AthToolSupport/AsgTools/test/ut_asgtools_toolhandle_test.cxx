// $Id: ut_asgtools_toolhandle_test.cxx 659434 2015-04-08 11:53:27Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"

/// Helper macro to test some statements
#define SIMPLE_ASSERT( EXP )                                         \
   do {                                                              \
      const bool result = EXP;                                       \
      if( ! result ) {                                               \
         std::cerr << "Failed to evaluate: " << #EXP << std::endl;   \
         return 1;                                                   \
      }                                                              \
   } while( 0 )

int main() {

   // Create some dummy tools on the stack:
   asg::AsgTool tool1( "dummyTool1" );
   asg::AsgMetadataTool tool2( "dummyTool2" );

   // Construct tool handles in a number of different ways:
   ToolHandle< asg::IAsgTool > handle1( 0 );
   ToolHandle< asg::IAsgTool > handle2( &tool1 );
   ToolHandle< asg::IAsgTool > handle3( "dummyTool2" );
   ToolHandle< asg::IAsgTool > handle4( "asg::AsgTool/dummyTool1" );
   ToolHandle< asg::IAsgTool > handle5( "asg::AsgMetadataTool" );

   // And now test if they behave as they should:
   SIMPLE_ASSERT( handle1.empty() == true );
   SIMPLE_ASSERT( handle1.typeAndName() == "" );
   SIMPLE_ASSERT( handle1.type() == "" );
   SIMPLE_ASSERT( handle1.name() == "" );

   SIMPLE_ASSERT( handle2.operator->() == &tool1 );
   SIMPLE_ASSERT( handle2.name() == tool1.name() );

   SIMPLE_ASSERT( handle3.operator->() == &tool2 );
   SIMPLE_ASSERT( handle3.name() == "dummyTool2" );

   SIMPLE_ASSERT( handle4.operator->() == &tool1 );
   SIMPLE_ASSERT( handle4.type() == "asg::AsgTool" );
   SIMPLE_ASSERT( handle4.name() == "dummyTool1" );

   SIMPLE_ASSERT( handle5.operator->() == 0 );
   SIMPLE_ASSERT( handle5.type() == "asg::AsgMetadataTool" );
   SIMPLE_ASSERT( handle5.name() == "asg::AsgMetadataTool" );

   // Return gracefully:
   return 0;
}
