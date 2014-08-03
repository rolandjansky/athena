///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WriteTruthParticles.cxx 
// Implementation file for class WriteTruthParticles
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// AnalysisExamples includes
#include "WriteTruthParticles.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
WriteTruthParticles::WriteTruthParticles( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_cnvTool( "TruthParticleCnvTool/CnvTool", this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "CnvTool",
		   m_cnvTool,
		   "Handle to the tool which converts a McEventCollection into"
		   " a TruthParticleContainer" );

}

// Destructor
///////////////
WriteTruthParticles::~WriteTruthParticles()
{ 
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode WriteTruthParticles::initialize()
{
  MsgStream msg( msgSvc(), name() );

  msg << MSG::INFO 
      << "Initializing " << name() << "..." 
      << endreq;

  /// retrieve the converter tool
  if ( !m_cnvTool.retrieve().isSuccess() ) {
    msg << MSG::ERROR
	<< "Could not retrieve the truth particle converter tool !!"
	<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode WriteTruthParticles::finalize()
{
  MsgStream msg( msgSvc(), name() );
  msg << MSG::INFO 
      << "Finalizing " << name() << "..." 
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode WriteTruthParticles::execute()
{  
  MsgStream msg( msgSvc(), name() );

  msg << MSG::DEBUG << "Executing " << name() << "..." 
      << endreq;

  // create a TruthParticleContainer from a McEventCollection
  if ( 0 == m_cnvTool || 
       !m_cnvTool->execute().isSuccess() ) {
    msg << MSG::WARNING 
	<< "Could not convert a McEventCollection into "
	<< "a TruthParticleContainer !"
	<< endreq;
    return StatusCode::RECOVERABLE;
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

