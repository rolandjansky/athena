/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// DeltaROverlapTool.cxx 
// Implementation file for class DeltaROverlapTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes

// AssociationComps includes
#include "DeltaROverlapTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
DeltaROverlapTool::DeltaROverlapTool( const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent ) : 
  OverlapBaseTool( type, name, parent )
{
  //
  // Property declaration
  // 

  declareProperty( "MinDeltaR", m_minDeltaR = 0., 
		   "Minimum value for DeltaR. This will be used to configure "
		   "the DeltaROverlapFilter predicate." );
  m_minDeltaR.declareUpdateHandler( &DeltaROverlapTool::setupDeltaR,
				    this );

  declareProperty( "MaxDeltaR", m_maxDeltaR = 0.02, 
		   "Maximum value for DeltaR. This will be used to configure "
		   "the DeltaROverlapFilter predicate." );
  m_maxDeltaR.declareUpdateHandler( &DeltaROverlapTool::setupDeltaR,
				    this );

}

// Destructor
///////////////
DeltaROverlapTool::~DeltaROverlapTool()
{}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

bool DeltaROverlapTool::overlap( const INavigable4Momentum& obj1, 
				 const INavigable4Momentum& obj2, 
				 std::list<const INavigable4Momentum*>* overlappingObjects ) const
{
  if ( m_filter.hasOverlap( obj1, obj2 ) ) {
    if ( 0 != overlappingObjects ) {
      overlappingObjects->push_back( &obj2 );
    }
    return true;
  } else {
    return false;
  }
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode DeltaROverlapTool::initializeTool()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_MSG_DEBUG ("configuring overlap predicates...");

  setupDeltaR( m_maxDeltaR );

  ATH_MSG_DEBUG ("Overlap predicates [OK]");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


