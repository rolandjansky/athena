
#include "xAODJet/Jet.h"

#include "gmock/gmock.h"

using ::testing::Return;
using ::testing::_;
using ::testing::SetArgReferee;

namespace Athena_test {

  class MockxAODJet : public xAOD::Jet {
  public:
    MockxAODJet() {}
    MOCK_CONST_METHOD0( pt, double() );
    MOCK_CONST_METHOD0( eta, double() );
    MOCK_CONST_METHOD0( phi, double() );
    MOCK_CONST_METHOD0( m, double () );
    MOCK_CONST_METHOD0( e, double() );
    MOCK_CONST_METHOD0( et, double() );
    MOCK_CONST_METHOD0( rapidity, double() );
    MOCK_CONST_METHOD0( p4, const TLorentzVector&() );
    MOCK_CONST_METHOD0( position, unsigned int() );
    MOCK_CONST_METHOD0( toString, std::string() );
    MOCK_CONST_METHOD2( getAttributeFloat, bool( const std::string&, 
						 float& ) );
  };

}  

namespace xAOD {

  template<> inline
  bool Jet::getAttribute<float>( const std::string& name, 
				 float& value ) const {
    return dynamic_cast<const Athena_test::MockxAODJet*>( this )
      ->getAttributeFloat( name, value );
  }
  
}

namespace Athena_test {

  TEST( MyxAODAnalysis, jetpt ) {
    MockxAODJet mockjet;
    ON_CALL( mockjet, pt() ).WillByDefault( Return( 1.0 ) );
    EXPECT_CALL( mockjet, pt() ).Times( 1 );
    xAOD::Jet& jet= mockjet;
    EXPECT_EQ( jet.pt(), 1.0 );
  }

  TEST( MyxAODAnalysis, jetgetAttribute ) {
    MockxAODJet mockjet;
    EXPECT_CALL( mockjet, getAttributeFloat( "test", _ ) )
      .WillOnce( DoAll( SetArgReferee<1>( 42 ), Return(true) ) );
    xAOD::Jet& jet= mockjet;
    float value;
    EXPECT_TRUE( jet.getAttribute<float>( "test", value ) );
    EXPECT_EQ( 42, value );
  }
  
}

int main( int argc, char **argv ) {
  ::testing::InitGoogleMock( &argc, argv );
  return RUN_ALL_TESTS();
}

