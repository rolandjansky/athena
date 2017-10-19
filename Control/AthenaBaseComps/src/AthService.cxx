///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthService.cxx 
// Implementation file for class AthService
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes

// AthenaBaseComps includes
#include "AthenaBaseComps/AthService.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
AthService::AthService( const std::string& name, 
			ISvcLocator* pSvcLocator ) : 
  ::Service      ( name, pSvcLocator ),
  ::AthMessaging ( msgSvc(), name ),
  m_ol(0)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

  m_outputLevel
    .declareUpdateHandler
    (&AthService::msg_update_handler, this);

}

// Destructor
///////////////
AthService::~AthService()
{ 
  ATH_MSG_DEBUG ("Calling destructor");
}

StatusCode AthService::sysInitialize()
{
  // Initialize it only if was not done
#ifdef GAUDIKERNEL_STATEMACHINE_H_
  if (Gaudi::StateMachine::OFFLINE != this->FSMState()) {
    m_targetState = Gaudi::StateMachine::CONFIGURED;

#else
  if( m_state != OFFLINE ) {
#endif
    ATH_MSG_WARNING ("Service already initialized");
    return StatusCode::SUCCESS;
  }

  // Set the Service's properties
  ATH_CHECK(setProperties());
  m_ol = m_outputLevel;
  // if ( !setProperties().isSuccess() ) {
  //   return StatusCode::FAILURE;
  // }
#ifdef GAUDIKERNEL_STATEMACHINE_H_
  m_state = Gaudi::StateMachine::ChangeState(Gaudi::StateMachine::CONFIGURE,
					     m_state);  
#else
  m_state = CONFIGURED;
#endif

  StatusCode sc = Service::sysInitialize();
  m_outputLevel = m_ol;
  return sc;
}

StatusCode AthService::initialize()
{
  m_outputLevel = m_ol;
  return StatusCode::SUCCESS;
}

StatusCode AthService::sysReinitialize()
{
  return Service::sysReinitialize();
}

StatusCode AthService::reinitialize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthService::sysFinalize()
{
#ifdef GAUDIKERNEL_STATEMACHINE_H_
  if (Gaudi::StateMachine::OFFLINE == this->FSMState()) {
    ATH_MSG_WARNING ("Service already offline");
    return StatusCode::SUCCESS;
  }
#else
  if( m_state == OFFLINE ) {
    ATH_MSG_WARNING ("Service already offline");
    return StatusCode::SUCCESS;
  }
  m_state = OFFLINE;
#endif
  return Service::sysFinalize();
}

StatusCode AthService::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode 
AthService::queryInterface( const InterfaceID& riid, 
			    void**             ppvi )
{
  return Service::queryInterface( riid, ppvi );
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

void 
AthService::msg_update_handler(Property& /*m_outputLevel*/) 
{
  this->setLevel (static_cast<MSG::Level> (outputLevel()));
}
