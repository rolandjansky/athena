/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// OldSpclMcFilterTool.cxx 
// Implementation file for class OldSpclMcFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <cmath>

// FrameWork includes

// CLHEP/HepMC includes
#include "TruthHelper/GenAccessIO.h"
#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenSimulStable.h"
#include "TruthHelper/IsGenerator.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Units/SystemOfUnits.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// McParticleTools includes
#include "OldSpclMcFilterTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace TruthHelper;
using CLHEP::GeV;

// Constructors
////////////////
OldSpclMcFilterTool::OldSpclMcFilterTool( const std::string& type, 
					  const std::string& name, 
					  const IInterface* parent ) : 
  TruthParticleFilterBaseTool( type, name, parent ),
  m_tesIO( 0 )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "rIsolation",
		   m_rIsol = 0.45,
		   "DeltaR isolation energy cut for electrons, muons, "
		   "taus and photons" );

  declareProperty( "ptGammaMin",
		   m_ptGamMin = 0.5*GeV,
		   "Minimum threshold for transverse momentum of photons" );

  declareProperty( "ptMin",             
		   m_ptMin = 0.5*GeV,
		   "Minimum threshold for transverse momentum for all particles.\n"
		   "Warning: this cut is applied *before* Pt photon cut !" );

  declareProperty( "etaRange", 
		   m_etaRange = 5.0,
		   "Eta acceptance cut applied on all stable particles" );

  // switches

  declareProperty( "IncludeSimul",         
		   m_includeSimul = true,
		   "Switch to include or not particles from detector simulation "
		   "(Geant 4)" );

  declareProperty( "IncludePartonShowers", 
		   m_includePartonShowers = true,
		   "Switch to include or not parton showers" );

  declareProperty( "RemoveDecayToSelf",    
		   m_removeDecayToSelf = true,
		   "Switch to remove particles which decay into themselves (t->tg) "
		   "*but* only for generated particles, not the ones from the " 
		   "Geant4 interactions" );

}

