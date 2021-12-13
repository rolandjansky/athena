/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRUTHUTILS_PIDHELPERS_H
#define TRUTHUTILS_PIDHELPERS_H
/// @file
///
/// PID-only functions with no HepMC dependence, both from MCUtils and ATLAS-specific.

#include "MCUtils/PIDUtils.h"

namespace MC {

  // Use the MCUtils and HEPUtils functions as if they were defined in the ATLAS MC and MC::PID namespaces
  using namespace MCUtils;
  using namespace HEPUtils;


  /// @brief Identify if the particle with given PDG ID would not interact with the detector, i.e. not a neutrino or WIMP
  inline bool isNonInteracting(int pid) {
    return !(PID::isStrongInteracting(pid) || PID::isEMInteracting(pid));
  }

  /// @brief Identify if the particle with given PDG ID would produce ID tracks but not shower in the detector if stable
  inline bool isChargedNonShowering(int pid) {
    if (PID::isMuon(pid)) return true;
    if (PID::isSUSY(pid)) return true; //(meta)stable charginos, R-hadrons etc
     return false;
   }

}
#endif
