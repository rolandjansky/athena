/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//

#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include <string>

#include "AsgTools/AnaToolHandle.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "AthAsgExUnittest/IMyPackageTool.h"
#include "AthAsgExUnittest/MyPackageTool.h"

#include "GaudiKernel/IAlgManager.h"
#include "Gaudi/Algorithm.h"

#include <iostream>
#include <fstream>

// Tool test suite:

namespace Athena_test {

  // Tool test suite:

  class MyPackageToolTest : public ::testing::Test  {
  public:
    MyPackageToolTest() : myTool( "MyTool" ) {}
    ~MyPackageToolTest() {}
    MyPackageTool myTool;
  };

  TEST_F( MyPackageToolTest, initialise ) {
    EXPECT_TRUE( myTool.initialize().isSuccess() );
  }

  TEST_F( MyPackageToolTest, property ) {
    EXPECT_TRUE( myTool.setProperty( "Property", 42.0 ).isSuccess() );
    EXPECT_TRUE( myTool.initialize().isSuccess() );
    std::string prop;
    EXPECT_TRUE( myTool.getProperty( "Property", prop ).isSuccess() );
    EXPECT_EQ( std::stod( prop ), 42.0 );
  }

  TEST_F( MyPackageToolTest, enumProperty ) {
    EXPECT_TRUE( myTool.setProperty( "EnumProperty", IMyPackageTool::Val2 ).isSuccess() );
    EXPECT_TRUE( myTool.initialize().isSuccess() );
    std::string prop;
    EXPECT_TRUE( myTool.getProperty( "EnumProperty", prop ).isSuccess() );
    EXPECT_EQ( std::stoi( prop ), IMyPackageTool::Val2 );
  }

  // Algorithm test suite:

  class MyPackageAlgTest : public InitGaudiGoogleTest {
  public:
    virtual void SetUp() override {
      // Algorithm and Tool properties via service:
      // see: Control/AthAnalysisBaseComps/AthAnalysisBaseComps/AthAnalysisHelper.h
      EXPECT_TRUE( AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg",
                                                              "MyProperty",
                                                              21 ).isSuccess() );
      EXPECT_TRUE( AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg",
                                                              "MyTool",
                                                              "MyPackageTool/AnotherName" ).isSuccess() );
      // Set property on the tool
      EXPECT_TRUE( AthAnalysisHelper::addPropertyToCatalogue( "MyPackageAlg.AnotherName",
                                                              "Property",
                                                              42.0 ).isSuccess() );
      // Create instance of my algorithm through Gaudi.
      IAlgManager* algMgr = svcMgr.as< IAlgManager >();
      EXPECT_TRUE( algMgr != nullptr );
      IAlgorithm* alg = nullptr;
      EXPECT_TRUE( algMgr->createAlgorithm( "MyPackageAlg", "MyPackageAlg",
                                            alg ).isSuccess() );
      EXPECT_TRUE( alg != nullptr );
      myAlg = dynamic_cast< Algorithm* >( alg );
      EXPECT_TRUE( myAlg != nullptr );
    }

    MyPackageTool* getMyTool() {
      ToolHandle<IMyPackageTool> toolHandle( "", myAlg );
      toolHandle.setTypeAndName( myAlg->getProperty( "MyTool" ).toString() );
      EXPECT_TRUE( toolHandle.retrieve().isSuccess() );
      IMyPackageTool* impt= toolHandle.get();
      MyPackageTool* mpt= dynamic_cast<MyPackageTool*>( impt );
      return mpt;
    }

    Algorithm* myAlg = nullptr;

  };

  TEST_F( MyPackageAlgTest, initialise ) {
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
  }

  TEST_F( MyPackageAlgTest, setProperty ) {
    EXPECT_TRUE( myAlg->setProperty( "MyProperty", 5 ).isSuccess() );
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
    std::string prop;
    EXPECT_TRUE( myAlg->getProperty( "MyProperty", prop ).isSuccess() );
    EXPECT_EQ( prop, "5" );
  }

  TEST_F( MyPackageAlgTest, sysInitialize ) {
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    std::string prop;
    EXPECT_TRUE( myAlg->getProperty( "MyProperty", prop ).isSuccess() );
    EXPECT_EQ( std::stoi( prop ), 21 );
  }

  TEST_F( MyPackageAlgTest, toolProperty ) {
    // sysInitialize() gets properties then call initialize()
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    MyPackageTool* mpt= getMyTool();
    std::string prop;
    EXPECT_TRUE( mpt->getProperty( "Property", prop ).isSuccess() );
    EXPECT_EQ( std::stod( prop ), 42.0 );
  }

}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
