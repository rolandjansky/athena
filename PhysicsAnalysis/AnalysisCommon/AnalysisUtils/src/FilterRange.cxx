/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// FilterRange.cxx 
// Implementation file for class FilterRange
// Author: S.Binet <binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cmath>

// Boost includes
#include <boost/numeric/interval/compare/explicit.hpp>

// AnalysisUtils includes
#include "AnalysisUtils/FilterRange.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Copy constructor
FilterRange::FilterRange( const FilterRange& range ) :
  m_range( range.m_range ),
  m_precision( range.m_precision )
{}

/// Assignment operator
FilterRange & FilterRange::operator = ( const FilterRange& range )
{
  if ( this != &range ) {
    m_range = interval_t( range.lower(), range.upper() );
    m_precision = range.m_precision;
  }

  return *this;
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
bool FilterRange::isInRange( const double point ) const
{ 
  // check if point is in range
  //if ( boost::numeric::in( point, *m_range ) ) return true;
  if ( ( ( lower() - point ) <= +m_precision ) &&
       ( ( upper() - point ) >= -m_precision ) ) return true;
  return false; 
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void FilterRange::setRange( const double lower, const double upper )
{
  /// Special case where one adds a no-op interval
  if ( lower == MINUS_INF() && upper == PLUS_INF() ) {
    m_range = boost::none;
    return;
  }

  /// Now handle the normal cases
  m_range = interval_t( lower, upper );
}

void FilterRange::include(double xMin, double xMax)
{ 
  /// Reset the whole range : this is temporary, 
  /// just to mimic FML::RangeSet API
  addRange(xMin,xMax); 
}

void FilterRange::includeAll() 
{ 
  m_range = boost::none;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
void FilterRange::addRange( const double xMin, const double xMax) 
{ 
  /// Special case where one adds a no-op interval
  if ( xMin == MINUS_INF() && xMax == PLUS_INF() ) {
    m_range = boost::none;
    return;
  }

  m_range = interval_t( xMin, xMax );
}

/////////////////////////////////////////////////////////////////// 
// Operators
/////////////////////////////////////////////////////////////////// 
bool operator==( const FilterRange& r1, const FilterRange& r2 )
{
  if ( r1.isActive() != r2.isActive() ) {
    return false;
  }

  if ( false == r1.isActive() && 
       false == r2.isActive() ) {
    return true;
  }
  const double precision = std::min( r1.precision(), r2.precision() );

  //return boost::numeric::interval_lib::poseq( *m_range, *(rhs.m_range) );
  return ( ( std::fabs(r1.lower() - r2.lower()) <= precision ) && 
	   ( std::fabs(r1.upper() - r2.upper()) <= precision ) );
}

bool operator!=( const FilterRange& r1, const FilterRange& r2 )
{
  return !( operator==(r1,r2));
}

bool operator<( const FilterRange& r1, const FilterRange& r2 )
{
  if ( r1.isActive() != r2.isActive() ) {
    /// !isActive <==> range= [-inf, +inf]
    if ( r2.isActive() ) {
      // case r1 = [-inf, +inf] and r2 = [a,b]
      return false;
    } else {
      // case r1 = [a, b] and r2 = [-inf, +inf]
      return true;
    }
  }

  if ( false == r1.isActive() && 
       false == r2.isActive() ) {
    return false;
  }

  return ( r1.upper() < r2.lower() );
}

bool operator>=( const FilterRange& r1, const FilterRange& r2 )
{
  return !( operator<(r1,r2) );
}

bool operator>( const FilterRange& r1, const FilterRange& r2 )
{
  if ( r1.isActive() != r2.isActive() ) {
    /// !isActive <==> range= [-inf, +inf]
    if ( r2.isActive() ) {
      // case r1 = [-inf, +inf] and r2 = [a,b]
      return true;
    } else {
      // case r1 = [a, b] and r2 = [-inf, +inf]
      return false;
    }
  }

  if ( false == r1.isActive() && 
       false == r2.isActive() ) {
    return false;
  }

  return ( r1.lower() > r2.upper() );
}

bool operator<=( const FilterRange& r1, const FilterRange& r2 )
{
  return !( operator>(r1,r2) );
}


