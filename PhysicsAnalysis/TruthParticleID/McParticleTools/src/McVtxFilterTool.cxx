/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// McVtxFilterTool.cxx 
// Implementation file for class McVtxFilterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <algorithm>

// FrameWork includes
#include "Gaudi/Property.h"

// HepMC includes
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleTools includes
#include "McVtxFilterTool.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/// Constructors
////////////////
McVtxFilterTool::McVtxFilterTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
  AthAlgTool( type, name, parent )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "McEvents",
		   m_mcEventsName = "TruthEvent",
		   "Location of the McEventCollection to be filtered" );

  declareProperty( "McEventsOutput", 
		   m_mcEventsOutputName = "GEN_AOD",
		   "Output McEventCollection location (filtered from the "
		   "McEventCollection)" );

  declareProperty( "DecayPatterns", 
		   m_decayPatterns,
		   "List of decay patterns the tool will try to match. \n\t"
		   "ex: 23 -> -5 + 5 to select Z bosons into bbbar pairs" );

  m_decayPatterns.declareUpdateHandler( &McVtxFilterTool::setupFilters, 
					this );

  declareProperty( "MatchSigns", 
		   m_matchSigns,
		   "List of booleans with which the tool will setup "
		   "McVtxFilters. This will setup the McVtxFilter::matchSign "
		   "data member." );
  m_matchSigns.declareUpdateHandler( &McVtxFilterTool::setupFilters, 
				     this );

  declareProperty( "MatchBranches", 
		   m_matchBranches,
		   "List of booleans with which the tool will setup "
		   "McVtxFilters. This will setup the "
		   "McVtxFilter::matchBranches data member." );

  m_matchBranches.declareUpdateHandler( &McVtxFilterTool::setupFilters, 
					this );

  declareProperty( "DoSignalProcessVtx", 
		   m_doSignalProcessVtx = true,
		   "Switch to include or not the signal_process_vertex into "
		   "the McEventCollection (default = true).\n" 
		   "Note that this is close to useless as the HEPEVT structure"
		   " does not hold this kind of informations..." );

  declareProperty( "FillTree",        
		   m_fillTree = false,
		   "Switch to fill the entire decay tree from a decay "
		   "vertex.\n"
		   "This can be usefull to record for example the whole shower "
		   "from a gluon or a quark. In that case, you may want to add "
		   "the according PDG ids into the list of particles to keep" );

  declareProperty( "ParticlesToKeep", 
		   m_particles,
		   "List of outgoing particles from the decay vertex to "
		   "include. This property has to be configured in sync. with "
		   "the 'FillTree' one." );

  declareInterface<IMcVtxFilterTool>(this);
}

/// Destructor
///////////////
McVtxFilterTool::~McVtxFilterTool()
{ 
  ATH_MSG_DEBUG("Calling destructor");
}

/// Athena Algorithm's Hooks
////////////////////////////
StatusCode McVtxFilterTool::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");

  if ( msgLvl(MSG::DEBUG) ) {
    displayOptions();
  }

  // configure the filters with the provided DecayPatterns (from
  // jobO or via the property interface
  setupFilters( m_decayPatterns );

  return StatusCode::SUCCESS;
}

