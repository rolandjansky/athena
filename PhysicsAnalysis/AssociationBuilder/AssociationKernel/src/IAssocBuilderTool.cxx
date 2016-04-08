/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// IAssocBuilderTool.cxx 
// Implementation file for class IAssocBuilderTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Framework includes

// AssociationKernel includes
#include "AssociationKernel/IAssocBuilderTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////

/// Destructor
///////////////
IAssocBuilderTool::~IAssocBuilderTool()
{}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode 
IAssocBuilderTool::buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
				const std::vector<const INavigable4MomentumCollection*>& objectsToAssociate,
				INav4MomAssocs& mapOfAssociations ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  const std::vector<const INavigable4MomentumCollection*>::const_iterator lastObj = objectsToAssociate.end();
  for ( std::vector<const INavigable4MomentumCollection*>::const_iterator itr = objectsToAssociate.begin();
	itr != lastObj;
	++itr ) {
    const INavigable4MomentumCollection * coll = *itr;
    if ( 0 == coll ) {
      continue;
    }
    if ( !buildAssocs( obj1, *coll, mapOfAssociations ).isSuccess() ) {
      sc = StatusCode::FAILURE;
    }
  }

  return sc;
}


StatusCode 
IAssocBuilderTool::buildAssocs( const ElementLink<INavigable4MomentumCollection>& obj1,
				const std::vector<const INavigable4MomentumCollection*>::const_iterator&  firstObjectToAssociate,
				const std::vector<const INavigable4MomentumCollection*>::const_iterator&   lastObjectToAssociate,
				INav4MomAssocs& mapOfAssociations ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  for ( std::vector<const INavigable4MomentumCollection*>::const_iterator itr = firstObjectToAssociate;
	itr != lastObjectToAssociate;
	++itr ) {
    const INavigable4MomentumCollection * coll = *itr;
    if ( 0 == coll ) {
      continue;
    }
    if ( !buildAssocs( obj1, *coll, mapOfAssociations ).isSuccess() ) {
      sc = StatusCode::FAILURE;
    }
  }

  return sc;
}


/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

