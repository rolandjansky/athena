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
#include "AtlasHepMC/Flow.h"
#include "AtlasHepMC/Polarization.h"
static const char * s_protocolSep = ":";

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
  m_ioBackendURL.declareUpdateHandler( &HepMcFloatWriterTool::setupBackend,this );
  
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
  if ( evtStore()->retrieve( mcEvts, m_mcEventsName ).isFailure() || 0 == mcEvts ) {
    ATH_MSG_ERROR("Could not retrieve a McEventCollection at [" << m_mcEventsName << "] !!");
    return StatusCode::FAILURE;
  }

  if ( mcEvts->empty() ) {
    ATH_MSG_WARNING("McEventCollection at [" << m_mcEventsName << "] is EMPTY !!");
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
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode HepMcFloatWriterTool::write( const HepMC::GenEvent* evt )
{
  std::ostringstream out;

  // precision 8 (# digits following decimal point) is the minimum that
  //  will capture the full information stored in a float
  out.precision( std::numeric_limits<double>::digits10 + 1 );
  // we use decimal to store integers, because it is smaller than hex!
  out.setf(std::ios::dec,std::ios::basefield);
  out.setf(std::ios::scientific,std::ios::floatfield);


#ifdef HEPMC3
  long evt_vertices_size=evt->vertices().size();
  std::shared_ptr<HepMC3::DoubleAttribute> A_alphaQCD=evt->attribute<HepMC3::DoubleAttribute>("alphaQCD");
  double  evt_alphaQCD=(A_alphaQCD?(A_alphaQCD->value()):0.0);
  std::shared_ptr<HepMC3::DoubleAttribute> A_alphaQED=evt->attribute<HepMC3::DoubleAttribute>("alphaQED");
  double  evt_alphaQED=(A_alphaQED?(A_alphaQED->value()):0.0);
  std::shared_ptr<HepMC3::DoubleAttribute> A_event_scale=evt->attribute<HepMC3::DoubleAttribute>("event_scale");
  double  evt_event_scale=(A_event_scale?(A_event_scale->value()):0.0);
  std::shared_ptr<HepMC3::VectorLongIntAttribute> A_random_states=evt->attribute<HepMC3::VectorLongIntAttribute>("random_states");
  std::vector<long int> random_states=(A_random_states?(A_random_states->value()):std::vector<long int>());
  long random_states_size=random_states.size();
#else 
  long evt_vertices_size=evt->vertices_size();
  double  evt_alphaQCD=evt->alphaQCD();
  double  evt_alphaQED=evt->alphaQED();
  double  evt_event_scale=evt->event_scale();
  std::vector<long int> random_states=evt->random_states();
  long random_states_size=random_states.size();
#endif
  out << "# -- GenEvent -->\n";
  out << "#" << evt->event_number() 
      << " " << evt_event_scale 
      << " " << evt_alphaQCD 
      << " " << evt_alphaQED
      << " " << HepMC::signal_process_id(evt)
      << " " << ( HepMC::signal_process_vertex(evt) ? HepMC::barcode(HepMC::signal_process_vertex(evt)) : 0 )
      << " " << evt_vertices_size
      << " " << random_states_size
      << "\n";
  out << "#";
  std::copy( random_states.begin(), random_states.end(), std::ostream_iterator<long int>(out, " ") );
  out << evt->weights().size() << "\n";

  out << "#";
  std::copy( evt->weights().begin(), evt->weights().end(), std::ostream_iterator<double>(out, " ") );
  out << '\n';

  out << "#-- particles --\n";
  for (auto p: *evt) {
    if ( p ) {
      out << "# " << HepMC::barcode(p) << " " << p->pdg_id() << "\n";
      
      const HepMC::FourVector mom = p->momentum();
      std::ostringstream buf;
      buf.precision( std::numeric_limits<float>::digits10 + 1 );
      buf.setf(std::ios::dec,std::ios::basefield);
      buf.setf(std::ios::scientific,std::ios::floatfield);
      
      const float px = static_cast<float>(mom.px());
      const float py = static_cast<float>(mom.py());
      const float pz = static_cast<float>(mom.pz());
      const float m  = static_cast<float>(mom.m());
      const float e  = static_cast<float>(std::sqrt( std::pow( px, 2 ) + std::pow( py, 2 ) + std::pow( pz, 2 ) + std::pow( m,  2 ) ) );
      buf << px << " " << py << " " << pz << " " << e   << " " << m   << "\n";

      out << buf.str();
      auto pol=HepMC::polarization(p);
      out << "# "<< p->status() 
	  << " " << pol.theta()
	  << " " << pol.phi()
	  << " " << ( p->end_vertex() ? HepMC::barcode(p->end_vertex()) : 0 )
	  << " " << HepMC::flow(p) 
	  << "\n";
    }
  }

  out << "#-- vertices -- \n";
#ifdef HEPMC3
  for (auto v: evt->vertices()) {
    if ( v ) { 
      out << "# " << HepMC::barcode(v) << " " << v->status() << "\n";
      const HepMC::FourVector pos = v->position();
      std::ostringstream buf;
      buf.precision( std::numeric_limits<float>::digits10 + 1 );
      buf.setf(std::ios::dec,std::ios::basefield);
      buf.setf(std::ios::scientific,std::ios::floatfield);
      
      buf << pos.x() << " " << pos.y() << " " << pos.z() << " "  << pos.t() << "\n";

      out << buf.str();
      out << "#";
      std::string svertexeights("1.0"); 
      auto vertexeights=v->attribute<HepMC3::VectorDoubleAttribute>("weights");
      if (vertexeights) vertexeights->to_string(svertexeights);
      out << svertexeights;
      out << '\n';
    }
#else
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
      
      buf << pos.x() << " " << pos.y() << " " << pos.z() << " "  << pos.t() << "\n";

      out << buf.str();
      out << "#";
      std::copy( v->weights().begin(), v->weights().end(), std::ostream_iterator<double>(out, " ") );
      out << '\n';
    }
#endif
  }
  out << "#<-- GenEvent --\n";

  (*m_ioBackend) << out.str() << std::flush;
  return StatusCode::SUCCESS;
}

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
  std::transform( protocol.begin(), protocol.end(), protocol.begin(), [](unsigned char c){ return std::tolower(c); } );
  if ( "ascii" == protocol ) {
    m_ioBackend = new std::ofstream( fileName.c_str(), std::ios::out | std::ios::trunc );

  } else {
    ATH_MSG_WARNING("UNKNOWN protocol [" << protocol << "] !!" << endmsg << "Will use [ascii] instead...");
    protocol = "ascii";
    m_ioBackend = new std::ofstream( fileName.c_str(), std::ios::out | std::ios::trunc );
  }
  ATH_MSG_DEBUG("Using protocol [" << protocol << "] and write to ["<< fileName << "]");
  return;
}
