///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonTestNoopAlg.cxx 
// Implementation file for class PerfMonTest::NoopAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// PerfMonTests includes
#include "PerfMonTestNoopAlg.h"

using namespace PerfMonTest;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
NoopAlg::NoopAlg( const std::string& name, 
		  ISvcLocator* pSvcLocator ) : 
  Algorithm   ( name,    pSvcLocator ),
  m_storeGate ( "StoreGateSvc", name ),
  m_msg       ( msgSvc(),       name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
NoopAlg::~NoopAlg()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode NoopAlg::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( outputLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;

  // Get pointer to StoreGateSvc and cache it :
  if ( !m_storeGate.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR 	
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode NoopAlg::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode NoopAlg::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;

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

