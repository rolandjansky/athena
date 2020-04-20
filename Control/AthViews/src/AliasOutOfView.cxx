/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AliasOutOfView.cxx 
// Implementation file for class AliasOutOfView
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AliasOutOfView.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthViews/View.h"

namespace AthViews {

AliasOutOfView::AliasOutOfView( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
}

AliasOutOfView::~AliasOutOfView()
{
}

StatusCode AliasOutOfView::initialize()
{
  for ( auto& obj : m_dataObjects.value() ) {

    // Check for misconfiguration
    if ( obj.key() == "" ) {
      ATH_MSG_ERROR( "Empty key " << obj << " not permitted" );
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Will alias " << m_viewName.value() << "_" << obj.key() << " to " << obj.key() );
  }

  // Inform the scheduler that these containers will be available
  if ( !setProperty( "ExtraOutputs", m_dataObjects.toString() ).isSuccess() ) {

    ATH_MSG_ERROR( "Failed setting property ExtraOutputs" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AliasOutOfView::execute()
{  
  // Retrieve the current view from the EventContext
  auto viewProxy = Atlas::getExtendedEventContext( getContext() ).proxy();

  ATH_MSG_DEBUG( "Executing " << name() << " with store " << viewProxy->name() );

  // Test each container
  for ( auto& obj : m_dataObjects.value() ) {

    // Create a VarHandleKey for the object inside the view
    SG::VarHandleKey vhk( obj.clid(), m_viewName + "_" + obj.key(), Gaudi::DataHandle::Reader );

    // Create a test proxy 
    SG::DataProxy* dp = viewProxy->proxy( obj.clid(), vhk.key() );

    // Test if the proxy is valid
    if ( dp ) { 
      ATH_MSG_DEBUG( "Found " << vhk.key() << " in " << viewProxy->name() );
      CHECK( evtStore()->setAlias( dp, obj.key() ) );
    } else {
      ATH_MSG_ERROR( "Did not find " << vhk.key() << " in " << viewProxy->name() );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
