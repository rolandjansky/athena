/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ViewSubgraphAlg.h"
#include "AthViews/ViewHelper.h"
#include "Gaudi/Property.h"
#include "StoreGate/WriteHandle.h"

namespace AthViews {

ViewSubgraphAlg::ViewSubgraphAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator )
{
}

ViewSubgraphAlg::~ViewSubgraphAlg()
{
}

StatusCode ViewSubgraphAlg::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  renounce( m_w_int ); // To test ViewDataVerifier
  CHECK( m_w_int.initialize() );
  CHECK( m_w_views.initialize() );
  if ( m_r_views.key() != "" ) CHECK( m_r_views.initialize() );
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

  const EventContext& ctx = getContext();

  //Start scheduler profiling
  auto profileCallback = [this, slot=ctx.slot()]( IScheduler::OccupancySnapshot snap ) -> void {
    std::string states = "";
    for ( int const stateTotal : snap.states[slot] ) {
      states += std::to_string( stateTotal ) + " ";
    }
    ATH_MSG_INFO( "Slot " << slot << " snapshot time " << snap.time.time_since_epoch().count() << " states: " << states );
  };
  m_scheduler->recordOccupancy( 0, std::move( profileCallback ) );
  
  //Make a vector of dummy data to initialise the views
  std::vector<int> viewData;
  auto viewVector = std::make_unique<ViewContainer>();
  for ( int viewIndex = 0; viewIndex < m_viewNumber; ++viewIndex )
  {
    viewData.push_back( ( viewIndex * 10 ) + 10 + ctx.evt() );
  }

  //Create the views and populate them
  CHECK( ViewHelper::makeAndPopulate( m_viewBaseName,      //Base name for all views to use
                                      viewVector.get(),    //Vector to store views
                                      m_w_int,             //A writehandlekey to use to access the views
                                      ctx,                 //The context of this algorithm
                                      viewData ) );        //Data to initialise each view - one view will be made per entry

  //Attach parent views to existing views
  if ( m_r_views.key() != "" )
  {
    SG::ReadHandle< ViewContainer > parentViewHandle( m_r_views, ctx );
    if ( parentViewHandle->size() == viewVector->size() )
    {
      for ( unsigned int viewIndex = 0; viewIndex < parentViewHandle->size(); ++viewIndex )
      {
        ATH_MSG_INFO( "Linking view " << viewVector->at( viewIndex )->name() << " to parent " << parentViewHandle->at( viewIndex )->name() );
        viewVector->at( viewIndex )->linkParent( parentViewHandle->at( viewIndex ) );
      }
    }
  }

  //Schedule the algorithms in views
  CHECK( ViewHelper::scheduleViews( viewVector.get(),       //View vector
                                    m_viewNodeName,         //Name of node to attach views to
                                    ctx,                    //Context to attach the views to
                                    m_scheduler.get() ) );  //ServiceHandle for the scheduler

  //Store the collection of views
  SG::WriteHandle< ViewContainer > outputViewHandle( m_w_views, ctx );
  CHECK( outputViewHandle.record( std::move( viewVector ) ) );

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
