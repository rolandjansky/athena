/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the DeltaROverlapFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <list>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <numeric>

// to ease tests
#include <boost/bind.hpp>

// DataModel includes
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "CLIDSvc/CLASS_DEF.h"

// EventKernel includes
#include "EventKernel/INavigable4Momentum.h"

// FourMom includes
#include "FourMom/P4PxPyPzE.h"
#include "Navigation/NavigableTerminalNode.h" 

// FourMomUtils includes
#include "FourMomUtils/P4Helpers.h"

// AssociationUtils includes
#include "AssociationUtils/DeltaROverlapFilter.h"

class MyNav4Mom : virtual public INavigable4Momentum,
		  public P4PxPyPzE,
		  public NavigableTerminalNode
{
public:
  MyNav4Mom():
    INavigable4Momentum(),
    P4PxPyPzE(),
    NavigableTerminalNode()
  {}
  
  MyNav4Mom( double px, double py, double pz, double ene ):
    INavigable4Momentum(),
    P4PxPyPzE(px, py, pz, ene),
    NavigableTerminalNode()
  {}
};

CLASS_DEF( DataVector<MyNav4Mom> , 77574488 , 1 )

class DeltaROverlapFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite
  CPPUNIT_TEST_SUITE( DeltaROverlapFilterTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSettersAndGetters );
  CPPUNIT_TEST( testPredicate );

  CPPUNIT_TEST( testCountIf );
  CPPUNIT_TEST( testFindIf );
  CPPUNIT_TEST( testAccumulate );

  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:

  unsigned int m_nmax;
  DataVector<MyNav4Mom>   m_dvMoms;
  std::vector<MyNav4Mom*> m_stlMoms;

