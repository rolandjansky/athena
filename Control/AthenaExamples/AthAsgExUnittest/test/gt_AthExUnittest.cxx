
//
//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthService.h"

#include "../src/IMyPackageTool.h"

#include <string>
#include <iostream>
#include <fstream>

namespace Athena_test {

  // Algorithm test suite:

  class MyPackageAlgTest : public InitGaudiGoogleTest {
  public:

    MyPackageAlgTest() 
    //  : InitGaudiGoogleTest( MSG::INFO ) // get usual message blurb
      : myAlg(nullptr)
    {}

    virtual void SetUp() override {

      ServiceHandle<IJobOptionsSvc> joSvc( "JobOptionsSvc",
                                           "MyPackageAlgTest" );
      joSvc->addPropertyToCatalogue( "MyPackageAlg",
                                     StringProperty( "MyProperty", Gaudi::Utils::toString( 21 ) ) ).ignore();
      joSvc->addPropertyToCatalogue( "MyPackageAlg",
                                     StringProperty( "MyTool", "MyPackageTool/AnotherName" ) ).ignore();
      joSvc->addPropertyToCatalogue( "MyPackageAlg.AnotherName",
                                     StringProperty( "Property", Gaudi::Utils::toString( 42.0 ) ) ).ignore();
      IAlgorithm* ialg= Algorithm::Factory::create( "MyPackageAlg",
                                                    "MyPackageAlg",
                                                    Gaudi::svcLocator() ).release();
      myAlg= dynamic_cast<Gaudi::Algorithm*>( ialg );

    }

    IMyPackageTool* getMyTool() {
      ToolHandle<IMyPackageTool> toolHandle( "", myAlg );
      toolHandle.setTypeAndName( myAlg->getProperty( "MyTool" ).toString() );
      EXPECT_TRUE( toolHandle.retrieve().isSuccess() );
      IMyPackageTool* impt= toolHandle.get();
      return impt;
    }

    int getIntProperty( const std::string & name ) {
      std::string prop;
      EXPECT_TRUE( myAlg->getProperty( name, prop ) );
      return std::stoi( prop );
    }

    Gaudi::Algorithm* myAlg;

  };
  
  TEST_F( MyPackageAlgTest, getDefaultPropertyValue ) {
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 1 );
  }
  
  TEST_F( MyPackageAlgTest, initialise ) {
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
  }
  
  TEST_F( MyPackageAlgTest, setProperty ) {
    EXPECT_TRUE( myAlg->setProperty( "MyProperty", 5 ).isSuccess() );
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 5 );
  }
  
  TEST_F( MyPackageAlgTest, getPropertyFromCatalogue ) {
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 21 );
  }
  
  TEST_F( MyPackageAlgTest, toolProperty ) {
    // sysInitialize() gets properties then calls initialize()
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    IMyPackageTool* mpt= getMyTool();
    double prop= mpt->useTheProperty();
    EXPECT_EQ( prop, 42.0 );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
