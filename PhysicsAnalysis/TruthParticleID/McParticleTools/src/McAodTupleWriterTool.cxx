/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McAodTupleWriterTool.cxx 
// Implementation file for class McAodTupleWriterTool
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

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleTools includes
#include "McAodTupleWriterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McAodTupleWriterTool::McAodTupleWriterTool( const std::string& type, 
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
		   m_outputFileName = "mcaod.root", 
		   "Name of the output file which will contain the McAod "
		   "tuple. Ex: mcaod.root" );
  m_outputFileName.declareUpdateHandler( &McAodTupleWriterTool::setupBackend,
					 this );

  declareProperty( "OutputStream", 
		   m_outputStreamName = "mcaod", 
		   "Name of the stream which will contain the McAod tuple. "
		   "Ex: mcaod" );

  declareProperty( "TruthParticles",
		   m_truthParticlesName = "GEN_EVENT",
		   "Input location of the TruthParticleContainer to write "
		   "out." );

  declareInterface<IIOMcAodTool>(this);
}

/// Destructor
///////////////
McAodTupleWriterTool::~McAodTupleWriterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");

}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodTupleWriterTool::initialize()
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

StatusCode McAodTupleWriterTool::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode McAodTupleWriterTool::execute()
{
  // retrieve the TruthParticleContainer
  const TruthParticleContainer * mc = 0;
  if ( evtStore()->retrieve( mc, m_truthParticlesName ).isFailure() ||
       0 == mc ) {
    ATH_MSG_ERROR("Could not retrieve a TruthParticleContainer at ["
		  << m_truthParticlesName << "] !!");
    return StatusCode::FAILURE;
  }

  if ( mc->empty() ) {
    ATH_MSG_WARNING("TruthParticleContainer at [" << m_truthParticlesName
		    << "] is EMPTY !!");
    return StatusCode::FAILURE;
  }

  return write(mc);
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode McAodTupleWriterTool::write( const TruthParticleContainer* mc )
{
  m_particles.m_nParticles = std::min<std::size_t>( s_nMax, mc->size() );
  for ( std::size_t i = 0; i != m_particles.m_nParticles; ++i ) {
    const TruthParticle * p = (*mc)[i];
    m_particles.m_px [i] = p->px();
    m_particles.m_py [i] = p->py();
    m_particles.m_pz [i] = p->pz();
    m_particles.m_m  [i] = p->m();
    m_particles.m_ene[i] = p->e();

    m_particles.m_pdgId[i]    = p->pdgId();
    m_particles.m_status[i]   = p->status();
    m_particles.m_barcode[i]  = p->barcode();

    using namespace TruthParticleParameters;

    m_particles.m_etcone10[i] = p->etIsol(etcone10);
    m_particles.m_etcone20[i] = p->etIsol(etcone20);
    m_particles.m_etcone30[i] = p->etIsol(etcone30);
    m_particles.m_etcone40[i] = p->etIsol(etcone40);
    m_particles.m_etcone45[i] = p->etIsol(etcone  );
    m_particles.m_etcone50[i] = p->etIsol(etcone50);
    m_particles.m_etcone60[i] = p->etIsol(etcone60);
    m_particles.m_etcone70[i] = p->etIsol(etcone70);
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

void McAodTupleWriterTool::setupBackend( Gaudi::Details::PropertyBase& /*m_outputFileName*/ )
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

void McAodTupleWriterTool::bookTuple()
{
  delete m_tuple;
  const std::string streamName = m_outputStreamName.value();
  TTree* t = new TTree("mcaod","McAod validation tuple");
  if ( !m_tupleSvc->regTree( "/" + streamName + "/mcaod", t ).isSuccess() ) {
    ATH_MSG_ERROR("Could not register McAod validation tuple !!");
    delete t; t = 0;
    throw GaudiException( "Could not register McAod validation tuple !!",
			  name(),
			  StatusCode::FAILURE );
  }

  // booking branches
  t->Branch( "nParts", &m_particles.m_nParticles, "nParts/i" );

  t->Branch( "px", m_particles.m_px.elems,  "px[nParts]/D" );
  t->Branch( "py", m_particles.m_py.elems,  "py[nParts]/D" );
  t->Branch( "pz", m_particles.m_pz.elems,  "pz[nParts]/D" );
  t->Branch( "m",  m_particles.m_m.elems,   "m[nParts]/D"  );
  t->Branch( "e",  m_particles.m_ene.elems, "e[nParts]/D"  );

  t->Branch( "pdgId", m_particles.m_pdgId.elems,   "pdgId[nParts]/I" );
  t->Branch( "sc",    m_particles.m_status.elems,  "sc[nParts]/I" );
  t->Branch( "bc",    m_particles.m_barcode.elems, "bc[nParts]/I" );

  t->Branch( "etcone10",  m_particles.m_etcone10.elems, "etcone10[nParts]/D" );
  t->Branch( "etcone20",  m_particles.m_etcone20.elems, "etcone20[nParts]/D" );
  t->Branch( "etcone30",  m_particles.m_etcone30.elems, "etcone30[nParts]/D" );
  t->Branch( "etcone40",  m_particles.m_etcone40.elems, "etcone40[nParts]/D" );
  t->Branch( "etcone45",  m_particles.m_etcone45.elems, "etcone45[nParts]/D" );
  t->Branch( "etcone50",  m_particles.m_etcone50.elems, "etcone50[nParts]/D" );
  t->Branch( "etcone60",  m_particles.m_etcone60.elems, "etcone60[nParts]/D" );
  t->Branch( "etcone70",  m_particles.m_etcone70.elems, "etcone70[nParts]/D" );

  m_tuple = t;
  return;
}