// Destructor
///////////////
OldSpclMcFilterTool::~OldSpclMcFilterTool()
{ 
  delete m_tesIO; m_tesIO = 0;
  ATH_MSG_DEBUG("Calling destructor");
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode OldSpclMcFilterTool::buildMcAod( const McEventCollection* in,
					    McEventCollection* out )
{
  if ( 0 == in || 0 == out ) {
    ATH_MSG_ERROR("Invalid pointer to McEventCollection !" << endmsg
		  << "  in: " << in << endmsg
		  << " out: " << out);
    return StatusCode::FAILURE;
  }

  // we just copy the input McEventCollection and put it into the output one
  out->operator=( *in );

  // select the barcodes of the "special" particles from the input GenEvent
  if ( selectSpclMcBarcodes().isFailure() ) {
    ATH_MSG_ERROR("Could not select the \"special\" barcodes !!");
    return StatusCode::FAILURE;
  }

  // remove the not "special" particles from the filtered McEventCollection
  if ( shapeGenEvent(out).isFailure() ) {
    ATH_MSG_ERROR("Could not remove the not \"special\" particles from the "\
		  "filtered McEventCollection !!");
    return StatusCode::FAILURE;
  }

  // reconnect the particles
  if ( reconnectParticles(in, out).isFailure() ) {
    ATH_MSG_ERROR("Could not reconnect the particles in the filtered "\
		  "McEventCollection !!");
    return StatusCode::FAILURE;
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
StatusCode OldSpclMcFilterTool::selectSpclMcBarcodes()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Initialize helper classes to test event and final particles

  static const IsGenStable      isStable;
  static const IsGenSimulStable isGSStable;
  static const IsGenerator      isGenerator;

  // Get all of the generated particles (does not have Geant secondaries)

  std::vector<HepMC::ConstGenParticlePtr> particles;
  if ( m_includeSimul ) {
    sc = m_tesIO->getMC(particles,  m_mcEventsReadHandleKey.key());
  } else {
    static const IsGenerator ifs;
    sc = m_tesIO->getMC(particles, &ifs, m_mcEventsReadHandleKey.key());
  }
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not get Monte Carlo particles from TDS at : "
		  << m_mcEventsReadHandleKey.key());
    return StatusCode::FAILURE;
  }

  m_barcodes.clear();

  // Loop over all particles, selecting special ones
  // keep track of them using their barcodes

  for (auto part : particles) {

    const int id      = part->pdg_id();
    const int ida     = std::abs(id);
    const HepMC::FourVector hlv = part->momentum();
    const double pt   = hlv.perp();
    const double eta  = hlv.pseudoRapidity();
    const double mass = hlv.m();
    const int barcode = part->barcode();

    const HepMC::GenVertex * decayVtx = part->end_vertex();
    const HepMC::GenVertex * prodVtx  = part->production_vertex();

    bool isSpcl = false;

    /// skip stuff with no end-vertex
    if( part->status() != 1 && !decayVtx ) continue;

    const bool accept = pt > m_ptMin;

    ///////////////////////////
    // Select special particles
    ///////////////////////////

    /// stable particles
    if (m_includeSimul ) {
        if ( isGSStable(part) && accept && std::abs(eta) < m_etaRange ) isSpcl = true;
    } else {
        if ( isStable(part) && accept && std::abs(eta) < m_etaRange ) isSpcl = true;
    }
    // e, mu, tau, neutrino 
    // Hard coded cut pt>2GeV to remove beam-jet Dalitz decays, etc.
    if( ida>10 && ida<17 && pt>2.*GeV && std::abs(eta) < m_etaRange ) isSpcl = true;

   /// Save photons
   ///
   if ( ida==22 && pt>m_ptGamMin && std::abs(eta) < m_etaRange ) {
     /// Save photon only if it does not decay (ie no end_vertex)
     if ( 0 == part->end_vertex() ) {
        isSpcl=true;
     } 
   } 

    // Any long-lived B meson or baryon 5x1 or 5xx2
    int ifl=0;
    if( ida>100 && ida<1000) ifl=ida/100;
    if( ida>1000 && ida<10000) ifl=ida/1000;
    int jj1 = ida%10;
    if( (ifl==5 && jj1<3 && mass<9.0*GeV) || ( ifl==4 && jj1<3 && mass<2.4*GeV) ) {
      if (accept) isSpcl = true;
    } 

    // Any heavy particle other than quarks or gluons.
    // M > 9.0GeV excludes B hadrons but includes Upsilon, W, Z, t, ...
    if( ida>5 && ida!=21 && ida!=9 && mass>9.*GeV && accept ) isSpcl=true;

    // save the quarks
    if (ida >= 1 && ida <= 5 && accept ) isSpcl = true;

    // save the gluons
    if ( ida == 21 && accept ) isSpcl = true;

    // save the double charged Higgs
    if ( (ida == 9900041 || ida == 9900042) && accept ) isSpcl = true; 

    // SUSY particles -- relies on PDG codes
    if( ida>1000000 && ida<3000000 && accept ) isSpcl = true;

    ////////////////////////////////////
    // Exclude invalid special particles
    ////////////////////////////////////
    
    // Monte Carlo internal particles per PDG
    if( ida>80 && ida<101 ) isSpcl=false;

    // Particles decaying into itself
    // This prevents documentaries from being reported twice and
    // eliminates (if asked to) parton showers like q -> qg.
    // But should not delete particle if child is from GEANT.
    if( isSpcl && decayVtx ) {
      HepMC::GenVertex * dcyVtx = part->end_vertex();
      HepMC::GenVertex::particle_iterator child  = dcyVtx->particles_begin(HepMC::children);
      HepMC::GenVertex::particle_iterator childE = dcyVtx->particles_end(HepMC::children);
      for(; child != childE; ++child) {
	if( (*child)->pdg_id()==id && //> looking for parton showers or documentaries
	    (*child)->barcode() !=barcode  && //> just to be sure that merging GEN_EVENT/G4Truth is OK
	    isGenerator(*child)  //> child is not from GEANT
	    ) {
	  if ( m_includePartonShowers ) {
	    // we keep the parent particle
	    // this could be vertices of the form :
	    // q -> q + g + b + bbar(hence the outgoing quark isn't decaying:documentary)
	    // t -> t + W + b (with outgoing top being a documentary)
	    // q -> q + g (real parton shower)
	    isSpcl = true;
	  } else {
	    isSpcl = false;
	  }
	}
      }
    }

    /////////////////////////////////////////
    /// ask McVtxFilterTool for its decision
    /// ie: final word is given to the user
    ////////////////////////////////////////
    if ( ( decayVtx && m_mcVtxFilterTool->isAccepted(decayVtx) ) || 
	 ( prodVtx  && m_mcVtxFilterTool->isAccepted(prodVtx) ) ) {
      isSpcl = true;
    }

    //////////////////////////////////////
    // Save special particles and children
    //////////////////////////////////////
    if( !isSpcl ) continue;
    m_barcodes.insert(barcode); // add it to list

    // Children
    if( isSpcl && decayVtx ) {
      HepMC::GenVertex::particle_iterator child  = part->end_vertex()->particles_begin(HepMC::children);
      HepMC::GenVertex::particle_iterator childE = part->end_vertex()->particles_end(HepMC::children);
      for(; child != childE; ++child) {
        if( isGenerator(*child) && !m_removeDecayToSelf) { 
	  m_barcodes.insert((*child)->barcode());// its not there already
        }
      }
    }

  }
  ATH_MSG_DEBUG("Read " << particles.size() 
		<< " and selected  " <<  m_barcodes.size() << " particles");

  return StatusCode::SUCCESS;
}

