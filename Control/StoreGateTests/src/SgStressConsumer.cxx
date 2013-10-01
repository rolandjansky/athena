///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SgStressConsumer.cxx 
// Implementation file for class SgStressConsumer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// StoreGateTests includes
#include "StoreGateTests/PayLoad.h"
#include "SgStressConsumer.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SgStressConsumer::SgStressConsumer( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_storeGate  ( "StoreGateSvc", name ),
  m_msg        ( msgSvc(),       name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "DataName",
		   m_dataName = "MyData",
		   "Output location of 'Data' objects" );

  declareProperty( "NbrOfObjects",
		   m_nObjs = 1000,
		   "Number of 'Data' objects to be stored each event" );

}

// Destructor
///////////////
SgStressConsumer::~SgStressConsumer()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SgStressConsumer::initialize()
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

StatusCode SgStressConsumer::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode SgStressConsumer::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;
  
  return readData();
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

StatusCode SgStressConsumer::readData()
{
  bool allGood = true;
  for ( std::size_t iObj = 0; iObj != m_nObjs; ++iObj ) {
    std::ostringstream outName;
    outName << m_dataName << "_payload_" << iObj;
    const SgTests::PayLoad * data = 0;
    const SgTests::PayLoadDv* dv  = 0;
    if ( !m_storeGate->retrieve( dv, outName.str() ).isSuccess() ) {
      m_msg << MSG::ERROR
	    << "Could not retrieve payload !!"
	    << endreq;
      allGood = false;
    }

    data = (*dv)[0];
    if ( data->m_data.empty() ) {
      m_msg << MSG::ERROR
	    << "**NOT** my data !!"
	    << endreq;
      allGood = false;
    }
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
