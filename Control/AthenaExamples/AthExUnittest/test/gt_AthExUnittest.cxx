
//
//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include <string>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthService.h"

#include "../AthExUnittest/IAthExUnittestTool.h"

#include <iostream>
#include <fstream>

namespace Athena_test {

  // Algorithm test suite:

  class AthExUnittestAlgTest : public InitGaudiGoogleTest {
  public:

    AthExUnittestAlgTest() 
    //  : InitGaudiGoogleTest( MSG::INFO ) // get usual message blurb
    {}

    virtual void SetUp() override {

      ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc( "JobOptionsSvc",
                                                           "AthExUnittestAlgTest" );
      joSvc->set( "AthExUnittestAlg.MyProperty", Gaudi::Utils::toString( 21 ) );
      joSvc->set( "AthExUnittestAlg.MyTool","AthExUnittestTool/AnotherName" );
      joSvc->set( "AthExUnittestAlg.AnotherName.Property", Gaudi::Utils::toString( 42.0 ) );
      IAlgorithm* ialg= Algorithm::Factory::create( "AthExUnittestAlg",
                                                    "AthExUnittestAlg",
                                                    Gaudi::svcLocator() ).release();
      myAlg= dynamic_cast<Gaudi::Algorithm*>( ialg );

    }

    IAthExUnittestTool* getMyTool() {
      ToolHandle<IAthExUnittestTool> toolHandle( "", myAlg );
      toolHandle.setTypeAndName( myAlg->getProperty( "MyTool" ).toString() );
      EXPECT_TRUE( toolHandle.retrieve().isSuccess() );
      IAthExUnittestTool* impt= toolHandle.get();
      return impt;
    }

    int getIntProperty( const std::string & name ) {
      std::string prop;
      EXPECT_TRUE( myAlg->getProperty( name, prop ).isSuccess() );
      return std::stoi( prop );
    }

    Gaudi::Algorithm* myAlg = nullptr;

  };
  
  TEST_F( AthExUnittestAlgTest, getDefaultPropertyValue ) {
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 1 );
  }
  
  TEST_F( AthExUnittestAlgTest, initialise ) {
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
  }
  
  TEST_F( AthExUnittestAlgTest, setProperty ) {
    EXPECT_TRUE( myAlg->setProperty( "MyProperty", 5 ).isSuccess() );
    EXPECT_TRUE( myAlg->initialize().isSuccess() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 5 );
  }
  
  TEST_F( AthExUnittestAlgTest, getPropertyFromCatalogue ) {
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    int prop= getIntProperty( "MyProperty" );
    EXPECT_EQ( prop, 21 );
  }
  
  TEST_F( AthExUnittestAlgTest, toolMethod ) {
    // sysInitialize() gets properties then calls initialize()
    EXPECT_TRUE( myAlg->sysInitialize().isSuccess() );
    IAthExUnittestTool* mpt= getMyTool();
    double prop= mpt->useTheProperty();
    EXPECT_EQ( prop, 42.0 );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

