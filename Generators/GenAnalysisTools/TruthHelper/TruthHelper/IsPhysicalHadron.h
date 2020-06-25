/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHHELPER_ISPHYSICALHADRON_H
#define TRUTHHELPER_ISPHYSICALHADRON_H

#include "TruthHelper/GenIMCselector.h"
#include <functional>

namespace TruthHelper {


  /// @deprecated Use the functions in TruthUtils instead
  class IsPhysicalHadron: public GenIMCselector {
  public:

    IsPhysicalHadron() { }

    GenIMCselector* create() const;

    bool operator()(HepMC::ConstGenParticlePtr  p ) const;

  };


}

#endif
