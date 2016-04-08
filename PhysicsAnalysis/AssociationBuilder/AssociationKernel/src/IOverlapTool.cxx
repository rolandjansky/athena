/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IOverlapTool.cxx 
// Implementation file for class IOverlapTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes

// AssociationKernel includes
#include "AssociationKernel/IOverlapTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

/// Destructor
///////////////
IOverlapTool::~IOverlapTool()
{}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

bool 
IOverlapTool::overlap( const INavigable4Momentum& obj1,
		       const INavigable4MomentumCollection& objectsToCheck,
		       std::list<const INavigable4Momentum*>* overlappingObjects ) const
{
  bool hasOverlap = false;
  INavigable4MomentumCollection::const_iterator itrEnd = objectsToCheck.end();
  for ( INavigable4MomentumCollection::const_iterator itr = objectsToCheck.begin();
	itr != itrEnd;
	++itr ) {
    const INavigable4Momentum* obj2 = *itr;
    if ( overlap( obj1, *obj2, overlappingObjects ) ) {
      hasOverlap = true;
    }
  }
  return hasOverlap;
}

bool 
IOverlapTool::overlap( const INavigable4Momentum& obj1,
		       const INavigable4MomentumCollection::const_iterator& firstObjectToCheck,
		       const INavigable4MomentumCollection::const_iterator&  lastObjectToCheck,
		       std::list<const INavigable4Momentum*>* overlappingObjects ) const
{
  bool hasOverlap = false;
  for ( INavigable4MomentumCollection::const_iterator itr = firstObjectToCheck;
	itr != lastObjectToCheck;
	++itr ) {
    const INavigable4Momentum* obj2 = *itr;
    if ( overlap( obj1, *obj2, overlappingObjects ) ) {
      hasOverlap = true;
    }
  }
  return hasOverlap;
}


/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

