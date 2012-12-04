/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the FilterRange class
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

// AnalysisUtils includes
#include "AnalysisUtils/FilterRange.h"

class FilterRangeTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "FilterRangeTest"
  CPPUNIT_TEST_SUITE( FilterRangeTest );

  CPPUNIT_TEST( testConstructor );
  
  CPPUNIT_TEST( testSetters );
  CPPUNIT_TEST( testGetters );

  CPPUNIT_TEST( testRangeRounding );

  CPPUNIT_TEST( testCopy );
  CPPUNIT_TEST( testCompareRanges );

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

  /// test various ways of constructing a FilterRange
  void testConstructor()
  {
    FilterRange r1;
    CPPUNIT_ASSERT( false == r1.isActive() );

    FilterRange r2(0., 10.);
    CPPUNIT_ASSERT( true  == r2.isActive() );

    FilterRange r3(0., 0.);
    CPPUNIT_ASSERT( true  == r3.isActive() );

    FilterRange r4( -FLT_MAX, FLT_MAX );
    CPPUNIT_ASSERT( false == r4.isActive() );

  }

  /// test the setters
  void testSetters()
  {
    FilterRange range;

    range.setRange( 0., 10. );
    CPPUNIT_ASSERT( true == range.isActive() );

    range.includeAll();
    CPPUNIT_ASSERT( false == range.isActive() );

    range.setRange( 0., 10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( range.isInRange(  0. ) );
    CPPUNIT_ASSERT( range.isInRange( 10. ) );

    range.setRange( -FLT_MAX, 0. );
    CPPUNIT_ASSERT( range.isInRange( -FLT_MAX ) );
    CPPUNIT_ASSERT( range.isInRange(       0. ) );

    range.setRange( 0., FLT_MAX );
    CPPUNIT_ASSERT( range.isInRange(       0. ) );
    CPPUNIT_ASSERT( range.isInRange(  FLT_MAX ) );

    range.setRange( -FLT_MAX, FLT_MAX );
    CPPUNIT_ASSERT( false == range.isActive() );

    range.setRange( 0., 10. );
    CPPUNIT_ASSERT( true == range.isActive() );

    range.setMin( 5. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( !range.isInRange(  0. ) );
    CPPUNIT_ASSERT(  range.isInRange(  5. ) );
    CPPUNIT_ASSERT(  range.isInRange( 10. ) );

    range.setMax( 5. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( !range.isInRange(  0. ) );
    CPPUNIT_ASSERT(  range.isInRange(  5. ) );
    CPPUNIT_ASSERT( !range.isInRange( 10. ) );

    range.includeAll();
    CPPUNIT_ASSERT( false == range.isActive() );

    range.include( 0., 10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( range.isInRange(  0. ) );
    CPPUNIT_ASSERT( range.isInRange(  5. ) );
    CPPUNIT_ASSERT( range.isInRange( 10. ) );

    range.include( -100., -10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( !range.isInRange(  0. ) );
    CPPUNIT_ASSERT(  range.isInRange( -10. ) );
    CPPUNIT_ASSERT(  range.isInRange( -100. ) );

  }

  /// test the getters
  void testGetters()
  {
    FilterRange range;
    CPPUNIT_ASSERT( false == range.isActive() );
    CPPUNIT_ASSERT( -FLT_MAX == range.lower()  );
    CPPUNIT_ASSERT( -FLT_MAX == range.getMin() );
    CPPUNIT_ASSERT( FLT_MAX  == range.upper()  );
    CPPUNIT_ASSERT( FLT_MAX  == range.getMax() );

    range.setRange( 0., 10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT(  0. == range.lower() );
    CPPUNIT_ASSERT(  0. == range.getMin() );
    CPPUNIT_ASSERT( 10. == range.upper() );
    CPPUNIT_ASSERT( 10. == range.getMax() );

    range.includeAll();
    CPPUNIT_ASSERT( false == range.isActive() );
    CPPUNIT_ASSERT( -FLT_MAX == range.lower()  );
    CPPUNIT_ASSERT( -FLT_MAX == range.getMin() );
    CPPUNIT_ASSERT( FLT_MAX  == range.upper()  );
    CPPUNIT_ASSERT( FLT_MAX  == range.getMax() );

    range.setMin( 0. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT(  0. == range.lower() );
    CPPUNIT_ASSERT(  0. == range.getMin() );
    CPPUNIT_ASSERT( FLT_MAX  == range.upper()  );
    CPPUNIT_ASSERT( FLT_MAX  == range.getMax() );

    range.setMax( 10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT(  0. == range.lower() );
    CPPUNIT_ASSERT(  0. == range.getMin() );
    CPPUNIT_ASSERT( 10. == range.upper() );
    CPPUNIT_ASSERT( 10. == range.getMax() );

    range.includeAll();
    range.setMax( 10. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT( -FLT_MAX == range.lower()  );
    CPPUNIT_ASSERT( -FLT_MAX == range.getMin() );
    CPPUNIT_ASSERT( 10. == range.upper() );
    CPPUNIT_ASSERT( 10. == range.getMax() );

    range.setMin( 0. );
    CPPUNIT_ASSERT( true == range.isActive() );
    CPPUNIT_ASSERT(  0. == range.lower() );
    CPPUNIT_ASSERT(  0. == range.getMin() );
    CPPUNIT_ASSERT( 10. == range.upper()  );
    CPPUNIT_ASSERT( 10. == range.getMax() );
    
  }

  /// Test range rounding
  void testRangeRounding()
  {
    FilterRange range;
    range.setRange( -FLT_MAX+1, FLT_MAX-1 );
    CPPUNIT_ASSERT( false == range.isActive() );
    CPPUNIT_ASSERT( FLT_MAX == FLT_MAX-1 );

    range.setRange( -FLT_MAX+10, FLT_MAX-10 );
    CPPUNIT_ASSERT( false == range.isActive() );
    CPPUNIT_ASSERT( FLT_MAX == FLT_MAX-10 );
  }
  
  /// Test the copy of ranges
  void testCopy()
  {
    FilterRange r1;
    FilterRange r2;

    CPPUNIT_ASSERT( r1 == r2 );
    CPPUNIT_ASSERT( r2 == r1 );

    r1.include( 20*GeV, 40*GeV );
    r2.include( 20*GeV, 40*GeV );

    CPPUNIT_ASSERT( r1 == r2 );
    CPPUNIT_ASSERT( r2 == r1 );

    CPPUNIT_ASSERT( r1.lower() == r2.lower() );
    CPPUNIT_ASSERT( r1.upper() == r2.upper() );

    FilterRange r3( r1 );
    CPPUNIT_ASSERT( r1 == r3 );
    CPPUNIT_ASSERT( r3 == r1 );
    CPPUNIT_ASSERT( r1.lower() == r3.lower() );
    CPPUNIT_ASSERT( r1.upper() == r3.upper() );

    FilterRange r4 = r2;
    CPPUNIT_ASSERT( r2 == r4 );
    CPPUNIT_ASSERT( r4 == r2 );
    CPPUNIT_ASSERT( r4.lower() == r2.lower() );
    CPPUNIT_ASSERT( r4.upper() == r2.upper() );

  }

  /// Test the comparison of ranges
  void testCompareRanges()
  {
    FilterRange r1;
    FilterRange r2;
    
    CPPUNIT_ASSERT( r1 == r1 );
    CPPUNIT_ASSERT( r1 == r2 );
    CPPUNIT_ASSERT( r2 == r1 );

    r1.include( 10*GeV, 20*GeV );
    r2.include( 10.*GeV, 20.*GeV );

    CPPUNIT_ASSERT( r1 == r1 );
    CPPUNIT_ASSERT( r1 == r2 );
    CPPUNIT_ASSERT( r2 == r1 );

    r1.include(  10*GeV,  20*GeV );
    r2.include( 100*GeV, 200*GeV );

    CPPUNIT_ASSERT( r2 == r2 );
    CPPUNIT_ASSERT( r1 != r2 );
    CPPUNIT_ASSERT( r2 != r1 );
    CPPUNIT_ASSERT( !(r1 == r2) );
    CPPUNIT_ASSERT( !(r2 == r1) );

    r1.include( 10*GeV, 20*GeV );
    r2.include( 15*GeV, 20*GeV );

    CPPUNIT_ASSERT( !(r1 == r2) );
    CPPUNIT_ASSERT( !(r2 == r1) );
    CPPUNIT_ASSERT( r1 != r2 );
    CPPUNIT_ASSERT( r2 != r1 );

    CPPUNIT_ASSERT( !(r1 < r2) );
    CPPUNIT_ASSERT( r2 >= r1 );

    r1.include( 10*GeV, 10*GeV );
    r2.include( 20*GeV, 20*GeV );
    CPPUNIT_ASSERT( r1 < r2 );

    r1.include( 10*GeV, 20*GeV );
    r2.include( 20*GeV, 25*GeV );
    CPPUNIT_ASSERT( !(r1 <  r2) );
    CPPUNIT_ASSERT(   r1 <= r2  );

  }

};

/// Registration of the test suite "FilterRangeTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( FilterRangeTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_testdriver.cxx>
