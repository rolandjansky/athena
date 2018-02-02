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
  AthAlgorithm( name, pSvcLocator ),
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
  ATH_MSG_DEBUG( "Calling destructor"  );
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
  ATH_MSG_INFO( "Initializing " << name() << "..."  );

  ATH_MSG_INFO( "Configured to dump [" << m_inav4momContainersName.value().size()
                << "] containers:" );
  for ( std::vector<std::string>::const_iterator 
	  itr = m_inav4momContainersName.value().begin(),
	  iEnd= m_inav4momContainersName.value().end();
	itr != iEnd;
	++itr )
  {
    ATH_MSG_INFO( " - " << *itr  );
  }
  ATH_MSG_INFO( "OutputStream : " << m_outputStreamName.value() );

  return StatusCode::SUCCESS;
}

StatusCode INav4MomDumper::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..."  );
  return StatusCode::SUCCESS;
}

StatusCode INav4MomDumper::execute()
{  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  typedef std::vector<std::string>::const_iterator ContNameIterator;

  for ( ContNameIterator 
	  itr  = m_inav4momContainersName.value().begin(),
	  iEnd = m_inav4momContainersName.value().end();
	itr != iEnd;
	++itr )
  {
    ATH_MSG_DEBUG( "dumping:  [" << *itr << "]..."  );

    if ( !dump( *itr ).isSuccess() ) {
      ATH_MSG_WARNING( "Problem while dumping [" << *itr << "] !!" );
      continue;
    }
    ATH_MSG_DEBUG( "dumping:  [" << *itr << "]... [OK]"  );
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
  if ( !evtStore()->contains<INav4Moms_t>( collName ) ) {
    ATH_MSG_WARNING( "No [" << collName
                     << "] INavigable4MomentumCollection in StoreGate !" );
    return StatusCode::RECOVERABLE;
  }

  const INav4Moms_t * coll = 0;
  if ( !evtStore()->retrieve( coll, collName ).isSuccess() ||
       0 == coll )
  {
    ATH_MSG_WARNING( "Could not retrieve any INavigable4MomentumCollection at ["
                     << collName << "] !!" );
    return StatusCode::RECOVERABLE;
  }

  std::ostringstream out;

  const std::size_t iMax = coll->size();
  out << "Retrieved [" << collName << "] ==> size = " << iMax << "\n";
  if ( coll->empty() ) {
    return StatusCode::SUCCESS;
  }

  // sorting the collection by Pt
  typedef std::set<const INavigable4Momentum*, P4Sorters::Descending::Pt> Coll_t;
  Coll_t inav4moms( coll->begin(), coll->end() );

  FourMomUtils::dump( out, inav4moms );
  
  if ( 0 != m_outputStream ) { (*m_outputStream)   << out.str() << std::flush;
  } else                     { ATH_MSG_DEBUG( out.str()  );
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
