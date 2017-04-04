
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//

#include "xAODJet/Jet.h"

#include "gtest/gtest.h"

// Example based on xAODJet/test/xAODJet_Jet_test.cxx

namespace Athena_test {

  class xAODJetTest : public ::testing::Test {
  public:
    xAODJetTest() {
      jet.makePrivateStore();
    }
    xAOD::Jet jet;
  };

  TEST_F( xAODJetTest, jetpt ) {
    jet.setJetP4( xAOD::JetFourMom_t( 1.0, 1.0, 0.0, 3.0 ) );
    EXPECT_EQ( jet.pt(), 1.0 );
  }

  TEST_F( xAODJetTest, jetgetAttribute ) {
    float value( 42.0 );
    jet.setAttribute<float>( "test", value ) ;
    float testvalue;
    EXPECT_TRUE( jet.getAttribute<float>( "test", testvalue ) );
    EXPECT_EQ( 42.0, testvalue );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

