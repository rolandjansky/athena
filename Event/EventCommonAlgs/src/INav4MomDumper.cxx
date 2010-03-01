///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomDumper.cxx 
// Implementation file for class INav4MomDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <set>

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"

// FourMomUtils includes
#include "FourMomUtils/P4Dumper.h"
#include "FourMomUtils/P4DescendingSorters.h"

// EventCommonAlgs includes
#include "EventCommonAlgs/INav4MomDumper.h"

namespace {
  struct ToLower
  {
    char operator() (char c) const  { return std::tolower(c); }
  };
}

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
INav4MomDumper::INav4MomDumper( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  Algorithm( name, pSvcLocator ),
  m_storeGate    ( "StoreGateSvc", name ),
  m_msg          ( msgSvc(),       name ),
  m_outputStream ( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr;
  
  descr = "List of INavigable4Momentum containers one wants to dump";
  declareProperty( "INav4Moms", 
		   m_inav4momContainersName,
		   descr );
  std::vector<std::string> inav4momContainersName( 0 );
  m_inav4momContainersName.set( inav4momContainersName );

  descr  = "Name of the output stream where we'll dump informations from the";
  descr += " INavigable4MomentumCollection object(s).";
  descr += " Valid stream names are: \n";
  descr += "  - \"MsgStream\"\n";
  descr += "  - \"stderr\"\n";
  descr += "  - \"stdout\"\n";
  descr += "  - \"/path/to/some/file\".";

  declareProperty( "OutputStream",
		   m_outputStreamName = "MsgStream",
		   descr );
  m_outputStreamName.declareUpdateHandler( &INav4MomDumper::setupOutputStream,
					   this );
}

// Destructor
///////////////
INav4MomDumper::~INav4MomDumper()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
  // delete output stream
  if ( m_outputStream && 
       ( m_outputStream != &std::cout && 
	 m_outputStream != &std::cerr ) ) {
    delete m_outputStream;
    m_outputStream = 0;
  }
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode INav4MomDumper::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( outputLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;

  // Get pointer to StoreGateSvc and cache it :
  if ( !m_storeGate.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR 	
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
    return StatusCode::FAILURE;
  }

  m_msg << MSG::INFO
	<< "Configured to dump [" << m_inav4momContainersName.value().size()
	<< "] containers:"
	<< endreq;
  for ( std::vector<std::string>::const_iterator 
	  itr = m_inav4momContainersName.value().begin(),
	  iEnd= m_inav4momContainersName.value().end();
	itr != iEnd;
	++itr ) {
    m_msg << " - " << *itr << endreq;
  }
  m_msg << MSG::INFO 
	<< "OutputStream : " << m_outputStreamName.value() 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode INav4MomDumper::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode INav4MomDumper::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;

  typedef std::vector<std::string>::const_iterator ContNameIterator;

  for ( ContNameIterator 
	  itr  = m_inav4momContainersName.value().begin(),
	  iEnd = m_inav4momContainersName.value().end();
	itr != iEnd;
	++itr ) {
    m_msg << MSG::DEBUG << "dumping:  [" << *itr << "]..." << endreq;

    if ( !dump( *itr ).isSuccess() ) {
      m_msg << MSG::WARNING
	    << "Problem while dumping [" << *itr << "] !!"
	    << endreq;
      continue;
    }
    m_msg << MSG::DEBUG << "dumping:  [" << *itr << "]... [OK]" << endreq;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode 
INav4MomDumper::dump( const std::string& collName )
{
  typedef INavigable4MomentumCollection INav4Moms_t;
  if ( !m_storeGate->contains<INav4Moms_t>( collName ) ) {
    m_msg << MSG::WARNING 
	  << "No [" << collName
	  << "] INavigable4MomentumCollection in StoreGate !"
	  << endreq;
    return StatusCode::RECOVERABLE;
  }

  const INav4Moms_t * coll = 0;
  if ( !m_storeGate->retrieve( coll, collName ).isSuccess() ||
       0 == coll ) {
    m_msg << MSG::WARNING
	  << "Could not retrieve any INavigable4MomentumCollection at ["
	  << collName << "] !!"
	  << endreq;
    return StatusCode::RECOVERABLE;
  }

  std::ostringstream out;

  const std::size_t iMax = coll->size();
  out << "Retrieved [" << collName << "] ==> size = " << iMax << "\n";
  if ( coll->empty() ) {
    return StatusCode::SUCCESS;
  }

  // sorting the collection by Pt
  typedef std::set<INavigable4Momentum*, P4Sorters::Descending::Pt> Coll_t;
  Coll_t inav4moms( coll->begin(), coll->end() );

  FourMomUtils::dump( out, inav4moms );
  
  if ( 0 != m_outputStream ) { (*m_outputStream)   << out.str() << std::flush;
  } else                     { m_msg << MSG::DEBUG << out.str() << endreq;
  }

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void INav4MomDumper::setupOutputStream( Property& /*outputStreamName*/ )
{
  // reset internal state
  if ( m_outputStream && 
       ( m_outputStream != &std::cout && m_outputStream != &std::cerr ) ) {
    delete m_outputStream;
    m_outputStream = 0;
  }

  std::string streamName = m_outputStreamName.value();

  // get the stream in lower case
  std::transform( streamName.begin(), streamName.end(),
		  streamName.begin(),
		  ToLower() );

  const std::string stdout    = "stdout";
  const std::string stderr    = "stderr";
  const std::string msgstream = "msgstream";

  if ( streamName == msgstream ) {
    m_outputStream = 0;
  } else if ( streamName == stdout ) {
    m_outputStream = &std::cout;
  } else if ( streamName == stderr ) {
    m_outputStream = &std::cerr;
  } else {
    m_outputStream = new std::ofstream( m_outputStreamName.value().c_str(),
					std::ios::out | std::ios::trunc );
  }

  return;
}
