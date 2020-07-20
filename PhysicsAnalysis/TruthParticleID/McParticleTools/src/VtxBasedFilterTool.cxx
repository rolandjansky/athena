/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// VtxBasedFilterTool.cxx 
// Implementation file for class VtxBasedFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cmath>

// FrameWork includes

// CLHEP/HepMC includes
#include "TruthHelper/IsGenerator.h"
#include "TruthHelper/IsGenStable.h"
#include "CLHEP/Units/SystemOfUnits.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// McParticleTools includes
#include "VtxBasedFilterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace TruthHelper;

/// Constructors
////////////////
VtxBasedFilterTool::VtxBasedFilterTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
  TruthParticleFilterBaseTool( type, name, parent )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "MaxHardScatteringVtxBarcode", 
		   m_maxHardScatteringVtxBarcode = 30,
		   "Property to setup the maximum vertex barcode to look for "
		   "hard-scattering vertices" );

}

/// Destructor
///////////////
VtxBasedFilterTool::~VtxBasedFilterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode VtxBasedFilterTool::buildMcAod( const McEventCollection* in,
					McEventCollection* out )
{
  if ( 0 == in || 0 == out ) {
    msg(MSG::ERROR)
      << "Invalid pointer to McEventCollection !" << endmsg
      << "  in: " << in << endmsg
      << " out: " << out << endmsg;
    return StatusCode::FAILURE;
  }

  for ( unsigned int iEvt = 0; iEvt != in->size(); ++iEvt ) {
    const HepMC::GenEvent * inEvt = (*in)[iEvt];
    if ( 0 == inEvt ) {
      msg(MSG::WARNING)
        << "Could not launch filtering procedure for GenEvent number ["
        << iEvt << "] from McEventCollection ["
        << m_mcEventsReadHandleKey.key() << " !!"
        << endmsg
        << "  inEvt: " << inEvt << endmsg;
      continue;
    }
    HepMC::GenEvent* outEvt = new HepMC::GenEvent( inEvt->signal_process_id(), 
                                                   inEvt->event_number() );
    outEvt->set_event_scale  ( inEvt->event_scale() );
    outEvt->set_alphaQCD     ( inEvt->alphaQCD() );
    outEvt->set_alphaQED     ( inEvt->alphaQED() );
    outEvt->weights() =        inEvt->weights();
    outEvt->set_random_states( inEvt->random_states() );
    if ( 0 != inEvt->heavy_ion() ) {
      outEvt->set_heavy_ion    ( *inEvt->heavy_ion() );
    }
    if ( 0 != inEvt->pdf_info() ) {
      outEvt->set_pdf_info     ( *inEvt->pdf_info() );
    }

    if ( buildGenEvent( inEvt, outEvt ).isFailure() ) {
      msg(MSG::ERROR)
	<< "Could filter GenEvent number [" << iEvt 
	<< "] from McEventCollection ["     << m_mcEventsReadHandleKey.key() 
	<< "] !!" << endmsg;
      delete outEvt;
      outEvt = 0;
      continue;
    }

    out->push_back( outEvt );
  }

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

StatusCode VtxBasedFilterTool::buildGenEvent( const HepMC::GenEvent* in,
					      HepMC::GenEvent* out )
{
  if ( 0 == in || 0 == out ) {
    msg(MSG::ERROR)
      << "Invalid pointer to GenEvent !!" << endmsg
      << "  in: " << in << endmsg
      << " out: " << out << endmsg;
    return StatusCode::FAILURE;
  }

  // loop over vertices
  for ( HepMC::GenEvent::vertex_const_iterator vtx = in->vertices_begin();
	vtx != in->vertices_end(); 
	++vtx ) {

    if ( !isAccepted(*vtx) ) {
      // no in-going nor out-going particles at this vertex matches 
      // the requirements: ==> Skip it
      continue;
    }
    
    if ( addVertex( *vtx, out ).isFailure() ) {
      msg(MSG::WARNING)
	<< "Could not add vertex [" << (*vtx)->barcode() << "]" << endmsg;
    }
  } //> end loop over vertices
  
  return StatusCode::SUCCESS;
}

bool VtxBasedFilterTool::isAccepted( const HepMC::GenVertex* vtx ) const
{
  if ( 0 == vtx ) {
    return false;
  }

  // check if this vertex looks like a vertex from the hard-scattering
  if ( isFromHardScattering(vtx) ) {
    return true;
  }

  // check if this vertex matches some decay pattern given 
  // to the McVtxFilterTool
  if ( m_mcVtxFilterTool->isAccepted(vtx) ) {
    return true;
  }

  return false;
}

StatusCode VtxBasedFilterTool::addVertex( const HepMC::GenVertex* srcVtx,
				       HepMC::GenEvent* evt ) const
{
  if ( 0 == srcVtx || 0 == evt ) {
    msg(MSG::ERROR)
      << "In addVertex(vtx,evt) : INVALID pointer given !!" << endmsg
      << " vtx: " << srcVtx << endmsg
      << " evt: " << evt << endmsg;
    return StatusCode::FAILURE;
  }

  HepMC::GenVertex * vtx = evt->barcode_to_vertex(srcVtx->barcode());
  if ( 0 == vtx ) {
    vtx = HepMC::newGenVertexPtr();
    vtx->set_position( srcVtx->position() );
    vtx->set_id( srcVtx->id() );
    vtx->suggest_barcode( srcVtx->barcode() );
    vtx->weights() = srcVtx->weights();
    evt->add_vertex(vtx);
  }

  ////////////////////////////
  /// Fill the parent branch
  for ( HepMC::GenVertex::particles_in_const_iterator 
	  parent    = srcVtx->particles_in_const_begin(),
	  parentEnd = srcVtx->particles_in_const_end();
	parent != parentEnd;
	++parent ) {
    HepMC::GenParticle * p = evt->barcode_to_particle( (*parent)->barcode() );
    if ( 0 == p ) {
      p = HepMC::newGenParticlePtr();
      p->set_momentum( (*parent)->momentum() );
      p->set_generated_mass( (*parent)->generated_mass() );
      p->set_pdg_id( (*parent)->pdg_id() );
      p->set_status( (*parent)->status() );
      p->set_flow( (*parent)->flow() );
      p->set_polarization( (*parent)->polarization() );
      p->suggest_barcode( (*parent)->barcode() );
    }
    // set the mother's decay to our (new) vertex
    vtx->add_particle_in( p );
    
  }//> loop over ingoing particles
  
  //////////////////////////////
  /// Fill the children branch
  for ( HepMC::GenVertex::particles_out_const_iterator 
	  child = srcVtx->particles_out_const_begin(),
	  childEnd = srcVtx->particles_out_const_end();
	child != childEnd;
	++child ) {
    HepMC::GenParticle * p = evt->barcode_to_particle( (*child)->barcode() );
    if ( 0 == p ) {
      p = HepMC::newGenParticlePtr();
      p->set_momentum( (*child)->momentum() );
      p->set_generated_mass( (*child)->generated_mass() );
      p->set_pdg_id( (*child)->pdg_id() );
      p->set_flow( (*child)->flow() );
      p->set_polarization( (*child)->polarization() );
      p->suggest_barcode( (*child)->barcode() );
    }

    // set the daughter's production vertex to our new vertex
    vtx->add_particle_out( p );

  }//> loop over outgoing particles

  return StatusCode::SUCCESS;
}

bool 
VtxBasedFilterTool::isFromHardScattering( const HepMC::GenVertex* vtx ) const
{
  if ( std::abs(vtx->barcode()) <= m_maxHardScatteringVtxBarcode.value() &&
       m_ppFilter.isAccepted(vtx) &&
       ! m_showerFilter.isAccepted(vtx) ) {

    return true;
  } else {
    return false;
  }
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode VtxBasedFilterTool::initializeTool()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // configure hard-scattering vertices
  m_ppFilter.setDecayPattern( "-1|-2|-3|-4|-5|-6|21 + 1|2|3|4|5|6|21 -> " );
  
  // filter which will be used to not select shower vertices while
  // looking for hard-scattering vertices
  m_showerFilter.setDecayPattern( "-> 91|92|94" );

  return StatusCode::SUCCESS;
}
