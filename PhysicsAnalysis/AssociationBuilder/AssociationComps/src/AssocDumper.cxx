///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AssocDumper.cxx 
// Implementation file for class AssocDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <iomanip> 
#include <fstream>
#include <sstream>
#include <ostream>

// FrameWork includes
#include "Gaudi/Property.h"

// CLHEP includes
#include "AthenaKernel/Units.h"

// NavFourMom includes
#include "NavFourMom/INav4MomAssocs.h"

// FourMomUtils includes
#include "FourMomUtils/P4DescendingSorters.h"

// AssociationComps includes
#include "AssocDumper.h"

using Athena::Units::GeV;

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
AssocDumper::AssocDumper( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm   ( name, pSvcLocator ),
  m_outputStream ( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr = "";

  descr = "StoreGate location of the INav4MomAssocs we want to print out";
  declareProperty( "INav4MomAssocs", 
		   m_inav4MomAssocsName = "",
		   descr );
  m_inav4MomAssocsName.declareUpdateHandler( &AssocDumper::setupINav4MomAssocs,
					     this );

  descr  = "Name of the output stream where we'll dump informations from the";
  descr += " INav4MomAssocs object.";
  descr += " Valid stream names are: \n";
  descr += "  - \"MsgStream\"\n";
  descr += "  - \"stderr\"\n";
  descr += "  - \"stdout\"\n";
  descr += "  - \"/path/to/some/file\".";

  declareProperty( "OutputStream",
		   m_outputStreamName = "MsgStream",
		   descr );
  m_outputStreamName.declareUpdateHandler( &AssocDumper::setupOutputStream,
					   this );
}

// Destructor
///////////////
AssocDumper::~AssocDumper()
{ 
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
StatusCode AssocDumper::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // setup our properties
  setupINav4MomAssocs( m_inav4MomAssocsName );
  setupOutputStream( m_outputStreamName );

  ATH_MSG_INFO
    ("Configured properties: " << endmsg
     << " - INav4MomAssocs: " << m_inav4MomAssocsName.value()
     << endmsg
     << " - OutputStream  : " << m_outputStreamName.value());

  return StatusCode::SUCCESS;
}

StatusCode AssocDumper::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode AssocDumper::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const INav4MomAssocs* assocs = 0;
  if ( !evtStore()->retrieve( assocs, 
			      m_inav4MomAssocsName.value() ).isSuccess() ||
       0 == assocs ) {
    ATH_MSG_WARNING
      ("Could not retrieve an INav4MomAssocs at ["
       << m_inav4MomAssocsName.value()
       << "] !!");
    return StatusCode::SUCCESS;
  }

  if ( !dump(assocs).isSuccess() ) {
    ATH_MSG_WARNING 
      ("Could not dump the content of INav4MomAssocs at ["
       << m_inav4MomAssocsName.value() << "] !!");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode AssocDumper::dump( const INav4MomAssocs* assocs )
{
  if ( 0 == assocs ) {
    ATH_MSG_WARNING ("Null pointer to INav4MomAssocs !");
    return StatusCode::RECOVERABLE;
  }

  std::stringstream out;
  out << "Store of associations: [" << m_inav4MomAssocsName.value() << "]"
      << " (size = " << assocs->size() << ")"
      << std::endl;

  // definition of the output format
  const unsigned int p = 6; // precision
  const unsigned int m = 9; // mantissa + space
  const unsigned int w = p + m;

  const std::string::size_type nCols = 80;
  const char *  lineElem = "-";
  const char * blineElem = "=";

  const std::string  line( nCols,  *lineElem );
  const std::string bline( nCols, *blineElem );

  out << "type"
      << std::setw(w) <<    "Px     " 
      << std::setw(w) <<    "Py     " 
      << std::setw(w) <<    "Pz     " 
      << std::setw(w) << "Ene (GeV) "
      << std::setw(w) <<   " #Assocs" 
      << std::endl
      << bline 
      << std::endl;

  typedef P4Sorters::Descending::Pt                                Sorter_t;
  typedef std::set< const INavigable4Momentum*, Sorter_t >         Coll_t;
  typedef std::map< const INavigable4Momentum*, Coll_t, Sorter_t > Map_t;

  Map_t sAssocs;
  for ( INav4MomAssocs::object_iterator 
	  objItr = assocs->beginObject(),
	  objEnd = assocs->endObject();
	objItr != objEnd;
	++objItr ) {
    const INavigable4Momentum* o = objItr.getObject();
    sAssocs[ o ] = Coll_t( assocs->beginAssociation(*objItr),
			   assocs->endAssociation  (*objItr) );
  }

  for ( Map_t::const_iterator 
	  objItr = sAssocs.begin(),
	  objEnd = sAssocs.end();
	objItr != objEnd;
	++objItr ) {
    const INavigable4Momentum* o = objItr->first;
    const Coll_t& assoc          = objItr->second;
    out << "obj:"
	<< std::fixed
	<< std::scientific
	<< std::setprecision(p) << std::setw(w) << o->px() / GeV
	<< std::setprecision(p) << std::setw(w) << o->py() / GeV
	<< std::setprecision(p) << std::setw(w) << o->pz() / GeV
	<< std::setprecision(p) << std::setw(w) << o->e()  / GeV
	<< std::setw(w) << assoc.size()
	<< std::endl
	<< line
	<< std::endl;
    for ( Coll_t::const_iterator 
	    assItr = assoc.begin(),
	    assEnd = assoc.end();
	  assItr != assEnd;
	  ++assItr ) {
      out << "ass:"
	  << std::fixed
	  << std::scientific
	  << std::setprecision(p) << std::setw(w) << (*assItr)->px() / GeV
	  << std::setprecision(p) << std::setw(w) << (*assItr)->py() / GeV
	  << std::setprecision(p) << std::setw(w) << (*assItr)->pz() / GeV
	  << std::setprecision(p) << std::setw(w) << (*assItr)->e()  / GeV
	  << std::endl;
    }
    out << bline << std::endl;
  }
  
  if ( m_outputStream ) {
    (*m_outputStream) << out.str() << std::endl;
  } else {
    ATH_MSG_INFO (out.str());
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

void AssocDumper::setupINav4MomAssocs( Gaudi::Details::PropertyBase& /*inav4MomAssocsName*/ )
{
  // nothing to do (yet?)
  return;
}

void AssocDumper::setupOutputStream( Gaudi::Details::PropertyBase& /*outputStreamName*/ )
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
