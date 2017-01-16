/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewSubgraphAlg.h"
#include "AthViews/ViewHelper.h"

// STL includes

// FrameWork includes
//#include "GaudiKernel/Property.h"

#include "CxxUtils/make_unique.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewSubgraphAlg::ViewSubgraphAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_w_views( "all_views" ),
  m_w_int( "view_start" ),
  m_w_allViewsDone( "all_views_done_dflow" ),
  m_viewNames( std::vector< std::string >() ),
  m_algPoolName( "" )
{
  //
  // Property declaration
  // 

  declareProperty( "ViewStart", m_w_int, "A number to start off the view" );
  
  declareProperty( "AllViewsDone", m_w_allViewsDone, "Data flow to indicate that all views have been completed" );

  declareProperty( "AllViews", m_w_views, "All views" );

  declareProperty( "ViewNames", m_viewNames, "View names" );

  declareProperty( "AlgPoolName", m_algPoolName, "Name for the algorithm pool service to use with the views" );
}

// Destructor
///////////////
ViewSubgraphAlg::~ViewSubgraphAlg()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewSubgraphAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewSubgraphAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewSubgraphAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //Create the container of views
  m_w_views.record( CxxUtils::make_unique< std::vector< SG::View* > >() );
  
  //Make a vector of dummy data to initialise the views
  std::vector<int> viewData;
  for ( unsigned int viewIndex = 0; viewIndex < m_viewNames.size(); viewIndex++ )
  {
    viewData.push_back( ( viewIndex * 10 ) + 10 + m_event_context->evt() );
  }

  //Create the views and populate them
  ViewHelper::MakeAndPopulate( m_viewNames[0],	//Base name for all views to use
		  *m_w_views,			//Vector to store views (within writehandle)
		  m_w_int,			//A writehandle to use to access the views (the handle itself, not the contents)
		  viewData );			//Data to initialise each view - one view will be made per entry

  //Specify algorithms to run in views (data flow info not available yet)
  std::vector< std::string > algorithmNameSequence = { "dflow_alg1", "dflow_alg2", "dflow_alg3" };

  ViewHelper::RunViews( *m_w_views,		//View vector
		  algorithmNameSequence,	//Algorithms to run in each view
		  *m_event_context,		//Context to attach the views to
		  serviceLocator()->service( m_algPoolName ) );	//Service to retrieve algorithms by name

  //Make a dummy output to organise data flow
  m_w_allViewsDone.record( CxxUtils::make_unique<int>( 1 ) );

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
