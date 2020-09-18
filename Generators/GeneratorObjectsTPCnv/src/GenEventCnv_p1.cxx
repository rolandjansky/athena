///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// GenEventCnv_p1.cxx 
// Implementation file for class GenEventCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/VectorMap.h"

// GeneratorObjectsTPCnv includes
#include "GeneratorObjectsTPCnv/GenEventCnv_p1.h"
#include "HepMcDataPool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

GenEventCnv_p1::GenEventCnv_p1( HepMC::DataPool* pool ) :
  m_pool( pool )
{}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
  
void GenEventCnv_p1::setDataPool( HepMC::DataPool* pool )
{ 
  m_pool = pool; 
}

void GenEventCnv_p1::persToTrans( const GenEvent_p1* persObj, 
				  HepMC::GenEvent* transObj, 
				  MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading HepMC::GenEvent from persistent state..."
      << endmsg;

  if ( 0 == m_pool ) {
    msg << MSG::ERROR
	<< "This instance of GenEventCnv_p1 has a null pointer to "
	<< "HepMC::DataPool !" << endmsg
	<< "This probably means the T/P converter (McEventCollectionCnv_pX) "
	<< "is misconfigured !!"
	<< endmsg;
    throw std::runtime_error("Null pointer to HepMC::DataPool !!");
  }

  const unsigned int nVertices = persObj->m_vertices.size();
  if ( m_pool->vtx.capacity() - m_pool->vtx.allocated() < nVertices ) {
    m_pool->vtx.reserve( m_pool->vtx.allocated() + nVertices );
  }
  const unsigned int nParts = persObj->m_particles.size();
  if ( m_pool->part.capacity() - m_pool->part.allocated() < nParts ) {
    m_pool->part.reserve( m_pool->part.allocated() + nParts );
  }

  transObj->set_signal_process_id( persObj->m_signalProcessId );
  transObj->set_event_number( persObj->m_eventNbr );
  transObj->set_event_scale ( persObj->m_eventScale );
  transObj->set_alphaQCD    ( persObj->m_alphaQCD );
  transObj->set_alphaQED    ( persObj->m_alphaQED );

  transObj->weights() = persObj->m_weights;
  transObj->set_random_states( persObj->m_randomStates );

  // this is b/c, using DataPool(s), we are recycling old instances.
  // => better have a clean new-old instance :)
  transObj->m_vertex_barcodes.clear();
  transObj->m_particle_barcodes.clear();

  transObj->m_pdf_info = 0;         //> not available at that time...

  // create a temporary map associating the barcode of an end-vtx to its 
  // particle.
  // As not all particles are stable (d'oh!) we take 50% of the number of
  // particles as an initial size of the hash-map (to prevent re-hash)
  ParticlesMap_t partToEndVtx(nParts/2);

  // create the vertices
  for ( unsigned int iVtx = 0; iVtx != nVertices; ++iVtx ) {
    const GenVertex_p1& persVtx = persObj->m_vertices[iVtx];
    transObj->add_vertex( createGenVertex( *persObj, persVtx, partToEndVtx ) );
  } //> end loop over vertices

  // set the signal process vertex
  const int sigProcVtx = persObj->m_signalProcessVtx;
  if ( sigProcVtx != 0 ) {
    HepMC::set_signal_process_vertex(transObj, HepMC::barcode_to_vertex(transObj,sigProcVtx ) );
  }

  // connect particles to their end vertices
  const ParticlesMap_t::iterator endItr= partToEndVtx.end();
  for ( ParticlesMap_t::iterator p = partToEndVtx.begin(); 
	p != endItr; 
	++p ) {
    auto decayVtx = HepMC::barcode_to_vertex(transObj, p->second );
    if ( decayVtx ) {
      decayVtx->add_particle_in( p->first );
    } else {
      msg << MSG::ERROR
	  << "GenParticle points to null end vertex !!" 
	  << endmsg;
    }
  }

  msg << MSG::DEBUG << "Loaded HepMC::GenEvent from persistent state [OK]"
      << endmsg;
  return;
}

void GenEventCnv_p1::transToPers( const HepMC::GenEvent*, 
				  GenEvent_p1*, 
				  MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Creating persistent state of HepMC::GenEvent..."
      << endmsg;

  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endmsg
      << "You are not supposed to end-up here ! Go away !"
      << endmsg;

  throw std::runtime_error( "Retired GenEventCnv_p1::transToPers() !!" );
  return;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

HepMC::GenVertexPtr 
GenEventCnv_p1::createGenVertex( const GenEvent_p1& persEvt,
				 const GenVertex_p1& persVtx,
				 ParticlesMap_t& partToEndVtx ) const
{
  HepMC::GenVertexPtr vtx = m_pool->vtx.nextElementPtr();
  vtx->m_position.setX( persVtx.m_x );
  vtx->m_position.setY( persVtx.m_y );
  vtx->m_position.setZ( persVtx.m_z );
  vtx->m_position.setT( persVtx.m_t );
  vtx->m_particles_in.clear();
  vtx->m_particles_out.clear();
  vtx->m_id      = persVtx.m_id;
  vtx->m_weights = persVtx.m_weights;
  vtx->m_event   = 0;
  vtx->m_barcode = persVtx.m_barcode;
  
  // handle the in-going (orphans) particles
  const unsigned int nPartsIn = persVtx.m_particlesIn.size();
  for ( unsigned int i = 0; i != nPartsIn; ++i ) {
     createGenParticle( persEvt.m_particles[persVtx.m_particlesIn[i]], 
			partToEndVtx );
  }
  
  // now handle the out-going particles
  const unsigned int nPartsOut = persVtx.m_particlesOut.size();
  for ( unsigned int i = 0; i != nPartsOut; ++i ) {
     vtx->add_particle_out( createGenParticle( persEvt.m_particles[persVtx.m_particlesOut[i]],
					       partToEndVtx ) );
  }

  return vtx;
}

HepMC::GenParticlePtr 
GenEventCnv_p1::createGenParticle( const GenParticle_p1& persPart,
				   ParticlesMap_t& partToEndVtx ) const
{


  HepMC::GenParticlePtr p = m_pool->part.nextElementPtr();
  p->m_momentum.setPx( persPart.m_px  );
  p->m_momentum.setPy( persPart.m_py  );
  p->m_momentum.setPz( persPart.m_pz  );
  p->m_momentum.setE ( persPart.m_ene );
  p->m_pdg_id               = persPart.m_pdgId;
  p->m_status               = persPart.m_status;
  p->m_polarization.m_theta = persPart.m_thetaPolarization;
  p->m_polarization.m_phi   = persPart.m_phiPolarization;
  p->m_production_vertex    = 0;
  p->m_end_vertex           = 0;
  p->m_barcode              = persPart.m_barcode;

  // fillin' the flow
  const unsigned int nFlow = persPart.m_flow.size();
  for ( unsigned int iFlow= 0; iFlow != nFlow; ++iFlow ) {
    p->m_flow.set_icode( persPart.m_flow[iFlow].first, 
			 persPart.m_flow[iFlow].second );
  }

  if ( persPart.m_endVtx != 0 ) {
    partToEndVtx[p] = persPart.m_endVtx;
  }

  return p;
}

