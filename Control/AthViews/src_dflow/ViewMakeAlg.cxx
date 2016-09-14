/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExStoreGateExample includes
#include "ViewMakeAlg.h"

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
ViewMakeAlg::ViewMakeAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_w_views( "all_views" ),
  m_w_int( "view_start" ),
  m_viewNames( std::vector< std::string >() )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "ViewStart", m_w_int, "A number to start off the view" );

  declareProperty( "AllViews", m_w_views, "All views" );

  declareProperty( "ViewNames", m_viewNames, "View names" );
}

// Destructor
///////////////
ViewMakeAlg::~ViewMakeAlg()
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewMakeAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewMakeAlg::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewMakeAlg::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  //Make a view for each name given
  m_w_views.record( CxxUtils::make_unique< std::vector< SG::View* > >() );
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
