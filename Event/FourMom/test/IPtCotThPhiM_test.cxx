/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>

#include "CLHEP/Units/SystemOfUnits.h"
#include "FourMom/P4IPtCotThPhiM.h"
#include "FourMom/P4Impl.h"
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"
#include "FourMom_test.h"

//************************************************************************

template<class I4Mom1_t, class I4Mom2_t>
void testDefaultConstructor()
{
  I4Mom1_t p;
  p4assert( p.iPt()   == 0./CLHEP::GeV );
  p4assert( p.cotTh() == 0.     );
  p4assert( p.phi()   == 0.     );
  p4assert( p.m()     == 0.*CLHEP::GeV );
}

template<class I4Mom1_t, class I4Mom2_t>
void testNativeConstructor( const double p1, const double p2,
			    const double p3, const double p4 )
{
  I4Mom1_t p( p1, p2, p3, p4 );
  p4assert( p.iPt()   == p1 );
  p4assert( p.cotTh() == p2 );
  p4assert( p.phi()   == p3 );
  p4assert( p.m()     == p4 );
  std::cout << p << std::endl;

  I4Mom1_t pp;
  p4assert( pp.iPt()   == 0./CLHEP::GeV );
  p4assert( pp.cotTh() == 0.     );
  p4assert( pp.phi()   == 0.     );
  p4assert( pp.m()     == 0.*CLHEP::GeV );
  pp = p;
  p4assert( pp.iPt()   == p1 );
  p4assert( pp.cotTh() == p2 );
  p4assert( pp.phi()   == p3 );
  p4assert( pp.m()     == p4 );

}

template<class I4Mom1_t, class I4Mom2_t>
void testHLVConstructor( const double px = 10.*CLHEP::GeV, 
			 const double py = 11.*CLHEP::GeV, 
			 const double pz = 12.*CLHEP::GeV, 
			 const double e  = 20.*CLHEP::GeV )
{
  CLHEP::HepLorentzVector hlv( px, py, pz, e );
  I4Mom1_t p( hlv );
  const CLHEP::HepLorentzVector p4 = p.hlv();
  HLV_ASSERT( p4, hlv );
}

template<class I4Mom1_t, class I4Mom2_t>
void testI4MomConstructors( const double px = 10.*CLHEP::GeV, 
			    const double py = 11.*CLHEP::GeV, 
			    const double pz = 12.*CLHEP::GeV, 
			    const double e  = 20.*CLHEP::GeV )
{
  CLHEP::HepLorentzVector hlv( px, py, pz, e );
  I4Mom1_t p4( hlv );
  {
    const I4Momentum& mom = p4;
    I4Mom2_t p( mom );
    const CLHEP::HepLorentzVector tmp = p.hlv();
    HLV_ASSERT( tmp, hlv );
  }
  {
    const I4Momentum * const mom = &p4;
    I4Mom2_t p( mom );
    const CLHEP::HepLorentzVector tmp = p.hlv();
    HLV_ASSERT( tmp, hlv );
  }
}

template<class I4Mom1_t, class I4Mom2_t>
void testConstructors()
{
  std::cout << "test [Constructors]..." << std::endl;
  testDefaultConstructor<I4Mom1_t, I4Mom2_t>();
  testNativeConstructor<I4Mom1_t, I4Mom2_t>( 1./(10*CLHEP::GeV), 1., M_PI, 1.*CLHEP::GeV );
  testHLVConstructor<I4Mom1_t, I4Mom2_t>();
  testI4MomConstructors<I4Mom1_t, I4Mom2_t>();

  return;
}

template<class I4Mom1_t, class I4Mom2_t>
void testGetters()
{
  std::cout << "test [Getters]..." << std::endl;
  FourMom::testGetters<I4Mom1_t, I4Mom2_t>( 10.*CLHEP::GeV, 11.*CLHEP::GeV, 
					    12.*CLHEP::GeV, 20.*CLHEP::GeV );
}

