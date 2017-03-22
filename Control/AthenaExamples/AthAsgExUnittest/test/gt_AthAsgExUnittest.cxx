
#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include <string>

#include "AsgTools/AnaToolHandle.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "../AthAsgExUnittest/IMyPackageTool.h"

#include "../AthAsgExUnittest/MyPackageTool.h"
#include "../src/MyPackageAlg.h"

#include <iostream>
#include <fstream>

// Tool test suite:

namespace Athena_test {

  // Tool test suite:

  class MyPackageToolTest : public ::testing::Test  {
  public:
    MyPackageToolTest() : myTool( "MyTool", true ) {}
    ~MyPackageToolTest() {}
    MyPackageTool myTool;
  };

  TEST_F( MyPackageToolTest, initialise ) {
    EXPECT_TRUE( myTool.initialize() );
  }

  TEST_F( MyPackageToolTest, property ) {
    EXPECT_TRUE( myTool.setProperty( "Property", 42.0 ) );
    EXPECT_TRUE( myTool.initialize() );
    std::string prop;
    myTool.getProperty( "Property", prop );
    EXPECT_EQ( std::stod( prop ), 42.0 );
  }
  
  TEST_F( MyPackageToolTest, enumProperty ) {
    EXPECT_TRUE( myTool.setProperty( "EnumProperty", IMyPackageTool::Val2 ) );
    EXPECT_TRUE( myTool.initialize() );
    std::string prop;
    myTool.getProperty( "EnumProperty", prop );
    EXPECT_EQ( std::stoi( prop ), IMyPackageTool::Val2 ); 
  }
  
  // Algorithm test suite:

  class MyPackageAlgTest : public InitGaudiGoogleTest {
  public:

    MyPackageAlgTest() 
    //  : InitGaudiGoogleTest( MSG::INFO ) // get usual message blurb
    {}

    virtual void SetUp() override {
      // Algorithm and Tool properties via service:
      // see: https://svnweb.cern.ch/trac/atlasoff/browser/Control/AthAnalysisBaseComps/trunk/AthAnalysisBaseComps/AthAnalysisHelper.h#L32
      AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg",
						 "MyProperty",
						 21 );
      AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg",
						 "MyTool",
						 "MyPackageTool/AnotherName" );
      // Set property on the tool
      AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg.AnotherName",
						 "Property", 
						 42.0 );
      // Create instance of my algorithm directly in dual_use package
      myAlg= new MyPackageAlg( "MyPackageAlg", Gaudi::svcLocator() );
    }

    MyPackageTool* getMyTool() {
      ToolHandle<IMyPackageTool> toolHandle( "", myAlg );
      toolHandle.setTypeAndName( myAlg->getProperty( "MyTool" ).toString() );
      toolHandle.retrieve();
      IMyPackageTool* impt= toolHandle.get();
      MyPackageTool* mpt= dynamic_cast<MyPackageTool*>( impt );
      return mpt;
    }

    MyPackageAlg* myAlg;

  };
  
  TEST_F( MyPackageAlgTest, initialise ) {
    EXPECT_TRUE( myAlg->initialize() );
  }
  
  TEST_F( MyPackageAlgTest, setProperty ) {
    EXPECT_TRUE( myAlg->setProperty( "MyProperty", 5 ) );
    EXPECT_TRUE( myAlg->initialize() );
    std::string prop;
    myAlg->getProperty( "MyProperty", prop );
    EXPECT_EQ( prop, "5" );
  }
  
  TEST_F( MyPackageAlgTest, sysInitialize ) {
    EXPECT_TRUE( myAlg->sysInitialize() );
    std::string prop;
    myAlg->getProperty( "MyProperty", prop );
    EXPECT_EQ( std::stoi( prop ), 21 );
  }
  
  TEST_F( MyPackageAlgTest, toolProperty ) {
    // sysInitialize() gets properties then call initialize()
    EXPECT_TRUE( myAlg->sysInitialize() );
    MyPackageTool* mpt= getMyTool();
    std::string prop;
    mpt->getProperty( "Property", prop );
    EXPECT_EQ( std::stod( prop ), 42.0 );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

