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
#include "Gaudi/Property.h"

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
  AthAlgorithm( name, pSvcLocator ),
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
  ATH_MSG_DEBUG ( "Calling destructor" );
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode WriteTruthParticles::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..."  );

  /// retrieve the converter tool
  if ( !m_cnvTool.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ( "Could not retrieve the truth particle converter tool !!" );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode WriteTruthParticles::finalize()
{
  ATH_MSG_INFO ( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode WriteTruthParticles::execute()
{  
  ATH_MSG_DEBUG ( "Executing " << name() << "..."  );

  // create a TruthParticleContainer from a McEventCollection
  if ( 0 == m_cnvTool || 
       !m_cnvTool->execute().isSuccess() ) {
    ATH_MSG_WARNING 
      ( "Could not convert a McEventCollection into "
	<< "a TruthParticleContainer !"	);
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

