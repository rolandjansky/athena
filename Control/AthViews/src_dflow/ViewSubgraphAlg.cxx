/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewSubgraphAlg.h"
#include "AthViews/ViewHelper.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewSubgraphAlg::ViewSubgraphAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )//,
  //m_w_int( "view_start" ) // needed since this one isn't declared fully in the header
{
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

  renounce( m_w_int ); // To test ViewDataVerifier
  CHECK( m_w_int.initialize() );
  CHECK( m_w_views.initialize() );
  CHECK( m_scheduler.retrieve() );

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

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif
  
  //Make a vector of dummy data to initialise the views
  std::vector<int> viewData;
  std::vector< SG::View* > viewVector;
  for ( int viewIndex = 0; viewIndex < m_viewNumber; ++viewIndex )
  {
    viewData.push_back( ( viewIndex * 10 ) + 10 + ctx.evt() );
  }

  //Create the views and populate them
  CHECK( ViewHelper::MakeAndPopulate( m_viewBaseName,   //Base name for all views to use
					viewVector,     //Vector to store views
					m_w_int,        //A writehandlekey to use to access the views
                                        ctx,            //The context of this algorithm
					viewData ) );   //Data to initialise each view - one view will be made per entry

  //Schedule the algorithms in views
  CHECK( ViewHelper::ScheduleViews( viewVector, //View vector
        m_viewNodeName,                         //Name of node to attach views to
        ctx,                                    //Context to attach the views to
        m_scheduler.get() ) );                  //ServiceHandle for the scheduler

  //Store the collection of views
  SG::WriteHandle< std::vector< SG::View* > > outputViewHandle( m_w_views, ctx );
  outputViewHandle.record( CxxUtils::make_unique< std::vector< SG::View* > >( viewVector ) );

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
