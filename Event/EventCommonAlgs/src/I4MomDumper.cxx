///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// I4MomDumper.cxx 
// Implementation file for class I4MomDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <sstream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <set>

// Boost includes
#include <boost/array.hpp>

// FrameWork includes
#include "GaudiKernel/Property.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"

// FourMomUtils includes
#include "FourMomUtils/P4DescendingSorters.h"

// EventCommonAlgs includes
#include "I4MomDumper.h"

static const boost::array<std::string, I4MomDumper::Display::Size> s_display
= { { "EEtaPhiM",
      "IPtCotThPhiM",
      "PtEtaPhiM",
      "PxPyPzE" } };

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
I4MomDumper::I4MomDumper( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_display      ( Display::PxPyPzE ),
  m_outputStream ( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr;
  
  descr = "List of INavigable4Momentum containers one wants to dump";
  declareProperty( "INav4Moms", 
		   m_i4momContainersName,
		   descr );
  std::vector<std::string> i4momContainersName( 0 );
  m_i4momContainersName.set( i4momContainersName );

  declareProperty( "Display",
		   m_displayName = "pxpypze",
		   "Tells how one wants the I4Momentum objects to be displayed"
		   " ie: eetaphim|iptcotthphim|ptetaphim|pxpypze" );
  m_displayName.declareUpdateHandler( &I4MomDumper::setupDisplay, this );

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
  m_outputStreamName.declareUpdateHandler( &I4MomDumper::setupOutputStream,
					   this );
}

// Destructor
///////////////
I4MomDumper::~I4MomDumper()
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
StatusCode I4MomDumper::initialize()
{
  ATH_MSG_INFO( "Initializing " << name() << "..."  );

  ATH_MSG_INFO( "Configured to dump [" << m_i4momContainersName.value().size()
                << "] containers:" );
  for ( std::vector<std::string>::const_iterator 
	  itr = m_i4momContainersName.value().begin(),
	  iEnd= m_i4momContainersName.value().end();
	itr != iEnd;
	++itr )
  {
    ATH_MSG_INFO( " - " << *itr  );
  }
  ATH_MSG_INFO( "Display : ["     << s_display[m_display] << "]\n"
                << "OutputStream : " << m_outputStreamName.value() );

  return StatusCode::SUCCESS;
}

StatusCode I4MomDumper::finalize()
{
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  return StatusCode::SUCCESS;
}

StatusCode I4MomDumper::execute()
{  
  ATH_MSG_DEBUG( "Executing " << name() << "..." );

  typedef std::vector<std::string>::const_iterator ContNameIterator;

  for ( ContNameIterator 
	  itr  = m_i4momContainersName.value().begin(),
	  iEnd = m_i4momContainersName.value().end();
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
I4MomDumper::dump( const std::string& collName )
{
  typedef INavigable4MomentumCollection I4Moms_t;
  if ( !evtStore()->contains<I4Moms_t>( collName ) ) {
    ATH_MSG_WARNING( "No [" << collName
                     << "] I4MomentumContainer in StoreGate !" );
    return StatusCode::RECOVERABLE;
  }

  const I4Moms_t * coll = 0;
  if ( !evtStore()->retrieve( coll, collName ).isSuccess() ||
       0 == coll )
  {
    ATH_MSG_WARNING( "Could not retrieve any I4MomentumContainer at ["
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
  typedef std::set<const I4Momentum*, P4Sorters::Descending::Pt> Coll_t;
  Coll_t i4moms( coll->begin(), coll->end() );

  if ( m_display == Display::EEtaPhiM ) {
    out << "[e,eta,phi,m] ="
	<< std::right << std::scientific << std::setprecision(8);
    for ( Coll_t::const_iterator itr = i4moms.begin(),
	    iEnd = i4moms.end();
	  itr != iEnd;
	  ++itr ) {
      out << std::setw(16) << (*itr)->e()
	  << std::setw(16) << (*itr)->eta()
	  << std::setw(16) << (*itr)->phi()
	  << std::setw(16) << (*itr)->m()
	  << "\n";
    }

  } else if ( m_display == Display::IPtCotThPhiM ) {
    out << "[ipt,cotTh,phi,m] ="
	<< std::right << std::scientific << std::setprecision(8);
    for ( Coll_t::const_iterator itr = i4moms.begin(),
	    iEnd = i4moms.end();
	  itr != iEnd;
	  ++itr ) {
      out << std::setw(16) << (*itr)->iPt()
	  << std::setw(16) << (*itr)->cotTh()
	  << std::setw(16) << (*itr)->phi()
	  << std::setw(16) << (*itr)->m()
	  << "\n";
    }

  } else if ( m_display == Display::PtEtaPhiM ) {
    out << "[pt,eta,phi,m] ="
	<< std::right << std::scientific << std::setprecision(8);
    for ( Coll_t::const_iterator itr = i4moms.begin(),
	    iEnd = i4moms.end();
	  itr != iEnd;
	  ++itr ) {
      out << std::setw(16) << (*itr)->pt()
	  << std::setw(16) << (*itr)->eta()
	  << std::setw(16) << (*itr)->phi()
	  << std::setw(16) << (*itr)->m()
	  << "\n";
    }
  } else {
    out << "[px,py,pz,e] ="
	<< std::right << std::scientific << std::setprecision(8);
    for ( Coll_t::const_iterator itr = i4moms.begin(),
	    iEnd = i4moms.end();
	  itr != iEnd;
	  ++itr ) {
      out << std::setw(16) << (*itr)->px()
	  << std::setw(16) << (*itr)->py()
	  << std::setw(16) << (*itr)->pz()
	  << std::setw(16) << (*itr)->e()
	  << "\n";
    }
  }
  out << "\n";

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

void I4MomDumper::setupDisplay( Property& /*displayName*/ )
{
  std::string& display = const_cast<std::string&>(m_displayName.value());

  // get the stream in lower case
  std::transform( display.begin(), display.end(),
		  display.begin(),
		  ToLower() );

  if        ( display == "eetaphim" )     { m_display = Display::EEtaPhiM;
  } else if ( display == "iptcotthphim" ) { m_display = Display::IPtCotThPhiM;
  } else if ( display == "ptetaphim" )    { m_display = Display::PtEtaPhiM;
  } else if ( display == "pxpypze" )      { m_display = Display::PxPyPzE;
  } else {
    ATH_MSG_WARNING( "Unknown value for display [" << m_displayName.value() << "] !!\n"
                     << " => will use [pxpypxe] instead..." );
    display = "pxpypze";
    m_display = Display::PxPyPzE;
  }

  return;
}

void I4MomDumper::setupOutputStream( Property& /*outputStreamName*/ )
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
