/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_GENALL_H
#define TRUTHHELPER_GENALL_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @deprecated Use the functions in TruthUtils instead
  class GenAll : public GenIMCselector {
  public:

    GenIMCselector* create() const {
      return new GenAll();
    }

    bool operator()(HepMC::ConstGenParticlePtr) const {
      return true;
    }

  };


}

#endif
