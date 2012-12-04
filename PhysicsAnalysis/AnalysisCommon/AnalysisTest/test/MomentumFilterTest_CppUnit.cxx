/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the MomentumFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// DataModel includes
#include "DataModel/DataVector.h"

// EventKernel
#include "EventKernel/I4Momentum.h"
#include "EventKernel/IParticle.h"

// FourMom includes
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4EEtaPhiM.h"

// AnalysisUtils includes
#include "AnalysisUtils/MomentumFilter.h"

class MomentumFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "MomentumFilterTest"
  CPPUNIT_TEST_SUITE( MomentumFilterTest );

  CPPUNIT_TEST( testCopyMomentumFilter );

  CPPUNIT_TEST( testSettersAndGetters );
  CPPUNIT_TEST( testModuloPhi );

  CPPUNIT_TEST( testPxFilter );
  CPPUNIT_TEST( testPyFilter );
  CPPUNIT_TEST( testPzFilter );
  CPPUNIT_TEST( testEneFilter );

  CPPUNIT_TEST( testEtaFilter );
  CPPUNIT_TEST( testPhiFilter );
  CPPUNIT_TEST( testMassFilter );
  CPPUNIT_TEST( testPtFilter );

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

  /// Test that momentum filter cuts are well copied
  void testCopyMomentumFilter()
  {
    MomentumFilter<I4Momentum> orig;
    orig.setRange( "px", 100.*GeV, 120.*GeV );
    orig.setRange( "py", 100.*GeV, 120.*GeV );
    orig.setRange( "pz", 100.*GeV, 120.*GeV );
    orig.setRange( "ene",200.*GeV, 200.*GeV );

    P4PxPyPzE p1( 100.*GeV, 100*GeV, 100*GeV, 200*GeV );
    P4PxPyPzE p2( 110.*GeV, 110*GeV, 110*GeV, 210*GeV );

    CPPUNIT_ASSERT(  orig.isAccepted( &p1 ) );
    CPPUNIT_ASSERT( !orig.isAccepted( &p2 ) );

    {
      MomentumFilter<I4Momentum> filter;
      filter.setEneMin( 205.*GeV );
      CPPUNIT_ASSERT( !filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT(  filter.isAccepted( &p2 ) );
      
      filter.setFilter( &orig );
      CPPUNIT_ASSERT(  filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT( !filter.isAccepted( &p2 ) );
    }
    {
      MomentumFilter<I4Momentum> filter;
      filter.setEneMin( 205.*GeV );
      CPPUNIT_ASSERT( !filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT(  filter.isAccepted( &p2 ) );
      
      filter = orig;
      CPPUNIT_ASSERT(  filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT( !filter.isAccepted( &p2 ) );
    }
    {
      MomentumFilter<I4Momentum> filter = orig;
      CPPUNIT_ASSERT(  filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT( !filter.isAccepted( &p2 ) );
    }      
    {
      MomentumFilter<I4Momentum> filter(orig);
      CPPUNIT_ASSERT(  filter.isAccepted( &p1 ) );
      CPPUNIT_ASSERT( !filter.isAccepted( &p2 ) );
    }      
  }

  /// Test the setters and getters
  void testSettersAndGetters()
  {
    MomentumFilter<I4Momentum> orig;

    MomentumFilter<I4Momentum> filter;
    CPPUNIT_ASSERT( -FLT_MAX == filter.pxMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.pxMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.pyMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.pyMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.pzMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.pzMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.eneMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.eneMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.etaMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.etaMax() );
    CPPUNIT_ASSERT(    -M_PI == filter.phiMin() );
    CPPUNIT_ASSERT(    +M_PI == filter.phiMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.massMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.massMax() );
    CPPUNIT_ASSERT( -FLT_MAX == filter.ptMin() );
    CPPUNIT_ASSERT( +FLT_MAX == filter.ptMax() );

    double min =  10*GeV;
    double max = 100*GeV;

    // Px
    filter.setFilter( &orig );
    filter.setPxMin( min );
    filter.setPxMax( max );
    CPPUNIT_ASSERT( min == filter.pxMin() );
    CPPUNIT_ASSERT( max == filter.pxMax() );

    // Py
    filter.setFilter( &orig );
    filter.setPyMin( min );
    filter.setPyMax( max );
    CPPUNIT_ASSERT( min == filter.pyMin() );
    CPPUNIT_ASSERT( max == filter.pyMax() );

    // Pz
    filter.setFilter( &orig );
    filter.setPzMin( min );
    filter.setPzMax( max );
    CPPUNIT_ASSERT( min == filter.pzMin() );
    CPPUNIT_ASSERT( max == filter.pzMax() );

    // Ene
    filter.setFilter( &orig );
    filter.setEneMin( min );
    filter.setEneMax( max );
    CPPUNIT_ASSERT( min == filter.eneMin() );
    CPPUNIT_ASSERT( max == filter.eneMax() );

    // Eta
    filter.setFilter( &orig );
    filter.setEtaMin( 1. );
    filter.setEtaMax( 2.5 );
    CPPUNIT_ASSERT(   1 == filter.etaMin() );
    CPPUNIT_ASSERT( 2.5 == filter.etaMax() );

    // Phi
    filter.setFilter( &orig );
    filter.setPhiMin( -M_PI );
    filter.setPhiMax( +M_PI );
    CPPUNIT_ASSERT( -M_PI == filter.phiMin() );
    CPPUNIT_ASSERT( +M_PI == filter.phiMax() );

    // Mass
    filter.setFilter( &orig );
    filter.setMassMin( min );
    filter.setMassMax( max );
    CPPUNIT_ASSERT( min == filter.massMin() );
    CPPUNIT_ASSERT( max == filter.massMax() );

    // Pt
    filter.setFilter( &orig );
    filter.setPtMin( min );
    filter.setPtMax( max );
    CPPUNIT_ASSERT( min == filter.ptMin() );
    CPPUNIT_ASSERT( max == filter.ptMax() );


    min =  20*GeV;
    max = 200*GeV;

    // Px
    filter.setFilter( &orig );
    filter.setRange( "px", min, max );
    CPPUNIT_ASSERT( min == filter.pxMin() );
    CPPUNIT_ASSERT( max == filter.pxMax() );

    // Py
    filter.setFilter( &orig );
    filter.setRange( "py", min, max );
    CPPUNIT_ASSERT( min == filter.pyMin() );
    CPPUNIT_ASSERT( max == filter.pyMax() );

    // Pz
    filter.setFilter( &orig );
    filter.setRange( "pz", min, max );
    CPPUNIT_ASSERT( min == filter.pzMin() );
    CPPUNIT_ASSERT( max == filter.pzMax() );

    // Ene
    filter.setFilter( &orig );
    filter.setRange( "ene", min, max );
    CPPUNIT_ASSERT( min == filter.eneMin() );
    CPPUNIT_ASSERT( max == filter.eneMax() );

    // Eta
    filter.setFilter( &orig );
    filter.setRange( "eta", 1., 2.5 );
    CPPUNIT_ASSERT(   1 == filter.etaMin() );
    CPPUNIT_ASSERT( 2.5 == filter.etaMax() );

    // Phi
    filter.setFilter( &orig );
    filter.setRange( "phi", -M_PI, +M_PI );
    CPPUNIT_ASSERT( -M_PI == filter.phiMin() );
    CPPUNIT_ASSERT( +M_PI == filter.phiMax() );
    filter.setFilter( &orig );
    filter.setRange( "phi", 0., +M_PI/2 );
    CPPUNIT_ASSERT(       0 == filter.phiMin() );
    CPPUNIT_ASSERT( +M_PI/2 == filter.phiMax() );
    filter.setFilter( &orig );
    filter.setRange( "phi", M_PI/2, +M_PI );
    CPPUNIT_ASSERT(  M_PI/2 == filter.phiMin() );
    CPPUNIT_ASSERT( +M_PI   == filter.phiMax() );
    filter.setFilter( &orig );
    filter.setRange( "phi", M_PI, 3*M_PI/2 );
    CPPUNIT_ASSERT( -M_PI/2 == filter.phiMin() );
    CPPUNIT_ASSERT(    M_PI == filter.phiMax() );
    filter.setFilter( &orig );
    filter.setRange( "phi", 3*M_PI/2, 4*M_PI/2 );
    CPPUNIT_ASSERT( -M_PI/2 == filter.phiMin() );
    CPPUNIT_ASSERT(       0 == filter.phiMax() );

    // Mass
    filter.setFilter( &orig );
    filter.setRange( "mass", min, max );
    CPPUNIT_ASSERT( min == filter.massMin() );
    CPPUNIT_ASSERT( max == filter.massMax() );

    // Pt
    filter.setFilter( &orig );
    filter.setRange( "pt", min, max );
    CPPUNIT_ASSERT( min == filter.ptMin() );
    CPPUNIT_ASSERT( max == filter.ptMax() );

  }

  /// Test the phi modulation
  void testModuloPhi()
  {
    MomentumFilter<I4Momentum> filter;
    
    CPPUNIT_ASSERT( -M_PI == filter.atlasPhi( -M_PI ) );
    CPPUNIT_ASSERT(  M_PI == filter.atlasPhi(  M_PI ) );
    CPPUNIT_ASSERT( -M_PI != filter.atlasPhi(  M_PI ) );
    CPPUNIT_ASSERT(  M_PI != filter.atlasPhi( -M_PI ) );

    CPPUNIT_ASSERT(     0 == filter.atlasPhi( 0 ) );
    CPPUNIT_ASSERT(  M_PI != filter.atlasPhi( 0 ) );
    CPPUNIT_ASSERT( -M_PI != filter.atlasPhi( 0 ) );

    CPPUNIT_ASSERT(  M_PI/2 != filter.atlasPhi( 0 ) );
    CPPUNIT_ASSERT( -M_PI/2 != filter.atlasPhi( 0 ) );

    filter.setRange( "phi", M_PI/2, M_PI );
    P4EEtaPhiM p1( 200.*GeV, 1.,   0., 5.*GeV );
   
    CPPUNIT_ASSERT( M_PI/2 == filter.phiMin() );
    CPPUNIT_ASSERT( M_PI   == filter.phiMax() );
    CPPUNIT_ASSERT( 0      == filter.atlasPhi( p1.phi() ) );

    CPPUNIT_ASSERT( -M_PI != filter.phiMin() );
    CPPUNIT_ASSERT( +M_PI == filter.phiMax() );

    CPPUNIT_ASSERT( -M_PI != filter.atlasPhi(filter.phiMin()) ||
		    +M_PI != filter.atlasPhi(filter.phiMax()) );
    CPPUNIT_ASSERT( !( (filter.phiMin() <= filter.atlasPhi(p1.phi()) &&
			filter.phiMax() >= filter.atlasPhi(p1.phi()) ) ) );
    CPPUNIT_ASSERT( !filter.isAccepted( &p1 ) );
  }

  /// Test filtering objects based on their Px
  void testPxFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setPxMin(   0.*GeV );
    filter.setPxMax( 100.*GeV );

    P4PxPyPzE p1( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV );
    P4PxPyPzE p2( 110.*GeV, 100.*GeV, 100.*GeV, 200.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPxMax( 110.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPxMin( 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "px", 0.*GeV, 100.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "px", 0.*GeV, 110.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setRange( "px", 105.*GeV, 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

  /// Test filtering objects based on their Py
  void testPyFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setPyMin(   0.*GeV );
    filter.setPyMax( 100.*GeV );

    P4PxPyPzE p1( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV );
    P4PxPyPzE p2( 100.*GeV, 110.*GeV, 100.*GeV, 200.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPyMax( 110.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPyMin( 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "py", 0.*GeV, 100.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "py", 105.*GeV, 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

  /// Test filtering objects based on their Pz
  void testPzFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setPzMin(   0.*GeV );
    filter.setPzMax( 100.*GeV );

    P4PxPyPzE p1( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV );
    P4PxPyPzE p2( 100.*GeV, 100.*GeV, 110.*GeV, 200.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPzMax( 110.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPzMin( 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "pz", 0.*GeV, 100.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "pz", 105.*GeV, 110.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

  /// Test filtering objects based on their Ene
  void testEneFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setEneMin(   0.*GeV );
    filter.setEneMax( 200.*GeV );

    P4PxPyPzE p1( 100.*GeV, 100.*GeV, 100.*GeV, 200.*GeV );
    P4PxPyPzE p2( 100.*GeV, 100.*GeV, 100.*GeV, 210.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setEneMax( 210.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setEneMin( 210.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "ene", 0.*GeV, 200.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "ene", 205.*GeV, 210.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

  /// Test filtering objects based on their Eta
  void testEtaFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setEtaMin( 0.0 );
    filter.setEtaMax( 2.5 );

    P4EEtaPhiM p1( 200.*GeV, 1., M_PI, 5.*GeV );
    P4EEtaPhiM p2( 200.*GeV, 3., M_PI, 5.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setEtaMax( 3. );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setEtaMin( 2.5 );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "eta", 0., 2.5 );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "eta", 2.5, 3. );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );

    /// Explore negative eta
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, -1., M_PI, 5.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, -3., M_PI, 5.*GeV ) );

    filter.setEtaMin( -2.5 );
    filter.setEtaMax(  0.0 );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setEtaMin( -3. );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setEtaMax( -2.5 );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "eta", -2.5, 0. );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "eta", -3., -2.5 );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );

  }

  /// Test filtering objects based on their Phi
  void testPhiFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setPhiMin( 0.0 );
    filter.setPhiMax( M_PI/2. );

    P4EEtaPhiM p1( 200.*GeV, 1., 0., 5.*GeV );
    P4EEtaPhiM p2( 200.*GeV, 3., M_PI, 5.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPhiMax( M_PI );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPhiMin( M_PI/2. );
    CPPUNIT_ASSERT( M_PI/2 == filter.phiMin() &&
		    M_PI   == filter.phiMax() );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "phi", 0., M_PI/2. );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "phi", M_PI/2., M_PI );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );

    /// Explore negative phi
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 1.,    0., 5.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 3., -M_PI, 5.*GeV ) );

    filter.setPhiMin( -M_PI/2. );
    filter.setPhiMax(  0.0 );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPhiMin( -M_PI );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPhiMax( -M_PI/2. );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "phi", -M_PI/2., 0. );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "phi", -M_PI, -M_PI/2. );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );

    filter.setRange( "phi", -M_PI/2, -M_PI );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );

    ///
    /// Explore modulo phi
    ///
    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 1.,     0., 5.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 3., 2*M_PI, 5.*GeV ) );

    // No phi range policy enforced on the 4-vectors
    CPPUNIT_ASSERT(     0. == p1.phi() );
    CPPUNIT_ASSERT( 2*M_PI == p2.phi() );

    filter.setRange( "phi", 0., 0. );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    filter.setRange( "phi", 2*M_PI, 2*M_PI );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    p1.set4Mom( P4EEtaPhiM( 200.*GeV, 1.,    M_PI/2., 5.*GeV ) );
    p2.set4Mom( P4EEtaPhiM( 200.*GeV, 3., -3*M_PI/2., 5.*GeV ) );

    filter.setRange( "phi", M_PI/2., M_PI/2. );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    filter.setRange( "phi", -3*M_PI/2, -3*M_PI/2 );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    filter.setRange( "phi", -3*M_PI/2, M_PI/2 );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    filter.setRange( "phi", M_PI/2, -3*M_PI/2 );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );

    // test that even if phiRange=[-PI, PI[, the phi range is *not* active
    MomentumFilter<I4Momentum> phiFilter;
    CPPUNIT_ASSERT( phiFilter.isAccepted( &p1 ) );
    CPPUNIT_ASSERT( phiFilter.isAccepted( &p2 ) );
  }

  /// Test filtering objects based on their Mass
  void testMassFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setMassMin(  0.*GeV );
    filter.setMassMax( 80.*GeV );
    
    P4EEtaPhiM p1( 200.*GeV, 1.,   0., 80.*GeV );
    P4EEtaPhiM p2( 200.*GeV, 3., M_PI, 90.*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setMassMax( 90.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setMassMin( 85.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "mass", 0.*GeV, 80.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "mass", 85.*GeV, 90.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

  /// Test filtering objects based on their Pt
  void testPtFilter()
  {
    MomentumFilter<I4Momentum> filter;
    filter.setPtMin(  0.*GeV );
    filter.setPtMax( 20.*GeV );
    
    P4EEtaPhiM p1( 200.*GeV, 3., M_PI, 90.*GeV ); //> Pt ~=  18 GeV
    P4EEtaPhiM p2( 200.*GeV, 1.,   0., 80.*GeV ); //> Pt ~= 119 GeV

    CPPUNIT_ASSERT(  17*GeV <= p1.pt() && p1.pt() <=  18*GeV );
    CPPUNIT_ASSERT( 118*GeV <= p2.pt() && p2.pt() <= 119*GeV );

    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setPtMax( 119.*GeV );
    CPPUNIT_ASSERT( filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( filter.isAccepted(&p2) );
    
    filter.setPtMin( 20.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
    filter.setRange( "pt", 0.*GeV, 20.*GeV );
    CPPUNIT_ASSERT(  filter.isAccepted(&p1) );
    CPPUNIT_ASSERT( !filter.isAccepted(&p2) );

    filter.setRange( "pt", 20.*GeV, 119.*GeV );
    CPPUNIT_ASSERT( !filter.isAccepted(&p1) );
    CPPUNIT_ASSERT(  filter.isAccepted(&p2) );
    
  }

};

/// Registration of the test suite "MomentumFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( MomentumFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_testdriver.cxx>
