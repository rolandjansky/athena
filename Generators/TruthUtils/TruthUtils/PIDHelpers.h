/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

/// @file
///
/// PID-only functions with no HepMC dependence, both from MCUtils and ATLAS-specific.

/* For any functions that we need to overload, we need to
carefully #define them to a local function name.  Then in
the namespace import below, we will still be able to re-define
the 'global' name referring to the local update.  isBSM is
our first case for this -- there are ATLAS-specific BSM models
that do not follow the PDG recommendations, but which we want
to having included in the definition of isBSM */
#define isBSM isBSM_MCUtils
#include "MCUtils/PIDUtils.h"
#undef isBSM

namespace MC {

  // Use the MCUtils and HEPUtils functions as if they were defined in the ATLAS MC and MC::PID namespaces
  using namespace MCUtils;
  using namespace HEPUtils;


  /// @brief Identify if the particle with given PDG ID would not interact with the detector, i.e. not a neutrino or WIMP
  inline bool isNonInteracting(int pid) {
    return !(PID::isStrongInteracting(pid) || PID::isEMInteracting(pid));
  }

  /// Is this a Vector-like Quark?
  inline bool isVLQ(int pid){
    // X, T, B, Y in that order
    return in_closed_range(abs(pid),6000005,6000008);
  }

  /// Is this from a Type-III See-Saw model
  inline bool isTypeIIISeeSaw(int pid){
    // Even numbers between 8000012 and 8000020: v1, v2, v3, tr0, tr+
    return _digit(n,pid) == 8 &&
           in_closed_range(abs(pid),8000012,8000020) &&
           pid%2 == 0;
  }

  /// Is this a vector triplet particle?
  inline bool isVectorTriplet(int pid){
    // Heavy Vector Triplet Z' and W'
    return in_closed_range(abs(pid),9000001,9000002);
  }

  /// Is this a doubly-charged Higgs boson?
  inline bool isDoublyChargedHiggs(int pid){
    // H_L++ and H_R++ (negative for --)
    return in_closed_range(abs(pid),9900041,9900042);
  }

  /// Is this from a dark sector model?
  inline bool isDarkSector(int pid){
    // Hidden scalar and hidden photon
    if (in_closed_range(abs(pid),700021,700022)) return true;
    if (!_digit(n,pid) == 3) return false;
    int fundId = _fundamentalID(pid);
    // Z_Dark, H_Dark, N_Dark1, N_Dark2
    return fundId == 1 || fundId == 5 ||
           fundId == 15 || fundId = 16;
  }

  /// Is this from a hiddel valley model?
  inline bool isHiddenValley(int pid){
    if (_digit(n,pid) != 4) return false;
    if (_digit(nr,pid) != 9) return false;
    if (_digit(nl,pid) != 0) return false;
    return true;
  }

  /// Is this a heavy neutrino?
  inline bool isHeavyNeutrino(int pid){
    if (_digit(n,pid) != 9) return false;
    if (_digit(nr,pid) != 9) return false;
    return isNeutrino( _fundamentalID(pid) );
  }

  /// @brief Is this an exotic particle?
  inline bool isExotic(int pid){
    return in_closed_range(abs(pid),40,80);
  }

  /// @brief Is this a BSM boson?
  inline bool isBSMBoson(int pid){
    return in_closed_range(abs(pid),32,39);
  }

  /// @brief Is this a BSM particle?
  inline bool isBSM(int pid) {
    return isVLQ(pid) || isTypeIIISeeSaw(pid) || isVectorTriplet(pid) ||
           isDoublyChargedHiggs(pid) || isDarkSector(pid) || isHiddenValley(pid) ||
           isHeavyNeutrino(pid) || isExotic(pid) || isBSMBoson(pid) ||
           abs(pid)==5100039 || // Special case: Gravitino in Pythia
           abs(pid)==43 || in_closed_range(apid, 601,606) || // Extra lepto-quark UFOs
           abs(pid)==1999 || // Coloron (rho)
           abs(pid)==6666 || // Pseudo Nambu-Goldstone ED model Higgs particle
           abs(pid)==9000006 || // New 'phi' particle from cosmo UFO model
           isBSM_MCUtils(pid);
  }

  /// @brief Identify if the particle with given PDG ID would produce ID tracks but not shower in the detector if stable
  inline bool isChargedNonShowering(int pid) {
    if (PID::isMuon(pid)) return true;
    if (PID::isSUSY(pid)) return true; //(meta)stable charginos, R-hadrons etc
     return false;
   }

}
