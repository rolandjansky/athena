/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// EtaPtFilterTool.cxx 
// Implementation file for class EtaPtFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cmath>

// FrameWork includes

// CLHEP includes
#include "TruthHelper/IsGenerator.h"
#include "TruthHelper/IsGenStable.h"
#include "CLHEP/Units/SystemOfUnits.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// McParticleTools includes
#include "EtaPtFilterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace TruthHelper;
using CLHEP::GeV;

/// Constructors
////////////////
EtaPtFilterTool::EtaPtFilterTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
  TruthParticleFilterBaseTool( type, name, parent )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "InnerEtaRegionCuts", 
		   m_innerEtaRegionCuts,
		   "Vector of cuts parameters for the inner region.\n"
		   "Two first elements are minimum and maximum |eta| values "
		   "for this inner region.\n"
		   "Third element is the minimum pT for the stable particles "
		   "to be accepted." );
  // defaults
  std::vector<double> innerEtaRegionCuts;
  innerEtaRegionCuts.push_back( 0.0     ); // minimum |eta|
  innerEtaRegionCuts.push_back( 3.5     ); // maximum |eta|
  innerEtaRegionCuts.push_back( 0.3*GeV ); // minimum pT of particles
  m_innerEtaRegionCuts.set( innerEtaRegionCuts );

  m_innerEtaRegionCuts.declareUpdateHandler( &EtaPtFilterTool::setupInnerEtaRegionCuts,
					     this );
  
  declareProperty( "OuterEtaRegionCuts", 
		   m_outerEtaRegionCuts,
		   "Vector of cuts parameters for the outer region.\n"
		   "Two first elements are minimum and maximum |eta| values "
		   "for this outer region.\n"
		   "Third element is the minimum pT for the stable particles "
		   "to be accepted." );
  // defaults
  std::vector<double> outerEtaRegionCuts;
  outerEtaRegionCuts.push_back( 3.5     ); // minimum |eta|
  outerEtaRegionCuts.push_back( 5.5     ); // maximum |eta|
  outerEtaRegionCuts.push_back( 1.0*GeV ); // minimum pT of particles
  m_outerEtaRegionCuts.set( outerEtaRegionCuts );

  m_outerEtaRegionCuts.declareUpdateHandler( &EtaPtFilterTool::setupOuterEtaRegionCuts,
					     this );

  declareProperty( "MaxHardScatteringVtxBarcode", 
		   m_maxHardScatteringVtxBarcode = 20,
		   "Property to setup the maximum vertex barcode to look for "
		   "hard-scattering vertices" );

  // switches
  declareProperty( "OnlyGenerator",      
		   m_onlyGenerator = false,
		   "Switch to only include particles from generation and "
		   "reject particles from detector simulation (Geant4)" );


  declareProperty( "ButKeepAllGeneratorStable",      
		   m_butKeepAllGeneratorStable = true,
		   "Switch to keep all generated stable particles (they sum to "
		   "14 TeV) regardless of the eta and pt cuts defined for the "
		   "remaining particles" );


  declareProperty( "KeepDocumentaries",      
		   m_keepDocumentaries = true,
		   "Switch to keep *all* generator particles which are "
		   "documentaries (statuscode == 3)" );

}

