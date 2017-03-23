
#include "xAODJet/Jet.h"

#include "gtest/gtest.h"

namespace Athena_test {

  TEST( MyxAODAnalysis, jetpt ) {
    xAOD::Jet jet;
    jet.makePrivateStore();
    jet.setJetP4( xAOD::JetFourMom_t( 1.0, 1.0, 0.0, 3.0 ) );
    EXPECT_EQ( jet.pt(), 1.0 );
  }

  TEST( MyxAODAnalysis, jetgetAttribute ) {
    xAOD::Jet jet;
    jet.makePrivateStore();
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

