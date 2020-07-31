/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENSIMULSTABLE_H
#define TRUTHHELPER_ISGENSIMULSTABLE_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Identify if the particle is considered stable at the post-G4 stage
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenSimulStable : public GenIMCselector {
  public:

    IsGenSimulStable() {}

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p) const;

  };


}

#endif
