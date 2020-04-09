/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

/// @file
///
/// Provides the HepMC tools from the external MCUtils header package,
/// ATLAS-specific HepMC functions not suitable for MCUtils.

#include "TruthUtils/PIDHelpers.h"
#include "TruthUtils/TruthParticleHelpers.h"
#include "MCUtils/PIDUtils.h"
#include "AtlasHepMC/GenEvent.h"

namespace MC {


  /// @name Extra ATLAS-specific particle classifier functions
  //@{

  /// @brief Determine if the particle is stable at the generator (not det-sim) level,
  ///
  /// The receipe for this is barcode < 200k and status = 1. Gen-stable particles decayed by
  /// G4 are not set to have status = 2 in ATLAS, but simply have more status = 1 children,
  /// with barcodes > 200k.
  inline bool isGenStable(const HepMC::ConstGenParticlePtr p) {
    return isGenStable(p->status(), HepMC::barcode(p));
  }


  /// @todo There are many kinds of stable: stable from generator, stable at intermediate stage of det sim transport, or stable after all det sim. Need fns for each?


  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  inline bool isSimStable(const HepMC::ConstGenParticlePtr p) {
    if (p->status() != 1) return false;
    if (isGenStable(p)) return p->end_vertex() == NULL;
    return true;
  }

  /// @brief Identify if the particle is considered stable at the post-detector-sim stage
  /// @todo I'm sure this shouldn't be exactly the same as isGenStable, but it is...
  /// @deprecated Use isSimulStable: this function _will_ be removed!
  inline bool isGenSimulStable(const HepMC::ConstGenParticlePtr p) {
    return isSimStable(p);
  }


  /// @brief Identify if the particle would not interact with the detector, i.e. not a neutrino or WIMP
  inline bool isNonInteracting(const HepMC::ConstGenParticlePtr p) {
    return MC::isNonInteracting(p->pdg_id()); //< From TruthUtils/PIDHelpers.h
  }


  /// @brief Identify if the particle could interact with the detector during the simulation, e.g. not a neutrino or WIMP
  inline bool isSimInteracting(const HepMC::ConstGenParticlePtr p) {
    if (! MC::isGenStable(p)) return false; //skip particles which the simulation would not see
    return !MC::isNonInteracting(p);
  }

  /// @brief Oddly-named alias for isSimInteracting
  /// @deprecated Use isSimInteracting: this function _will_ be removed!
  inline bool isGenInteracting(const HepMC::ConstGenParticlePtr p) {
    return isSimInteracting(p);
  }

  //@}


}
