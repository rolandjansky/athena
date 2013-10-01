///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SgStressProducer.cxx 
// Implementation file for class SgStressProducer
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// DataModel includes
#include "DataModel/DataPool.h"

// StoreGateTests includes
#include "StoreGateTests/PayLoad.h"
#include "SgStressProducer.h"

typedef DataPool<SgTests::PayLoad> PayLoadPool_t;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
SgStressProducer::SgStressProducer( const std::string& name, 
			      ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_storeGate  ( "StoreGateSvc", name ),
  m_msg        ( msgSvc(),       name ),
  m_pool       ( 0 )
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

  declareProperty( "ObjectsSize",
		   m_dataSize = 100,
		   "Size of each 'Data' objects to be stored each event" );

  declareProperty( "UseDataPool",
		   m_useDataPool = false,
		   "Switch to use or not an arena (memory pool) to allocate "
		   "payload" );
}

// Destructor
///////////////
SgStressProducer::~SgStressProducer()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
  delete m_pool;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SgStressProducer::initialize()
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
  
  if ( m_useDataPool ) {
    delete m_pool; m_pool = new PayLoadPool_t;
    m_pool->reserve( m_nObjs );
  }
  return StatusCode::SUCCESS;
}

StatusCode SgStressProducer::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode SgStressProducer::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;
  
  if ( !createData().isSuccess() ) {
    m_msg << MSG::ERROR
	  << "Could not create PayLoad data !!"
	  << endreq;
    return StatusCode::FAILURE;
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

StatusCode SgStressProducer::createData()
{
  bool allGood = true;
  if ( m_useDataPool ) {
    /// free all memory in the pool.
    m_pool->erase();
//     m_pool->reserve(m_nObjs);
  }

  for ( std::size_t iObj = 0; iObj != m_nObjs; ++iObj ) {
    std::ostringstream outName;
    outName << m_dataName << "_payload_" << iObj;
    SgTests::PayLoad * data = 0;
    SgTests::PayLoadDv*  dv = new SgTests::PayLoadDv;

    if ( m_useDataPool ) {
      dv->clear( SG::VIEW_ELEMENTS );
      data = m_pool->nextElementPtr();
    } else {
      dv->clear( SG::OWN_ELEMENTS );
      data = new SgTests::PayLoad;
    }
    dv->push_back( data );

    if ( !m_storeGate->record(dv, outName.str()).isSuccess() ) {
      m_msg << MSG::ERROR
	    << "Could not store data at [" << outName.str() << "] !!"
	    << endreq;
      delete dv; dv = 0;
      allGood = false;
      continue;
    }
    if ( !m_storeGate->setConst(dv).isSuccess() ) {
      m_msg << MSG::WARNING << "Could not setConst data at ["
	    << outName.str() << "] !!"
	    << endreq;
    }  

    // filling data
    data->m_data.reserve( m_dataSize );
    for ( std::size_t i = 0; i != static_cast<std::size_t>(m_dataSize); ++i ) {
      data->m_data.push_back( i );
    }
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

