/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// OverlapBaseTool.cxx 
// Implementation file for class OverlapBaseTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AssociationComps includes
#include "OverlapBaseTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
OverlapBaseTool::OverlapBaseTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
  AthAlgTool     ( type, name, parent ),
  m_overlapTools ( this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "OverlapTools", 
		   m_overlapTools,
		   "Array of overlap tools which will be interrogated "
		   "to decide if 2 objects are overlapping. This is a "
		  "logical AND." );

  // default values
  //m_overlapTools.push_back( "" ); <== no sensible default

  declareInterface<IOverlapTool>(this);
}

// Destructor
///////////////
OverlapBaseTool::~OverlapBaseTool()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode OverlapBaseTool::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // setup the IOverlapTools
  if ( !m_overlapTools.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve array of tools [" << m_overlapTools << "] !!");
    return StatusCode::FAILURE;
  }

  if ( !initializeTool().isSuccess() ) {
    ATH_MSG_ERROR ("Could not initialize concrete Tool !!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode OverlapBaseTool::append( IOverlapTool* tool )
{
  if ( 0 == tool ) {
    return StatusCode::RECOVERABLE;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode OverlapBaseTool::initializeTool()
{
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

