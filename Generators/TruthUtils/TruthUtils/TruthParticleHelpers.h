/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRUTHUTILS_TRUTHPARTICLEHELPERS_H
#define TRUTHUTILS_TRUTHPARTICLEHELPERS_H
/// @file
///
/// Provides the HepMC tools from the external MCUtils header package,
/// ATLAS-specific HepMC functions not suitable for MCUtils.

#include "TruthUtils/PIDHelpers.h"

namespace MC {

  // Use the MCUtils and HEPUtils functions as if they were defined in the ATLAS MC and MC::PID namespaces
  using namespace MCUtils;
  using namespace HEPUtils;


  /// @brief Constant defining the barcode threshold distinguishing generator record entries from detector sim ones
  /// @todo The sim barcodes start at 1M in MC15, so we should update the 200k threshold,
  ///   but >= 200k is still a valid test for b = 1M so let's keep it this way until MC12 is long-dead.
  const int SIM_BARCODE_THRESHOLD = 200000;


  /// @name Extra ATLAS-specific particle classifier functions
  //@{

  /// @brief Determine if the particle is stable at the generator (not det-sim) level,
  ///
  /// The receipe for this is barcode < 200k and status = 1. Gen-stable particles decayed by
  /// G4 are not set to have status = 2 in ATLAS, but simply have more status = 1 children,
  /// with barcodes > 200k.
  inline bool isGenStable(int status, int barcode) {
    if (status != 1) return false;
    return barcode < SIM_BARCODE_THRESHOLD;
  }

  //@}

}
#endif
