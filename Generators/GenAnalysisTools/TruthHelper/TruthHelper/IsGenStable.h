/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENSTABLE_H
#define TRUTHHELPER_ISGENSTABLE_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Identify if the particle is considered stable before input to G4
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenStable: public GenIMCselector {
  public:

    IsGenStable() {}

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr p ) const;

  };


}

#endif
