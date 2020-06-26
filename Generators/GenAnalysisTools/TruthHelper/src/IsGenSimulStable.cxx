/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHelper/IsGenSimulStable.h"
#include "TruthUtils/HepMCHelpers.h"

namespace TruthHelper {


  /// @brief Identify particles which are stable as far as the detector simulation is concerned
  /// @note This behaves identically to IsGenStable: not good!
  bool IsGenSimulStable::operator()(HepMC::ConstGenParticlePtr p) const {
    return MC::isSimStable(p);
  }


  GenIMCselector* IsGenSimulStable::create() const {
    return new IsGenSimulStable(*this);
  }


}