StatusCode McVtxFilterTool::execute()
{  
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG("Executing " << name() << "...");

  ATH_MSG_VERBOSE("Retrieve the McEventCollection to be filtered for");
  const McEventCollection * mcColl = 0;
  sc = evtStore()->retrieve( mcColl, m_mcEventsName );

  if ( sc.isFailure() || 0 == mcColl ) {
    msg(MSG::ERROR)
      << "Failed to retrieve McEventCollection at : "
      << m_mcEventsName
      << endmsg
      << "because : sc= " << ( sc.isFailure() ? "FAILURE" : "OK" ) 
      << " and mcEvent= " << mcColl
      << endmsg;
    return StatusCode::SUCCESS;
  } 

  ATH_MSG_VERBOSE("Create a new McEventCollection which will hold the "\
		  "filtered GenEvent");
  McEventCollection * filteredColl = new McEventCollection;
  sc = evtStore()->record(filteredColl, m_mcEventsOutputName, true);
  sc = evtStore()->setConst(filteredColl);
  ATH_MSG_VERBOSE("Recorded and locked in StoreGate");
  
  filterMcEventCollection( mcColl, filteredColl );
  return sc;
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

void McVtxFilterTool::displayOptions() const
{
  msg(MSG::INFO)
    << "Options for " << name() << " :" << endmsg
    << "\tDecay Patterns: [ ";
  for ( std::vector<std::string>::const_iterator itr = m_decayPatterns.value().begin();
	itr != m_decayPatterns.value().end();
	++itr ) {
    msg(MSG::INFO) << (*itr) << "; ";
  }
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "Particles to keep: [";
  for ( std::vector<long>::const_iterator itr = m_particles.value().begin();
	itr != m_particles.value().end();
	++itr ) {
    msg(MSG::INFO) << (*itr) << ", ";
  }
  msg(MSG::INFO) << "]" << endmsg;

  return;
}

void McVtxFilterTool::stats() const
{
  msg(MSG::INFO) << "Statistics for each filter : " << endmsg;
  for( unsigned int i = 0; i < m_counter.size(); ++i ) {
    msg(MSG::INFO)
      << "\t==> [" << m_decayPatterns.value()[i] << "] accepted " 
      << m_counter[i] << " vertices" << endmsg;
  }
  return;
}

bool McVtxFilterTool::isAccepted( const HepMC::GenVertex* vtx ) const
{
  for( DataVector<McVtxFilter>::const_iterator filter = m_filters.begin();
       filter != m_filters.end();
       ++filter ) {
    if ( (*filter)->isAccepted( vtx ) ) {
      return true;
    }
  }//> end loop over McVtxFilters

  return false;
}

/////////////////////////////////////////////////////////////////// 
/// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void 
McVtxFilterTool::filterMcEventCollection( const McEventCollection* mcColl,
					  McEventCollection* filterColl )
{
  if ( 0 == mcColl ) {
    ATH_MSG_ERROR("McEventCollection is NULL pointer !!");
    return;
  }

  /// Create a map to store the GenVertex barcodes which have been
  /// recorded by a "full-vertex" (McVtxFilter which has a parent branch
  /// and a child branch)
  std::map<int, bool> bcToFullVtx;

  /// Local copy of the GenEvent from StoreGate
  const HepMC::GenEvent * evtSrc = (*mcColl->begin());
  
  /// Create the event container, with Signal Process 
  /// and event number from the GenEvent source
  HepMC::GenEvent * evt = new HepMC::GenEvent( evtSrc->signal_process_id(), 
					       evtSrc->event_number() );
  evt->set_event_scale( evtSrc->event_scale() );
  evt->set_alphaQCD( evtSrc->alphaQCD() );
  evt->set_alphaQED( evtSrc->alphaQED() );
  evt->weights() =   evtSrc->weights();
  evt->set_random_states( evtSrc->random_states() );
  if ( 0 != evtSrc->heavy_ion() ) {
    evt->set_heavy_ion    ( *evtSrc->heavy_ion() );
  }
  if ( 0 != evtSrc->pdf_info() ) {
    evt->set_pdf_info     ( *evtSrc->pdf_info() );
  }

  for ( HepMC::GenEvent::particle_const_iterator itrPart=evtSrc->particles_begin();
	itrPart != evtSrc->particles_end();
	++itrPart ) {
    const HepMC::GenVertex * dcyVtx = (*itrPart)->end_vertex();
    if ( dcyVtx ) {
      int vtxBC = dcyVtx->barcode();
      ATH_MSG_VERBOSE("Doing vtx: " << vtxBC);

      int i = 0;
      for( DataVector<McVtxFilter>::const_iterator filter = m_filters.begin();
	   filter != m_filters.end();
	   ++filter,++i ) {
	ATH_MSG_VERBOSE("Processing with filter[" << i << "]...");
	if ( (*filter)->isAccepted( dcyVtx ) ) {
	  m_counter[i] += 1;
	  ATH_MSG_VERBOSE("Filter[" << i << "] accepted this vtx : " << vtxBC);

	  /// Check if this vertex has already been recorded 
	  /// in the new GenEvent
	  if ( 0 != evt->barcode_to_vertex(vtxBC) ) {
	    //
	    // nothing to do
	    //
	    continue;
	  } else {
	    /// this is a whole new GenVertex which has to added
	    /// so we add it in the list of barcodes
	    bcToFullVtx.insert(std::pair<int,bool>( vtxBC,
						    (*filter)->isFullVtx() ) );
	    addVertex( dcyVtx, evt, VtxType::IsRootVertex );
	  }//> (part of the) vertex has already been recorded

	}//> accepted
      }//> end loop over filters
    }//> valid decay vertex
  }//> loop over particles

  if ( m_doSignalProcessVtx ) {
    /// Handling the signal_process_vertex stuff
    /// Signal_process_vertex pointer is not set by all generators, but in
    /// case it has been set up, one should keep this usefull information,
    /// shouldn't we ?
    /// So first we check it is present in the original GenEvent, and then
    /// we check if it as not already added by any previous McVtxFilter (by 
    /// chance or because it was just meant to be)
    const HepMC::GenVertex * sigProcVtx = evtSrc->signal_process_vertex();
    if ( 0 != sigProcVtx ) {
      if ( evt->barcode_to_vertex(sigProcVtx->barcode()) ) {
	/// it has already been copied
      } else {
	/// copy it
	addVertex( sigProcVtx, evt );

	HepMC::GenVertex * vtx = evt->barcode_to_vertex(sigProcVtx->barcode());
	evt->set_signal_process_vertex( vtx );

      }//> signal process vertex has to be added
    } else {
      //> Original GenEvent has a NO signal process vertex set-up
      ATH_MSG_DEBUG("You asked to record signal_process_vertex but :"
		    << endmsg
		    << " there is NO signal_process_vertex in this event !!");
    }
  
  } //> end do SignalProcessVtx

  filterColl->push_back(evt);
  return;
}

/////////////////////////////////////////////////////////////////// 
/// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
///////////////////////////////////////////////////////////////////

void McVtxFilterTool::addVertex( const HepMC::GenVertex* srcVtx, 
				 HepMC::GenEvent * evt,
				 const VtxType::Flag vtxType ) const
{
  ATH_MSG_VERBOSE("In McVtxFilterTool::addVertex( vtxType= "<<vtxType<< " )");
  HepMC::GenVertex * vtx = evt->barcode_to_vertex(srcVtx->barcode());
  if ( 0 == vtx ) {
    vtx = new HepMC::GenVertex();
    vtx->set_position( srcVtx->position() );
    vtx->set_id( srcVtx->id() );
    vtx->suggest_barcode( srcVtx->barcode() );
    vtx->weights() = srcVtx->weights();
    evt->add_vertex(vtx);
  }
  
  /// Fill the parent branch
  for ( HepMC::GenVertex::particles_in_const_iterator parent = srcVtx->particles_in_const_begin();
	parent != srcVtx->particles_in_const_end();
	++parent ) {
    HepMC::GenParticle * mother = evt->barcode_to_particle( (*parent)->barcode() );
    if ( 0 == mother ) {
      mother = new HepMC::GenParticle;
      mother->set_momentum( (*parent)->momentum() );
      mother->set_generated_mass( (*parent)->generated_mass() );
      mother->set_pdg_id( (*parent)->pdg_id() );
      mother->set_status( (*parent)->status() );
      mother->set_flow( (*parent)->flow() );
      mother->set_polarization( (*parent)->polarization() );
      mother->suggest_barcode( (*parent)->barcode() );

    }
    // set the mother's decay to our (new) vertex
    vtx->add_particle_in( mother );
    
  }//> loop over ingoing particles
  
  /// Fill the children branch
  for ( HepMC::GenVertex::particles_out_const_iterator child = srcVtx->particles_out_const_begin();
	child != srcVtx->particles_out_const_end();
	++child ) {
    HepMC::GenParticle * daughter = evt->barcode_to_particle( (*child)->barcode() );
    if ( 0 == daughter ) {
      if ( !keepParticle( vtxType, *child ) ) {
	// only include selected particles via the "ParticlesToKeep" property
	ATH_MSG_VERBOSE("Skipping outgoing particle id|bc: ["
			<< (*child)->pdg_id() << "|" 
			<< (*child)->barcode() << "]");
      } else {
	daughter = new HepMC::GenParticle;
	daughter->set_momentum( (*child)->momentum() );
   daughter->set_generated_mass( (*child)->generated_mass() );
	daughter->set_pdg_id( (*child)->pdg_id() );
	daughter->set_status( (*child)->status() );
	daughter->set_flow( (*child)->flow() );
	daughter->set_polarization( (*child)->polarization() );
	daughter->suggest_barcode( (*child)->barcode() );

      }
    }
    if ( daughter ) {
      // set the daughter's production vertex to our new vertex
      vtx->add_particle_out( daughter );
    }

    if ( m_fillTree && keepParticle( vtxType, *child ) ) {
      const HepMC::GenVertex * decayVertex = (*child)->end_vertex();
      if ( 0 != decayVertex ) {
	// recursively fill the tree with all decay vertices and final state
	// particles of selected outgoing particles lines
	// => We are no longer sitting at the decay vertex so we tell it
	// via the IsNotRootVertex flag
	addVertex( decayVertex, evt, VtxType::IsNotRootVertex );
      }
    }
  }//> loop over outgoing particles

  return;
}

bool McVtxFilterTool::keepParticle( const VtxType::Flag vtxType, 
				    const HepMC::GenParticle* part ) const 
{
  // no particle, so no particle to keep. Simple, isn't ?
  if ( 0 == part ) {
    return false;
  }

  // By default, we keep all particles
  if ( m_particles.value().empty() ) {
    return true;
  }

  // only filter decay particles of the root vertex
  // ie: the vertex which matches a decay pattern setup by the
  // "DecayPatterns" property.
  if ( vtxType == VtxType::IsNotRootVertex ) {
    return true;

  } else if ( vtxType == VtxType::IsRootVertex ) {
    // we are at a Root vertex.
    // And we only keep the outgoing particles which are in our list
    const int pdgId = part->pdg_id();
    if ( std::find( m_particles.value().begin(), 
		    m_particles.value().end(), 
		    pdgId ) == 
	 m_particles.value().end() ) {
      return false;
    } else {
      return true;
    }
  } else {
    
    // Humm.. we don't know anything about this VtxType...
    // by default we keep all particles
    // But we tell user that something is wrong !
    msg(MSG::WARNING)
      << "In keepParticle: Don't know anything about this VtxType ["
      << vtxType << "] !!"
      << endmsg
      << "We'll keep this particle [bc= " << part->barcode() 
      << "] but : Check your jobOption !!"
      << endmsg;
    return true;
  }
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void McVtxFilterTool::setupFilters( Gaudi::Details::PropertyBase& /*decayPatterns*/ )
{
  m_filters.clear();
  m_counter.clear();
  
  const std::vector<std::string>& decayPatterns = m_decayPatterns.value();
  std::vector<bool> matchSigns    = m_matchSigns.value();
  std::vector<bool> matchBranches = m_matchBranches.value();

  // Normally, using the Configurables, the lengths of properties should
  // be the same... but who knows. Better test it.
  // On the other hand, for compatibility reasons, we allow the situation
  // where *both* matchSigns and matchBranches are empty.
  // Only after having removed the "old-style" case we can raise an exception.
  const bool oldCfg = ( m_matchSigns.value().empty() &&
			m_matchBranches.value().empty() );
  if ( !oldCfg && 
       ( decayPatterns.size() != matchSigns.size() ||
	 decayPatterns.size() != matchBranches.size() ||
	 matchSigns.size()    != matchBranches.size() ) ) {
    std::string error;
    error  = "Lengths of ArrayProperty configuring McVtxFilters ";
    error += "are DIFFERING !!";
    msg(MSG::WARNING)
      << "In setupFilter: " << error 
      << endmsg
      << "\t# decayPatterns: " << decayPatterns.size() << endmsg
      << "\t# matchSigns:    " << matchSigns.size()    << endmsg
      << "\t# matchBranches: " << matchBranches.size() << endmsg
      << "Reference size is 'decayPatterns':" << endmsg
      << "\t==> Will fill holes with default values !" << endmsg
      << "\t==> Will drop extra values !"
      << endmsg;
    //throw std::runtime_error(error);
  }

  const unsigned int nDecayPatterns = decayPatterns.size();
  m_filters.resize( nDecayPatterns );
  m_counter.resize( nDecayPatterns, 0 );

  if ( nDecayPatterns > matchSigns.size() ) {
    // fill it  with default values
    const unsigned int nCount = nDecayPatterns - matchSigns.size();
    std::fill_n( std::back_inserter(matchSigns),
		 nCount, // number of times we fill it
		 false );// default value
  }

  if ( nDecayPatterns > matchBranches.size() ) {
    // fill it  with default values
    const unsigned int nCount = nDecayPatterns - matchBranches.size();
    std::fill_n( std::back_inserter(matchBranches),
		 nCount, // number of times we fill it
		 false );// default value
  }

  for ( unsigned int i = 0; i != nDecayPatterns; ++i ) {
    m_filters[i] = new McVtxFilter( decayPatterns[i],
				    bool(matchSigns[i]),
				    bool(matchBranches[i]) );
  }

  return;
}