StatusCode OldSpclMcFilterTool::shapeGenEvent( McEventCollection* genAod )
{
  //now remove all the particles except those whose barcodes are marked
  for ( McEventCollection::const_iterator evt = genAod->begin(); 
	evt != genAod->end(); 
	++evt) {
    std::vector<HepMC::GenParticle*> going_out;

    std::list<int> evtBarcodes;
    for ( HepMC::GenEvent::particle_const_iterator p = (*evt)->particles_begin();
	  p != (*evt)->particles_end(); 
	  ++p ) {
      evtBarcodes.push_back( (*p)->barcode() );
    }

    for ( std::list<int>::const_iterator itrBc = evtBarcodes.begin();
	  itrBc != evtBarcodes.end();
	  ++itrBc ) {

      HepMC::GenParticle * p = (*evt)->barcode_to_particle(*itrBc);

      ATH_MSG_DEBUG("[pdg,bc]= " << p->pdg_id() << ", " << p->barcode());
      if ( m_barcodes.find(p->barcode()) == m_barcodes.end() ) { 
	going_out.push_back(p); // list of useless particles
	HepMC::GenVertex * pvtx = p->production_vertex();
	HepMC::GenVertex * evtx = p->end_vertex();

	std::pair<int,int> bcNext( 0, 0 );
	if ( msgLvl(MSG::DEBUG) ) {
	  msg(MSG::DEBUG)
	    << "Removing [" 
	    << p->pdg_id() << ", " 
	    << p->barcode() << "]" 
	    << "\tprod/endVtx: " << pvtx 
	    << "/"  << evtx 
	    << endmsg;
	  std::list<int>::const_iterator pNext = itrBc;
	  ++pNext;
	  if ( pNext != evtBarcodes.end() ) {
	    bcNext.first = (*evt)->barcode_to_particle(*pNext)->barcode();
	  }
	}
	
	if (pvtx) pvtx->remove_particle(p); //remove from production vertex from useless partilcle
	if (evtx) { // if it has end vertex, may need to move the out partilces
	  if(pvtx){ // move the partilces back
	    if ( msgLvl(MSG::DEBUG) ) {
	      msg(MSG::DEBUG) << "\tin endVtx   "<< endmsg;
	    }
	    while ( evtx->particles_out_const_begin() !=  evtx->particles_out_const_end()) {
	      HepMC::GenVertex::particles_out_const_iterator np = evtx->particles_out_const_begin();
	      pvtx->add_particle_out(*np); // note that this really is a MOVE!!! it get taken off evtx by magic
	    }
	  }//> end if [prod vertex]
	  evtx->remove_particle(p); // disconnect from end vertex	  
	}//> end if [decay vertex]

	if ( msgLvl(MSG::DEBUG) ) {
	  std::list<int>::const_iterator pNext = itrBc;
	  ++pNext;
	  if ( pNext != evtBarcodes.end() ) {
	    bcNext.second = (*evt)->barcode_to_particle(*pNext)->barcode();
	  }

	  if ( bcNext.first != bcNext.second ) {
	    ATH_MSG_WARNING("\tIterator has been CORRUPTED !!" << endmsg
			    << "\tbcNext: " << bcNext.first
			    << " --> " << bcNext.second);
	  } else {
	    ATH_MSG_DEBUG("\tIterator OK:" << endmsg
			  << "\tbcNext: " << bcNext.first
			  << " --> " << bcNext.second);
	  }
	}

      }//> particle has to be removed
    }//> loop over particles (via their barcode)


    // now get rid of all dead particles
    for ( std::vector<HepMC::GenParticle*>::iterator d = going_out.begin(); 
	  d != going_out.end(); 
	  ++d ){
      delete *d;
    }

    // there may be a bunch of vertices with no particles connected to them:
    // ==> Get rid of them
    std::vector<HepMC::GenVertex*> going_out_again;
    for ( HepMC::GenEvent::vertex_const_iterator v = (*evt)->vertices_begin();
	  v != (*evt)->vertices_end(); ++v ) {
      if ( (*v)->particles_in_size() == 0 && (*v)->particles_out_size() == 0 ){
	going_out_again.push_back(*v);
      }
    }//> loop over vertices

   // now get rid of all dead vertices
    for ( std::vector<HepMC::GenVertex*>::iterator d = going_out_again.begin();
	  d != going_out_again.end(); 
	  ++d ){
      delete *d;
    }
    
  }//> loop over GenEvents in McEventCollection
  
  // Set the signal_process_vertex to NULL if not to be recorded 
  for ( McEventCollection::iterator evt = genAod->begin(); 
	evt != genAod->end(); 
	++evt) { 
    const HepMC::GenVertex * sigProcVtx = (*evt)->signal_process_vertex(); 
    if ( 0 != sigProcVtx ) { 
      const int sigProcBC = sigProcVtx->barcode(); 
      bool isInColl = false; 
      for ( HepMC::GenEvent::vertex_const_iterator itrVtx = (*evt)->vertices_begin(); 
	    itrVtx != (*evt)->vertices_end(); 
	    ++itrVtx ) { 
	if ( sigProcBC == (*itrVtx)->barcode() ) { 
	  isInColl = true; 
	  break; 
	} 
      }  //> loop over vertices 
      if ( !isInColl ) { 
	(*evt)->set_signal_process_vertex(0); 
      } 
    }//> a signal_process_vertex has been setup 
  }//> loop over GenEvent's 

  return StatusCode::SUCCESS;
}

