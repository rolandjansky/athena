/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// DeltaROverlapFilter.cxx 
// Implementation file for class DeltaROverlapFilter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cfloat>

// FourMomUtils includes
#include "FourMomUtils/P4Helpers.h"

// AssociationUtils includes
#include "AssociationUtils/DeltaROverlapFilter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

DeltaROverlapFilter::DeltaROverlapFilter() :
  IOverlapFilter( ),
  m_minDeltaR ( 0. ),
  m_maxDeltaR ( DBL_MAX )
{}

DeltaROverlapFilter::DeltaROverlapFilter( const DeltaROverlapFilter& rhs ) :
  IOverlapFilter( rhs ),
  m_minDeltaR( rhs.m_minDeltaR ),
  m_maxDeltaR( rhs.m_maxDeltaR )
{}

DeltaROverlapFilter& 
DeltaROverlapFilter::operator=( const DeltaROverlapFilter& rhs )
{
  if ( this != &rhs ) {
    IOverlapFilter::operator=( rhs );
    m_minDeltaR = rhs.m_minDeltaR;
    m_maxDeltaR = rhs.m_maxDeltaR;
  }
  return *this;
}

DeltaROverlapFilter::DeltaROverlapFilter( double maxDeltaR ) :
  IOverlapFilter( ),
  m_minDeltaR(        0. ),
  m_maxDeltaR( maxDeltaR )
{}

DeltaROverlapFilter::DeltaROverlapFilter( double minDeltaR, 
					  double maxDeltaR ) :
  IOverlapFilter( ),
  m_minDeltaR( minDeltaR ),
  m_maxDeltaR( maxDeltaR )
{}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

bool DeltaROverlapFilter::hasOverlap( const INavigable4Momentum& obj1,
				      const INavigable4Momentum& obj2 ) const
{
  const double deltaR = P4Helpers::deltaR( obj1, obj2 );
  return ( m_minDeltaR <= deltaR ) && ( deltaR < m_maxDeltaR );
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

