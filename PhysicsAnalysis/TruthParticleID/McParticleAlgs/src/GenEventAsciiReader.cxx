/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// GenEventAsciiReader.cxx 
// Implementation file for class GenEventAsciiReader
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes
#include <stdexcept>

// McParticleKernel includes
#include "McParticleKernel/IIOHepMcTool.h"

// McParticleAlgs includes
#include "GenEventAsciiReader.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
GenEventAsciiReader::GenEventAsciiReader( const std::string& name, 
					  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr = "";
  descr = "Tool to read a HepMC::GenEvent from an ASCII file";
  declareProperty( "McReader",
		   m_genEventReader = IIOHepMcTool_t( "HepMcReaderTool", 
						      this ),
		   descr );
}

/// Destructor
///////////////
GenEventAsciiReader::~GenEventAsciiReader()
{}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode GenEventAsciiReader::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  setupGenEventReaderTool();

  return StatusCode::SUCCESS;
}

StatusCode GenEventAsciiReader::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode GenEventAsciiReader::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  if ( !m_genEventReader->execute().isSuccess() ) {
    ATH_MSG_WARNING("Problem executing IIOHepMcTool !!");
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void GenEventAsciiReader::setupGenEventReaderTool()
{
  if ( !m_genEventReader.retrieve().isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not retrieve algTool IIOHepMcTool [" << m_genEventReader.type()
       << "] !!");
      throw std::runtime_error("Could not setup McReader property !");
  } else {
    ATH_MSG_DEBUG
      ("Retrieved and configured algTool [" << m_genEventReader.type() << "]");
  }
  
  return;
}

