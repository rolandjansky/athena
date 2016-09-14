/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExStoreGateExample includes
#include "ViewSubgraphAlg.h"
#include "GaudiKernel/EventContext.h"
#include "AthViews/GraphExecutionTask.h"

#include "tbb/task.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IScheduler.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

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
  //declareProperty( "Property", m_nProperty );

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

  //Subgraph a view for each name given
  m_w_views.record( CxxUtils::make_unique< std::vector< SG::View* > >() );
  tbb::task_list allTasks;
  for ( unsigned int viewIndex = 0; viewIndex < m_viewNames.size(); viewIndex++ )
  {
    std::string const viewName = m_viewNames[ viewIndex ];
    ATH_MSG_INFO( "Making new view: " << viewName );

    //Create the view object
    SG::View * newView = new SG::View( viewName );
    m_w_views->push_back( newView );

    //Write data to the new view
    StatusCode sc = m_w_int.setProxyDict( newView );
    if ( !sc.isSuccess() ) ATH_MSG_ERROR( "setProxyDict() failed for new view" );
    m_w_int.record( CxxUtils::make_unique<int>( ( viewIndex * 10 ) + 10 + m_event_context->evt() ) );

    //Make a context with the view attached
    EventContext * viewContext = new EventContext( *m_event_context );
    viewContext->setProxy( newView );

    //Make a subgraph
    std::vector< std::string > algorithmNameSequence = { "dflow_alg1", "dflow_alg2", "dflow_alg3" };
    tbb::task * t = new( tbb::task::allocate_root() )GraphExecutionTask( algorithmNameSequence, viewContext, serviceLocator()->service( m_algPoolName ) );
    allTasks.push_back( *t );
  }
  tbb::task::spawn_root_and_wait( allTasks );

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
