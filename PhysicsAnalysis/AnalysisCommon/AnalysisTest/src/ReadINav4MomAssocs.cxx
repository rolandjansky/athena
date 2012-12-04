/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ReadINav4MomAssocs.cxx 
// Implementation file for class ReadINav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// AnalysisTest includes
#include "AnalysisTest/ReadINav4MomAssocs.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
ReadINav4MomAssocs::ReadINav4MomAssocs( const std::string& name, 
			      ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_storeGate  ( 0 )
{
  //
  // Property declaration
  // 
  declareProperty( "Assocs", m_inavAssocsName = "INav4MomAssocs" );

}

/// Destructor
///////////////
ReadINav4MomAssocs::~ReadINav4MomAssocs()
{ 
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Calling destructor" << endreq;
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode ReadINav4MomAssocs::initialize()
{
  MsgStream log( msgSvc(), name() );

  log << MSG::INFO 
      << "Initializing " << name() << "..." 
      << endreq;

  // Get pointer to StoreGateSvc and cache it :
  if ( !service( "StoreGateSvc", m_storeGate ).isSuccess() ) {
    log << MSG::ERROR 	
	<< "Unable to retrieve pointer to StoreGateSvc"
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode ReadINav4MomAssocs::finalize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO 
      << "Finalizing " << name() << "..." 
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode ReadINav4MomAssocs::execute()
{  
  MsgStream log( msgSvc(), name() );

  log << MSG::DEBUG << "Executing " << name() << "..." 
      << endreq;

  if ( !readAssocs<INav4MomAssocs>( m_inavAssocsName ).isSuccess() ) {
    log << MSG::ERROR
	<< "Could not perform reading test of INav4MomAssocs !!"
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