template<class I4Mom1_t, class I4Mom2_t>
void testSetters( const double px, const double py, 
		  const double pz, const double e )
{
  CLHEP::HepLorentzVector hlv( px, py, pz, e );
  {
    I4Mom1_t p;
    p.set4Mom( hlv );
    const CLHEP::HepLorentzVector p4 = p.hlv();
    HLV_ASSERT( p4, hlv );
  }
  {
    I4Mom1_t p;
    I4Mom2_t pp( hlv );
    const I4Momentum& mom = pp;
    p.set4Mom( mom );
    const CLHEP::HepLorentzVector p4 = p.hlv();
    HLV_ASSERT( p4, hlv );
  }
  {
    I4Mom1_t p;
    I4Mom2_t pp( hlv );
    const I4Momentum * const mom = &pp;
    p.set4Mom( mom );
    const CLHEP::HepLorentzVector p4 = p.hlv();
    HLV_ASSERT( p4, hlv );
  }
  {
    I4Mom1_t p;
    p4assert( p.iPt() != (1./ hlv.perp()) );
    p.setIPt( 1. / hlv.perp() );
    p4assert( std::abs( p.iPt() - (1./ hlv.perp()) ) < epsilon );

    p4assert( p.cotTh() != std::sinh(hlv.eta()) );
    p.setCotTh( std::sinh(hlv.eta()) );
    p4assert( std::abs( p.cotTh() - std::sinh(hlv.eta()) ) < epsilon );

    p4assert( p.phi() != hlv.phi() );
    p.setPhi( hlv.phi() );
    p4assert( std::abs( p.phi() - hlv.phi() ) < epsilon );

    p4assert( p.m() != hlv.m() );
    p.setM( hlv.m() );
    p4assert( std::abs( p.m() - hlv.m() ) < epsilon );

    const CLHEP::HepLorentzVector p4 = p.hlv();
    HLV_ASSERT( p4, hlv );
  }
}

template<class I4Mom1_t, class I4Mom2_t>
void testSetters()
{
  std::cout << "test [Setters]..." << std::endl;
  testSetters<I4Mom1_t, I4Mom2_t>( 10.*CLHEP::GeV, 11.*CLHEP::GeV, 12.*CLHEP::GeV, 20.*CLHEP::GeV );
}



template<class I4Mom1_t, class I4Mom2_t>
void testKine( const double px, const double py, 
		  const double pz, const double e )
{
  CLHEP::HepLorentzVector hlv( px, py, pz, e );
  I4Mom1_t p;
  p.set4Mom( hlv );

  p4assert ( std::abs(std::atan2(p.sinPhi(),p.cosPhi()) -p.phi()) < epsilon );
  p4assert ( std::abs(p.sinPhi()*p.sinPhi()+p.cosPhi()*p.cosPhi() -1.) < epsilon );
  p4assert ( std::abs(-std::log(std::tan(std::atan(p.tanTh())/2.))-p.eta() ) < epsilon );
  
}

template<class I4Mom1_t, class I4Mom2_t>
void testKine()
{
  std::cout << "test [Kine]..." << std::endl;
  testKine<I4Mom1_t, I4Mom2_t>( 10.*CLHEP::GeV, 11.*CLHEP::GeV, 12.*CLHEP::GeV, 20.*CLHEP::GeV );
}

template<class I4Mom1_t, class I4Mom2_t>
void tests()
{

  std::cout << "IPtCotThPhiM tests... [epsilon = " << epsilon << "]" 
	    << std::endl;
  testConstructors<I4Mom1_t,I4Mom2_t>();
  testConstructors<I4Mom2_t,I4Mom1_t>();

  testGetters<I4Mom1_t,I4Mom2_t>();
  testGetters<I4Mom2_t,I4Mom1_t>();

  testSetters<I4Mom1_t,I4Mom2_t>();
  testSetters<I4Mom2_t,I4Mom1_t>();

  testKine<I4Mom1_t,I4Mom2_t>();
  testKine<I4Mom2_t,I4Mom1_t>();

}

int main()
{
  try {
    tests<P4IPtCotThPhiM, 
          P4IPtCotThPhiM>();

    tests<P4IPtCotThPhiM, 
          P4Impl<P4ImplIPtCotThPhiM> >();

    tests<P4Impl<P4ImplIPtCotThPhiM>, 
          P4Impl<P4ImplIPtCotThPhiM> >();

    
  }
  catch (std::exception& e)
  {
    std::cout << "Caught exception: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  std::cout << "done." << std::endl;
  return EXIT_SUCCESS;
}
