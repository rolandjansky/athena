/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENINTERACTING_H
#define TRUTHHELPER_ISGENINTERACTING_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Identify if the particle is an interacting one, e.g. hadron, electron, photon or muon
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenInteracting : public GenIMCselector {
  public:

    IsGenInteracting() {}

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr p ) const;

  };


}

#endif
