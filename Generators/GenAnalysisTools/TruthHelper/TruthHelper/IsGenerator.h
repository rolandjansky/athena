/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISGENERATOR_H
#define TRUTHHELPER_ISGENERATOR_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @brief Returns true if the particle was produced by an event generator
  /// @deprecated Use the functions in TruthUtils instead
  class IsGenerator : public GenIMCselector {
  public:

    IsGenerator() {}
    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p ) const;

  };


}

#endif
