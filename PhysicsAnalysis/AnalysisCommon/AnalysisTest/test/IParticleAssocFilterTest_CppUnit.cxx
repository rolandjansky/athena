/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the IParticleAssocFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// DataModel includes
#include "DataModel/DataVector.h"

// FourMom includes
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4EEtaPhiM.h"

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

// AnalysisAssociation includes
#include "AnalysisAssociation/IParticleAssocFilter.h"

class IParticleAssocFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "IParticleAssocFilterTest"
  CPPUNIT_TEST_SUITE( IParticleAssocFilterTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testCopyIParticleAssocFilter );
  CPPUNIT_TEST( testSettersAndGetters );

  CPPUNIT_TEST( testDeltaPxAssoc );
  CPPUNIT_TEST( testDeltaPyAssoc );
  CPPUNIT_TEST( testDeltaPzAssoc );
  CPPUNIT_TEST( testDeltaEneAssoc );

  CPPUNIT_TEST( testDeltaEtaAssoc );
  CPPUNIT_TEST( testDeltaPhiAssoc );
  CPPUNIT_TEST( testDeltaMassAssoc );
  CPPUNIT_TEST( testDeltaPtAssoc );

  //CPPUNIT_TEST( testDeltaRAssoc );

  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
  

public:

  /// Set up the data members
  void setUp()
  {
  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
  }

  /// Test asociation filter constructor
  void testConstructor()
  {
    IParticleAssocFilter filter;
    CPPUNIT_ASSERT( filter.deltaPxMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPxMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPyMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPyMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPzMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPzMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaEneMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaEneMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaEtaMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaEtaMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPhiMin() == -M_PI );
    CPPUNIT_ASSERT( filter.deltaPhiMax() ==  M_PI );
    CPPUNIT_ASSERT( filter.deltaMassMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaMassMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPtMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaPtMax() ==  FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaRMin() == -FLT_MAX );
    CPPUNIT_ASSERT( filter.deltaRMax() ==  FLT_MAX );
  }

  /// Test that asociation filter cuts are well copied
  void testCopyIParticleAssocFilter()
  {
    IParticleAssocFilter orig;
    orig.setDeltaPx ( 10.*GeV, 20.*GeV );
    orig.setDeltaPy ( 10.*GeV, 20.*GeV );
    orig.setDeltaPz ( 10.*GeV, 20.*GeV );
    orig.setDeltaEne( 10.*GeV, 20.*GeV );

    Jet p1;
    Jet p2;
    p1.set4Mom( P4PxPyPzE( 100.*GeV, 100*GeV, 100*GeV, 200*GeV ) );
    p2.set4Mom( P4PxPyPzE( 110.*GeV, 110*GeV, 110*GeV, 210*GeV ) );
    CPPUNIT_ASSERT( orig.isAccepted( &p1, &p2 ) );

    IParticleAssocFilter assoc;
    assoc.setDeltaPx( 100.*GeV, 200.*GeV );
    CPPUNIT_ASSERT( !assoc.isAccepted( &p1, &p2 ) );

    assoc.setFilter( &orig );
    CPPUNIT_ASSERT( assoc.isAccepted( &p1, &p2 ) );

    // FIXME
    //assoc.reset();
    CPPUNIT_ASSERT( assoc.isAccepted( &p1, &p2 ) );

    IParticleAssocFilter defaultFilter;
    CPPUNIT_ASSERT( defaultFilter.isAccepted( &p1, &p2 ) );

  }

  /// Test the setters and getters of the IParticleAssocFilter class
  void testSettersAndGetters()
  {
    IParticleAssocFilter assocFilter;
    const double min = M_PI/2.;
    const double max = M_PI;

    CPPUNIT_ASSERT( min < max );

    assocFilter.setDeltaPx( min, max );
    assocFilter.setDeltaPy( min, max );
    assocFilter.setDeltaPz( min, max );
    assocFilter.setDeltaEne( min, max );

    assocFilter.setDeltaEta( min, max );
    assocFilter.setDeltaPhi( min, max );
    assocFilter.setDeltaMass( min, max );
    assocFilter.setDeltaPt( min, max );

    assocFilter.setDeltaR( min, max );

    CPPUNIT_ASSERT( min == assocFilter.deltaPxMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaPxMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaPyMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaPyMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaPzMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaPzMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaEneMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaEneMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaEtaMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaEtaMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaPhiMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaPhiMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaMassMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaMassMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaPtMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaPtMax() );

    CPPUNIT_ASSERT( min == assocFilter.deltaRMin() );
    CPPUNIT_ASSERT( max == assocFilter.deltaRMax() );

    /// Explore other interface
    IParticleAssocFilter assocFilter2;
    
    // Max
    assocFilter2.setDeltaPxMax( max );
    assocFilter2.setDeltaPyMax( max );
    assocFilter2.setDeltaPzMax( max );
    assocFilter2.setDeltaEneMax( max );

    assocFilter2.setDeltaEtaMax( max );
    assocFilter2.setDeltaPhiMax( max );
    assocFilter2.setDeltaMassMax( max );
    assocFilter2.setDeltaPtMax( max );

    assocFilter2.setDeltaRMax( max );

    // Min
    assocFilter2.setDeltaPxMin( min );
    assocFilter2.setDeltaPyMin( min );
    assocFilter2.setDeltaPzMin( min );
    assocFilter2.setDeltaEneMin( min );

    assocFilter2.setDeltaEtaMin( min );
    assocFilter2.setDeltaPhiMin( min );
    assocFilter2.setDeltaMassMin( min );
    assocFilter2.setDeltaPtMin( min );

    assocFilter2.setDeltaRMin( min );

    CPPUNIT_ASSERT( min == assocFilter2.deltaPxMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaPxMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaPyMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaPyMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaPzMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaPzMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaEneMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaEneMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaEtaMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaEtaMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaPhiMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaPhiMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaMassMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaMassMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaPtMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaPtMax() );

    CPPUNIT_ASSERT( min == assocFilter2.deltaRMin() );
    CPPUNIT_ASSERT( max == assocFilter2.deltaRMax() );

  }
  
  /// Test the association between objects based on their Px
  void testDeltaPxAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaPxMin(  0.*GeV );
    assocFilter.setDeltaPxMax( 10.*GeV );

    Jet p1;
    Jet p2;
    p1.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV ) );
    p2.set4Mom( P4PxPyPzE( 110.*GeV, 100.*GeV, 100.*GeV, 200.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPxMax( 9.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );

  }

  /// Test the association between objects based on their Py
  void testDeltaPyAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaPyMin(  0.*GeV );
    assocFilter.setDeltaPyMax( 10.*GeV );

    Jet p1;
    Jet p2;
    p1.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV ) );
    p2.set4Mom( P4PxPyPzE( 100.*GeV, 110.*GeV, 100.*GeV, 200.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPyMax( 9.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
    
  }

  /// Test the association between objects based on their Pz
  void testDeltaPzAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaPzMin(  0.*GeV );
    assocFilter.setDeltaPzMax( 10.*GeV );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV ) );
    p2.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 110.*GeV, 200.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPzMax( 9.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
    
  }

  /// Test the association between objects based on their energy
  void testDeltaEneAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaEneMin(  0.*GeV );
    assocFilter.setDeltaEneMax( 10.*GeV );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV ) );
    p2.set4Mom( P4PxPyPzE( 100.*GeV, 100.*GeV, 100.*GeV, 210.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaEneMax( 9.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
    
  }

  /// Test the association between objects based on their eta
  void testDeltaEtaAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaEtaMin(  0.0 );
    assocFilter.setDeltaEtaMax(  2.5 );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,      0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, M_PI/2., 90.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaEtaMax( 2. );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, -1.0,      0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, -2.5, M_PI/2., 90.*GeV ) );

    /// Test negative eta
    assocFilter.setDeltaEtaMax( 1.5 );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaEta( 2., 3. );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
  }

  /// Test the association between objects based on their phi
  void testDeltaPhiAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaPhiMin(   0.0   );
    assocFilter.setDeltaPhiMax( M_PI/2. );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,      0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, M_PI/2., 90.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPhi( 0., M_PI/4. );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );

    /// Test negative phi
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,       0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, -M_PI/2., 90.*GeV ) );

    assocFilter.setDeltaPhi( 0., M_PI/2. );
    
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPhi( 0., M_PI/4. );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );

    /// Test Modulo
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,     0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, 2*M_PI, 90.*GeV ) );

    assocFilter.setDeltaPhi( 0., 0. );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    /// Test Modulo at discontinuity point
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,  M_PI, 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, -M_PI, 90.*GeV ) );
    
    assocFilter.setDeltaPhi( 0., 0. );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    /// Test Modulo at discontinuity point
    const double denom = 12;
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,  M_PI-M_PI/denom, 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, -M_PI+M_PI/denom, 90.*GeV ) );
    
    assocFilter.setDeltaPhi( 0., 2*M_PI/denom );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, -M_PI+M_PI/(denom-1), 90.*GeV ) );
    CPPUNIT_ASSERT(  assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT(  assocFilter.isAccepted(&p2, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );

  }

  /// Test the association between objects based on their mass
  void testDeltaMassAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaMassMin(  0.*GeV );
    assocFilter.setDeltaMassMax( 11.*GeV );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,      0., 110.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, M_PI/2., 120.*GeV ) );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaMass( 0.*GeV, 9.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
    
    assocFilter.setDeltaMass( 0.*GeV, 10.*GeV );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    /// Test the accuracy of P4EEtaPhiM
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,      0., 10.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, M_PI/2., 20.*GeV ) );

    assocFilter.setDeltaMass( 0.*GeV, 10.*GeV );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    /// Test the accuracy of P4EEtaPhiM
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 0.0,      0., 80.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 2.5, M_PI/2., 90.*GeV ) );

    assocFilter.setDeltaMass( 0.*GeV, 10.*GeV );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

  }

  /// Test the association between objects based on their Pt
  void testDeltaPtAssoc()
  {
    IParticleAssocFilter assocFilter;
    assocFilter.setDeltaPtMin(   0.*GeV );
    assocFilter.setDeltaPtMax( 200.*GeV );

    Jet p1;
    Jet p2;

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 3., M_PI, 90.*GeV ) ); //> Pt ~=  18 GeV
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 1.,   0., 80.*GeV ) ); //> Pt ~= 119 GeV

    CPPUNIT_ASSERT(  17*GeV <= p1.pt() && p1.pt() <=  18*GeV );
    CPPUNIT_ASSERT( 118*GeV <= p2.pt() && p2.pt() <= 119*GeV );

    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p1) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );

    assocFilter.setDeltaPtMax( 100.*GeV );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( !assocFilter.isAccepted(&p2, &p1) );
    
    assocFilter.setDeltaPt( 100.*GeV, 105.*GeV );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p1, &p2) );
    CPPUNIT_ASSERT( assocFilter.isAccepted(&p2, &p1) );
    
  }

};

/// Registration of the test suite "IParticleAssocFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( IParticleAssocFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_SGtestdriver.cxx>