StatusCode OldSpclMcFilterTool::reconnectParticles( const McEventCollection* in,
						    McEventCollection* out )
{
  if ( 0 == in || 0 == out ) {
    ATH_MSG_ERROR("Invalid pointer to McEventCollection !!" << endmsg
		  << "  in: " << in << endmsg
		  << " out: " << out);
    return StatusCode::FAILURE;
  }

  for ( unsigned int iEvt = 0; iEvt != in->size(); ++iEvt) {
    const HepMC::GenEvent * evt    = (*in)[iEvt];
    HepMC::GenEvent       * outEvt = (*out)[iEvt];
    
    // Reconnect the particles
    ATH_MSG_VERBOSE("Reconnecting particles...");
    for ( HepMC::GenEvent::particle_iterator itrPart = outEvt->particles_begin();
	  itrPart != outEvt->particles_end();
	  ++itrPart ) {
      if ( (*itrPart)->end_vertex() ) {
	continue;
      }
      if ( rebuildLinks( evt, outEvt, *itrPart ).isFailure() ) {
	ATH_MSG_WARNING("Could not rebuild links for this particle [pdgId,bc]= "
			<< (*itrPart)->pdg_id()
			<< ", " << (*itrPart)->barcode());
      } else if ( msgLvl(MSG::VERBOSE) ) {
	msg(MSG::VERBOSE)
	  << "==========================================================="
	  << endmsg
	  << "Production vertex for particle " 
	  << (*itrPart)->barcode() << " : ";
	if ( (*itrPart)->production_vertex() ) {
	  std::stringstream prodVtx("");
	  (*itrPart)->production_vertex()->print(prodVtx);
	  msg(MSG::VERBOSE) << std::endl
			    << prodVtx.str()
			    << endmsg;
	} else {
	  msg(MSG::VERBOSE) << "[No production vertex]" << endmsg;
	}
	
	msg(MSG::VERBOSE) << "Decay vertex for particle " 
			  << (*itrPart)->barcode() << " : ";
	if ( (*itrPart)->end_vertex() ) {
	  std::stringstream dcyVtx("");
	  (*itrPart)->end_vertex()->print(dcyVtx);
	  msg(MSG::VERBOSE) << std::endl
			    << dcyVtx.str()
			    << endmsg;
	} else {
	  msg(MSG::VERBOSE) << endmsg
			    << "[No decay vertex]" << endmsg;
	}
      }//> end VERBOSE messages
      
    }//> loop over particles
  }//> loop over GenEvents
  
  return StatusCode::SUCCESS;
}

