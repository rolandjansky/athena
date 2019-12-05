/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/FixHepMC.h"
#include "TruthUtils/HepMCHelpers.h"
using namespace std;


FixHepMC::FixHepMC(const string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
  , m_loopKilled(0)
  , m_pdg0Killed(0)
  , m_decayCleaned(0)
  , m_muonVtxChanged(0)
  , m_totalSeen(0)
{
  declareProperty("KillLoops", m_killLoops = true, "Remove particles in loops?");
  declareProperty("KillPDG0", m_killPDG0 = true, "Remove particles with PDG ID 0?");
  declareProperty("CleanDecays", m_cleanDecays = true, "Clean decay chains from non-propagating particles?");
  declareProperty("LoopsByBarcode", m_loopByBC = false, "Detect loops based on barcodes as well as vertices?");
  declareProperty("SetMuonsToProdVtx", m_setMuonsToProdVtx = false, "Detect muons at very large displacement and set to collision vertex. For H7+matchbox");
}


StatusCode FixHepMC::execute() {

  // If all options are off, then just return and exit. Not all of the code runs on afterburners.
  // So we exit if everything is off.
  if(!(m_killLoops || m_killPDG0 || m_cleanDecays || m_loopByBC || m_setMuonsToProdVtx)) return StatusCode::SUCCESS;

  // This patch is to fix the NLO MatchBox+H7 sample. Muons can travel some large distance before turning status 1. Here we reset their vertex to the status 11 muon vertex
  if(m_setMuonsToProdVtx){
    for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {
      const HepMC::GenEvent* evt = *itr;

      for (HepMC::GenEvent::vertex_const_iterator vitr = evt->vertices_begin(); vitr != evt->vertices_end(); ++vitr ) {
	const HepMC::GenVertex* vtx = *vitr;
	const HepMC::ThreeVector pos = vtx->point3d();
	// Check for too-far-displaced vertices
	// Anything which propagates macroscopically should be set stable in evgen for G4 to handle
	const double dist_trans2 = pos.x()*pos.x() + pos.y()*pos.y(); // in mm2
	const double dist2 = dist_trans2 + pos.z()*pos.z(); // in mm2
	const double dist = sqrt(dist2); // in mm
	if(1.0<dist){// check if the distance is larger than 1mm
	  HepMC::GenVertex::particle_iterator par = (*vitr)->particles_begin(HepMC::parents);
	  for (; par != (*vitr)->particles_end(HepMC::parents); ++par) {
	    if(fabs((*par)->pdg_id())==13){
	      unsigned npartvtxin = ((*vitr)->particles_in_size());
	      unsigned npartvtxout = ((*vitr)->particles_out_size());
	      if(npartvtxin>1){
		ATH_MSG_WARNING("Changing H7+MatchBox muon vertex. more than just one muon found in: " << npartvtxin << " out: " << npartvtxout << ". doing nothing!");
	      }else if(npartvtxout>1){
		ATH_MSG_INFO("Changing H7+MatchBox muon vertex. more than just one muon found in: " << npartvtxin << " out: " << npartvtxout << ". setting both to the original vertex! Mostly FSR photon");
		HepMC::GenVertex::particle_iterator parc = (*vitr)->particles_begin(HepMC::parents);
		for (; parc != (*vitr)->particles_end(HepMC::parents); ++parc) {
		  ATH_MSG_INFO("More than 1 outgoing. Particle: " << (*parc)->pdg_id() << " status: " << (*parc)->status());
		  (*parc)->end_vertex()->set_position((*parc)->production_vertex()->position());
		}
	      }else{ // only 1 muon in and 1 muons out. setting this vertex back to the point of origin
		(*par)->end_vertex()->set_position((*par)->production_vertex()->position());
		m_muonVtxChanged+=1;
	      }
	      ATH_MSG_VERBOSE("Found vertex position displaced by more than  1mm: " << dist << "mm");
	      ATH_MSG_VERBOSE("Outgoing particle : ");
	      ATH_MSG_VERBOSE("production vertex = " << (*par)->production_vertex()->point3d().x() << ", " << (*par)->production_vertex()->point3d().y() << ", " << (*par)->production_vertex()->point3d().z());
	      ATH_MSG_VERBOSE("end vertex        = " << (*par)->end_vertex()->point3d().x() << ", " << (*par)->end_vertex()->point3d().y() << ", " << (*par)->end_vertex()->point3d().z());
	      ATH_MSG_VERBOSE("parents info: ");
	      if ((*par)->production_vertex()) {
		HepMC::GenVertex::particle_iterator p_parents = (*par)->production_vertex()->particles_begin(HepMC::parents);
		for(; p_parents != (*par)->production_vertex()->particles_end(HepMC::parents); ++p_parents) {
		  if((*p_parents)->status()!=11){
		    ATH_MSG_WARNING("Changing H7+MatchBox muon vertex. The muons parent is NOT status 11. This was not expected.");
		    (*p_parents)->print();
		  }
		  if(npartvtxout>1){// more than just the muon going out, then printing
		    ATH_MSG_INFO("More than 1 outgoing. Parent: " << (*p_parents)->pdg_id() << " status: " << (*p_parents)->status());
		  }
		  ATH_MSG_VERBOSE("parent production vertex = " << (*p_parents)->production_vertex()->point3d().x() << ", " << (*p_parents)->production_vertex()->point3d().y() << ", " << (*p_parents)->production_vertex()->point3d().z());
		  ATH_MSG_VERBOSE("parent end vertex        = " << (*p_parents)->end_vertex()->point3d().x() << ", " << (*p_parents)->end_vertex()->point3d().y() << ", " << (*p_parents)->end_vertex()->point3d().z());
		  ATH_MSG_VERBOSE("\t");
		}
	      }
	    }
	  }
	}
      }
    }
  }// end this H7+MatchBox fix
  // If this is just for H7+Matchbox, we are done and exiting
  if(m_setMuonsToProdVtx) return StatusCode::SUCCESS;

  // Continue with the remaining code that runs only on generation
  for (McEventCollection::const_iterator ievt = events()->begin(); ievt != events()->end(); ++ievt) {
    HepMC::GenEvent* evt = *ievt;

    // Add a unit entry to the event weight vector if it's currently empty
    if (evt->weights().empty()) {
      ATH_MSG_DEBUG("Adding a unit weight to empty event weight vector");
      evt->weights().push_back(1);
    }

    // Set a (0,0,0) vertex to be the signal vertex if not already set
    if (evt->signal_process_vertex() == NULL) {
      const HepMC::FourVector nullpos;
      for (HepMC::GenEvent::vertex_const_iterator iv = evt->vertices_begin(); iv != evt->vertices_end(); ++iv) {
        if ((*iv)->position() == nullpos) {
          ATH_MSG_DEBUG("Setting representative event position vertex");
          evt->set_signal_process_vertex(const_cast<HepMC::GenVertex*>(*iv));
          break;
        }
      }
    }

    // Event particle content cleaning -- remove "bad" structures
    vector<HepMC::GenParticle*> toremove; toremove.reserve(10);
    long seenThisEvent = 0;
    /// @todo Use nicer particles accessor from TruthUtils / HepMC3 when it exists
    for (HepMC::GenEvent::particle_const_iterator ip = evt->particles_begin(); ip != evt->particles_end(); ++ip) {
      // Skip this particle if (somehow) its pointer is null
      if (*ip == NULL) continue;
      m_totalSeen += 1;
      seenThisEvent += 1;

      // Flag to declare if a particle should be removed
      bool bad_particle = false;

      // Check for loops
      if ( m_killLoops && isLoop(*ip) ) {
        bad_particle = true;
        m_loopKilled += 1;
        ATH_MSG_DEBUG( "Found a looper : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }
      // Check on PDG ID 0
      if ( m_killPDG0 && isPID0(*ip) ) {
        bad_particle = true;
        m_pdg0Killed += 1;
        ATH_MSG_DEBUG( "Found PDG ID 0 : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }

      // Clean decays
      if ( m_cleanDecays && isNonTransportableInDecayChain(*ip) ) {
        bad_particle = true;
        m_decayCleaned += 1;
        ATH_MSG_DEBUG( "Found a bad particle in a decay chain : " );
        if ( msgLvl( MSG::DEBUG ) ) (*ip)->print();
      }

      // Only add to the toremove vector once, even if multiple tests match
      if (bad_particle) toremove.push_back(*ip);
    }
    // Escape here if there's nothing more to do, otherwise do the cleaning
    if (toremove.empty()) continue;
    ATH_MSG_DEBUG("Cleaning event record of " << toremove.size() << " bad particles");

    // Properties before cleaning
    const int num_particles_orig = evt->particles_size();
    const int num_orphan_vtxs_orig = MC::const_vertices_match(evt, MC::isDisconnected).size();
    const int num_noparent_vtxs_orig = MC::const_vertices_match(evt, MC::hasNoParents).size();
    const int num_nochild_vtxs_orig = MC::const_vertices_match(evt, MC::hasNoChildren).size();

    // Clean!
    MC::reduce(evt , toremove);

    // Properties after cleaning
    const int num_particles_filt = evt->particles_size();
    const int num_orphan_vtxs_filt = MC::const_vertices_match(evt, MC::isDisconnected).size();
    const int num_noparent_vtxs_filt = MC::const_vertices_match(evt, MC::hasNoParents).size();
    const int num_nochild_vtxs_filt = MC::const_vertices_match(evt, MC::hasNoChildren).size();

    // Write out the change in the number of particles
    ATH_MSG_INFO("Particles filtered: " << num_particles_orig << " -> " << num_particles_filt);
    // Warn if the numbers of "strange" vertices have changed
    if (num_orphan_vtxs_filt != num_orphan_vtxs_orig)
      ATH_MSG_WARNING("Change in orphaned vertices: " << num_orphan_vtxs_orig << " -> " << num_orphan_vtxs_filt);
    if (num_noparent_vtxs_filt != num_noparent_vtxs_orig)
      ATH_MSG_WARNING("Change in no-parent vertices: " << num_noparent_vtxs_orig << " -> " << num_noparent_vtxs_filt);
    if (num_nochild_vtxs_filt != num_nochild_vtxs_orig)
      ATH_MSG_WARNING("Change in no-parent vertices: " << num_nochild_vtxs_orig << " -> " << num_nochild_vtxs_filt);

  }
  return StatusCode::SUCCESS;
}


StatusCode FixHepMC::finalize() {
  if (m_killLoops  ) ATH_MSG_INFO( "Removed " <<   m_loopKilled << " of " << m_totalSeen << " particles because of loops." );
  if (m_killPDG0   ) ATH_MSG_INFO( "Removed " <<   m_pdg0Killed << " of " << m_totalSeen << " particles because of PDG ID 0." );
  if (m_cleanDecays) ATH_MSG_INFO( "Removed " << m_decayCleaned << " of " << m_totalSeen << " particles while cleaning decay chains." );
  if (m_setMuonsToProdVtx) ATH_MSG_INFO( "Set Muon Vertex is running for H7+MatchBox? " << m_setMuonsToProdVtx << " changed " << m_muonVtxChanged << " muon vertex positions." );
  return StatusCode::SUCCESS;
}


/// @name Classifiers for identifying particles to be removed
//@{

// Identify PDG ID = 0 particles, usually from HEPEVT padding
bool FixHepMC::isPID0(const HepMC::GenParticle* p) {
  return p->pdg_id() == 0;
}

// Identify non-transportable stuff _after_ hadronisation
bool FixHepMC::isNonTransportableInDecayChain(const HepMC::GenParticle* p) {
  return !MC::isTransportable(p) && MC::fromDecay(p);
}

// Identify internal "loop" particles
bool FixHepMC::isLoop(const HepMC::GenParticle* p) {
  if (p->production_vertex() == p->end_vertex() && p->end_vertex() != NULL) return true;
  if (m_loopByBC && p->production_vertex()) {
    /// @todo Use new particle MC::parents(...) tool
    for (HepMC::GenVertex::particle_iterator itrParent = p->production_vertex()->particles_begin(HepMC::parents);
         itrParent != p->production_vertex()->particles_end(HepMC::parents); ++itrParent) {
      if ( (*itrParent)->barcode() > p->barcode() ) {
        ATH_MSG_VERBOSE("Found a loop (a la Sherpa sample) via barcode.");
        return true; // Cannot vectorize, but this is a pretty short loop
      } // Check on barcodes
    } // Loop over parent particles
  } // Has a production vertex
  return false;
}

//@}

#endif
