/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

/// @file A backward compatibility header which pulls in the HepMC tools
///       adapted (and synced?) with the external MCUtils header package.
///       Some (old/deprecated) ATLAS functions not suitable for MCUtils
///       are defined here, too... for now!

#include "TruthUtils/HepMCUtils.h"
#include "TruthUtils/Clustering.h"

// Common imports from external namespaces
/// @todo Remove these
using HepMC::GenEvent;
using HepMC::GenParticle;
using HepMC::GenVertex;
using fastjet::PseudoJet;
using fastjet::ClusterSequence;

// Alias BOOST_FOREACH as foreach in this semi-safe way
/// @todo Get rid of this in favour of range-for loops with C++11
#include <boost/foreach.hpp>
#ifndef foreach
namespace boost { namespace BOOST_FOREACH = foreach; }
#define foreach BOOST_FOREACH
#endif


namespace MC {

  /// @name Extra ATLAS-specific particle classifier functions
  //@{

  /// @brief Determine if the particle is stable at the generator (not det-sim) level,
  ///
  /// I.e. barcode < 200k and either status 1 or status % 1000 = 2 with a decay vertex
  /// barcode < -200k and/or status > 1000 (the latter would indicate a gen-stable decayed by the G4 sim.)
  inline bool isGenStable(const HepMC::GenParticle* p) {
    if (p->barcode() >= 200000) return false; // This particle is from G4
    if (p->pdg_id() == 21 && p->momentum().e() == 0) return false; //< Workaround for a gen bug?
    return ((p->status() % 1000 == 1) || //< Fully stable, even if marked that way by G4
            (p->status() % 1000 == 2 && p->end_vertex() != NULL && p->end_vertex()->barcode() < -200000)); //< Gen-stable with G4 decay
    /// @todo Add a no-descendants-from-G4 check?
  }


  /// @todo There are many kinds of stable: stable from generator, stable at intermediate stage of det sim transport, or stable after all det sim. Need fns for each?


  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  /// @todo I'm sure this shouldn't be exactly the same as isGenStable, but it is...
  inline bool isSimStable(const HepMC::GenParticle* p) {
    /// @todo This was commented out in the original: isn't it what we want?
    // return p->status() % 1000 == 1;
    return isGenStable(p);
  }

  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  /// @todo I'm sure this shouldn't be exactly the same as isGenStable, but it is...
  /// @deprecated Use isSimulStable: this function _will_ be removed!
  inline bool isGenSimulStable(const HepMC::GenParticle* p) {
    return isSimStable(p);
  }


  /// @brief Identify if the particle could interact with the detector, e.g. not a neutrino or WIMP
  inline bool isSimInteracting(const HepMC::GenParticle* p) {
    if (! MC::isGenStable(p)) return false;
    const int apid = abs(p->pdg_id());
    if (apid == 12 || apid == 14 || apid == 16) return false;
    /// @todo Use MCUtils functions to make this more general, e.g. isStronglyInteracting || isEMInteracting (need to write these fns)
    if (p->status() % 1000 == 1) { //< Isn't this implied by isGenStable?
      if (apid == 1000022 || apid == 1000024 || apid == 5100022) return false;
      if (apid == 39 || apid == 1000039 || apid == 5000039) return false;
    }
    return true;
  }

  /// @brief Oddly-named alias for isSimInteracting
  /// @deprecated Use isSimInteracting: this function _will_ be removed!
  inline bool isGenInteracting(const HepMC::GenParticle* p) {
    return isSimInteracting(p);
  }

  //@}


}
