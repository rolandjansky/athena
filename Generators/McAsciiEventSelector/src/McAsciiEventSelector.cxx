///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiEventSelector.cxx 
// Implementation file for class McAsciiEventSelector
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

//#include "GaudiKernel/Tokenizer.h"

// EventInfo includes
#include "EventInfo/EventInfo.h"

// Package includes
#include "McAsciiEventSelector.h"
#include "McAsciiAddress.h"
#include "McAsciiContext.h"
#include "McAsciiCnvSvc.h"
#include "McAsciiFileHelper.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

McAsciiEventSelector::McAsciiEventSelector( const std::string& name,
					    ISvcLocator* svcLoc ) :
  AthService( name,    svcLoc ),
  m_asciiCnvSvc (nullptr)
{
  declareProperty( "InputCollections", 
		   m_asciiFileNames,
		   "Input (ascii) file names" );

  declareProperty( "RunNumber",           
		   m_runNbr = 0,
		   "Run number to start with" );
  m_runNbr.verifier().setLower( 0 );

  declareProperty( "EventsPerRun",        
		   m_eventsPerRun = 100000000,
		   "Maximal number of events per run" );
  m_eventsPerRun.verifier().setLower( 0 );

  declareProperty( "FirstEvent", 
		   m_firstEventNbr = 0,
		   "Number of the first event" );
  m_firstEventNbr.verifier().setLower( 0 );   

  declareProperty( "InitialTimeStamp",    
		   m_initTimeStamp = 0,
		   "Timestamp value to start with" );
  m_initTimeStamp.verifier().setLower( 0 );   

  declareProperty( "TimeStampInterval", 
		   m_timeStampInterval = 0,
		   "Value of the timestamp interval" );
}

// Destructor
///////////////
McAsciiEventSelector::~McAsciiEventSelector()
{
  //delete context; ???
}

