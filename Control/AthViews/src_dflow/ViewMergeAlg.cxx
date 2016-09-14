/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExStoreGateExample includes
#include "ViewMergeAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "CxxUtils/make_unique.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewMergeAlg::ViewMergeAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_w_ints( "dflow_ints" ),
  m_r_ints( "dflow_ints" ),
  m_r_views( "all_views" )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "IntsFromViews", m_r_ints, "Data flow of ints" );

  declareProperty( "MergedInts", m_w_ints, "Data flow of ints" );

  declareProperty( "AllViews", m_r_views, "All views" );
}

// Destructor
///////////////
ViewMergeAlg::~ViewMergeAlg()
{
	m_r_ints.reset();
	m_w_ints.reset();
	m_r_views.reset();
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewMergeAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewMergeAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewMergeAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //Loop over all views
  std::vector< int > outputVector;
  for ( SG::View * view : *( m_r_views ) )
  {
	  ATH_MSG_INFO( "Merging view: " << view->name() );

	  //Retrieve the view data
	  StatusCode sc = m_r_ints.setProxyDict( view );
	  if ( !sc.isSuccess() )
	  {
		  ATH_MSG_ERROR( "Unable to load view " << view->name() );
		  continue;
	  }

	  //Merge the view data
	  if ( m_r_ints.isValid() )
	  {
		  outputVector.insert( outputVector.end(), m_r_ints->begin(), m_r_ints->end() );
	  }
  }

  //Output the merged data
  if ( !m_w_ints.setProxyDict( 0 ).isSuccess() )
  {
	  ATH_MSG_INFO( "Unable to load main event store" );
  }
  m_w_ints = CxxUtils::make_unique< std::vector< int > >( outputVector );
  for ( int const test : outputVector )
  {
	  ATH_MSG_INFO( test );
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

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

} //> end namespace AthViews
