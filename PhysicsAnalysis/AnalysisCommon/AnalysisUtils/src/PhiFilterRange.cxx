/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PhiFilterRange.cxx 
// Implementation file for class PhiFilterRange
// Author: S.Binet <binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cmath>

// Boost includes
#include <boost/numeric/interval/compare/explicit.hpp>

// AnalysisUtils includes
#include "AnalysisUtils/PhiFilterRange.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Copy constructor
PhiFilterRange::PhiFilterRange( const PhiFilterRange& range ) :
  FilterRange( range )
{}

/// Assignment operator
PhiFilterRange & PhiFilterRange::operator = ( const PhiFilterRange& range )
{
  if ( this != &range ) {
    FilterRange::operator=(range);
  }

  return *this;
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////
bool PhiFilterRange::isInRange( const double point ) const
{ 
  // check if point is in range
  return FilterRange::isInRange( atlasPhi(point) ); 
}

double PhiFilterRange::atlasPhi( const double phi ) const
{
  if ( M_PI == std::fabs(phi) ) {
    return phi;
  }
  if ( FLT_MAX != std::fabs(phi) ) {
    return std::fmod( phi + 3*M_PI, 2*M_PI ) - M_PI;
  } else {
    if ( phi > 0. ) {
      return M_PI;
    } else {
      return -M_PI;
    }
  }
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void PhiFilterRange::include(double xMin, double xMax)
{ 
  /// Reset the whole range : this is temporary, 
  /// just to mimic FML::RangeSet API
  addRange(xMin,xMax); 
}

void PhiFilterRange::includeAll() 
{ 
  m_range = boost::none;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
void PhiFilterRange::addRange( const double xMin, const double xMax) 
{ 
  /// Special case where one adds a no-op interval
  if ( (xMin == -M_PI    && xMax == +M_PI    ) ||
       (xMin == -FLT_MAX && xMax == +FLT_MAX ) ) {
    m_range = boost::none;
    return;
  }

  /// Take care of inversion of bounds because of the moduli
  const double phiMin = atlasPhi( xMin );
  const double phiMax = atlasPhi( xMax );
  
  if ( phiMin < phiMax ) {
    m_range = interval_t( phiMin, phiMax );
  } else {
    m_range = interval_t( phiMax, phiMin );
  }
}

/////////////////////////////////////////////////////////////////// 
// Operators
/////////////////////////////////////////////////////////////////// 
bool operator==( const PhiFilterRange& r1, const PhiFilterRange& r2 )
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
  return ( (std::fabs(r1.lower() - r2.lower()) <= precision ) && 
	   (std::fabs(r1.upper() - r2.upper()) <= precision ) );
}

bool operator!=( const PhiFilterRange& r1, const PhiFilterRange& r2 )
{
  return !( operator==(r1,r2));
}

bool operator<( const PhiFilterRange& r1, const PhiFilterRange& r2 )
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

bool operator>=( const PhiFilterRange& r1, const PhiFilterRange& r2 )
{
  return !( operator<(r1,r2) );
}

bool operator>( const PhiFilterRange& r1, const PhiFilterRange& r2 )
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

bool operator<=( const PhiFilterRange& r1, const PhiFilterRange& r2 )
{
  return !( operator>(r1,r2) );
}


