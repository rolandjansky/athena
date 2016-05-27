/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExStoreGateExample includes
#include "ViewIncidentMakeAlg.h"
//#include "AthViews/AthEventContext.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IScheduler.h"
//#include "GaudiKernel/IAlgResourcePool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "CxxUtils/make_unique.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewIncidentMakeAlg::ViewIncidentMakeAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthViewAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty("ViewStart",
                  m_w_int = SG::WriteHandle< int >("view_start"),
                  "A number to start off the view");

  declareProperty("AllViews",
                  m_w_views = SG::WriteHandle< std::vector< SG::View* > >("all_views"),
                  "All views");

  declareProperty("ViewNames",
                  m_viewNames = std::vector< std::string >(),
                  "View names");
}

// Destructor
///////////////
ViewIncidentMakeAlg::~ViewIncidentMakeAlg()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewIncidentMakeAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewIncidentMakeAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewIncidentMakeAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //Make a view for each name given
  m_w_views = CxxUtils::make_unique< std::vector< SG::View* > >();
  //m_w_views = std::vector< SG::View* >();
  for ( unsigned int viewIndex = 0; viewIndex < m_viewNames.size(); viewIndex++ )
  {
    std::string const viewName = m_viewNames[ viewIndex ];
    ATH_MSG_INFO( "Making new view: " << viewName );

    //Create the view object
    SG::View * newView = new SG::View( viewName );
    m_w_views->push_back( newView );

    //Write data to the new view
    CHECK( m_w_int.setStore( newView ) );
    m_w_int = CxxUtils::make_unique<int>( ( viewIndex * 10 ) + 10 + m_event_context->evt() );
    //m_w_int = int( ( viewIndex * 10 ) + 10 + m_event_context->evt() );

    //Try this out
    /*SmartIF< IAlgResourcePool > m_algResourcePool = 0;
    m_algResourcePool = serviceLocator()->service( "AlgResourcePool" );
    IAlgorithm * ialgoPtr = nullptr;
    StatusCode testPool = m_algResourcePool->acquireAlgorithm( name(), ialgoPtr );
    ATH_MSG_INFO( "This should fail because I'm me: " << testPool );*/

    SmartIF< IScheduler > m_scheduler = 0;
    m_scheduler = serviceLocator()->service( "ForwardSchedulerSvc" );

    //Test the HACK
    //AthEventContext * testNewContext = new AthEventContext( m_event_context );
    EventContext * testNewContext = new EventContext( *m_event_context );
    //testNewContext->setView( newView );
    testNewContext->setProxy( newView );
    ATH_MSG_INFO( "BEN scheduling dflow_algs in context " << testNewContext );
    StatusCode sc = m_scheduler->runAlgorithm( "dflow_alg1#dflow_alg2#dflow_alg3", testNewContext );
    ATH_MSG_INFO( "BEN scheduling result: " << sc );
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
