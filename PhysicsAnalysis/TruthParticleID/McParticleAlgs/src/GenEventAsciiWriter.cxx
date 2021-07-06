/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// GenEventAsciiWriter.cxx 
// Implementation file for class GenEventAsciiWriter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <stdexcept>

// McParticleKernel includes
#include "McParticleKernel/IIOHepMcTool.h"

// McParticleAlgs includes
#include "GenEventAsciiWriter.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
GenEventAsciiWriter::GenEventAsciiWriter( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr = "";
  descr = "Tool to write the reference HepMC::GenEvent into a dedicated file";
  declareProperty( "McWriter",
		   m_genEventWriter = IIOHepMcTool_t( "HepMcWriterTool", 
						      this ),
		   descr );
}

/// Destructor
///////////////
GenEventAsciiWriter::~GenEventAsciiWriter()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode GenEventAsciiWriter::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  setupGenEventWriterTool();

  return StatusCode::SUCCESS;
}

StatusCode GenEventAsciiWriter::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode GenEventAsciiWriter::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  if ( !m_genEventWriter->execute().isSuccess() ) {
    ATH_MSG_WARNING("Problem executing IIOHepMcTool !!");
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void GenEventAsciiWriter::setupGenEventWriterTool()
{
  if ( !m_genEventWriter.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve algTool IIOHepMcTool [" << m_genEventWriter.type()
       << "] !!");
      throw std::runtime_error("Could not setup McWriter property !");
  } else {
    ATH_MSG_DEBUG
      ("Retrieved and configured algTool [" << m_genEventWriter.type() << "]");
  }
  
  return;
}

