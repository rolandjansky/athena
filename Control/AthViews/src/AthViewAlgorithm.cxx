///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthViewAlgorithm.h 
// Header file for class AthViewAlgorithm
// Author: B Wynne, based on S Binet's AthAlgorithm
/////////////////////////////////////////////////////////////////// 

// AthenaBaseComps includes
#include "AthViews/AthViewAlgorithm.h"
#include "AthenaKernel/ExtendedEventContext.h"

#include "GaudiKernel/IJobOptionsSvc.h"

// STL includes

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthViewAlgorithm::AthViewAlgorithm( const std::string& name, 
			    ISvcLocator* pSvcLocator,
			    const std::string& version ) :
  AthAlgorithm( name, pSvcLocator, version )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  
  //Flags for requiring views (or not)
  m_require_view = false;
  m_require_not_view = false;
  declareProperty( "RequireView", m_require_view, "True if this algorithm may only run in a view" );
  declareProperty( "RequireNotView", m_require_not_view, "True if this algorithm may only run on whole events" );
}

// Destructor
///////////////
AthViewAlgorithm::~AthViewAlgorithm()
{ 
  ATH_MSG_DEBUG ("Calling destructor");
}

StatusCode AthViewAlgorithm::sysInitialize()
{
  //We need the properties now!
  StatusCode sc = setProperties();
  if( sc.isFailure() ) return StatusCode::FAILURE;

  //Check for stupid
  if ( m_require_view && m_require_not_view )
  {
    ATH_MSG_FATAL( "AthViewAlgorithm config fail: requiring view and not view" );
    return StatusCode::FAILURE;
  }

  return AthAlgorithm::sysInitialize();
}
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
StatusCode AthViewAlgorithm::sysExecute(const EventContext& ctx) {
#else
StatusCode AthViewAlgorithm::sysExecute() {
  const EventContext& ctx = *getContext();
#endif
  ATH_MSG_WARNING( "AthViewAlgorithm is now OBSOLETE, please migrate your code" );

  //Skip the algorithm if views are required or avoided
  SG::View * myView = eventView(ctx);
  if ( !myView && m_require_view )
  {
    ATH_MSG_INFO( "Skipping execution of " << name() << ": not in event view" );
    return StatusCode::SUCCESS;
  }
  if ( myView && m_require_not_view )
  {
    ATH_MSG_INFO( "Skipping execution of " << name() << ": in event view" );
    return StatusCode::SUCCESS;
  }

  //Set all DataHandles to use the EventView pointer
  for ( auto handle : inputHandles() )
  {
    SG::VarHandleBase * athenaHandle = static_cast< SG::VarHandleBase* >( handle );
    CHECK( athenaHandle->setProxyDict( myView ) );
  }
  for ( auto handle : outputHandles() )
  {
    SG::VarHandleBase * athenaHandle = static_cast< SG::VarHandleBase* >( handle );
    CHECK( athenaHandle->setProxyDict( myView ) );
  }

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  return AthAlgorithm::sysExecute(ctx);
#else
  return AthAlgorithm::sysExecute();
#endif
}

//Retrieve the EventView pointer from the context if it exists
SG::View * AthViewAlgorithm::eventView(const EventContext& ctx)
{
  //Try to get the view from context
  if ( ! ctx.valid() ) return 0; //but why no context?
  SG::View * myView = dynamic_cast< SG::View * >
    ( ctx.getExtension<Atlas::ExtendedEventContext>()->proxy() );
  if ( myView )
  {
    ATH_MSG_DEBUG( "Algorithm " << name() << " is in view " << myView );
  }
  else
  {
    ATH_MSG_DEBUG( "Algorithm " << name() << " is not in a view" );
  }
  return myView;
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