StatusCode OldSpclMcFilterTool::rebuildLinks( const HepMC::GenEvent * mcEvt,
					      HepMC::GenEvent * outEvt,
					      HepMC::GenParticle * mcPart )
{
  if ( !mcPart ) {
    ATH_MSG_WARNING("Null GenParticle: can not rebuildLinks");
    return StatusCode::FAILURE;
  }

  if ( mcPart->end_vertex() ) {
    ATH_MSG_VERBOSE("GenParticle has already a decay vertex : nothing to do");
    return StatusCode::SUCCESS;
  }

  if ( !mcEvt ) {
    ATH_MSG_WARNING("Null input HepMC::GenEvent : can not rebuildLinks");
    return StatusCode::FAILURE;
  }

  if ( !outEvt ) {
    ATH_MSG_WARNING("Null output HepMC::GenEvent: can not rebuildLinks");
    return StatusCode::FAILURE;
  }

  // Cache some useful infos
  const int pdgId = mcPart->pdg_id();
  const int bc    = mcPart->barcode();
  /*const*/ HepMC::GenParticle * inPart = mcEvt->barcode_to_particle(bc);
  /*const*/ HepMC::GenVertex   * dcyVtx = inPart->end_vertex();

  if ( !dcyVtx ) {
    ATH_MSG_VERBOSE("No decay vertex for the particle #" << bc << " : "
		    << "No link to rebuild...");
    return StatusCode::SUCCESS;
  }

  const HepMC::IteratorRange range = HepMC::descendants;
  std::list<int> bcChildPart;
  std::list<int> bcChildVert;

  //
  // Loop over all descendants of the GenParticle
  // Store the barcode of the GenParticles entering into each GenVertex
  //
  const HepMC::GenVertex::vertex_iterator endVtx = dcyVtx->vertices_end(range);
  for ( HepMC::GenVertex::vertex_iterator itrVtx = dcyVtx->vertices_begin( range );
	itrVtx != endVtx;
	++itrVtx ) {
    bool foundPdgId = false;
    HepMC::GenVertex::particles_in_const_iterator endPart = (*itrVtx)->particles_in_const_end();
    for ( HepMC::GenVertex::particles_in_const_iterator itrPart = (*itrVtx)->particles_in_const_begin();
	  itrPart != endPart;
	  ++itrPart ) {

      // because the vertices are traversed in POST ORDER !!
      // (quoting GenVertex::vertex_iterator)
      bcChildPart.push_front( (*itrPart)->barcode() );

      if ( (*itrPart)->pdg_id() == pdgId ) {
	foundPdgId = true;
      }
    }//> loop over in-going particles of this vertex

    if ( foundPdgId ) {
      bcChildVert.push_front( (*itrVtx)->barcode() );
    }

  }//> loop over descendants of decay vertex

  //
  // Now we loop over the previously stored barcodes and
  // we connect our GenParticle to the first found barcode
  // 
  std::list<int>::const_iterator bcVtxEnd = bcChildVert.end();
  for ( std::list<int>::const_iterator itrBcVtx = bcChildVert.begin();
	itrBcVtx != bcVtxEnd;
	++itrBcVtx ) {
    HepMC::GenVertex * childVtx = outEvt->barcode_to_vertex(*itrBcVtx);
    if ( childVtx ) {
      if ( childVtx->particles_in_size() > 0 ) {
	HepMC::GenVertex::particles_in_const_iterator endPart = childVtx->particles_in_const_end();
	for ( HepMC::GenVertex::particles_in_const_iterator itrPart = childVtx->particles_in_const_begin();
	      itrPart != endPart;
	      ++itrPart ) {
	  if ( (*itrPart)->pdg_id() == pdgId ) {
	    HepMC::GenVertex * prodVtx = (*itrPart)->production_vertex();
	    if ( prodVtx ) {
	      if ( prodVtx->particles_in_size() > 0 ) {
		// Humm... This is not what we'd have expected
		// so we skip it
		if ( msgLvl(MSG::VERBOSE) ) {
		  msg(MSG::VERBOSE)
		    << "found a particle [bc,pdgId]= "
		    << (*itrPart)->barcode() << ", "
		    << "but its production vertex has incoming particles !"
		    << endmsg;
		  continue;
		}
		// create a GenVertex which will be the decay vertex of our
		// GenParticle and the production vertex of the GenParticle
		// we just found
		HepMC::GenVertexPtr linkVtx = HepMC::newGenVertexPtr();
		outEvt->add_vertex( linkVtx );
		linkVtx->add_particle_in( mcPart );
		linkVtx->add_particle_out( *itrPart );
		
		msg(MSG::ERROR)
		  << "====================================================="
		  << endmsg
		  << "Created a GenVertex - link !"
		  << std::endl;
		std::stringstream vtxLink("");
		linkVtx->print(vtxLink);
		msg(MSG::ERROR)
		  << vtxLink.str()
		  << endmsg
		  << "====================================================="
		  << endmsg;
	      }
	    }
	  }
	}//> loop over incoming particles
      } else { 
	// no incoming particle : so we just add this particle
	// a bit odd though : FIXME ?
	childVtx->add_particle_in(mcPart);
	msg(MSG::WARNING) << "Odd situation:" << std::endl;
	std::stringstream vtxDump( "" );
	childVtx->print(vtxDump);
	msg(MSG::WARNING) << vtxDump.str() << endmsg;
	return StatusCode::SUCCESS;
      }//> end if incoming particles
    }//> found a child-vertex
  }//> loop over child-vertex-barcodes

  return StatusCode::FAILURE;
}

StatusCode OldSpclMcFilterTool::initializeTool() 
{
  ATH_MSG_DEBUG("Calling initializeTool");
  delete m_tesIO; m_tesIO = 0;
  // accessor for particles  
  m_tesIO = new GenAccessIO();
  if( m_tesIO == 0 ) {
    ATH_MSG_ERROR("Unable to retrieve GenAccessIO pointer");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

