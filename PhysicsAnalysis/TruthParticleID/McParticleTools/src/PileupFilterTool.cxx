/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PileupFilterTool.cxx 
// Implementation file for class pileupFilterTool
// Author: Ketevi.A.Assamagan<ketevi@bnl.gov>
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
#include "AtlasHepMC/Relatives.h"
#include "CLHEP/Units/SystemOfUnits.h"

// McParticleKernel includes
#include "McParticleKernel/IMcVtxFilterTool.h"

// McParticleTools includes
#include "PileupFilterTool.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace TruthHelper;
using CLHEP::GeV;

// Constructors
////////////////
PileupFilterTool::PileupFilterTool( const std::string& type, 
		 		    const std::string& name, 
				    const IInterface* parent ) : 
  TruthParticleFilterBaseTool( type, name, parent ),
  m_barcodes (   ),
  m_tesIO    ( 0 )
{

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

  declareProperty( "rRange",          
		   m_rRange = 500.0,
		   "radius acceptance - in milimeters - cut applied on all stable "
		   "particles" );

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
PileupFilterTool::~PileupFilterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
  delete m_tesIO;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

StatusCode PileupFilterTool::buildMcAod( const McEventCollection* in,
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
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 
StatusCode PileupFilterTool::selectSpclMcBarcodes()
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

  //+++ Get True Vertices from Storegate
  const McEventCollection* mcTruth(0);
  sc = evtStore()->retrieve(mcTruth, m_mcEventsReadHandleKey.key());
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("MC Event " << m_mcEventsReadHandleKey.key() << " not found.");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("McEventCollection successfully retrieved" << endmsg
		<< "Number of truth particles: " << mcTruth->size());

  McEventCollection::const_iterator mcEventItr  = mcTruth->begin();
  McEventCollection::const_iterator mcEventItrE = mcTruth->end();

  for ( ; mcEventItr != mcEventItrE; ++mcEventItr ) {

     const HepMC::GenEvent* genEvent = (*mcEventItr);

#ifdef HEPMC3
     auto vxp = genEvent->vertices().begin();
#else
     HepMC::GenEvent::vertex_const_iterator vxp = genEvent->vertices_begin();
#endif
     const float xp = (*vxp)->position().x();
     const float yp = (*vxp)->position().y();
     const float zp = (*vxp)->position().z();

     // Loop over all particles, selecting special ones
     // keep track of them using their barcodes
     for (auto  part: *genEvent) {
        const int id      = part->pdg_id();
        const HepMC::FourVector hlv = part->momentum();
        const double pt   = hlv.perp();
        const int barcode = HepMC::barcode(part);
        HepMC::ConstGenVertexPtr decayVtx = part->end_vertex();
        HepMC::ConstGenVertexPtr prodVtx  = part->production_vertex();
        bool isSpcl = false;
        /// skip stuff with no end-vertex
        if( part->status() != 1 && !decayVtx ) continue;

        float xi = (prodVtx->position()).x();
        float yi = (prodVtx->position()).y();
        float zi = (prodVtx->position()).z();

        ATH_MSG_DEBUG("Primary Vertex = " 
		      << xp << " " << yp << " " << zp << " " 
		      << "Production Vertex = " 
		      << xi << " " << yi << " " << zi << " " 
		      << "Particle ID = " << id);

        float deltaR = std::sqrt( (xp-xi)*(xp-xi) + (yp-yi)*(yp-yi) );

        const bool accept = (pt > m_ptMin) && (deltaR < m_rRange);
        ///////////////////////////
        // Select special particles
        ///////////////////////////
        /// stable particles
        if (m_includeSimul ) {
           if ( isGSStable(part) && accept ) isSpcl = true;
        } else {
           if ( isStable(part) && accept ) isSpcl = true;
        }

        //////////////////////////////////////
        // Save special particles and children
        //////////////////////////////////////
        if( !isSpcl ) continue;
        m_barcodes.insert(barcode); // add it to list

        // Children
        if( isSpcl && decayVtx ) {
          for(auto child: *(part->end_vertex())) {
             if( isGenerator(child) && !m_removeDecayToSelf) { 
	       m_barcodes.insert(HepMC::barcode(child));// its not there already
             }
          }
        }
     } 
  }
  ATH_MSG_DEBUG("Read " << particles.size() 
		<< " and selected  " <<  m_barcodes.size() << " particles");

  return StatusCode::SUCCESS;
}

StatusCode PileupFilterTool::shapeGenEvent( McEventCollection* genAod )
{
  //now remove all the particles except those whose barcodes are marked
  for ( McEventCollection::const_iterator evt = genAod->begin(); 
	evt != genAod->end(); 
	++evt) {
    std::vector<HepMC::GenParticlePtr> going_out;

    std::list<int> evtBarcodes;
    for ( auto p: **evt) {
      evtBarcodes.push_back( HepMC::barcode(p) );
    }

    for ( std::list<int>::const_iterator itrBc = evtBarcodes.begin();
	  itrBc != evtBarcodes.end();
	  ++itrBc ) {
//AV:  We modify the event!
      HepMC::GenParticlePtr p = HepMC::barcode_to_particle((HepMC::GenEvent*)(*evt),*itrBc);
      ATH_MSG_DEBUG("[pdg,bc]= " << p->pdg_id() << ", " << HepMC::barcode( p));
      if ( m_barcodes.find(HepMC::barcode(p)) == m_barcodes.end() ) { 
	going_out.push_back(p); // list of useless particles
	HepMC::GenVertexPtr pvtx = p->production_vertex();
	HepMC::GenVertexPtr evtx = p->end_vertex();

	std::pair<int,int> bcNext( 0, 0 );
	if ( msgLvl(MSG::DEBUG) ) {
	  msg(MSG::DEBUG)
	    << "Removing [" 
	    << p->pdg_id() << ", " 
	    <<HepMC::barcode( p) << "]" 
	    << "\tprod/endVtx: " << pvtx 
	    << "/"  << evtx 
	    << endmsg;
	  std::list<int>::const_iterator pNext = itrBc;
	  ++pNext;
	  if ( pNext != evtBarcodes.end() ) {
	    bcNext.first = HepMC::barcode(HepMC::barcode_to_particle(*evt,*pNext));
	  }
	}
	
#ifdef HEPMC3
	if (pvtx) pvtx->remove_particle_out(p); //remove from production vertex from useless partilcle
#else
	if (pvtx) pvtx->remove_particle(p); //remove from production vertex from useless partilcle
#endif
	if (evtx) { // if it has end vertex, may need to move the out partilces
	  if(pvtx){ // move the partilces back
	    if ( msgLvl(MSG::DEBUG) ) {
	      msg(MSG::DEBUG) << "\tin endVtx   "<< endmsg;
	    }
#ifdef HEPMC3
	    while ( evtx->particles_out().begin() !=  evtx->particles_out().end()) {
	      pvtx->add_particle_out(evtx->particles_out().front());
	    }
	  }//> end if [prod vertex]
	  evtx->remove_particle_out(p); // disconnect from end vertex	
#else 
	    while ( evtx->particles_out_const_begin() !=  evtx->particles_out_const_end()) {
	      HepMC::GenVertex::particles_out_const_iterator np = evtx->particles_out_const_begin();
	      pvtx->add_particle_out(*np); // note that this really is a MOVE!!! it get taken off evtx by magic
	    }
	  }//> end if [prod vertex]
	  evtx->remove_particle(p); // disconnect from end vertex	
#endif  
	}//> end if [decay vertex]

	if ( msgLvl(MSG::DEBUG) ) {
	  std::list<int>::const_iterator pNext = itrBc;
	  ++pNext;
	  if ( pNext != evtBarcodes.end() ) {
	    bcNext.second = HepMC::barcode(HepMC::barcode_to_particle(*evt,*pNext));
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


#ifdef HEPMC3
    // there may be a bunch of vertices with no particles connected to them:
    // ==> Get rid of them
    std::vector<HepMC::ConstGenVertexPtr> going_out_again;
    for ( HepMC::ConstGenVertexPtr v: (*evt)->vertices() ) {
      if ( v->particles_in().size() == 0 && v->particles_out().size() == 0 ){
	going_out_again.push_back(v);
      }
    }//> loop over vertices
#else 
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
#endif
    
  }//> loop over GenEvents in McEventCollection
  
  // Set the signal_process_vertex to NULL if not to be recorded 
  for ( McEventCollection::iterator evt = genAod->begin(); 
	evt != genAod->end(); 
	++evt) { 
    auto sigProcVtx = HepMC::signal_process_vertex(*evt); 
    if ( !sigProcVtx ) continue;
      const int sigProcBC = HepMC::barcode(sigProcVtx); 
      bool isInColl = false; 
#ifdef HEPMC3
      for ( auto itrVtx: (*evt)->vertices()){ 
	if ( sigProcBC == HepMC::barcode(itrVtx) ) { 
	  isInColl = true; 
	  break; 
	} 
      }  //> loop over vertices 
//AV: We don't set nullptr as signal vertex in HepMC3
     if ( !isInColl ) { 
         (*evt)->remove_attribute("signal_process_vertex");
      }
#else 
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
#endif
  }//> loop over GenEvent's 

  return StatusCode::SUCCESS;
}

StatusCode PileupFilterTool::reconnectParticles( const McEventCollection* in,
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
    for ( auto itrPart: *outEvt) {
      if ( itrPart->end_vertex() ) {
	continue;
      }
      if ( rebuildLinks( evt, outEvt, itrPart ).isFailure() ) {
	ATH_MSG_WARNING("Could not rebuild links for this particle [pdgId,bc]= "
			<< itrPart->pdg_id()
			<< ", " << HepMC::barcode(itrPart));
      } else if ( msgLvl(MSG::VERBOSE) ) {
	msg(MSG::VERBOSE)
	  << "==========================================================="
	  << endmsg
	  << "Production vertex for particle " 
	  << HepMC::barcode(itrPart) << " : ";
	if ( itrPart->production_vertex() ) {
	  std::stringstream prodVtx("");
	  HepMC::Print::line(prodVtx,itrPart->production_vertex());
	  msg(MSG::VERBOSE) << std::endl
			    << prodVtx.str()
			    << endmsg;
	} else {
	  msg(MSG::VERBOSE) << "[No production vertex]" << endmsg;
	}
	
	msg(MSG::VERBOSE) << "Decay vertex for particle " 
			  << HepMC::barcode(itrPart) << " : ";
	if ( itrPart->end_vertex() ) {
	  std::stringstream dcyVtx("");
	  HepMC::Print::line(dcyVtx,itrPart->end_vertex());
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

StatusCode PileupFilterTool::rebuildLinks( const HepMC::GenEvent * mcEvt,
					      HepMC::GenEvent * outEvt,
					      HepMC::GenParticlePtr mcPart )
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
  const int bc    = HepMC::barcode(mcPart);
#ifdef HEPMC3
  HepMC::ConstGenParticlePtr  inPart = HepMC::barcode_to_particle(mcEvt,bc);
  HepMC::ConstGenVertexPtr    dcyVtx = inPart->end_vertex();
#else
//AV: Const correctness is broken in HepMC2
  HepMC::GenParticlePtr  inPart = HepMC::barcode_to_particle(mcEvt,bc);
  HepMC::GenVertexPtr    dcyVtx = inPart->end_vertex();
#endif

  if ( !dcyVtx ) {
    ATH_MSG_VERBOSE("No decay vertex for the particle #" << bc << " : "
		    << "No link to rebuild...");
    return StatusCode::SUCCESS;
  }

  std::list<int> bcChildPart;
  std::list<int> bcChildVert;

  //
  // Loop over all descendants of the GenParticle
  // Store the barcode of the GenParticles entering into each GenVertex
  //
#ifdef HEPMC3
  auto descendants=HepMC::descendant_vertices(dcyVtx);
  for ( auto itrVtx: descendants) {
    bool foundPdgId = false;
    for ( auto itrPart: itrVtx->particles_in()) {
      // because the vertices are traversed in POST ORDER !!
      bcChildPart.push_front( HepMC::barcode(itrPart));
      if ( itrPart->pdg_id() == pdgId ) {
	foundPdgId = true;
      }
    }//> loop over in-going particles of this vertex
    if ( foundPdgId ) {
      bcChildVert.push_front( HepMC::barcode(itrVtx));
    }
  }//> loop over descendants of decay vertex
#else
  const HepMC::GenVertex::vertex_iterator endVtx = dcyVtx->vertices_end(HepMC::descendants);
  for ( HepMC::GenVertex::vertex_iterator itrVtx = dcyVtx->vertices_begin( HepMC::descendants );
	itrVtx != endVtx;
	++itrVtx ) {
    bool foundPdgId = false;
    HepMC::GenVertex::particles_in_const_iterator endPart = (*itrVtx)->particles_in_const_end();
    for ( HepMC::GenVertex::particles_in_const_iterator itrPart = (*itrVtx)->particles_in_const_begin();
	  itrPart != endPart;
	  ++itrPart ) {



      // because the vertices are traversed in POST ORDER !!
      bcChildPart.push_front( (*itrPart)->barcode() );

      if ( (*itrPart)->pdg_id() == pdgId ) {
	foundPdgId = true;
      }
    }//> loop over in-going particles of this vertex

    if ( foundPdgId ) {
      bcChildVert.push_front( (*itrVtx)->barcode() );
    }

  }//> loop over descendants of decay vertex
#endif

  //
  // Now we loop over the previously stored barcodes and
  // we connect our GenParticle to the first found barcode
  // 
#ifdef HEPMC3
  std::list<int>::const_iterator bcVtxEnd = bcChildVert.end();
  for ( std::list<int>::const_iterator itrBcVtx = bcChildVert.begin();
	itrBcVtx != bcVtxEnd;
	++itrBcVtx ) {
    HepMC::GenVertexPtr childVtx = HepMC::barcode_to_vertex(outEvt,*itrBcVtx);
    if ( childVtx ) {
      if ( childVtx->particles_in().size() > 0 ) {
	for ( auto itrPart:  childVtx->particles_in()) {
	  if ( itrPart->pdg_id() == pdgId ) {
	    HepMC::GenVertexPtr  prodVtx = itrPart->production_vertex();
	    if ( prodVtx ) {
	      if ( prodVtx->particles_in().size() > 0 ) {
		// Humm... This is not what we'd have expected
		// so we skip it
		if ( msgLvl(MSG::VERBOSE) ) {
		  msg(MSG::VERBOSE)
		    << "found a particle [bc,pdgId]= "
		    << HepMC::barcode(itrPart) << ", "
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
		linkVtx->add_particle_out( itrPart );
		
		msg(MSG::ERROR)
		  << "====================================================="
		  << endmsg
		  << "Created a GenVertex - link !"
		  << std::endl;
		std::stringstream vtxLink("");
		HepMC::Print::line(vtxLink,linkVtx);
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
	HepMC::Print::line(vtxDump,childVtx);
	msg(MSG::WARNING) << vtxDump.str() << endmsg;
	return StatusCode::SUCCESS;
      }//> end if incoming particles
    }//> found a child-vertex
  }//> loop over child-vertex-barcodes
#else
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
#endif

  return StatusCode::FAILURE;
}

StatusCode PileupFilterTool::initializeTool() 
{
  ATH_MSG_DEBUG("Calling initializeTool");
  // accessor for particles
  delete m_tesIO;
  m_tesIO = new GenAccessIO();
  if( 0 == m_tesIO ) {
    ATH_MSG_ERROR("Unable to retrieve GenAccessIO pointer");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

