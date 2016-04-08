/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

/// @file
///
/// Provides the HepMC tools from the external MCUtils header package,
/// ATLAS-specific HepMC functions not suitable for MCUtils.

#include "TruthUtils/PIDHelpers.h"
#include "TruthUtils/TruthParticleHelpers.h"
#include "MCUtils/HepMCUtils.h"
// #include "MCUtils/Clustering.h"

// Common imports from external namespaces
using HepMC::GenEvent;
using HepMC::GenParticle;
using HepMC::GenVertex;
//using fastjet::PseudoJet;
//using fastjet::ClusterSequence;

// Alias BOOST_FOREACH as foreach in this semi-safe way
#include <boost/foreach.hpp>
#ifndef foreach
namespace boost { namespace BOOST_FOREACH = foreach; }
#define foreach BOOST_FOREACH
#endif


namespace MC {

  // Use the MCUtils and HEPUtils functions as if they were defined in the ATLAS MC and MC::PID namespaces
  using namespace MCUtils;
  using namespace HEPUtils;


  /// @name Extra ATLAS-specific particle classifier functions
  //@{

  /// @brief Determine if the particle is stable at the generator (not det-sim) level,
  ///
  /// The receipe for this is barcode < 200k and status = 1. Gen-stable particles decayed by
  /// G4 are not set to have status = 2 in ATLAS, but simply have more status = 1 children,
  /// with barcodes > 200k.
  inline bool isGenStable(const HepMC::GenParticle* p) {
    return isGenStable(p->status(), p->barcode());
  }


  /// @todo There are many kinds of stable: stable from generator, stable at intermediate stage of det sim transport, or stable after all det sim. Need fns for each?


  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  inline bool isSimStable(const HepMC::GenParticle* p) {
    if (p->status() != 1) return false;
    if (isGenStable(p)) return p->end_vertex() == NULL;
    return true;
  }

  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  /// @todo I'm sure this shouldn't be exactly the same as isGenStable, but it is...
  /// @deprecated Use isSimulStable: this function _will_ be removed!
  inline bool isGenSimulStable(const HepMC::GenParticle* p) {
    return isSimStable(p);
  }


  /// @brief Identify if the particle would not interact with the detector, i.e. not a neutrino or WIMP
  inline bool isNonInteracting(const HepMC::GenParticle* p) {
    return MC::isNonInteracting(p->pdg_id()); //< From TruthUtils/PIDHelpers.h
  }

  /// @brief Identify if the particle could interact with the detector during the simulation, e.g. not a neutrino or WIMP
  /// @todo This one can't be made to only take a PDG ID argument since it needs to check gen-stability via status & decay links
  // inline bool isSimInteracting(int pid) {
  //   if (! MC::isGenStable(pid)) return false; //skip particles which the simulation would not see
  //   return !MC::isNonInteracting(pid);
  // }
  /// @brief Identify if the particle could interact with the detector during the simulation, e.g. not a neutrino or WIMP
  inline bool isSimInteracting(const HepMC::GenParticle* p) {
    if (! MC::isGenStable(p)) return false; //skip particles which the simulation would not see
    return !MC::isNonInteracting(p);
  }

  /// @brief Oddly-named alias for isSimInteracting
  /// @deprecated Use isSimInteracting: this function _will_ be removed!
  inline bool isGenInteracting(const HepMC::GenParticle* p) {
    return isSimInteracting(p);
  }

  //@}


}