StatusCode McAsciiEventSelector::initialize()
{
  ATH_MSG_INFO( "Enter McAsciiEventSelector initialization..." );

  const std::size_t nbrAsciiFiles = m_asciiFileNames.value().size();
  if ( nbrAsciiFiles <= 0 ) {
    ATH_MSG_ERROR( "You need to give at least 1 input file !!"
                   << "(Got [" << nbrAsciiFiles << "] file instead !)" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Selector configured to read [" << nbrAsciiFiles << "] file(s)..." );
  }

  m_asciiFiles.resize( nbrAsciiFiles );
  for ( std::size_t i = 0; i != nbrAsciiFiles; ++i ) {
    const std::string& asciiFileName = m_asciiFileNames.value()[i];
    const std::size_t evtMax = McAsciiFileHelper::evtMax( asciiFileName,
							  msg() );
    ATH_MSG_DEBUG( " [" << i << "] : " 
                   << asciiFileName
                   << " => #evts = " << evtMax );
    m_asciiFiles[i] = std::make_pair( asciiFileName, evtMax );
  }
  m_curEvtNbr = 0;
  m_curFile   = m_asciiFiles.begin();

  IConversionSvc* cnvSvc = 0;
  ATH_CHECK( service("McAsciiCnvSvc", cnvSvc, true) );
  m_asciiCnvSvc = dynamic_cast<McAsciiCnvSvc*>( cnvSvc );
  if ( 0 == m_asciiCnvSvc ) {
    ATH_MSG_ERROR( "Could not dyn-cast to McAsciiCnvSvc !!" );
    return StatusCode::FAILURE;
  }
  
  m_asciiCnvSvc->setInputFile( m_curFile->first );

  return StatusCode::SUCCESS;
}

StatusCode McAsciiEventSelector::stop()     {
  ATH_MSG_INFO( "Stop..."  );

  ServiceHandle<IIncidentSvc> incSvc ("IncidentSvc", name());
  ATH_CHECK( incSvc.retrieve() );
  Incident lastInputIncident(name(), "LastInput");
  incSvc->fireIncident(lastInputIncident);

  return StatusCode::SUCCESS;
}

StatusCode McAsciiEventSelector::finalize()
{
  ATH_MSG_INFO( "Finalize..."  );
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
McAsciiEventSelector::queryInterface( const InterfaceID& riid, 
				      void** ppvInterface )
{
  if ( IEvtSelector::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEvtSelector*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode McAsciiEventSelector::next( Context& ctx ) const
{
  ATH_MSG_DEBUG( "............. Next Event ............." );

  McAsciiContext* asciiCtx = dynamic_cast<McAsciiContext*>(&ctx);

  if ( 0 == asciiCtx ) {
    ATH_MSG_ERROR( "Could not dyn-cast to McAsciiContext !!" );
    return StatusCode::FAILURE;
  }

  asciiCtx->next();

  if ( m_curEvtNbr >= m_curFile->second ) {

    // go to next file ?
    ++m_curFile;
    if ( m_curFile == m_asciiFiles.end() ) {
      ATH_MSG_INFO( "EventSelector: End of input"  );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "switching to next input ASCII file [" 
                     << m_curFile->first << "]..." );
      m_curEvtNbr = 0;
      m_asciiCnvSvc->setInputFile( m_curFile->first );
    }
  }

  ++m_curEvtNbr;
  return m_asciiCnvSvc->cnv();
}

StatusCode McAsciiEventSelector::next( Context& ctx, int jump ) const
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctx);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode
McAsciiEventSelector::previous( Context& ctx ) const 
{
  McAsciiContext* asciiCtx = dynamic_cast<McAsciiContext*>(&ctx);

  if ( 0 == asciiCtx ) {
    ATH_MSG_ERROR( "Could not dyn-cast to McAsciiContext !!"  );
    return StatusCode::FAILURE;
  }

  asciiCtx->previous();
  return StatusCode::SUCCESS;
}

StatusCode 
McAsciiEventSelector::previous( Context& ctx, int jump ) const 
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctx);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode
McAsciiEventSelector::last( Context& /*ctxt*/ ) const
{
  ATH_MSG_ERROR( "............. Last Event Not Implemented ............."  );
  return StatusCode::FAILURE;
}


StatusCode 
McAsciiEventSelector::rewind( Context& ctx ) const 
{
  McAsciiContext* asciiCtx = dynamic_cast<McAsciiContext*>(&ctx);

  if ( 0 == asciiCtx ) {
    ATH_MSG_ERROR( "Could not dyn-cast to McAsciiContext !!"  );
    return StatusCode::FAILURE;
  }

  asciiCtx->rewind();
  return StatusCode::SUCCESS;
}

StatusCode
McAsciiEventSelector::createContext( Context*& refCtx ) const
{
  McAsciiContext* ctx = 
    new McAsciiContext( 
	this,
	static_cast<unsigned int>(m_runNbr.value()),
	static_cast<unsigned int>(m_firstEventNbr.value()),
	static_cast<unsigned int>(m_eventsPerRun.value()),
	static_cast<unsigned int>(m_initTimeStamp.value()),
	static_cast<unsigned int>(m_timeStampInterval.value()) );
  refCtx = ctx;

  return StatusCode::SUCCESS;
}

StatusCode
McAsciiEventSelector::createAddress( const Context& refCtx, 
				     IOpaqueAddress*& addr ) const 
{
  const McAsciiContext* asciiCtx = dynamic_cast<const McAsciiContext*>(&refCtx);

  if ( 0 == asciiCtx ) {
    ATH_MSG_ERROR( "Could not dyn-cast to a McAsciiContext !!"  );
    return StatusCode::FAILURE;
  }

  addr = new McAsciiAddress( EventInfo::classID(), 
			     "McEventInfo",
			     asciiCtx->runNumber(), 
			     asciiCtx->eventNumber(),
			     asciiCtx->timeStamp() );
  return StatusCode::SUCCESS;
}

StatusCode
McAsciiEventSelector::releaseContext( Context*& /*refCtxt*/ ) const
{
  ATH_MSG_ERROR( "............. releaseContext Not Implemented ............." );

  return StatusCode::FAILURE;
}

StatusCode
McAsciiEventSelector::resetCriteria( const std::string&, Context& ) const 
{
  ATH_MSG_ERROR( "............. resetCriteria Not Implemented ............." );

  return StatusCode::FAILURE;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 
