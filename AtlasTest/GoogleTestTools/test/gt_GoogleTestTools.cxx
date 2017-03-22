
#include "GoogleTestTools/InitGaudiGoogleTest.h"


namespace Athena_test {

  class InitGaudiGoogleTestTest : public InitGaudiGoogleTest {
  public:

    InitGaudiGoogleTestTest() 
    : InitGaudiGoogleTest( MSG::INFO ) // get usual Gaudi message blurb
    {}

    //virtual void SetUp() override {}

  };
  
  TEST_F( InitGaudiGoogleTestTest, basictest ) {
    EXPECT_TRUE( theApp != nullptr );
    EXPECT_TRUE( propMgr.isValid() );
    EXPECT_TRUE( svcLoc.isValid() );
    EXPECT_TRUE( svcMgr.isValid() );
  }

}


int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