/// Destructor
///////////////
EtaPtFilterTool::~EtaPtFilterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode EtaPtFilterTool::buildMcAod( const McEventCollection* in,
					McEventCollection* out )
{
  if ( 0 == in || 0 == out ) {
    ATH_MSG_ERROR("Invalid pointer to McEventCollection !" << endmsg
		  << "  in: " << in << endmsg
		  << " out: " << out);
    return StatusCode::FAILURE;
  }

  for ( unsigned int iEvt = 0; iEvt != in->size(); ++iEvt ) {
    const HepMC::GenEvent * inEvt = (*in)[iEvt];
    if ( 0 == inEvt ) {
      ATH_MSG_WARNING
        ("Could not launch filtering procedure for GenEvent number ["
         << iEvt << "] from McEventCollection ["
         << m_mcEventsReadHandleKey.key() << " !!"
         << endmsg
         << "  inEvt: " << inEvt);
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
      ATH_MSG_ERROR("Could filter GenEvent number [" << iEvt 
		    << "] from McEventCollection ["     << m_mcEventsReadHandleKey.key() 
		    << "] !!");
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

StatusCode EtaPtFilterTool::buildGenEvent( const HepMC::GenEvent* in,
					   HepMC::GenEvent* out )
{
  if ( 0 == in || 0 == out ) {
    ATH_MSG_ERROR("Invalid pointer to GenEvent !!" << endmsg
		  << "  in: " << in << endmsg
		  << " out: " << out);
    return StatusCode::FAILURE;
  }

  // loop over vertices
  for ( HepMC::GenEvent::vertex_const_iterator vtx = in->vertices_begin();
	vtx != in->vertices_end(); 
	++vtx ) {
    
    bool isSignalVertex = isSignalProcessVertex(*vtx, in);
    if ( !isAccepted(*vtx) and !isSignalVertex ) {
      // no in-going nor out-going particles at this vertex matches 
      // the requirements nor it is a signal process vertex : ==> Skip it
      continue;
    }
    
    if ( addVertex( *vtx, out, isSignalVertex ).isFailure() ) {
      ATH_MSG_WARNING("Could not add vertex [" << (*vtx)->barcode() << "]");
    }

  } //> end loop over vertices
  
  return StatusCode::SUCCESS;
}

bool EtaPtFilterTool::isAccepted( HepMC::ConstGenParticlePtr mc ) const
{
  if ( 0 == mc ) {
    return false;
  }
  
  if ( m_butKeepAllGeneratorStable.value() ) {
    static IsGenStable isStable;
    if ( isStable(mc) ) 
      return true;
  }

  if ( m_onlyGenerator.value() ) {
    // helper class to know if a GenParticle has been produced at Generator 
    // level. ie: not at simulation level (Geant4)
    static IsGenerator ifs;
    if ( !ifs(mc) ) {
      return false;
    }
  }

  // FIXME: this is generator dependent... 
  //        one would need a TruthHelper predicate
  if ( m_keepDocumentaries.value() ) {
    if ( mc->status() == 3 ) {
      return true;
    }
  }

  const HepMC::FourVector hlv = mc->momentum();
  const double pt  = hlv.perp();
  const double eta = hlv.pseudoRapidity();

  if ( ( std::abs( eta )  >=  m_innerEtaRegionCuts.value()[0] && 
	 std::abs( eta )  <   m_innerEtaRegionCuts.value()[1] &&
	 pt               >   m_innerEtaRegionCuts.value()[2] ) 
       
       ||
       
       ( std::abs( eta )  >=  m_outerEtaRegionCuts.value()[0] && 
	 std::abs( eta )  <   m_outerEtaRegionCuts.value()[1] &&
	 pt               >   m_outerEtaRegionCuts.value()[2] ) ) {
    return true;
  } else {
    return false;
  }
}

bool EtaPtFilterTool::isAccepted( const HepMC::GenVertex* vtx ) const
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

  // ///////////////////////////////////////////////////////////////////////
  // Now we check if at least one in- or out-going particle can be accepted.
  // If yes, then we accept the entire vertex
  //

  // check the parent branch
  for ( HepMC::GenVertex::particles_in_const_iterator 
	  p    = vtx->particles_in_const_begin(),
	  pEnd = vtx->particles_in_const_end();
	p != pEnd;
	++p ) {
    if ( isAccepted(*p) ) {
      return true;
    }
  }//> end loop over parents
  
  // check the child branch
  for ( HepMC::GenVertex::particles_out_const_iterator 
	  p    = vtx->particles_out_const_begin(),
	  pEnd = vtx->particles_out_const_end();
	p != pEnd;
	++p ) {
    if ( isAccepted(*p) ) {
      return true;
    }
  }//> end loop over children

  return false;
}

bool EtaPtFilterTool::isSignalProcessVertex( const HepMC::GenVertex* vtx, const HepMC::GenEvent* evt )
{
  if (evt->signal_process_vertex() == vtx) {
    ATH_MSG_DEBUG("Signal Process vertex found: " << vtx << " = ("
		  << vtx->position().x() << ", " << vtx->position().y() 
		  << ", " << vtx->position().z() << ")");
    return true;
  }
  return false;
}

StatusCode EtaPtFilterTool::addVertex( const HepMC::GenVertex* srcVtx, HepMC::GenEvent* evt,
				       bool isSignalVertex) const
{
  if ( 0 == srcVtx || 0 == evt ) {
    ATH_MSG_ERROR("In addVertex(vtx,evt) : INVALID pointer given !!" << endmsg
		  << " vtx: " << srcVtx << endmsg
		  << " evt: " << evt);
    return StatusCode::FAILURE;
  }

  HepMC::GenVertex * vtx = evt->barcode_to_vertex(srcVtx->barcode());
  if ( 0 == vtx ) {
    vtx = new HepMC::GenVertex();
    vtx->set_position( srcVtx->position() );
    vtx->set_id( srcVtx->id() );
    vtx->suggest_barcode( srcVtx->barcode() );
    vtx->weights() = srcVtx->weights();
    evt->add_vertex(vtx);
    if (isSignalVertex) evt->set_signal_process_vertex(vtx);
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
      p = new HepMC::GenParticle;
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
      p = new HepMC::GenParticle;
      p->set_momentum( (*child)->momentum() );
      p->set_generated_mass( (*child)->generated_mass() );
      p->set_pdg_id( (*child)->pdg_id() );
      if ( m_butKeepAllGeneratorStable && !isAccepted(*child) && (*child)->status() == 2 ) 
	p->set_status( 10902 ) ;
      else
	p->set_status( (*child)->status() );
      p->set_flow( (*child)->flow() );
      p->set_polarization( (*child)->polarization() );
      p->suggest_barcode( (*child)->barcode() );
    }

    // set the daughter's production vertex to our new vertex
    vtx->add_particle_out( p );

  }//> loop over outgoing particles

  return StatusCode::SUCCESS;
}

bool EtaPtFilterTool::isFromHardScattering( const HepMC::GenVertex* vtx ) const
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

StatusCode EtaPtFilterTool::initializeTool()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // make sure the properties are synchronised
  setupInnerEtaRegionCuts(m_innerEtaRegionCuts);
  setupOuterEtaRegionCuts(m_outerEtaRegionCuts);

  if ( m_innerEtaRegionCuts.value().size() != 3 ||
       m_outerEtaRegionCuts.value().size() != 3 ) {
    ATH_MSG_ERROR
      ("Wrong size for eta regions cut :" << endmsg
       << "\tinner region: " << m_innerEtaRegionCuts.value().size()
       << endmsg
       << "\touter region: " << m_outerEtaRegionCuts.value().size()
       << endmsg
       << "You have to provide a list of cuts of the form : " << endmsg
       << " |etaMin| |etaMax| ptMin");
    return StatusCode::FAILURE;
  }

  // configure hard-scattering vertices
  m_ppFilter.setDecayPattern( "-1|-2|-3|-4|-5|-6|21 + 1|2|3|4|5|6|21 -> " );
  
  // filter which will be used to not select shower vertices while
  // looking for hard-scattering vertices
  m_showerFilter.setDecayPattern( "-> 91|92|94" );

  ATH_MSG_INFO
    ("Inner Eta region cuts : nCuts = " 
     << m_innerEtaRegionCuts.value().size()
     << endmsg
     << "\tetaMin = " << m_innerEtaRegionCuts.value()[0] << endmsg
     << "\tetaMax = " << m_innerEtaRegionCuts.value()[1] << endmsg
     << "\tPtMin  = " << m_innerEtaRegionCuts.value()[2] << endmsg
     << "Outer Eta region cuts : nCuts = " 
     << m_outerEtaRegionCuts.value().size()
     << endmsg
     << "\tetaMin = " << m_outerEtaRegionCuts.value()[0] << endmsg
     << "\tetaMax = " << m_outerEtaRegionCuts.value()[1] << endmsg
     << "\tPtMin  = " << m_outerEtaRegionCuts.value()[2]);

  return StatusCode::SUCCESS;
}

void EtaPtFilterTool::setupInnerEtaRegionCuts( Gaudi::Details::PropertyBase& /*innerRegion*/ )
{
  // nothing to do (yet?)
  return;
}

void EtaPtFilterTool::setupOuterEtaRegionCuts( Gaudi::Details::PropertyBase& /*outerRegion*/ )
{
  // nothing to do (yet?)
  return;
}
