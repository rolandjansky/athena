
#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthService.h"

#include "../AthAsgExUnittest/IMyPackageTool.h"

#include <string>
#include <iostream>
#include <fstream>

namespace Athena_test {

  // Algorithm test suite:

  class MyPackageAlgTest : public InitGaudiGoogleTest {
  public:

    MyPackageAlgTest() 
    //  : InitGaudiGoogleTest( MSG::INFO ) // get usual message blurb
    {}

    virtual void SetUp() override {

      ServiceHandle<IJobOptionsSvc> joSvc( "JobOptionsSvc",
					   "MyPackageAlgTest" );
      joSvc->addPropertyToCatalogue( "MyPackageAlg",
				     StringProperty( "MyProperty",
						     Gaudi::Utils::toString( 21 ) ) );
      joSvc->addPropertyToCatalogue( "MyPackageAlg",
				     StringProperty( "MyTool",
						     "MyPackageTool/AnotherName" ) );
      joSvc->addPropertyToCatalogue( "MyPackageAlg.AnotherName",
				     StringProperty( "Property",
						     Gaudi::Utils::toString( 42.0 ) ) );
      IAlgorithm* ialg= Algorithm::Factory::create( "MyPackageAlg",
						    "MyPackageAlg",
						    Gaudi::svcLocator() );
      myAlg= dynamic_cast<Algorithm*>( ialg );

    }

    IMyPackageTool* getMyTool() {
      ToolHandle<IMyPackageTool> toolHandle( "", myAlg );
      toolHandle.setTypeAndName( myAlg->getProperty( "MyTool" ).toString() );
      EXPECT_TRUE( toolHandle.retrieve() );
      IMyPackageTool* impt= toolHandle.get();
      return impt;
    }

    int getIntProperty( const std::string & name ) {
      std::string prop;
      EXPECT_TRUE( myAlg->getProperty( name, prop ) );
      return std::stoi( prop );
    }

    Algorithm* myAlg;

  };
  
  TEST_F( MyPackageAlgTest, getDefaultPropertyValue ) {
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 1 );
  }
  
  TEST_F( MyPackageAlgTest, initialise ) {
    EXPECT_TRUE( myAlg->initialize() );
  }
  
  TEST_F( MyPackageAlgTest, setProperty ) {
    EXPECT_TRUE( myAlg->setProperty( "MyProperty", 5 ) );
    EXPECT_TRUE( myAlg->initialize() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 5 );
  }
  
  TEST_F( MyPackageAlgTest, getPropertyFromCatalogue ) {
    EXPECT_TRUE( myAlg->sysInitialize() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 21 );
  }
  
  TEST_F( MyPackageAlgTest, toolProperty ) {
    // sysInitialize() gets properties then calls initialize()
    EXPECT_TRUE( myAlg->sysInitialize() );
    IMyPackageTool* mpt= getMyTool();
    double prop= mpt->useTheProperty();
    EXPECT_EQ( prop, 42.0 );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

