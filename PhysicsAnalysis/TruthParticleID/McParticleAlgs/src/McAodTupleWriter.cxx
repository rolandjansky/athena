/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McAodTupleWriter.cxx 
// Implementation file for class McAodTupleWriter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <stdexcept>

// FrameWork includes

// McParticleKernel includes
#include "McParticleKernel/IIOMcAodTool.h"

// McParticleAlgs includes
#include "McAodTupleWriter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McAodTupleWriter::McAodTupleWriter( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  AthAlgorithm  ( name, pSvcLocator ),
  m_mcAodWriter ( "McAodTupleWriterTool", this )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "McWriter",
		   m_mcAodWriter = IIOMcAodTool_t( "McAodTupleWriterTool", 
						   this ),
		   "Tool to write the reference TruthParticleContainer into "
		   "a dedicated file" );
}

/// Destructor
///////////////
McAodTupleWriter::~McAodTupleWriter()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodTupleWriter::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  setupMcAodWriterTool();
  return StatusCode::SUCCESS;
}

StatusCode McAodTupleWriter::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode McAodTupleWriter::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  if ( !m_mcAodWriter->execute().isSuccess() ) {
    ATH_MSG_WARNING ("Problem executing IIOMcAodTool !!");
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void McAodTupleWriter::setupMcAodWriterTool()
{
  if ( !m_mcAodWriter.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve algTool IIOMcAodTool [" << m_mcAodWriter.type()
       << "] !!");
      throw std::runtime_error("Could not setup McWriter property !");
  } else {
    ATH_MSG_DEBUG
      ("Retrieved and configured algTool [" << m_mcAodWriter.type() << "]");
  }
  
  return;
}

