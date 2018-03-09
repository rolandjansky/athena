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
  , m_totalSeen(0)
{
  declareProperty("KillLoops", m_killLoops = true, "Remove particles in loops?");
  declareProperty("KillPDG0", m_killPDG0 = true, "Remove particles with PDG ID 0?");
  declareProperty("CleanDecays", m_cleanDecays = true, "Clean decay chains from non-propagating particles?");
  declareProperty("LoopsByBarcode", m_loopByBC = false, "Detect loops based on barcodes as well as vertices?");
}


StatusCode FixHepMC::execute() {
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
