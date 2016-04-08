/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// AssocBuilderTool.cxx 
// Implementation file for class AssocBuilderTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes

// AssociationComps includes
#include "AssocBuilderTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AssocBuilderTool::AssocBuilderTool( const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent ) : 
  AthAlgTool    ( type, name,   parent ),
  m_overlapTools( this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "OverlapTools", 
		   m_overlapTools, 
		   "Array IOverlapTools one wants to run to decide if "
		   "2 objects are overlapping and hence to be associated" );

  // default values:
  m_overlapTools.push_back( "DeltaROverlapTool/DeltaROverlapTool" );
  declareInterface<IAssocBuilderTool>(this);
}

// Destructor
///////////////
AssocBuilderTool::~AssocBuilderTool()
{ 
  ATH_MSG_DEBUG ( "Calling destructor" );
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode AssocBuilderTool::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() << "..."  );
  ATH_CHECK( m_overlapTools.retrieve() );
  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
StatusCode 
AssocBuilderTool::buildAssocs( const ElementLink<INavigable4MomentumCollection>& objLink,
			       const INavigable4MomentumCollection& objectsToAssociate,
			       INav4MomAssocs& mapOfAssociations ) const
{
  if ( !objLink.isValid() ) {
    ATH_MSG_WARNING( "InValid ElementLink<INav4MomColl> !!" );
    return StatusCode::RECOVERABLE;
  }
  typedef IOverlapTools_t::const_iterator CstIter_t;
  typedef std::list<const INavigable4Momentum*>::const_iterator ListIter_t;
  typedef ElementLink<INavigable4MomentumCollection> AssoLink_t;

  for ( CstIter_t itr = m_overlapTools.begin(), itrEnd = m_overlapTools.end();
	itr != itrEnd;
	++itr ) {
    std::list<const INavigable4Momentum*> overlaps;
    const INavigable4Momentum * obj = *objLink;
    if ( obj && (*itr)->overlap( *obj, objectsToAssociate, &overlaps ) ) {
      for ( ListIter_t overlap = overlaps.begin(), overlapEnd = overlaps.end();
	    overlap != overlapEnd;
	    ++overlap ) {
	AssoLink_t assoLink( *overlap, objectsToAssociate );

	// protection against association to itself:
	if ( *objLink == *assoLink ) {
	  continue;
	}

	mapOfAssociations.addAssociation( objLink.getDataPtr(),  objLink.index(),
					  assoLink.getDataPtr(), assoLink.index() );
      }
    }
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

