///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ViewDataVerifier.cxx 
// Implementation file for class ViewDataVerifier
// Author: B. Wynne <bwynne@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "ViewDataVerifier.h"

// STL includes

// FrameWork includes
#include "AthenaKernel/ExtendedEventContext.h"

namespace AthViews {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ViewDataVerifier::ViewDataVerifier( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
}

// Destructor
///////////////
ViewDataVerifier::~ViewDataVerifier()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ViewDataVerifier::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

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

StatusCode ViewDataVerifier::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode ViewDataVerifier::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // Retrieve the current view from the EventContext
  auto viewProxy = getContext().getExtension<Atlas::ExtendedEventContext>()->proxy();

  ATH_MSG_INFO( name() << " running with store " << viewProxy->name() );

  // Test each container
  for ( auto &obj : m_load.value() ) {

    // Create a VarHandleKey (copied from SGInputLoader)
    SG::VarHandleKey vhk( obj.clid(), obj.key(), Gaudi::DataHandle::Writer );

    // Create a test proxy 
    SG::DataProxy* dp = viewProxy->proxy( obj.clid(), vhk.key() );

    // Test if the proxy is valid
    if ( dp ) { 
      ATH_MSG_INFO( "Found " << obj.key() << " in " << viewProxy->name() );
    } else {
      ATH_MSG_ERROR( "Did not find " << obj.key() << " in " << viewProxy->name() );
    }
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
