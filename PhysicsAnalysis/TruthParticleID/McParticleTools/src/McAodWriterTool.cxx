/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McAodWriterTool.cxx 
// Implementation file for class McAodWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <algorithm>
#include <cctype>
#include <iomanip>

// FrameWork includes

// HepMC includes
#include "AtlasHepMC/GenParticle.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleTools includes
#include "McAodWriterTool.h"

static const char * s_protocolSep = ":";

struct ToLower
{
  char operator() (char c) const  { return std::tolower(c); }
};

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McAodWriterTool::McAodWriterTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_ioBackend( 0 )
{
  //
  // Property declaration
  // 

  declareProperty( "Output", 
		   m_ioBackendURL = "ascii:mcaod.txt", 
		   "Name of the back-end we'll use to write out the "
		   "TruthParticleContainer.\nEx: ascii:mcaod.txt" );
  m_ioBackendURL.declareUpdateHandler( &McAodWriterTool::setupBackend,
				       this );

  declareProperty( "TruthParticles",
		   m_truthParticlesName = "SpclMC",
		   "Input location of the TruthParticleContainer to write out" );

  declareInterface<IIOMcAodTool>(this);
}

/// Destructor
///////////////
McAodWriterTool::~McAodWriterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");

  if ( m_ioBackend ) {
    delete m_ioBackend;
    m_ioBackend = 0;
  }
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodWriterTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  // Get pointer to StoreGateSvc and cache it :
  if ( !evtStore().retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }

  // setup backend
  if ( 0 == m_ioBackend ) {
    setupBackend(m_ioBackendURL);
  }

  return StatusCode::SUCCESS;
}

StatusCode McAodWriterTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  ATH_MSG_DEBUG("Closing backend...");
  m_ioBackend->close();

  return StatusCode::SUCCESS;
}

StatusCode McAodWriterTool::execute()
{
  // retrieve the TruthParticles
  const TruthParticleContainer * mcParts = 0;
  if ( evtStore()->retrieve( mcParts, m_truthParticlesName ).isFailure() ||
       0 == mcParts ) {
    ATH_MSG_WARNING("Could not retrieve a TruthParticleContainer at ["
		    << m_truthParticlesName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  return write(mcParts);
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode McAodWriterTool::write( const TruthParticleContainer* mcParts )
{
  if ( 0 == mcParts ) {
    ATH_MSG_WARNING("NULL pointer to TruthParticleContainer !!");
    return StatusCode::RECOVERABLE;
  }

  std::ofstream& out = *m_ioBackend;
  out << std::fixed;

  out << "---" << std::setw(5) << mcParts->size() << std::endl;
  for ( unsigned int i = 0; i != mcParts->size(); ++i ) {
    const TruthParticle* mc = (*mcParts)[i];
    out << "P " << std::setprecision(3) << std::setw(10) << mc->px()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->py()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->pz()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->e()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->pdgId()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->charge()
	<< " "  << std::setprecision(3) << std::setw(10) << mc->genParticle()->barcode()
	<< std::endl;
    out << "EtIsol " 
	<< std::setw(3) << TruthParticleParameters::NbrOfCones
	<< " : ";
    for ( unsigned int iCone = 0; 
	  iCone != TruthParticleParameters::NbrOfCones;
	  ++iCone ) {
      out << " " << std::setprecision(3) << std::setw(10) 
	  << mc->etIsol(static_cast<TruthParticleParameters::ConeSize>(iCone));
    }
    out << std::endl;
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void McAodWriterTool::setupBackend( Gaudi::Details::PropertyBase& /*prop*/ )
{
  // defaults
  std::string protocol = "ascii";
  std::string fileName = "mcaod.txt";

  // reset internal state
  if ( m_ioBackend ) {
    delete m_ioBackend;
    m_ioBackend = 0;
  }

  // caching URL
  const std::string& url = m_ioBackendURL.value();
  
  std::string::size_type protocolPos = url.find(s_protocolSep);

  if ( std::string::npos != protocolPos ) {
    protocol = url.substr( 0, protocolPos );
    fileName = url.substr( protocolPos+1, std::string::npos );
  } else {
    //protocol = "ascii";
    fileName = url;
  }

  // get the protocol name in lower cases
  std::transform( protocol.begin(), protocol.end(), 
		  protocol.begin(),
		  ToLower() );

  if ( "ascii" == protocol ) {
    m_ioBackend = new std::ofstream( fileName.c_str(),
				     std::ios::out | std::ios::trunc );

  } else {
    ATH_MSG_WARNING("UNKNOWN protocol [" << protocol << "] !!" << endmsg
		    << "Will use [ascii] instead...");
    protocol = "ascii";
    m_ioBackend = new std::ofstream( fileName.c_str(),
				     std::ios::out | std::ios::trunc );
  }    

  ATH_MSG_DEBUG("Using protocol [" << protocol << "] and write to ["
		<< fileName << "]");
  return;
}
