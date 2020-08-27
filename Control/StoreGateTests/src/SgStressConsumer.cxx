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
#include "Gaudi/Property.h"

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

}

// Destructor
///////////////
SgStressConsumer::~SgStressConsumer()
{ 
  ATH_MSG_DEBUG ( "Calling destructor" );
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode SgStressConsumer::initialize()
{
  // configure our MsgStream
  msg().setLevel( msgLevel() );

  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  return StatusCode::SUCCESS;
}

StatusCode SgStressConsumer::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode SgStressConsumer::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
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
    if ( !evtStore()->retrieve( dv, outName.str() ).isSuccess() ) {
      ATH_MSG_ERROR( "Could not retrieve payload !!" );
      allGood = false;
    }

    data = (*dv)[0];
    if ( data->m_data.empty() ) {
      ATH_MSG_ERROR( "**NOT** my data !!" );
      allGood = false;
    }
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
