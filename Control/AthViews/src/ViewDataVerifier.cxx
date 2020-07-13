/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ViewDataVerifier.cxx 
// Implementation file for class ViewDataVerifier
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "ViewDataVerifier.h"

// STL includes

// FrameWork includes
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthViews/View.h"

namespace AthViews {

ViewDataVerifier::ViewDataVerifier( const std::string& name,
                      ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
}

ViewDataVerifier::~ViewDataVerifier()
{}

StatusCode ViewDataVerifier::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Debug message indicating what containers will be loaded
  // Code copied from SGInputLoader
  if ( m_load.value().size() > 0 ) {
    std::ostringstream str;
    str << "Will explicitly preload the following DataObjects:";
    for ( auto &obj : m_load.value() ) {
      str << "\n    + " << obj;
      if ( obj.key() == "" ) {
        sc = StatusCode::FAILURE;
        str << "   ERROR: empty key is not allowed!";
      }
    }
    ATH_MSG_INFO( str.str() );
  }

  // Inform the scheduler that these containers will be available
  if ( !setProperty( "ExtraOutputs", m_load.toString() ).isSuccess() ) {
    ATH_MSG_WARNING( "Failed setting property ExtraOutputs" );
  }

  return sc;
}

StatusCode ViewDataVerifier::execute(const EventContext& ctx) const
{  
  // Retrieve the current view from the EventContext
  auto viewProxy = Atlas::getExtendedEventContext(ctx).proxy();

  ATH_MSG_DEBUG( "Executing " << name() << " running with store " << viewProxy->name() );

  // Test each container
  for ( auto &obj : m_load.value() ) {

    // Create a VarHandleKey (copied from SGInputLoader)
    SG::VarHandleKey vhk( obj.clid(), obj.key(), Gaudi::DataHandle::Writer );

    // Create a test proxy 
    SG::DataProxy* dp = viewProxy->proxy( obj.clid(), vhk.key() );

    // Test if the proxy is valid
    if ( dp ) { 
      ATH_MSG_DEBUG( "Found " << obj.key() << " in " << viewProxy->name() );
    } else {
      ATH_MSG_DEBUG( "Did not find " << obj.key() << " in " << viewProxy->name() );
      const SG::View* view = dynamic_cast<const SG::View*>( viewProxy );
      if ( view != 0 ) {
        ATH_MSG_DEBUG( "Available content is: " << view->dump() );
      } else {
        ATH_MSG_DEBUG( "Not a View" );
      }
      //return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
