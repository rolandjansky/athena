///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiCnvSvc.cxx 
// Implementation file for class McAsciiCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <stdexcept>

// Framework includes
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IToolSvc.h"

// McParticleKernel includes
#include "McParticleKernel/IIOHepMcTool.h"

// Package includes
#include "McAsciiCnvSvc.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McAsciiCnvSvc::McAsciiCnvSvc( const std::string& name, ISvcLocator* svcLoc ) :
  ConversionSvc( name, svcLoc, repSvcType() ),
  m_msg  ( msgSvc(),  name ),
  m_cnvTool( 0 )
{
  declareProperty( "McEventsOutput",
		   m_mcEventsOutputName = "GEN_EVENT",
		   "Output location of the McEventCollection" );
}

// Destructor
///////////////
McAsciiCnvSvc::~McAsciiCnvSvc()
{}

StatusCode McAsciiCnvSvc::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( m_outputLevel.value() );

  m_msg << MSG::INFO << "Enter McAsciiCnvSvc initialization..."
	<< endmsg;

  if ( !ConversionSvc::initialize().isSuccess() ) {
    m_msg << MSG::ERROR
	  << "Unable to initialize base class ::ConversionSvc !!"
	  << endmsg;
    return StatusCode::FAILURE;
  }

  // retrieve tool
  setupCnvTool();

  return StatusCode::SUCCESS;
}

StatusCode McAsciiCnvSvc::updateServiceState( IOpaqueAddress* pAddress )
{
  static bool first = true;
  //  static int fid   = 0;
  //  static int recid = 0;
  if ( 0 != pAddress )    {
    GenericAddress* pAddr = dynamic_cast<GenericAddress*>(pAddress);
    if ( 0 != pAddr )    {
      if ( first )    {
        first = false;
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

long
McAsciiCnvSvc::repSvcType() const {
  return storageType();
}

long
McAsciiCnvSvc::storageType() {
  static long type = 223442331;
  return type;
}

StatusCode McAsciiCnvSvc::cnv()
{
  return m_cnvTool->execute();
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
void McAsciiCnvSvc::setInputFile( const std::string& asciiFileName )
{
  if ( asciiFileName.empty() ) {
    m_msg << MSG::ERROR
	  << "Empty input ASCII file name !!"
	  << endmsg;
    throw std::runtime_error( "Empty input ASCII file name !!" );
  }

  const std::string propName = "Input";
  StringProperty inputFileName;
  inputFileName.assign( m_cnvTool->getProperty( propName ) );

  const std::string propValue = asciiFileName;
  inputFileName.set( "ascii:"+propValue  );
  inputFileName.setName( propName );

  if ( !m_cnvTool->setProperty( inputFileName ).isSuccess() ) {
    m_msg << MSG::ERROR
	  << "Could not configure the converter tool to read from file ["
	  << propValue << "] !!"
	  << endmsg;
    throw std::runtime_error( "Failed to configure ASCII cnv tool !!" );
  }

  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
void McAsciiCnvSvc::setupCnvTool()
{
  {
    /// Get pointer to ToolSvc and cache it :
    IToolSvc * toolSvc = 0;
    if ( service( "ToolSvc", toolSvc ).isFailure() || 0 == toolSvc ) {
      m_msg << MSG::FATAL
	    << "Tool Service not found !!"
	    << endmsg;
      throw std::runtime_error( "Could not find ToolSvc !!" );
    }

    const std::string cnvName = "HepMcReaderTool/CnvTool";
    /// Retrieves a private AlgTool to create a McEventCollection
    /// from an ASCII file
    IAlgTool* algTool = 0;
    if ( toolSvc->retrieveTool( cnvName, algTool, this ).isFailure() ) {
      m_msg << MSG::ERROR
	    << "Creation of algTool [" << cnvName << "]" 
	    << " FAILED !"
	    << endmsg;
      throw std::runtime_error( "Could not retrieve [" +cnvName+"] !" );
    } else {
      m_msg << MSG::DEBUG
	    << "AlgTool [" << cnvName << "] successfully created"
	    << endmsg;
      m_cnvTool = dynamic_cast<IIOHepMcTool*>(algTool);

      if ( 0 == m_cnvTool ) {
	m_msg << MSG::ERROR
	      << "Could not dynamic-cast to IIOHepMcTool* !!"
	      << endmsg;
	throw std::runtime_error( "Could not dyn-cast to IIOHepMcTool* !!" );
      }
    }
  }

  // now we configure the output location
  {
    const std::string propName = "McEventsOutput";
    StringProperty outputName;
    outputName.assign( m_cnvTool->getProperty( propName ) );
    
    const std::string propValue = m_mcEventsOutputName.value();
    outputName.set( propValue  );
    outputName.setName( propName );
    
    if ( !m_cnvTool->setProperty( outputName ).isSuccess() ) {
      m_msg << MSG::ERROR
	    << "Could not configure the converter tool to record the "
	    << "McEventCollection in ["
	    << propValue << "] !!"
	    << endmsg;
      throw std::runtime_error( "Failed to configure ASCII cnv tool !!" );
    }
  }

  return;
}
