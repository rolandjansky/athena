/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENNONINTERACTING_H
#define TRUTHHELPER_ISGENNONINTERACTING_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Identify if the particle is non interacting, e.g. a neutrino
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenNonInteracting : public GenIMCselector {
  public:

    IsGenNonInteracting() {}

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p ) const;

  };


}

#endif
