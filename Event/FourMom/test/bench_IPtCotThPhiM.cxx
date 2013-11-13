/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

// Boost includes
#include <boost/timer.hpp>

#include "FourMom/P4IPtCotThPhiM.h"
#include "FourMom/P4Impl.h"
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

const double milliseconds = 1e3;

template<class I4Mom_t>
double testEmptyLoop( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    // do nothing
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
double testConstructor( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    I4Mom_t p( 10./CLHEP::GeV, 1., M_PI, 10.*CLHEP::GeV );
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
double testNativeGetter( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    I4Mom_t p( 10./CLHEP::GeV, 1., M_PI, 10.*CLHEP::GeV );
    p.iPt();
    p.cotTh();
    p.phi();
    p.m();
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
double testGetter( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    I4Mom_t p( 10./CLHEP::GeV, 1., M_PI, 10.*CLHEP::GeV );
    p.px();
    p.py();
    p.pz();
    p.et();
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
double testHlv( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    I4Mom_t p( 10./CLHEP::GeV, 1., M_PI, 10.*CLHEP::GeV );
    p.hlv();
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
double testHlvOp( const unsigned int iMax )
{
  boost::timer chrono;
  for ( unsigned int i = 0; i != iMax; ++i ) {
    I4Mom_t p( 10./CLHEP::GeV, 1., M_PI, 10.*CLHEP::GeV );
    I4Mom_t p4( p.hlv() + p.hlv() );
  }
  return chrono.elapsed() * milliseconds;
}

template<class I4Mom_t>
void test( const unsigned int iMax, const std::string& name )
{

  typedef std::vector< std::pair<std::string, double> > BenchTable;
  BenchTable bench;

  bench.push_back( BenchTable::value_type( "Empty Loop",
					   testEmptyLoop<I4Mom_t>(iMax) ) );
  bench.push_back( BenchTable::value_type( "Constructor",
					   testConstructor<I4Mom_t>(iMax) ) );
  bench.push_back( BenchTable::value_type( "Native Getters",
					   testNativeGetter<I4Mom_t>(iMax) ) );
  bench.push_back( BenchTable::value_type( "Virtual Getters",
					   testGetter<I4Mom_t>(iMax) ) );
  bench.push_back( BenchTable::value_type( "Get HLV",
					   testHlv<I4Mom_t>(iMax) ) );
  bench.push_back( BenchTable::value_type( "Construct from HLV",
					   testHlvOp<I4Mom_t>(iMax) ) );

  std::stringstream className;
  className << "size(" << name << ")"; 
  std::cout << "##" << std::endl
	    << "##"
	    << std::setw(30) << className.str() 
	    << ": "
	    << std::setw(10) << sizeof(I4Mom_t) 
	    << std::endl
	    << "### Results (" << name << ") ###" 
	    << std::endl;
  for ( BenchTable::const_iterator itr = bench.begin();
	itr != bench.end();
	++itr ) {
    std::cout << "##" 
	      << std::setw(30) << itr->first
	      << ": "
	      << std::setw(10)
	      << itr->second << " ms"
	      << std::endl;
  }
}

int main()
{
  std::cout << "## Benchmark [IPtCotThPhiM]..." << std::endl;

  const unsigned int iMax = 10000000;
  std::cout << "## Configured to loop #" << iMax << " times..." << std::endl;

  test<P4ImplIPtCotThPhiM>         ( iMax, "P4ImplIPtCotThPhiM" );
  test<P4Impl<P4ImplIPtCotThPhiM> >( iMax, "P4Impl<IPtCotThPhiM>" );
  test<P4IPtCotThPhiM>             ( iMax, "P4IPtCotThPhiM" );

  return EXIT_SUCCESS;
}
