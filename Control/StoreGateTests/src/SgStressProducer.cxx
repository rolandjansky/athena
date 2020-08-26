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
#include "Gaudi/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// DataModel includes
#include "AthAllocators/DataPool.h"

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
  AthAlgorithm( name, pSvcLocator )
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
  ATH_MSG_DEBUG ( "Calling destructor" );
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SgStressProducer::initialize()
{
  // configure our MsgStream
  msg().setLevel( msgLevel() );

  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode SgStressProducer::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode SgStressProducer::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  
  if ( !createData().isSuccess() ) {
    ATH_MSG_ERROR ( "Could not create PayLoad data !!" );
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
  PayLoadPool_t pool;
  if ( m_useDataPool ) {
    /// free all memory in the pool.
    pool.erase();
    pool.reserve (m_nObjs);
  }

  for ( std::size_t iObj = 0; iObj != m_nObjs; ++iObj ) {
    std::ostringstream outName;
    outName << m_dataName << "_payload_" << iObj;
    SgTests::PayLoad * data = 0;
    SgTests::PayLoadDv*  dv = new SgTests::PayLoadDv;

    if ( m_useDataPool ) {
      dv->clear( SG::VIEW_ELEMENTS );
      data = pool.nextElementPtr();
    } else {
      dv->clear( SG::OWN_ELEMENTS );
      data = new SgTests::PayLoad;
    }
    dv->push_back( data );

    if ( !evtStore()->record(dv, outName.str()).isSuccess() ) {
      ATH_MSG_ERROR ( "Could not store data at [" << outName.str() << "] !!" );
      delete dv; dv = 0;
      allGood = false;
      continue;
    }
    if ( !evtStore()->setConst(dv).isSuccess() ) {
      ATH_MSG_WARNING( "Could not setConst data at [" << outName.str() << "] !!");
    }  

    // filling data
    data->m_data.reserve( m_dataSize );
    for ( std::size_t i = 0; i != static_cast<std::size_t>(m_dataSize); ++i ) {
      data->m_data.push_back( i );
    }
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

