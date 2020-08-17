/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// HepMcTupleWriterTool.cxx 
// Implementation file for class HepMcTupleWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
//#include <sstream>

// FrameWork includes
#include "GaudiKernel/ITHistSvc.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/GaudiException.h"

// ROOT includes
#include "TTree.h"

// CLHEP includes
#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Units/SystemOfUnits.h"

// McParticleTools includes
#include "HepMcTupleWriterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
HepMcTupleWriterTool::HepMcTupleWriterTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent ) : 
  AthAlgTool( type, name, parent ),
  m_tupleSvc ( "THistSvc",     name ),
  m_tuple    ( 0 )
{
  //
  // Property declaration
  // 

  declareProperty( "Output", 
		   m_outputFileName = "hepmc.root", 
		   "Name of the output file which will contain the HepMC tuple"
		   "\nEx: hepmc.root" );
  m_outputFileName.declareUpdateHandler( &HepMcTupleWriterTool::setupBackend,
					 this );

  declareProperty( "OutputStream", 
		   m_outputStreamName = "hepmc", 
		   "Name of the stream which will contain the HepMC tuple"
		   "\nEx: hepmc" );

  declareProperty( "McEvents",
		   m_mcEventsName = "GEN_EVENT",
		   "Input location of the McEventCollection to write out" );

  declareInterface<IIOHepMcTool>(this);
}

/// Destructor
///////////////
HepMcTupleWriterTool::~HepMcTupleWriterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode HepMcTupleWriterTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // Get pointer to StoreGateSvc and cache it :
  if ( !evtStore().retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }

  // Get pointer to ITHistSvc and cache it :
  if ( !m_tupleSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to ITHistSvc");
    return StatusCode::FAILURE;
  }

  // setup backend
  setupBackend(m_outputFileName);

  // book tuple
  bookTuple();

  return StatusCode::SUCCESS;
}

StatusCode HepMcTupleWriterTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode HepMcTupleWriterTool::execute()
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
    ATH_MSG_WARNING("McEventCollection at ["<<m_mcEventsName<<"] is EMPTY !!");
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

StatusCode HepMcTupleWriterTool::write( const HepMC::GenEvent* evt )
{
  m_particles.m_nParticles = std::min<std::size_t>( s_nMax,
						    evt->particles_size() );
  std::size_t i = 0;
  for ( HepMC::GenEvent::particle_const_iterator 
	  p = evt->particles_begin(),
	  pEnd = evt->particles_end();
	p != pEnd && i != static_cast<std::size_t>(m_particles.m_nParticles);
	++p,++i ) {
    const HepMC::FourVector mom = (*p)->momentum();
    m_particles.m_px [i] = mom.px();
    m_particles.m_py [i] = mom.py();
    m_particles.m_pz [i] = mom.pz();
    m_particles.m_m  [i] = mom.m();
    m_particles.m_ene[i] = mom.e();

    m_particles.m_pdgId[i]   = (*p)->pdg_id();
    m_particles.m_status[i]  = (*p)->status();
    m_particles.m_barcode[i] = (*p)->barcode();
  }

  // commit event
  m_tuple->Fill();

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

void HepMcTupleWriterTool::setupBackend( Gaudi::Details::PropertyBase& /*m_outputFileName*/ )
{
  const bool createIf = false;
  IProperty * tSvc = 0;
  if ( !service( m_tupleSvc.name(), tSvc, createIf ).isSuccess() ) {
    ATH_MSG_ERROR("Could not retrieve THistSvc handle !!");
    throw GaudiException( "Could not retrieve THistSvc", 
			  name(), 
			  StatusCode::FAILURE );
  }

  const std::string streamName = m_outputStreamName.value();

  const std::string propName = "Output";
  StringArrayProperty outputFileName;
  outputFileName.assign( tSvc->getProperty( propName ) );
  std::vector<std::string> updatedProp( outputFileName.value() );
  updatedProp.push_back
    ( streamName+" DATAFILE='"+m_outputFileName.value()+"' "
      "TYP='ROOT' "
      "OPT='RECREATE'" 
      );
  outputFileName.set( updatedProp );
  outputFileName.setName( propName );
  if ( !tSvc->setProperty( outputFileName ).isSuccess() ) {
    ATH_MSG_ERROR("Could not configure the THistSvc's output filename ["
		  << m_outputFileName.value() << "] !!");
    throw GaudiException( "Could not configure THistSvc output file !!", 
			  name(),
			  StatusCode::FAILURE );
  }

  return;
}

void HepMcTupleWriterTool::bookTuple()
{
  const std::string streamName = m_outputStreamName.value();
  TTree* t = new TTree("hepmc","HepMC validation tuple");
  if ( !m_tupleSvc->regTree( "/" + streamName + "/hepmc", t ).isSuccess() ) {
    ATH_MSG_ERROR("Could not register HepMC validation tuple !!");
    delete t; t = 0;
    throw GaudiException( "Could not register HepMC validation tuple !!",
			  name(),
			  StatusCode::FAILURE );
  }

  // booking branches
  t->Branch( "nParts", &m_particles.m_nParticles, "nParts/I" );

  t->Branch( "px", m_particles.m_px.elems,  "px[nParts]/D" );
  t->Branch( "py", m_particles.m_py.elems,  "py[nParts]/D" );
  t->Branch( "pz", m_particles.m_pz.elems,  "pz[nParts]/D" );
  t->Branch( "m",  m_particles.m_m.elems,   "m[nParts]/D"  );
  t->Branch( "e",  m_particles.m_ene.elems, "e[nParts]/D"  );

  t->Branch( "pdgId", m_particles.m_pdgId.elems,   "pdgId[nParts]/I" );
  t->Branch( "sc",    m_particles.m_status.elems,  "sc[nParts]/I" );
  t->Branch( "bc",    m_particles.m_barcode.elems, "bc[nParts]/I" );

  m_tuple = t;
  return;
}
