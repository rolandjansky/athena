/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MinimalViewAlg.h"
#include "AthViews/ViewHelper.h"
#include "StoreGate/WriteHandle.h"

namespace AthViews {

// Constructor
MinimalViewAlg::MinimalViewAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
MinimalViewAlg::~MinimalViewAlg()
{
}

// Athena Algorithm hooks
StatusCode MinimalViewAlg::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() << "..." );

  CHECK( m_scheduler.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MinimalViewAlg::finalize()
{
  ATH_MSG_DEBUG( "Finalizing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode MinimalViewAlg::execute()
{
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  // Create the view
  auto theView = ViewHelper::makeView( m_viewNodeName, -1, true );

  // Schedule the algorithms in the view
  const EventContext& ctx = getContext();
  return ViewHelper::scheduleSingleView( theView,
                                         m_viewNodeName,         //Name of node to attach views to
                                         ctx,                    //Context to attach the views to
                                         Atlas::getExtendedEventContext( ctx ).conditionsRun(),
                                         m_scheduler.get() );    //ServiceHandle for the scheduler
}

} //> end namespace AthViews