public:

  /// Set up the data members
  void setUp()
  {
    m_nmax = 11;
    const double x = 10.;

    m_dvMoms.resize(m_nmax);
    m_stlMoms.resize(m_nmax);
    for ( unsigned int i = 0; i != m_nmax; ++i ) {
      m_dvMoms[i]  = new MyNav4Mom( (i+1)*x, (i+2)*x, (i+3)*x, (i+4)*x );
      m_stlMoms[i] = new MyNav4Mom( (i+1)*x, (i+2)*x, (i+3)*x, (i+4)*x );
    }
  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
    m_dvMoms.clear();
    for ( unsigned int i = 0; i != m_nmax; ++i ) {
      delete m_stlMoms[i];
      m_stlMoms[i] = 0;
    }
  }

  /// Test the McVtxFilter constructors
  void testConstructor()
  {
    {
      DeltaROverlapFilter filter;
      CPPUNIT_ASSERT( filter.minDeltaR() == 0. );
      CPPUNIT_ASSERT( filter.maxDeltaR() == DBL_MAX );
    }

    {
      DeltaROverlapFilter filter( 0.7 );
      CPPUNIT_ASSERT( filter.minDeltaR() == 0.  );
      CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );
    }

    {
      DeltaROverlapFilter filter( 0.4, 0.7 );
      CPPUNIT_ASSERT( filter.minDeltaR() == 0.4 );
      CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );
    }

    {
      DeltaROverlapFilter filter( 0.4, 0.7 );
      CPPUNIT_ASSERT( filter.minDeltaR() == 0.4 );
      CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );

      DeltaROverlapFilter g(filter);
      CPPUNIT_ASSERT( g.minDeltaR() == 0.4 );
      CPPUNIT_ASSERT( g.maxDeltaR() == 0.7 );

      DeltaROverlapFilter h;
      CPPUNIT_ASSERT( h.minDeltaR() == 0. );
      CPPUNIT_ASSERT( h.maxDeltaR() == DBL_MAX );

      h = filter;
      CPPUNIT_ASSERT( h.minDeltaR() == 0.4 );
      CPPUNIT_ASSERT( h.maxDeltaR() == 0.7 );
    }
  }

  void testSettersAndGetters()
  {
    DeltaROverlapFilter filter( 0.4, 0.7 );
    CPPUNIT_ASSERT( filter.minDeltaR() == 0.4 );
    CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );
    
    filter.setMinDeltaR( 0.3 );
    CPPUNIT_ASSERT( filter.minDeltaR() == 0.3 );
    CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );
    
    filter.setMaxDeltaR( 0.8 );
    CPPUNIT_ASSERT( filter.minDeltaR() == 0.3 );
    CPPUNIT_ASSERT( filter.maxDeltaR() == 0.8 );
    
    filter.setDeltaR( 0.4, 0.7 );
    CPPUNIT_ASSERT( filter.minDeltaR() == 0.4 );
    CPPUNIT_ASSERT( filter.maxDeltaR() == 0.7 );
  }

  void testPredicate()
  {
    {
      DeltaROverlapFilter filter;
      MyNav4Mom mom1( 10., 10., 10., 20. );
      MyNav4Mom mom2( 10., 10., 10., 20. );
      CPPUNIT_ASSERT( P4Helpers::deltaR(mom1, mom2) == 0. );
      CPPUNIT_ASSERT( filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( filter(mom1, mom2) );
    }
    {
      DeltaROverlapFilter filter( 0.1, 0.2 );
      MyNav4Mom mom1( 10., 10., 10., 20. );
      MyNav4Mom mom2( 10., 10., 10., 20. );
      CPPUNIT_ASSERT( P4Helpers::deltaR(mom1, mom2) == 0. );
      CPPUNIT_ASSERT( !filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( !filter(mom1, mom2) );
    }
    {
      typedef ElementLink<DataVector<MyNav4Mom> > Link_t;
      std::vector<Link_t> moms;
      moms.push_back( Link_t( m_dvMoms, 0 ) );
      DeltaROverlapFilter filter( 0.1, 0.2 );
      CPPUNIT_ASSERT( P4Helpers::deltaR( *moms[0], *moms[0] ) == 0. );
      CPPUNIT_ASSERT( !filter.hasOverlap(**moms[0],**moms[0]) );
      CPPUNIT_ASSERT( !filter.hasOverlap(*moms[0],*moms[0]) );
      CPPUNIT_ASSERT( !filter( *moms[0], *moms[0] ) );
    }
    {
      MyNav4Mom mom1( -10., -10., -10., 20. );
      MyNav4Mom mom2(  10.,  10.,  10., 20. );
      const double minDeltaR = 3.40646;
      const double maxDeltaR = 3.40647;
      CPPUNIT_ASSERT( P4Helpers::deltaR(mom1, mom2) <  maxDeltaR );
      CPPUNIT_ASSERT( P4Helpers::deltaR(mom1, mom2) >= minDeltaR );

      DeltaROverlapFilter filter;

      CPPUNIT_ASSERT( filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( filter(mom1, mom2) );

      filter.setDeltaR( 0., minDeltaR );
      CPPUNIT_ASSERT( !filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( !filter(mom1, mom2) );

      filter.setDeltaR( minDeltaR, maxDeltaR );
      CPPUNIT_ASSERT( filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( filter(mom1, mom2) );

      filter.setDeltaR( maxDeltaR, DBL_MAX );
      CPPUNIT_ASSERT( !filter.hasOverlap(mom1, mom2) );
      CPPUNIT_ASSERT( !filter(mom1, mom2) );
    }
  }

  template <class CONT>
  void testCountIf_t( const CONT& cont, const DeltaROverlapFilter& filter,
		      const int INRANGE ) 
  {
    const int OUTRANGE = static_cast<int>(cont.size()) - INRANGE;
    {
      const int n = std::count_if( cont.begin(), cont.end(), 
				   std::bind2nd( filter, cont[0] ) );
      CPPUNIT_ASSERT( n == (INRANGE+1) );
    }

    {
      const int n = std::count_if( cont.begin(), cont.end(), 
				   boost::bind( filter, _1, cont[0] ) );
      CPPUNIT_ASSERT( n == (INRANGE+1) );
    }

    {
      const int n = std::count_if( cont.begin(), cont.end(), 
				   boost::bind( filter, cont[0], _1 ) );
      CPPUNIT_ASSERT( n == (INRANGE+1) );
    }

    {
      const int n = std::count_if( cont.begin(), cont.end(), 
				   boost::bind( std::logical_not<bool>(),
						boost::bind( filter, 
							     cont[0], _1 ) ) );
      CPPUNIT_ASSERT( n == (OUTRANGE-1) );
    }
  }

  void testCountIf()
  {
    const int INRANGE = 6;
    DeltaROverlapFilter filter( 0., P4Helpers::deltaR( m_stlMoms[0],
						       m_stlMoms[INRANGE] ) );

//     std::cout << std::endl;
//     for ( unsigned int i = 0; i != m_nmax; ++i ) {
//       std::cout << i << " => " << P4Helpers::deltaR(m_stlMoms[0], m_stlMoms[i])
// 		<< std::endl;
//     }

    testCountIf_t( m_dvMoms,  filter, INRANGE );
    testCountIf_t( m_stlMoms, filter, INRANGE );
  }

  template <class CONT>
  void testFindIf_t( const CONT& cont, const DeltaROverlapFilter& filter,
		     const int INRANGE ) 
  {
    const int OUTRANGE = static_cast<int>(cont.size()) - INRANGE;
    {
      typename CONT::const_iterator itr;
      itr = std::find_if( cont.begin(), cont.end(), 
			  std::bind2nd( filter, cont[0] ) );
      CPPUNIT_ASSERT( itr == cont.begin() );
    }

    {
      typename CONT::const_iterator itr;
      itr = std::find_if( cont.begin(), cont.end(), 
			  boost::bind( filter, _1, cont[0] ) );
      CPPUNIT_ASSERT( itr == cont.begin() );
    }

    {
      typename CONT::const_iterator itr;
      itr = std::find_if( cont.begin(), cont.end(), 
			  boost::bind( filter, cont[0], _1 ) );
      CPPUNIT_ASSERT( itr == cont.begin() );
    }

    {
      typename CONT::const_iterator itr;
      itr = std::find_if( cont.begin(), cont.end(), 
			   boost::bind( std::logical_not<bool>(),
					boost::bind( filter, 
						     cont[0], _1 ) ) );
      CPPUNIT_ASSERT( itr == (cont.begin()+INRANGE+1) );
    }

    {
      typename CONT::const_reverse_iterator ritr;
      ritr = std::find_if( cont.rbegin(), cont.rend(), 
			   boost::bind( filter, cont[0], _1 ) );
      CPPUNIT_ASSERT( ritr == (cont.rbegin()+OUTRANGE-1) );
    }

    {
      typename CONT::const_reverse_iterator ritr;
      ritr = std::find_if( cont.rbegin(), cont.rend(), 
			   boost::bind( std::logical_not<bool>(),
					boost::bind( filter, 
						     cont[0], _1 ) ) );
      CPPUNIT_ASSERT( ritr == cont.rbegin() );
    }
  }

  void testFindIf()
  {
    const int INRANGE = 6;
    DeltaROverlapFilter filter( 0., P4Helpers::deltaR( m_stlMoms[0],
						       m_stlMoms[INRANGE] ) );
    testFindIf_t( m_dvMoms,  filter, INRANGE );
    testFindIf_t( m_stlMoms, filter, INRANGE );
  }

  template <class CONT>
  void testAccumulate_t( const CONT& cont, const DeltaROverlapFilter& filter,
			 const int INRANGE ) 
  {
    {
      bool allOk = true;
      allOk = std::accumulate( cont.begin(), 
			       cont.begin()+INRANGE, 
			       allOk,
			       boost::bind( std::logical_and<bool>(),
					    _1,
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

    {
      bool allOk = true;
      allOk = std::accumulate( cont.rbegin()+INRANGE, 
			       cont.rend(), 
			       allOk,
			       boost::bind( std::logical_and<bool>(),
					    _1,
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

    {
      bool allOk = true;
      allOk = std::accumulate( cont.begin(), 
			       cont.begin()+INRANGE+1, 
			       allOk,
			       boost::bind( std::logical_and<bool>(),
					    _1, // allOk_sum
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

    {
      bool allOk = false;
      allOk = std::accumulate( cont.begin(), 
			       cont.end(), 
			       allOk,
			       boost::bind( std::logical_or<bool>(),
					    _1, // allOk_sum
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

    {
      bool allOk = false;
      allOk = std::accumulate( cont.begin()+INRANGE, 
			       cont.end(), 
			       allOk,
			       boost::bind( std::logical_or<bool>(),
					    _1, // allOk_sum
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

    {
      bool allOk = false;
      allOk = std::accumulate( cont.rbegin(),
			       cont.rend()-INRANGE, 
			       allOk,
			       boost::bind( std::logical_or<bool>(),
					    _1, // allOk_sum
					    boost::bind( filter, 
							 cont[0], _2 ) ) );

      CPPUNIT_ASSERT( allOk );
    }

  }

  void testAccumulate()
  {
    const int INRANGE = 6;
    DeltaROverlapFilter filter( 0., P4Helpers::deltaR( m_stlMoms[0],
						       m_stlMoms[INRANGE] ) );

    testAccumulate_t( m_dvMoms,  filter, INRANGE );
    testAccumulate_t( m_stlMoms, filter, INRANGE );
  }

};

/// Registration of the test suite "DeltaROverlapFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( DeltaROverlapFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_SGtestdriver.cxx>
