/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// HepMcFloatWriterTool.cxx 
// Implementation file for class HepMcFloatWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <limits>

// FrameWork includes

#include "GeneratorObjects/McEventCollection.h"
#include "HepPDT/ParticleDataTable.hh"

// McParticleTools includes
#include "HepMcFloatWriterTool.h"

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
HepMcFloatWriterTool::HepMcFloatWriterTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_ioBackend( 0 )
{
  //
  // Property declaration
  // 

  declareProperty( "Output", 
		   m_ioBackendURL = "ascii:hepmc.genevent.txt", 
		   "Name of the back-end we'll use to write out the HepMC::GenEvent."
		   "\nEx: ascii:hepmc.genevent.txt" );
  m_ioBackendURL.declareUpdateHandler( &HepMcFloatWriterTool::setupBackend,
				       this );
  
  declareProperty( "McEvents",
		   m_mcEventsName = "GEN_EVENT",
		   "Input location of the McEventCollection to write out" );

  declareInterface<IIOHepMcTool>(this);
}

/// Destructor
///////////////
HepMcFloatWriterTool::~HepMcFloatWriterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");

  if ( m_ioBackend ) {
    delete m_ioBackend;
    m_ioBackend = 0;
  }
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode HepMcFloatWriterTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // setup backend
  if ( 0 == m_ioBackend ) {
    setupBackend(m_ioBackendURL);
  }

  (*m_ioBackend) << "# epsilon<float> = " 
		 << std::numeric_limits<float>::epsilon()
		 << "\n"
		 << "# dbl precision  = "
		 << std::numeric_limits<double>::digits10 + 1
		 << "\n"
		 << "# flt precision  = "
		 << std::numeric_limits<float>::digits10 + 1
		 << "\n";

  return StatusCode::SUCCESS;
}

StatusCode HepMcFloatWriterTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode HepMcFloatWriterTool::execute()
{
  // retrieve the McEventCollection
  const McEventCollection * mcEvts = 0;
  if ( evtStore()->retrieve( mcEvts, m_mcEventsName ).isFailure() ||
       0 == mcEvts ) {
    ATH_MSG_ERROR("Could not retrieve a McEventCollection at ["
		  << m_mcEventsName << "] !!");
    return StatusCode::FAILURE;
  }

  if ( mcEvts->empty() ) {
    ATH_MSG_WARNING("McEventCollection at [" << m_mcEventsName
		    << "] is EMPTY !!");
    return StatusCode::FAILURE;
  }

  const HepMC::GenEvent * evt = mcEvts->front();
  if ( !evt ) {
    ATH_MSG_ERROR("Retrieved NULL pointer to HepMC::GenEvent !!");
    return StatusCode::FAILURE;
  }

  return write(evt);
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode HepMcFloatWriterTool::write( const HepMC::GenEvent* evt )
{
  //m_ioBackend->write_comment( m_mcEventsName.value() );
  //m_ioBackend->write_event(evt);
  std::ostringstream out;

  // precision 8 (# digits following decimal point) is the minimum that
  //  will capture the full information stored in a float
  out.precision( std::numeric_limits<double>::digits10 + 1 );
  // we use decimal to store integers, because it is smaller than hex!
  out.setf(std::ios::dec,std::ios::basefield);
  out.setf(std::ios::scientific,std::ios::floatfield);

  std::vector<long int> random_states = evt->random_states();

  out << "# -- GenEvent -->\n";
  out << "#" << evt->event_number() 
      << " " << evt->event_scale() 
      << " " << evt->alphaQCD() 
      << " " << evt->alphaQED()
      << " " << evt->signal_process_id()
      << " " << ( evt->signal_process_vertex() ?
		  evt->signal_process_vertex()->barcode() : 0 )
      << " " << evt->vertices_size()
      << " " << random_states.size()
      << "\n";
  out << "#";
  std::copy( random_states.begin(), random_states.end(),
	     std::ostream_iterator<long int>(out, " ") );
  out << evt->weights().size() << "\n";

  out << "#";
  std::copy( evt->weights().begin(), evt->weights().end(),
	     std::ostream_iterator<double>(out, " ") );
  out << '\n';

  out << "#-- particles --\n";
  for ( HepMC::GenEvent::particle_const_iterator 
	  i    = evt->particles_begin(),
	  iEnd = evt->particles_end();
	i != iEnd;
	++i ) {
    const HepMC::GenParticle * p = *i;
    if ( p ) {
      out << "# " << p->barcode() << " " << p->pdg_id() << "\n";
      
      const HepMC::FourVector mom = p->momentum();
      std::ostringstream buf;
      buf.precision( std::numeric_limits<float>::digits10 + 1 );
      buf.setf(std::ios::dec,std::ios::basefield);
      buf.setf(std::ios::scientific,std::ios::floatfield);
      
      const float px = static_cast<float>(mom.px());
      const float py = static_cast<float>(mom.py());
      const float pz = static_cast<float>(mom.pz());
      const float m  = static_cast<float>(mom.m());
      const float e  = 
	static_cast<float>(std::sqrt( std::pow( px, 2 ) +
				      std::pow( py, 2 ) +
				      std::pow( pz, 2 ) +
				      std::pow( m,  2 ) ) );
      buf << px << " " << py << " " << pz << " " << e 
	  << " " << m 
	  << "\n";

      out << buf.str();
      out << "# "<< p->status() 
	  << " " << p->polarization().theta()
	  << " " << p->polarization().phi()
	  << " " << ( p->end_vertex() ? p->end_vertex()->barcode() : 0 )
	  << " " << p->flow() 
	  << "\n";
    }
  }

  out << "#-- vertices -- \n";
  for ( HepMC::GenEvent::vertex_const_iterator 
	  i    = evt->vertices_begin(),
	  iEnd = evt->vertices_end();
	i != iEnd;
	++i ) {
    const HepMC::GenVertex * v = *i;
    if ( v ) {
      out << "# " << v->barcode() << " " << v->id() << "\n";

      const HepMC::FourVector pos = v->position();
      std::ostringstream buf;
      buf.precision( std::numeric_limits<float>::digits10 + 1 );
      buf.setf(std::ios::dec,std::ios::basefield);
      buf.setf(std::ios::scientific,std::ios::floatfield);
      
      buf << pos.x() << " " << pos.y() << " " << pos.z() << " "
	  << pos.t() << "\n";

      out << buf.str();
      out << "#";
      std::copy( v->weights().begin(), v->weights().end(),
		 std::ostream_iterator<double>(out, " ") );
      out << '\n';
    }
  }
  out << "#<-- GenEvent --\n";

  (*m_ioBackend) << out.str() << std::flush;
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

void HepMcFloatWriterTool::setupBackend( Gaudi::Details::PropertyBase& /*prop*/ )
{
  // defaults
  std::string protocol = "ascii";
  std::string fileName = "hepmc.genevent.txt";

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
